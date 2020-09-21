<?php

libraries_load('php-panopto-upload-api');

/**
 * Implement Panopto video upload  
 * @param AbstractObject $object An Islandora Object
 * @param boolean $force 
 * @return array An array that describes the success or failure of the upload 
 */
function islandora_video_streaming_panopto_upload($object, $force) {
  if ($force || !isset($object['PANOPTO_INFO'])) {
    module_load_include('inc', 'islandora_video', "includes/derivatives" );
    $archival_file = islandora_video_copy_obj_datastream($object);
    if ($archival_file === FALSE) {
      return islandora_video_no_obj_datastream($object->id);
    }
    $file_path = drupal_realpath($archival_file['file']->uri);
    $failureMessage = array(
      'success' => false,
      'messages' => array(),
    );
    $streaming = array();
   
    $manifest_file = islandora_video_streaming_panopto_create_manifest_for_video($file_path);

    $folder_id = variable_get('islandora_video_streaming_panopto_folder_id');
    $host = variable_get('islandora_video_streaming_panopto_host');
    $client = new \PanoptoUpload\Client($host);
    $client_id = variable_get('islandora_video_streaming_panopto_client_id');
    $secret_client_id = variable_get('islandora_video_streaming_panopto_secret_client_id');
    $username = variable_get('islandora_video_streaming_panopto_username');
    $password = variable_get('islandora_video_streaming_panopto_password');
    $client->authenticate($client_id, $secret_client_id, $username, $password);
    try {
      $session = $client->newSession($folder_id);
      $session->uploadFile($manifest_file);
      $session->uploadFile($file_path);
      $client->finishSession($session);
      // have to call the get status endpoint to populate the SessionId
      while(true) {
        $status = $client->getSessionStatus($session);
        if ($status > 1) {
          break;
        } else {
          sleep(5);
        }
      }
      $streaming['upload_id'] = $session->sessionId();
    }catch (Exception $ex) {
      $messages = array(
        'message' => t('Panopto upload error: @exception for object @pid'),
        'message_sub' => array(
          '@exception' => $ex->getMessage(),
          '@pid' => $object->id,
        ),
        'type' => 'islandora_video_streaming_panopto',
        'severity' => WATCHDOG_ERROR,
      );
      $failureMessage['messages'][] = $messages;
      watchdog_exception('islandora_video_streaming_panopto', $ex);
    }    

    drupal_unlink($manifest_file);    
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
            'type' => 'islandora_video_streaming_panopto',
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

/**
 * Creates manifest file, in temporary storage, needed for Panopto Video Upload
 * @param string $file_path The filepath of the Islandora object
 * @return string The filename or filepath of the newly created manifest file
 */
function islandora_video_streaming_panopto_create_manifest_for_video($file_path) {
  $file_name = basename($file_path);
  $manifest_template = drupal_get_path('module','islandora_video_streaming') . DIRECTORY_SEPARATOR . 'includes' . DIRECTORY_SEPARATOR . 'upload_manifest_template.xml';
  $template = file_get_contents($manifest_template);
  $template = str_replace("{Title}", $file_name, $template);
  $template = str_replace("{Description}", 'This is a video session with the uploaded video file '.$file_name, $template);
  $template = str_replace("{Filename}", $file_name, $template);
  $template = str_replace("{Date}", date('c'), $template);
  
  $part_of_file_name = explode(".", $file_name);
  $updated_manifest_file = tempnam('temporary://', $part_of_file_name[0]);
  rename($updated_manifest_file, $updated_manifest_file .= '.xml');
  file_put_contents($updated_manifest_file, $template);

  return $updated_manifest_file;
}
