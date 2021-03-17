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
    $transcript_path = '';
    $transcript_file = '';
    if (isset($object['TRANSCRIPT'])) {
      $transcript_path = drupal_tempnam('temporary://', 'transcript');
      $object['TRANSCRIPT']->getContent($transcript_path);
      $transcript_file = basename($transcript_path);
      // Panopto will choke on valid WebVTT files because of the header
      if ($object['TRANSCRIPT']->mimetype === 'text/vtt') {
          // Remove the WEBVTT header and any empty leading lines
          $vtt = file_get_contents($transcript_path);
          $vtt = preg_replace('/^WEBVTT.*[\r\n]+/', '', $vtt, 1);
          file_put_contents($transcript_path, $vtt);
      }
    }
    $failureMessage = array(
      'success' => false,
      'messages' => array(),
    );
    $streaming = array();
  
    $manifest_file = islandora_video_streaming_panopto_create_manifest_for_video($file_path, $object->label, $object->id, $transcript_file);

    $folder_id = variable_get('islandora_video_streaming_panopto_folder_id');
    $host = variable_get('islandora_video_streaming_panopto_host');
    $client = new \PanoptoUpload\Client($host);
    $client_id = variable_get('islandora_video_streaming_panopto_client_id');
    $secret_client_id = variable_get('islandora_video_streaming_panopto_secret_client_id');
    $username = variable_get('islandora_video_streaming_panopto_username');
    $password = variable_get('islandora_video_streaming_panopto_password');
    try {
      $client->authenticate($client_id, $secret_client_id, $username, $password);
      $session = $client->newSession($folder_id);
      $session->uploadFile($manifest_file);
      $session->uploadFile($file_path);
      if ($transcript_path) {
        $session->uploadFile($transcript_path);
      }
      $client->finishSession($session);
      // have to call the get status endpoint to populate the SessionId
      while (true) {
        $status = $client->getSessionStatus($session);
        if ($status > 1) {
          break;
        } else {
          sleep(5);
        }
      }
      // TODO: somehow $session->sessionId() can be empty, with no exceptions thrown
      $streaming['upload_id'] = $session->sessionId();
    } catch (Exception $ex) {
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
    if ($transcript_path) {
      drupal_unlink($transcript_path);
    }
    drupal_unlink($manifest_file);    
    file_delete($archival_file['file'], true);
    if ($streaming) {
       // Write the data to a temporary file.
      $temp_name = drupal_tempnam('temporary://', 'stream');
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
      $result = islandora_video_add_datastream($object, 'PANOPTO_INFO', $temp_name);
      if ($result['success'] && $object['PANOPTO_INFO']->mimetype === 'application/octet-stream') {
          $object['PANOPTO_INFO']->mimetype = 'application/json';
      }
      return $result;
    } else {
      return $failureMessage;
    }
  }
}

/**
 * Creates manifest file, in temporary storage, needed for Panopto Video Upload
 * @param string $file_path The filepath of the video
 * @param string $title The title of the video
 * @param string $description The description of the video (optional)
 * @param string $transcript_file The filename of the transcript (optional)
 * @return string The filename or filepath of the newly created manifest file
 */
function islandora_video_streaming_panopto_create_manifest_for_video($file_path, $title, $description = '', $transcript_file = '') {
  $file_name = basename($file_path);
  $manifest_template = drupal_get_path('module','islandora_video_streaming') . DIRECTORY_SEPARATOR . 'includes' . DIRECTORY_SEPARATOR . 'upload_manifest_template.xml';
  $template = file_get_contents($manifest_template);
  $template = str_replace("{Title}", htmlentities($title), $template);
  $template = str_replace("{Description}", htmlentities($description ? $description : $title), $template);
  $template = str_replace("{Filename}", htmlentities($file_name), $template);
  $template = str_replace("{Date}", date('c'), $template);
  if ($transcript_file) {
    $template = str_replace("</Video>", '<Transcripts><Transcript><File>'.htmlentities($transcript_file).'</File></Transcript></Transcripts>'."\n".'</Video>', $template);
  }
  $updated_manifest_file = drupal_tempnam('temporary://', 'manifest');
  rename($updated_manifest_file, $updated_manifest_file .= '.xml');
  file_put_contents($updated_manifest_file, $template);

  return $updated_manifest_file;
}
