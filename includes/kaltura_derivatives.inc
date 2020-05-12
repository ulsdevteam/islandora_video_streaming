<?php

/**
 * Implement Kaltura video upload  
 * @param AbstractObject $object An Islandora Object
 */
function islandora_video_streaming_kaltura_upload($object, $force = false) {
    //If the object's KALTURA_STREAM is null then we add it into the file system
    //using islandora_video_copy_obj_datastream
    if ($force || !isset($object['KALTURA_STREAM'])) {
    $archival_file = islandora_video_copy_obj_datastream($object);
    if ($archival_file === FALSE) {
      return islandora_video_no_obj_datastream($object->id);
    }
    $filePath = drupal_realpath($archival_file['file']->uri); //copy of datastream
    drupal_load('module', 'kaltura');
    $failureMessage = array(
      'success' => false,
      'messages' => array(
        'message' => t('Failed to upload object @pid to Kaltura, but no exception caught.'),
        'message_sub' => array('@pid' => $object->id),
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      ),
    );
    $streaming = array();
    // See: kaltura/kaltura_upload.inc kaltura_uploader_form_submit
    try {
      $helpers = new KalturaHelpers();
      $client = $helpers->getKalturaClient();

      // TODO: would it be better or worse to skip the KalturaMediaEntry ?
      //It would be worse? Seems like everything is reliant on entry id and
      //Token does not contain entry id, it contains only a string.
      $entry = new KalturaMediaEntry();
      $entry->name = pathinfo($filePath, PATHINFO_FILENAME);
      $entry->mediaType = kaltura_get_media_type_by_mime(file_get_mimetype($filePath)); //original datafile

      //returns token of video and uploads to kaltura
      $token = $client->media->upload(drupal_realpath($filePath));
      $entry = $client->media->addFromUploadedFile($entry, $token);

      $entity = kaltura_entry_create(array('kaltura_entryid' => $entry->id));
      kaltura_map_properties($entry, $entity);
            
      kaltura_save_entry_metadata($entry);
      kaltura_entry_save($entity); //write entity to drupal db
      
      $streaming['entry_id'] = $entry->id;
    }
    catch (Exception $e) {
      $failureMessage['messages'][0]['message'] = t('Failed to upload object @pid to Kaltura, exception: @exception');
      $failureMessage['messages'][0]['message_sub']['@exception'] = $e->getMessage();
      watchdog_exception('islandora_video_streaming_kaltura', $e);
    }
        
    //Deletes tmp file
    file_delete($archival_file['file'], true);
    
    if ($streaming) {
      // Write the data to a temporary file.
      $temp_name = drupal_tempnam('temporary://', 'file');
      if (file_put_contents($temp_name, json_encode($streaming)) === FALSE) {
        $failureMessage['messages'][0]['message'] = t('Failed to create temporary file @filename for object @pid when uploading to Kaltura.');
        $failureMessage['messages'][0]['message_sub']['@filename'] = $temp_name;
        return $failureMessage;
      }
      $arr = islandora_video_add_datastream($object, 'KALTURA_STREAM', $temp_name);
      
      if ($arr['success'] == TRUE){ 
          $ds = $object['KALTURA_STREAM'];
          $ds->mimetype = "application/json"; //string literal
      }
      return $arr;
    } else {
      return $failureMessage;
    }
  }
}

