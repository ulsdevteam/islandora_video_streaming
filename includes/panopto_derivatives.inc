<?php

/**
 * Implement Panopto video upload  
 * @param AbstractObject $object An Islandora Object
 * @param String $filePath video file path
 */


libraries_load('aws'); 

use Aws\S3\S3Client;
use Aws\Exception\AwsException;
use Aws\S3\ObjectUploader;
use Aws\S3\MultipartUploader;
use Aws\Exception\MultipartUploadException;

# Filename of manifest XML file. Any filename is acceptable.
const MANIFEST_FILE_NAME = '/tmp/upload_manifest_generated.xml';

function islandora_video_streaming_panopto_upload($object, $filePath) {
  if ($filePath || !isset($object['PANOPTO_INFO'])) {
    module_load_include('inc', 'islandora_video', "includes/derivatives" );
    $archival_file = islandora_video_copy_obj_datastream($object);
    if ($archival_file === FALSE) {
      return islandora_video_no_obj_datastream($object->id);
    }
    $filePath = drupal_realpath($archival_file['file']->uri);
    $failureMessage = array(
      'success' => false,
      'messages' => array(),
    );
    $streaming = array();
    
    $oauthendpoint = variable_get('islandora_video_streaming_panopto_host');
    $uploadendpoint = $oauthendpoint . '/Panopto/PublicAPI/Rest/sessionUpload';
    $oauthendpoint .= "/Panopto/oauth2/connect/token";
    $token = islandora_video_streaming_panopto_get_token($oauthendpoint);
    $session = null;
    if ($token) {
      $session = islandora_video_streaming_panopto_get_session($token, $uploadendpoint); 
    }
    else {
      $messages = array(
        'message' => t('Failed to retrieve token for object: @pid'),
        'message_sub' => array('@pid' => $object->id),
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      );
      $failureMessage['messages'][] = $messages;
      return $failureMessage;
    }
    $success = false;
    if ($session) {
      try {
        $upload_id = $session['ID'];
        $upload_target = $session['UploadTarget'];
        $file_uri = explode("/", $filePath);
        $FILENAME = end($file_uri);
        
        islandora_video_streaming_panopto_create_manifest_for_video($filePath, MANIFEST_FILE_NAME);
        islandora_video_streaming_panopto_upload_file($upload_target, MANIFEST_FILE_NAME);
        islandora_video_streaming_panopto_upload_file($upload_target, $filePath);
        islandora_video_streaming_panopto_finish_upload($uploadendpoint, $session, $token);
        
        try {
          $success = monitor_progress($uploadendpoint, $upload_id, $token);
        } catch (Exception $ex) {
          $messages = array(
          'message' => t('Panopto progress error: @exception'),
          'message_sub' => array('@exception' => $ex->getMessage()),
          'type' => 'watchdog',
          'severity' => WATCHDOG_ERROR,
          );
          $failureMessage['messages'][] = $messages;
          watchdog_exception('islandora_video_streaming_panopto', $ex);
          return $failureMessage;
        }
        $streaming['upload_id'] = $session['ID'];
      } catch (Exception $ex) {
        $messages = array(
          'message' => t('Panopto upload error: @exception'),
          'message_sub' => array('@exception' => $ex->getMessage()),
          'type' => 'watchdog',
          'severity' => WATCHDOG_ERROR,
        );
        $failureMessage['messages'][] = $messages;
        watchdog_exception('islandora_video_streaming_panopto', $ex);
      }
    } else {
      $messages = array(
        'message' => t('Failed to retrieve session for object: @pid'),
        'message_sub' => array('@pid' => $object->id),
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      );
      $failureMessage['messages'][] = $messages;
      return $failureMessage;
    }
    file_delete($archival_file['file'], true);
    if ($streaming) {
       // Write the data to a temporary file.
      $temp_name = drupal_tempnam('temporary://', 'file');
      if (file_put_contents($temp_name, json_encode($streaming)) === FALSE) {
        $messages = array(
            'message' => t("Failed to create temporary file @filename for object @pid when uploading to Panopto"),
            'message_sub' => array(
              '@pid' => $object->id, 
              '@filename' => $temp_name
            ),
            'type' => 'watchdog',
            'severity' => WATCHDOG_ERROR,
        );
        $failureMessage['messages'][] = $messages;
        return $failureMessage;
      }
      return islandora_video_add_datastream($object, 'PANOPTO_INFO', $temp_name);
    } else {
      return $failureMessage;
    }
  }
}

/*
 * Requests token and returns token
 * @param String $oauthendpoint 
 */
function islandora_video_streaming_panopto_get_token($oauthendpoint) {
  $client_id = variable_get('islandora_video_streaming_panopto_client_id');
  $secret_client_id = variable_get('islandora_video_streaming_panopto_secret_client_id');
  $username = variable_get('islandora_video_streaming_panopto_username');
  $password = variable_get('islandora_video_streaming_panopto_password');
  
  $response = null;
  $token = null;
  
  if (isset($client_id) && isset($secret_client_id)) {
    $authorization = base64_encode("$client_id:$secret_client_id");
    $headers = array("Authorization: Basic {$authorization}", "Content-Type: application/x-www-form-urlencoded");
    $content = 'grant_type=password&username='.urlencode(strtolower($username)).'&password='.urlencode($password).'&scope=api';
    $response = islandora_video_streaming_panopto_request_curl($oauthendpoint, $headers, $content, 'POST', array(200 => 'access_token'));
  }
  if ($response) {
    $token = $response['access_token'];
  }
  return $token;
}

/*
 * Retrieves session needed for upload
 * @param $token
 * @param $uploadendpoint
 */
function islandora_video_streaming_panopto_get_session($token, $uploadendpoint) {
  $session = null;
  $headers = array("Authorization: Bearer {$token}", "Content-Type: application/json");
  $folder_id = variable_get('islandora_video_streaming_panopto_folder_id');
  $content = json_encode(array(
    'FolderId' => $folder_id,
  ));
  $response = islandora_video_streaming_panopto_request_curl($uploadendpoint, $headers, $content, 'POST', array(201 => 'ID'));
  if ($response) {
    $session = $response;
  }
  return $session;
}

/*
 * @param $endpoint
 * @param $headers
 * @param $content
 * @param $type
 * @param $expect
 */
function islandora_video_streaming_panopto_request_curl($endpoint, $headers, $content = '', $type = 'GET', $expect = array()) {
  $cURL = curl_init();
  curl_setopt($cURL, CURLOPT_URL, $endpoint);
  curl_setopt($cURL, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($cURL, CURLOPT_HEADER, 1);
  curl_setopt($cURL, CURLINFO_HEADER_OUT, true);
  switch (strtoupper($type)) {
    case 'POST':
    curl_setopt($cURL, CURLOPT_POST, true);
    break;
    case 'PUT':
    curl_setopt($cURL, CURLOPT_CUSTOMREQUEST, 'PUT');
    break;
  }
  if ($content) {
    curl_setopt($cURL, CURLOPT_POSTFIELDS, $content);
  }
  curl_setopt($cURL, CURLOPT_HTTPHEADER, $headers);
  $response = curl_exec($cURL);
  $header_size = curl_getinfo($cURL, CURLINFO_HEADER_SIZE);
  $response_code = curl_getinfo($cURL, CURLINFO_RESPONSE_CODE);
  $headerSent = curl_getinfo($cURL, CURLINFO_HEADER_OUT );
  curl_close($cURL);
  if ($response === false) {
    $messages = array(
      'message' => t("Unable to receive response from curl_exec() for object @pid"),
      'message_sub' => array('@pid' => $object->id),
      'type' => 'watchdog',
      'severity' => WATCHDOG_ERROR,
    );
    $failureMessage['messages'][] = $messages;
  } else {
    $headers = substr($response, 0, $header_size);
    $body = substr($response, $header_size);
    if (empty($expect) || (isset($expect[$response_code]) && empty($expect[$response_code]))) {
      return $body;
    } else if (isset($expect[$response_code])) {
      $expectation = $expect[$response_code];
      $json = json_decode($body, true);
      if (is_array($json)) {
        if (isset($json[$expectation])) {
          return $json;
        } else {
          $messages = array(
            'message' => t("For object @pid : JSON key from $endpoint ($expectation) missing . $response"),
            'message_sub' => array('@pid' => $object->id),
            'type' => 'watchdog',
            'severity' => WATCHDOG_ERROR,
          );
          $failureMessage['messages'][] = $messages;
        }
      } else {
        $messages = array(
          'message' => t("For object @pid : Expected JSON key from $endpoint but could not decode. $response"),
          'message_sub' => array('@pid' => $object->id),
          'type' => 'watchdog',
          'severity' => WATCHDOG_ERROR,
        );
        $failureMessage['messages'][] = $messages;
      }
    } else {
      $messages = array(
        'message' => t("For object @pid : Expected one of " .implode(',', array_keys($expect))."; got a $response_code"),
        'message_sub' => array('@pid' => $object->id), 
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      );
      $failureMessage['messages'][] = $messages;
    }
  }
  return;
}

/*
 * Uploads file to Panopto using AWS S3 Uploader
 * @param $upload_target - url of upload target
 * @param $file_path - path of object
 */
function islandora_video_streaming_panopto_upload_file($upload_target, $file_path) {
  $element = explode("/", $upload_target);
  $prefix = array_pop($element);
  $service_endpoint = implode("/", $element);
  $bucket = array_pop($element);
  $file_name = basename($file_path);
  $object_key = $prefix."/".$file_name;
  
  $s3Params = array(
      'endpoint' => $service_endpoint,
      'region'  => 'us-east-1',
      'version' => '2006-03-01',
      'credentials' => [
        'key'    => 'dummy',
        'secret' => 'dummy'
      ]
  );
  // Create an S3Client
  $s3Client = new S3Client($s3Params);

  $source = fopen($file_path, 'rb');
  $uploader = new ObjectUploader(
    $s3Client,
    $bucket,
    $object_key,
    $source
  );

  do {
    try {
      $result = $uploader->upload();
    } catch (Exception $ex) {
      $messages = array(
        'message' => t('S3Client and Uploader error: @exception'),
        'message_sub' => array('@exception' => $ex->getMessage()),
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      );
      $failureMessage['messages'][] = $messages;
      watchdog_exception('islandora_video_streaming_panopto', $ex);
    }
  } while (!isset($result));
}

/*
 * Creates manifest file, in temporary storage, needed for Panopto Video Upload
 * @param $file_path
 * @param $manifest_file_name
 */
function islandora_video_streaming_panopto_create_manifest_for_video($file_path = null, $manifest_file_name=null) {
  $file_name = basename($file_path);

  $template = file_get_contents('https://raw.githubusercontent.com/Panopto/upload-python-sample/master/simplest/upload_manifest_template.xml');
  $template = str_replace("{Title}", $file_name, $template);
  $template = str_replace("{Description}", 'This is a video session with the uploaded video file '.$file_name, $template);
  $template = str_replace("{Filename}", $file_name, $template);
  $template = str_replace("{Date}", date('c'), $template);
  
  $finished = file_put_contents(MANIFEST_FILE_NAME, $template);
}

/*
 * Finishes upload
 * @param $baseUrl
 * @param $session_upload
 * @param $token
 */
function islandora_video_streaming_panopto_finish_upload($baseUrl, $session_upload, $token) {
 $upload_id = $session_upload['ID'];
 $upload_target = $session_upload['UploadTarget'];
  
 $url = $baseUrl."/".$upload_id;
 $payload = $session_upload;
 $payload['State'] = 1;
 $headers = array("Authorization: Bearer {$token}", "Content-Type: application/json");

 $resp = islandora_video_streaming_panopto_request_curl($url, $headers, json_encode($payload), 'PUT');
}

/*
 * Monitors progress made by Uploader
 * @param $baseUrl
 * @param $upload_id
 * @param $token
 */
function monitor_progress($baseUrl, $upload_id, $token) {
  while(1){
      sleep(5);
      $url = $baseUrl."/".$upload_id;
      $headers = array("Authorization: Bearer {$token}", "Content-Type: application/json");
      $resp = islandora_video_streaming_panopto_request_curl($url, $headers);
      $response = json_decode($resp, true);
      error_log(var_export($response, true));
      if ($response['State'] === 0) {
          # If we get Unauthorized and token is refreshed, ignore the response at this time and wait for next time.
          continue;
      }
    break;
  }
  switch ($response['State']) {
    case 1:
    case 3:
    case 4:
      return true;
    default:
      return false;
  }
}