<?php

function islandora_video_streaming_panopto_admin(array $form, array &$form_state) {
  $form['islandora_video_streaming_panopto_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Host'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_host', TRUE),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_username', TRUE),
  
  );
  $form['islandora_video_streaming_panopto_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_password', TRUE),
   
  );
  $form['islandora_video_streaming_panopto_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_client_id', TRUE),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_secret_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret Client ID'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_secret_client_id', TRUE),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_folder_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Folder ID'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_folder_id', TRUE)
  );
  return system_settings_form($form);
}

