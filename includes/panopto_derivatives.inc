<?php

/**
 * Implement Panopto video upload  
 * @param AbstractObject $object An Islandora Object
 * @param String $filePath video file path
 */
libraries_load('aws'); //aws info file needs to be created in library and aws 
//library downloaded

use Aws\S3\S3Client;
use Aws\Exception\AwsException;
use Aws\S3\ObjectUploader;
use Aws\S3\MultipartUploader;
use Aws\Exception\MultipartUploadException;

const MANIFEST_FILE_TEMPLATE = 'upload_manifest_template.xml';

# Filename of manifest XML file. Any filename is acceptable.
const MANIFEST_FILE_NAME = 'upload_manifest_generated.xml';

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
      'messages' => array(
        'message' => t('Failed to upload object @pid to Panopto, but no exception caught.'),
        'message_sub' => array('@pid' => $object->id),
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      ),
    );
    $streaming = array();
    //TODO: upload, record result in $streaming
    //1.Get token
    $oauthendpoint = variable_get('islandora_video_streaming_panopto_host', TRUE);
    $uploadendpoint = $oauthendpoint . '/Panopto/PublicAPI/Rest/sessionUpload';
    $oauthendpoint .= "/Panopto/oauth2/connect/token";
    $token = islandora_video_streaming_panopto_get_token($oauthendpoint);
    //2. Get session
    $session = null;
    if ($token) {
      $session = islandora_video_streaming_panopto_get_session($token, $uploadendpoint); 
      dpm('$token exists');
    }
    else {
        error_log("token does not exist");
    }
    //3.Use S3 uploader
    if ($session) {
      dpm('$session exists');
      try {
        $upload_id = $session['ID'];
        $upload_target = $session['UploadTarget'];
        $FILEPATH = $filePath;
        upload_file($upload_target, $FILEPATH);
        $template = create_manifest_for_video($FILEPATH, MANIFEST_FILE_NAME);
        upload_manifest_file($upload_target, $template);
        
        finish_upload($uploadendpoint, $session, $token);
        
        $streaming['upload_id'] = $session['ID'];
      } catch (Exception $ex) {

      }
    }
    
    if ($streaming) {
       // Write the data to a temporary file.
      $temp_name = drupal_tempnam('temporary://', 'file');
      if (file_put_contents($temp_name, json_encode($streaming)) === FALSE) {
        $failureMessage['messages'][0]['message'] = t('Failed to create temporary file @filename for object @pid when uploading to Panopto.');
        $failureMessage['messages'][0]['message_sub']['@filename'] = $temp_name;
        return $failureMessage;
      }
      return islandora_video_add_datastream($object, 'PANOPTO_INFO', $temp_name);
    } else {
      return $failureMessage;
    }
  }
}

/*
 * Requests token and hopefully returns token
 * @param String $oauthendpoint 
 */
function islandora_video_streaming_panopto_get_token($oauthendpoint) {
  $client_id = variable_get('islandora_video_streaming_panopto_client_id', TRUE);
  $secret_client_id = variable_get('islandora_video_streaming_panopto_secret_client_id', TRUE);
  $username = variable_get('islandora_video_streaming_panopto_username', TRUE);
  $password = variable_get('islandora_video_streaming_panopto_password', TRUE);
  
  $response = null;
  $token = null;
  
  if (isset($client_id) && isset($secret_client_id)) {
    $authorization = base64_encode("$client_id:$secret_client_id");
    $headers = array("Authorization: Basic {$authorization}", "Content-Type: application/x-www-form-urlencoded");
    $content = 'grant_type=password&username='.urlencode(strtolower($username)).'&password='.urlencode($password).'&scope=api';
    $response = request_curl($oauthendpoint, $headers, $content, 'POST', array(200 => 'access_token'));
  }
  if ($response) {
    $token = $response['access_token'];
  }
  return $token;
}

/*
 * Gets session needed for upload
 * @param $token
 * @param $uploadendpoint
 */
function islandora_video_streaming_panopto_get_session($token, $uploadendpoint) {
  $session = null;
  $headers = array("Authorization: Bearer {$token}", "Content-Type: application/json");
  $folder_id = variable_get('islandora_video_streaming_panopto_folder_id', TRUE);
  $content = json_encode(array(
    'FolderId' => $folder_id,
  ));
  $response = request_curl($uploadendpoint, $headers, $content, 'POST', array(201 => 'ID'));
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
function request_curl($endpoint, $headers, $content = '', $type = 'GET', $expect = array()) {
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
  error_log('Sent:'."\n".$headerSent."\n".$content."\n");
  curl_close($cURL);
  if ($response === false) {
    dpm('curl for '.$endpoint.' failed: '.curl_error($cURL));
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
          //change this to failureMessage
          dpm('JSON key from '.$endpoint.' ('.$expectation.') missing: '.$response);
        }
      } else {
        dpm('Expected JSON from '.$endpoint.', but could not decode: '.$response);
      }
    } else {
      dpm('Expected one of '.implode(',', array_keys($expect)).'; got a '.$response_code);
    }
  }
  return;
}

/*
 * Uploads file to Panopto using AWS S3 Uploader
 * @param $upload_target - url of upload target
 * @param $file_path - path of object
 */
function upload_file($upload_target, $file_path) {
  $element = explode("/", $upload_target);
  $prefix = array_pop($element);
  $service_endpoint = implode("/", $element);
  $bucket = array_pop($element);
  $object_key = $prefix."/".$file_path;
  
  $s3Client = new S3Client([
      'endpoint' => $service_endpoint,
      'region'  => 'us-east-1',
      'version' => '2006-03-01',
      'credentials' => [
        'key'    => 'dummy',
        'secret' => 'dummy'
    ]
  ]);
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
      if ($result["@metadata"]["statusCode"] == '200') {
        dpm('<p>File successfully uploaded to ' . $result["ObjectURL"] . '.</p>');
      }
      dpm($result);
    } catch (Exception $ex) {
      rewind($source);
          $uploader = new MultipartUploader($s3Client, $source, [
              'state' => $ex->getState(),
          ]);
    }
  } while (!isset($result));
}

/*
 * Manifest file creation needed for Panopto Video Upload
 * @param $file_path
 * @param $manifest_file_name
 */
function create_manifest_for_video($file_path = null, $manifest_file_name=null) {
  $file_name = basename($file_path);

  $template = file_get_contents('https://raw.githubusercontent.com/Panopto/upload-python-sample/master/simplest/upload_manifest_template.xml');
  $template = str_replace("{Title}", $file_name, $template);
  $template = str_replace("{Description}", 'This is a video session with the uploaded video file '.$file_name, $template);
  $template = str_replace("{Filename}", $file_path, $template);
  $template = str_replace("{Date}", date('Y-m-d H:i:s'), $template);
  
  $finished = 'Finished generated .xml file';
  file_put_contents($manifest_file_name, $template);
  return $template;
}

/*
 * Finishes upload
 * @param $baseUrl
 * @param $session_upload
 * @param $token
 */
function finish_upload($baseUrl, $session_upload, $token) {
 $upload_id = $session_upload['ID'];
 $upload_target = $session_upload['UploadTarget'];
  
 $url = $baseUrl."/".$upload_id;
 $payload = $session_upload;
 $payload['State'] = 1;
 $headers = array("Authorization: Bearer {$token}", "Content-Type: application/json");


 $resp = request_curl($url, $headers, json_encode($payload), 'PUT');
}


/*
 * Uploads manifest file 
 * @param $UploadTarget 
 * @param $file_path
 */
function upload_manifest_file($UploadTarget, $file_path) {
  $element = explode("/", $UploadTarget);
  $prefix = array_pop($element);
  $service_endpoint = implode("/", $element);
  $bucket = array_pop($element);
  $object_key = $prefix."/".MANIFEST_FILE_NAME;
  

  // Create an S3Client
  $s3Client = new S3Client([
      'endpoint' => $service_endpoint,
      'region'  => 'us-east-1',
      'version' => '2006-03-01',
      'credentials' => [
        'key'    => 'dummy',
        'secret' => 'dummy'
    ]
  ]);

  // Use multipart upload
  $source = $file_path;//$file_path;//'earth.mp4';//'/path/to/large/file.zip';
  $uploader = new ObjectUploader(
    $s3Client,
    $bucket,
    $object_key,
    $source
  );

  do {
      try {
          $result = $uploader->upload();
          if ($result["@metadata"]["statusCode"] == '200') {
              dpm('<p>File successfully uploaded to ' . $result["ObjectURL"] . '.</p>');
          }
          echo($result["@metadata"]["statusCode"]);
      } catch (MultipartUploadException $e) {
          rewind($source);
          $uploader = new MultipartUploader($s3Client, $source, [
              'state' => $e->getState(),
          ]);
      }
  } while (!isset($result));
}
