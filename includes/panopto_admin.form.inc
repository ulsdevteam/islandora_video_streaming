<?php
/*
 * Defines the admin settings form.
 * 
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   The Drupal form definition.
 */
function islandora_video_streaming_panopto_admin(array $form, array &$form_state) {
  $form['islandora_video_streaming_panopto_host_information'] = array(
    '#type' => 'fieldset',
    '#title' => t('Host and Login Information'),
  );
  $form['islandora_video_streaming_panopto_host_information']['islandora_video_streaming_panopto_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Host'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_host'),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_host_information']['islandora_video_streaming_panopto_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_username'),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_host_information']['islandora_video_streaming_panopto_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_password'),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_client_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Client Information'),
    '#description' => t('Client ID and Client Secret can be found or generated through API Clients 
      in User Settings (click on the arrow below User name on top right of webpage).'),
  );
  $form['islandora_video_streaming_panopto_client_fieldset']['islandora_video_streaming_panopto_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_client_id'),
    '#required' => TRUE,
  );
  $form['islandora_video_streaming_panopto_client_fieldset']['islandora_video_streaming_panopto_secret_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Secret'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_secret_client_id'),
    '#required' => TRUE
  );
  $form['islandora_video_streaming_panopto_folder_id_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Folder ID'),
    '#description' => t('Folder ID can be found through the Manage tab in Settings, must be in the specified
      folder.'), 
  );
  $form['islandora_video_streaming_panopto_folder_id_fieldset']['islandora_video_streaming_panopto_folder_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Folder ID'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_folder_id'),
    '#required' => TRUE,
  );
  $form['islandora_video_streaming_panopto_video_information'] = array(
    '#type' => 'fieldset',
    '#title' => t('Video Information'),
    '#description' => t('Information about video options can be found here: https://support.panopto.com/s/article/Embed-a-Video'),
  );
  $form['islandora_video_streaming_panopto_video_information']['islandora_video_streaming_panopto_video_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_video_height'),
    '#required' => TRUE,
  );
  $form['islandora_video_streaming_panopto_video_information']['islandora_video_streaming_panopto_video_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_video_width'),
    '#required' => TRUE,
  );
  $form['islandora_video_streaming_panopto_video_information']['islandora_video_streaming_panopto_video_start_at'] = array(
    '#type' => 'textfield',
    '#title' => t('Start At (HH:mm:ss, hours and minutes optional)'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_video_start_at'),
  );
  $form['islandora_video_streaming_panopto_video_information']['islandora_video_streaming_panopto_video_interactivity'] = array(
    '#type' => 'radios',
    '#title' => t('Interactivity'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_video_interactivity'),
    '#options' => array(
      'all' => t('All'),
      'search' => t('Search'),
      'none' => t('None')
    ),
    '#required' => TRUE,
  );
  $form['islandora_video_streaming_panopto_video_information']['islandora_video_streaming_panopto_video_autoplay'] = array(
    '#type' => 'radios',
    '#title' => t('Autoplay'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_video_autoplay'),
    '#options' => array(
      'true' => t('True'),
      'false' => t('False'),
    ),
    '#required' => TRUE,
  );
  $form['islandora_video_streaming_panopto_video_information']['islandora_video_streaming_panopto_video_offer_viewer'] = array(
    '#type' => 'radios',
    '#title' => t('Offer Viewer'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_video_offer_viewer'),
    '#options' => array(
      'true' => t('True'),
      'false' => t('False'),
    ),
    '#required' => TRUE,
  );
  $form['islandora_video_streaming_panopto_video_information']['islandora_video_streaming_panopto_video_showtitle'] = array(
    '#type' => 'radios',
    '#title' => t('Show Title'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_video_showtitle'),
    '#options' => array(
      'true' => t('True'),
      'false' => t('False'),
    ),
    '#required' => TRUE,
  );
  $form['islandora_video_streaming_panopto_video_information']['islandora_video_streaming_panopto_video_showbrand'] = array(
    '#type' => 'radios',
    '#title' => t('Show Brand'),
    '#default_value' => variable_get('islandora_video_streaming_panopto_video_showbrand'),
    '#options' => array(
      'true' => t('True'),
      'false' => t('False'),
    ),
    '#required' => TRUE,
  );
  
  
  
  return system_settings_form($form);
}

