<?php

/**
 * Implement Panopto video upload  
 * @param AbstractObject $object An Islandora Object
 * @param String $filePath video file path
 */
function islandora_video_streaming_panopto_upload($object, $filePath) {
  if ($force || !isset($object['KALTURA_STREAM'])) {
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
 
    if ($streaming) {
       // Write the data to a temporary file.
      $temp_name = drupal_tempnam('temporary://', 'file');
      if (file_put_contents($temp_name, json_encode($streaming)) === FALSE) {
        $failureMessage['messages'][0]['message'] = t('Failed to create temporary file @filename for object @pid when uploading to Panopto.');
        $failureMessage['messages'][0]['message_sub']['@filename'] = $temp_name;
        return $failureMessage;
      }
      return islandora_video_add_datastream($object, 'KALTURA_STREAM', $temp_name);
    } else {
      return $failureMessage;
    }
  }
}

