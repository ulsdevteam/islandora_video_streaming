<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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
function islandora_video_streaming_panopto_admin(array $form, array &$form_state) {
  $fields = field_read_fields(array('type' => 'field_panopto_entryid'));
  $form = array();
  $form['fieldselect'] = array(
    '#type' => 'fieldset',
    '#title' => t('Panopto field'),
    'islandora_video_streaming_panopto_field' => array(
      '#type' => 'select',
      '#options' => array_combine(array_keys($fields), array_keys($fields)),
      '#empty_value' => '',
      '#title' => t('The Panopto-managed field'),
      '#description' => t('You must select the field which has been configured in the Manage Fields tab of the Panopto settings which has the configuration you want to use for display within Islandora'),
      '#default_value' => variable_get('islandora_video_streaming_panopto_field'),
    ),
  );
  return system_settings_form($form);
}
