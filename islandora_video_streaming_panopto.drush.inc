<?php

/**
 * @file
 * Drush file for the Islandora Video Streaming Panopto module.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_video_streaming_panopto_drush_command() {
  $items = array();
  $items['islandora_video_streaming_panopto_upload'] = array(
    'description' => 'Upload the object specified to Panopto.',
    'examples' => array(
      'drush islandora_video_streaming_panopto_upload --user=admin --object=islandora:12345',
    ),
    'options' => array(
      'object' => array(
        'description' => 'The PID of the objects you want to upload.',
        'required' => TRUE,
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['islandora_video_streaming_panopto_delete'] = array(
    'description' => 'Delete the object from Panopto and remove the PANOPTO_INFO datastream.',
    'examples' => array(
      'drush islandora_video_streaming_panopto_delete --user=admin --object=islandora:12345',
    ),
    'options' => array(
      'object' => array(
        'description' => 'The PID of the object for which the Panopto derivative should be deleted' ,
        'required' => TRUE,
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Uploads the specified object to Panopto, creating the PANOPTO_INFO datastream
 */
function drush_islandora_video_streaming_panopto_upload() {
  module_load_include('inc', 'islandora_video_streaming', 'includes/panopto_derivatives');
  $pid = (drush_get_option('object'));
  // Load the object.
  if (!$object = islandora_object_load($pid)) {
    drush_set_error('IBD_OBJECT_NOT_FOUND',
      dt('The specified object (!pid) does not exist or is not accessible.',
        array('!pid' => $pid)));
    return;
  }
  islandora_video_streaming_panopto_upload($object, true);
}

/**
 * Deletes the specified object to Panopto, removing the PANOPTO_INFO datastream
 */
function drush_islandora_video_streaming_panopto_delete() {
  $pid = (drush_get_option('object'));
  // Load the object.
  if (!$object = islandora_object_load($pid)) {
    drush_set_error('IBD_OBJECT_NOT_FOUND',
      dt('The specified object (!pid) does not exist or is not accessible.',
        array('!pid' => $pid)));
    return;
  }
  $object->purgeDatastream('PANOPTO_INFO');
}
