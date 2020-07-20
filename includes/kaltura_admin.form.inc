<?php
/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_video_streaming_kaltura_admin(array $form, array &$form_state) {
  $fields = field_read_fields(array('type' => 'field_kaltura_entryid'));
  $form = array();
  $form['fieldselect'] = array(
    '#type' => 'fieldset',
    '#title' => t('Kalutra field'),
    'islandora_video_streaming_kaltura_field' => array(
      '#type' => 'select',
      '#options' => array_combine(array_keys($fields), array_keys($fields)),
      '#empty_value' => '',
      '#title' => t('The Kaltura-managed field'),
      '#description' => t('You must select the field which has been configured in the Manage Fields tab of the Kaltura settings which has the configuration you want to use for display within Islandora'),
      '#default_value' => variable_get('islandora_video_streaming_kaltura_field'),
    ),
  );
  return system_settings_form($form);
}
