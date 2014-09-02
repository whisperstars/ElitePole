<?php

/**
 * @file
 * The transformer webcomponent
 *
 * @author Glenn De Backer <glenn at coworks dot be>
 *
 */

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_transformer() {
  return array(
      'name' => '',
      'form_key' => NULL,
      'pid' => 0,
      'weight' => 0,
      'value' => '',
      'mandatory' => TRUE,
      'extra' => array(
          'size' => '',
          'maxlength' => '25',
          'disabled' => 0,
          'unique' => TRUE,
          'title_display' => 0,
          'description' => t(''),
              'attributes' => array(),
              'private' => FALSE,
          'field_pattern' => '',
          'field_placeholder' => TRUE,
      ),
  );
}

/**
 * implement _theme
 */
function _webform_theme_transformer() {
  return array(
      'webform_display_transformer' => array(
          'render element' => 'element',
      ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_transformer($component) {

  // add transformer edit javascript
  drupal_add_js(drupal_get_path('module', 'webform_transformer') . '/js/transformer_edit.js');

  $form = array();

  $form['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Default value'),
      '#default_value' => $component['value'],
      '#description' => t('The default value of the field.'),
      '#size' => 60,
      '#maxlength' => 1024,
      '#weight' => 0,
  );

  $form['extra']['field_pattern'] = array(
      '#type' => 'textfield',
      '#title' => t('Pattern'),
      '#default_value' => $component['extra']['field_pattern'],
      '#description' => t('&nbsp;'),
      '#size' => 60,
      '#required' => TRUE,
      '#maxlength' => 255,
      '#weight' => 1.2,
  );

  $form['extra']['field_predefined_pattern'] = array(
      '#type' => 'select',
      '#title' => t('Predefined patterns'),
      '#description' => t(''),
      '#size' => 60,
            '#required' => FALSE,
            '#multiple' => FALSE,
      '#options' => patterns(),
            '#size' => 0,
      '#validated' => FALSE,
            '#weight' => 1.1,
        );

  $form['extra']['field_placeholder'] = array(
      '#type' => 'checkbox',
      '#title' => t('Placeholder'),
      '#description' => t('Enables a placeholder that will be displayed in the textfield.'),
      '#default_value' => $component['extra']['field_placeholder'],
      '#weight' => 1.3,
  );

  // if you couldn't detect placeholder.min.js
  if (!file_exists(libraries_get_path('placeholders.js') . '/placeholders.min.js')) {
    $form['extra']['field_placeholder']['#description'] = $form['extra']['field_placeholder']['#description'] . ' ' . t('Warning couldn\'t detect placeholder.js, so older browsers may not show the placeholder.');
  }



  return $form;
}

/**
 * Render a Webform component to be part of a form.
 */
function _webform_render_transformer($component, $value = NULL, $filter = TRUE) {

  // add transformer javascript
  drupal_add_js(drupal_get_path('module', 'webform_transformer') . '/js/transformer.js');

  // check if placeholder.js exist
  if (file_exists(libraries_get_path('placeholders.js') . '/placeholders.min.js')) {
    // add placeholder polyfill to support browsers that don't support the placeholder attribute
    drupal_add_js(libraries_get_path('placeholders.js') . '/placeholders.min.js');
  }


  $element = array(
      '#type' => 'textfield',
      '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
      '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
      '#default_value' => $filter ? _webform_filter_values($component['value'], NULL, NULL, NULL, FALSE) : $component['value'],
      '#attributes' => array('data-transformer' => $filter ? _webform_filter_values($component['extra']['field_pattern'], NULL, NULL, NULL, FALSE) : $component['extra']['field_pattern']) ,
      '#required' => $component['mandatory'],
      '#weight' => $component['weight'],
      '#theme_wrappers' => array('webform_element'),
      '#webform_component' => $component,
  );

  // if placeholder is enabled
  if ($component['extra']['field_placeholder']) {
      // generate random pattern
      $placeholder_str = '';

      $pattern_chars = str_split($component['extra']['field_pattern']);

      foreach ($pattern_chars as $char) {
          if ($char == '#') {
              $placeholder_str .= rand(0, 9);
          }
          else {
             $placeholder_str .= $char;
          }
      }

      // set placeholder attribute
      $element['#attributes']['placeholder'] = $placeholder_str;
  }

  return $element;
}

/**
 * Display the result of a submission for a component.
 */
function _webform_display_transformer($component, $value, $format = 'html') {
  return array(
      '#title' => $component['name'],
      '#weight' => $component['weight'],
      '#theme' => 'webform_display_transformer',
      '#theme_wrappers' => $format == 'html' ? array('webform_element')
      : array('webform_element_text'),
      '#post_render' => array('webform_element_wrapper'),
      '#component' => $component,
      '#format' => $format,
      '#value' => $value[0],
      '#webform_component' => $component,
  );
}


/**
 * Format the output of data for this component.
 */
function theme_webform_display_transformer($variables) {
  $element = $variables['element'];
  $value = $element['#format'] == 'html' ? check_plain($element['#value']) : $element['#value'];
  return $value !== '' ? ( $value ) : ' ';
}

/**
 * Return the heading for this component to be displayed in a CSV file.
 */
function _webform_csv_headers_transformer($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $component['name'];
  return $header;
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_transformer($component, $value) {
  return check_plain(empty($value[0]) ? '' : $value[0]);
}


/**
 * Format the submitted data of a component for CSV downloading.
 */
function _webform_csv_data_transformer($component, $export_options, $value) {
  return !isset($value[0]) ? '' : $value[0];
}


/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_transformer($component, $sids = array()) {
  $query = db_select('webform_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
  ->fields('wsd', array('data'))
  ->condition('nid', $component['nid'])
  ->condition('cid', $component['cid']);

  if (count($sids)) {
    $query->condition('sid', $sids, 'IN');
  }

  $nonblanks = 0;
  $submissions = 0;
  $wordcount = 0;

  $result = $query->execute();
  foreach ($result as $data) {
    if (drupal_strlen(trim($data['data'])) > 0) {
      $nonblanks++;
    }
    $submissions++;
  }

  $rows[0] = array(t('Left Blank'), ($submissions - $nonblanks));
  $rows[1] = array(t('User entered value'), $nonblanks);

  return $rows;
}
