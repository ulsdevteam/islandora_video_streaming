<?php

function islandora_video_streaming_panopto_admin(array $form, array &$form_state) {
  $form['client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => variable_get('client_id, TRUE'),
    '#required' => TRUE
  );
  $form['secret_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret Client ID'),
    '#default_value' => variable_get('secret_client_id, TRUE'),
    '#required' => TRUE
  );
  $form['folder_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Folder ID'),
    '#default_value' => variable_get('folder_id, TRUE')
  );
  return system_settings_form($form);
}

