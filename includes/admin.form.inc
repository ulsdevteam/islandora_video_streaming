<?php

/**
 * Helper function to provide form components
 * @return array Drupal form definition
**/
function islandora_video_streaming_viewer_form($variable_id = NULL, $mimetype = NULL, $model = NULL) {
    $form = array();
    $viewers = islandora_get_viewers($mimetype, $model);
    if (!empty($viewers)) {
        $no_viewer = array();
        $no_viewer['none'] = array(
            'label' => t('None'),
            'description' => t("Don't use a viewer for this solution pack."),
        );
        $viewers = array_merge_recursive($no_viewer, $viewers);
    }

    $viewers_config = variable_get($variable_id, array());
    $form['streaming_viewers'] = array(
        '#type' => 'fieldset',
        '#title' => t('Video Streaming Service'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
    );

    if (!empty($viewers)) {
        $form['streaming_viewers'][$variable_id] = array(
            '#type' => 'item',
            '#title' => t('Select a streaming service'),
            '#description' => t('Preferred streaming service which provided by third-party modules for your solution pack.'),
            '#tree' => TRUE,
            '#theme' => 'islandora_viewers_table',
        );

        foreach ($viewers as $name => $profile) {
            $options[$name] = '';
            $form['streaming_viewers'][$variable_id]['name'][$name] = array(
                '#type' => 'hidden',
                '#value' => $name,
            );
            $form['streaming_viewers'][$variable_id]['label'][$name] = array(
                '#type' => 'item',
                '#markup' => $profile['label'],
            );
            $form['streaming_viewers'][$variable_id]['description'][$name] = array(
                '#type' => 'item',
                '#markup' => $profile['description'],
            );
        }
        $form['streaming_viewers'][$variable_id]['default'] = array(
            '#type' => 'radios',
            '#options' => isset($options) ? $options : array(),
            '#default_value' => !empty($viewers_config) ? $viewers_config['default'] : 'none',
        );
    } else {
        $form['streaming_viewers'][$variable_id . '_no_viewers'] = array(
            '#markup' => t('No viewers detected.'),
        );
        variable_del($variable_id);
    }
    return $form;
}
/**
 * Helper function to provide form components
 * @return array Drupal form definition
**/
function islandora_video_streaming_settings_form() {
  $variable_id = 'islandora_video_streaming_settings';
  $viewers = array();
  $options = [
      'kaltura' => ['label' => 'Kaltura', 'description' => 'Use Kaltura and override the viewer'],
      'panopto' => ['label' => 'Panopto', 'description' => 'Use Panopto and override the viewer'],
      'none' => ['label' => 'None', 'description' => 'Use the default viewer setting.']
      ];

  foreach ($options as $key => $value) {
    $option = array();
    $option[$key] = array(
      'label' => t($value['label']),
      'description' => t($value['description']),
    );
    $viewers = array_merge_recursive($option, $viewers);
  }
  $viewers_config = variable_get($variable_id, array());
  $form['streaming_viewers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Video Streaming Service'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  if (!empty($viewers)) {
    $form['streaming_viewers'][$variable_id] = array(
      '#type' => 'item',
      '#title' => t('Select a streaming service'),
      '#description' => t('Preferred streaming service which provided by third-party modules for your solution pack.'),
      '#tree' => TRUE,
      '#theme' => 'islandora_viewers_table', // FIXME: this theme code outputs an HTML id https://github.com/Islandora/islandora/blob/02b7b2b1d7e9307b361554c1d665b40b01db1885/includes/solution_packs.inc#L744-L774
    );

    foreach ($viewers as $name => $profile) {
      $options[$name] = '';
      $form['streaming_viewers'][$variable_id]['name'][$name] = array(
        '#type' => 'hidden',
        '#value' => $name,
      );
      $form['streaming_viewers'][$variable_id]['label'][$name] = array(
        '#type' => 'item',
        '#markup' => $profile['label'],
      );
      $form['streaming_viewers'][$variable_id]['description'][$name] = array(
        '#type' => 'item',
        '#markup' => $profile['description'],
      );
    }
    $form['streaming_viewers'][$variable_id]['default'] = array(
      '#type' => 'radios',
      '#options' => isset($options) ? $options : array(),
      '#default_value' => !empty($viewers_config) ? $viewers_config['default'] : 'none',
    );
  }
  else {
    $form['streaming_viewers'][$variable_id . '_no_viewers'] = array(
      '#markup' => t('No viewers detected.'),
    );
  }
  return $form;
}
