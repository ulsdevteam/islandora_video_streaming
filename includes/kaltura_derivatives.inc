<?php

/**
 * Implement Kaltura video upload  
 * @param AbstractObject $object An Islandora Object
 */
function islandora_video_streaming_kaltura_upload($object, $force = false) {
  if ($force || !isset($object['KALTURA_STREAM'])) {
    $archival_file = islandora_video_copy_obj_datastream($object);
    if ($archival_file === FALSE) {
      return islandora_video_no_obj_datastream($object->id);
    }
    $filePath = drupal_realpath($archival_file['file']->uri);
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
      $entry = new KalturaMediaEntry();
      $entry->name = pathinfo($filePath, PATHINFO_FILENAME);
      $entry->mediaType = kaltura_get_media_type_by_mime(file_get_mimetype($filePath));

      $token = $client->media->upload(drupal_realpath($filePath));
      $entry = $client->media->addFromUploadedFile($entry, $token);

      $entity = kaltura_entry_create(array('kaltura_entryid' => $entry->id));
      kaltura_map_properties($entry, $entity);
      // TODO: duplicate metadata via kaltura_save_entry_metadata() ?
      kaltura_entry_save($entity);
      $streaming['entry_id'] = $entry->id;

    }
    catch (Exception $e) {
      $failureMessage['messages'][0]['message'] = t('Failed to upload object @pid to Kaltura, exception: @exception');
      $failureMessage['messages'][0]['message_sub']['@exception'] = $e->getMessage();
      watchdog_exception('islandora_video_streaming_kaltura', $e);
    }

    if ($streaming) {
      // Write the data to a temporary file.
      $temp_name = drupal_tempnam('temporary://', 'file');
      if (file_put_contents($temp_name, json_encode($streaming)) === FALSE) {
        $failureMessage['messages'][0]['message'] = t('Failed to create temporary file @filename for object @pid when uploading to Kaltura.');
        $failureMessage['messages'][0]['message_sub']['@filename'] = $temp_name;
        return $failureMessage;
      }
      return islandora_video_add_datastream($object, 'KALTURA_STREAM', $temp_name);
    } else {
      return $failureMessage;
    }
  }
}

