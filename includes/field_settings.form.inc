<?php

function fedora_commons_export_field_settings_form($form, $form_state, $type, $field) {
  $type = str_replace('-', '_', $type);
  $field_settings = fedora_commons_export_field_settings($type, $field);
  if (empty($field_settings)) {
    $settings = fedora_commons_export_field_settings_default();
  }
  else {
    $settings = $field_settings[0];
    $settings->new = FALSE;
  }

  $form = array();
  $form['settings'] = array(
    '#type' => 'value',
    '#value' => $settings,
  );
  $form['bundle'] = array(
    '#type' => 'value',
    '#value' => $type,
  );
  $form['field'] = array(
    '#type' => 'value',
    '#value' => $field,
  );
  $form['export'] = array(
    '#type' => 'checkbox',
    '#title' => t('Export to Fedora Commons?'),
    '#description' => t('Should this field be exported to Fedora Commons?'),
    '#default_value' => $settings->export,
  );
  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Fedora Commons Settings'),
    '#collapsible' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[name="export"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['general']['export_type'] = array(
    '#type' => 'select',
    '#title' => t('Fedora Commons data structure'),
    '#description' => t('The type of data this field should be exported as.'),
    '#options' => array(
      'xml_value' => t('XML value'),
      'managed_datastream' => t('Managed datastream'),
    ),
    '#empty_option' => t('- Select -'),
    '#default_value' => $settings->export_type,
  );
  $form['general']['dsid'] = array(
    '#type' => 'textfield',
    '#title' => t('Datastream ID'),
    '#default_value' => $settings->dsid,
  );
  $form['xml'] = array(
    '#type' => 'fieldset',
    '#title' => t('XML Settings'),
    '#collapsible' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[name="export_type"]' =>  array('value' => 'xml_value'),
      ),
    ),
  );
  $form['xml']['xml_path_context'] = array(
    '#type' => 'select',
    '#title' => t('Path context'),
    '#options' => array(
      'parent' => t('Parent'),
      'document' => t('Document'),
    ),
    '#empty_option' => t('- Select -'),
    '#default_value' => $settings->xml_path_context,
  );
  $form['xml']['xml_field_type'] = array(
    '#type' => 'select',
    '#title' => t('XML field type'),
    '#options' => array(
      'element' => t('Element'),
      'attribute' => t('Attribute'),
      'xml' => t('XML'),
    ),
    '#empty_option' => t('- Select -'),
    '#default_value' => $settings->xml_field_type,
  );
  $form['xml']['xml_field_value'] = array(
    '#type' => 'textfield',
    '#title' => t('XML value'),
    '#default_value' => $settings->xml_field_value,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function fedora_commons_export_field_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];

  // Update settings
  if (!$values['settings']->new) {
    unset($values['settings']);
    drupal_write_record('fedora_commons_export_fields', $values, array('bundle', 'field'));
  }
  else {
    // Insert settings
    unset($values['settings']);
    drupal_write_record('fedora_commons_export_fields', $values);
  }
}

function fedora_commons_export_field_settings_default() {
  $settings = new stdClass();
  $settings->new = TRUE;
  $settings->export = 0;
  //$settings->export_type
  return $settings;
}