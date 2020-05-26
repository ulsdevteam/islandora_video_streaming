<?php

/**
 * Implement Panopto video upload  
 * @param AbstractObject $object An Islandora Object
 * @param String $filePath video file path
 * 
 * [5/19/20, 11:21:37 AM] Graham, Clinton T: Client ID is:
3941e4a4-bba8-46e3-a113-aaf600fd8ca3
[5/19/20, 11:21:52 AM] Graham, Clinton T: Secret is:
kukfAxIeR3WnnitXhZKduDyGGYUY1QskJEbEk87B3zQ=
 */
function islandora_video_streaming_panopto_upload($object, $filePath) {
  if ($filePath || !isset($object['PANOPTO_INFO'])) {
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
    
    //TODO: upload, record result in $streaming
    //1.Get token
    $oauthendpoint = variable_get('islandora_video_streaming_panopto_host', TRUE);
    $oauthendpoint .= "/Panopto/oauth2/connect/token";
    $token = islandora_video_streaming_panopto_get_token($oauthendpoint);
    //2. Get session
    $uploadendpoint = $oauthendpoint . '/Panopto/PublicAPI/Rest/sessionUpload';
    
    $session = null;
    if ($token) {
      $session = islandora_video_streaming_panopto_get_session($token, $uploadendpoint);
    }
    //3.Use S3 uploader
    
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
 * Requests token 
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
    $authorization = base64_encode("$client_id:$client_secret");
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