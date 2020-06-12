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
    $old_content= null; 
    $old_entity_id = null;
    //if an existing object['KALTURA_STREAM'] is present, delete existing kaltura
      //video and media entry. 
    if (isset($object['KALTURA_STREAM'])) {
      //fetch actual datastream content FROM islandora
      $old_content = $object['KALTURA_STREAM']->content;
      //if $old_content exists then delete it from Kaltura server
      if (isset($old_content)) {
        //decode $old_content for entry_id
        $decoded = json_decode($old_content, true);
          //check if json is valid
        if ($decoded !== null) {
          $entity = kaltura_entry_load($decoded['entry_id']);
          if ($entity !== False) {
            $old_entity_id = entity_id('kaltura_entry', $entity);
            kaltura_entry_delete($old_entity_id);
          }
        }
      }
    }
    $filePath = drupal_realpath($archival_file['file']->uri); //copy of datastream
    drupal_load('module', 'kaltura');
    $failureMessage = array(
      'success' => false,
      'messages' => array(),
    );
    $streaming = array();
     // See: kaltura/kaltura_upload.inc kaltura_uploader_form_submit
    try {
      $helpers = new KalturaHelpers();
      $client = $helpers->getKalturaClient();
      if (isset($old_content) && isset($old_entity_id)) {
        try {
          $myService = new KalturaBaseEntryService($client);
          $myService->delete($old_entity_id);
        } catch (Exception $ex) {
          $messages = array(
            'message' => t('KalturaBaseEntry error: @exception'),
            'message_sub' => array('@exception' => $ex->getMessage()),
            'type' => 'watchdog',
            'severity' => WATCHDOG_ERROR,
          );
          $failureMessage['messages'][] = $messages;
          watchdog_exception('islandora_video_streaming_kaltura', $ex);
        }
      }
      $entry = new KalturaMediaEntry();
      $entry->name = pathinfo($filePath, PATHINFO_FILENAME);
      $entry->mediaType = kaltura_get_media_type_by_mime(file_get_mimetype($filePath)); //original datafile
      //returns token of video and uploads to kaltura
      $token = $client->media->upload(drupal_realpath($filePath));
      $entry = $client->media->addFromUploadedFile($entry, $token);

      $entity = kaltura_entry_create(array('kaltura_entryid' => $entry->id));
      kaltura_map_properties($entry, $entity);
      $entity->kaltura_title = $object->id; 
      $entity->kaltura_description = $object->label;
      
      kaltura_entry_save($entity); //write entity to drupal db
    
      try {
        $kaltura_baseEntry = new KalturaBaseEntry();
        $kaltura_baseEntry->name = $object->id;
        $kaltura_baseEntry->description = $object->label;
        $updated_kaltura = $client->baseEntry->update($entry->id, $kaltura_baseEntry);
      } catch (Exception $e) {
        $messages = array(
          'message' => t('Failed to edit metadata of object @pid in Kaltura'),
          'message_sub' => array('@pid' => $object->id),
          'type' => 'watchdog',
          'severity' => WATCHDOG_ERROR,
        );
        $failureMessage['messages'][] = $messages;
      }
      
      $streaming['entry_id'] = $entry->id;
    } catch (Exception $e) {
      $messages = array(
        'message' => t('Failed to upload object @pid to Kaltura, exception: @exception'),
        'message_sub' => array(
          '@pid' => $object->id,
          '@exception' => $e->getMessage(),
        ),
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      );
      $failureMessage['messages'][] = $messages;
      watchdog_exception('islandora_video_streaming_kaltura', $e);
    }
      //Deletes tmp file
    file_delete($archival_file['file'], true);
    if ($streaming) {
      // Write the data to a temporary file.
      $temp_name = drupal_tempnam('temporary://', 'file');
      if (file_put_contents($temp_name, json_encode($streaming)) === FALSE) {
        $messages = array(
          'message' => t('Failed to create temporary file @filename for object @pid when uploading to Kaltura.'),
          'message_sub' => array(
            '@pid' => $object->id,
            '@filename' => $temp_name,
          ),
          'type' => 'watchdog',
          'severity' => WATCHDOG_ERROR,
        );
        $failureMessage['messages'][] = $messages;
        return $failureMessage;
      }
      $result = islandora_video_add_datastream($object, 'KALTURA_STREAM', $temp_name);
      if ($result['success'] == TRUE){ 
          $ds = $object['KALTURA_STREAM'];
          $ds->mimetype = "application/json"; //string literal
      }
      return $result;
    } else {
      return $failureMessage;
    }
  }
}
