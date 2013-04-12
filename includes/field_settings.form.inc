<?php

function fedora_commons_export_field_settings_form($form, $form_state, $type, $field) {
  $type = str_replace('-', '_', $type);
  $field_settings = fedora_commons_export_field_settings($type, $field);
  if (empty($field_settings)) {
    $settings = new stdClass();
    $settings->new = TRUE;
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
  );
  if (isset($settings->export)) {
    $form['export']['#default_value'] = $settings->export;
  }

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
  );
  if (isset($settings->export_type)) {
    $form['general']['export_type']['#default_value'] = $settings->export_type;
  }
  $form['general']['dsid'] = array(
    '#type' => 'textfield',
    '#title' => t('Datastream ID'),
  );
  if (isset($settings->dsid)) {
    $form['general']['dsid']['#default_value'] = $settings->dsid;
  }
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
  $form['xml']['create'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create'),
    '#description' => t('If checked, create an XML element when creating or updating a node.')
  );
  if (isset($settings->create_value)) {
    $form['xml']['create']['#default_value'] = TRUE;
  }
  $form['xml']['create_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create Settings'),
    '#collapsible' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[name="create"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['xml']['create_group']['create_context'] = array(
    '#type' => 'select',
    '#title' => t('Path context'),
    '#options' => array(
      'parent' => t('Parent'),
      'document' => t('Document'),
    ),
    '#empty_option' => t('- Select -'),
  );
  if (isset($settings->create_context)) {
    $form['xml']['create_group']['create_context']['#default_value'] = $settings->create_context;
  }
  $form['xml']['create_group']['create_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
  );
  if (isset($settings->create_path)) {
    $form['xml']['create_group']['create_path']['#default_value'] = $settings->create_path;
  }
  $form['xml']['create_group']['create_type'] = array(
    '#type' => 'select',
    '#title' => t('XML field type'),
    '#options' => array(
      'element' => t('Element'),
      'attribute' => t('Attribute'),
      'xml' => t('XML'),
    ),
    '#empty_option' => t('- Select -'),
  );
  if (isset($settings->create_type)) {
    $form['xml']['create_group']['create_type']['#default_value'] = $settings->create_type;
  }
  $form['xml']['create_group']['create_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Create'),
  );
  if (isset($settings->create_value)) {
    $form['xml']['create_group']['create_value']['#default_value'] = $settings->create_value;
  }
  $form['xml']['read'] = array(
    '#type' => 'checkbox',
    '#title' => t('Read'),
    '#description' => t('If checked, read an XML value when importing a Fedora Commons object.')
  );
  if (isset($settings->read_path)) {
    $form['xml']['read']['#default_value'] = TRUE;
  }
  $form['xml']['read_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Read Settings'),
    '#collapsible' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[name="read"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['xml']['read_group']['read_context'] = array(
    '#type' => 'select',
    '#title' => t('Path context'),
    '#options' => array(
      'parent' => t('Parent'),
      'document' => t('Document'),
    ),
    '#empty_option' => t('- Select -'),
  );
  if (isset($settings->read_context)) {
    $form['xml']['read_group']['read_context']['#default_value'] = $settings->read_context;
  }
  $form['xml']['read_group']['read_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Read'),
  );
  if (isset($settings->read_path)) {
    $form['xml']['read_group']['read_path']['#default_value'] = $settings->read_path;
  }
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
  drupal_set_message(t('Fedora Commons settings saved.'));
}