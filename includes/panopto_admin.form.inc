<?php
/*
 * Defines the admin settings form.
 * 
 * Client id and client secret can be found or generated through API Clients 
 * in User Settings (click on the arrow below User name on top right of webpage).
 * 
 * Folder ID can be found through the Manage tab in Settings, must be in the specified
 * folder. 
 * 
 * To generate a folder ID, create a new folder. Follow previous steps for
 * finding the folder ID for the new folder.
 * 
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   The Drupal form definition.
 */
function islandora_video_streaming_panopto_admin(array $form, array &$form_state) {
  $form['islandora_video_streaming_panopto_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Host'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_host'),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_username'),
  
  );
  $form['islandora_video_streaming_panopto_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_password'),
   
  );
  $form['islandora_video_streaming_panopto_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_client_id'),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_secret_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Secret'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_secret_client_id'),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_folder_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Folder ID'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_folder_id'),
    '#required' => TRUE
  );
  return system_settings_form($form);
}

