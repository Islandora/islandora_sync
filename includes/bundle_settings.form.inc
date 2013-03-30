<?php

function fedora_commons_export_bundle_settings_form($form, $form_state, $bundle) {
  $form = array();
  $bundle = str_replace('-', '_', $bundle);
  $settings = fedora_commons_export_bundle_settings($bundle);

  if (isset($settings[0]->id)) {
    $sync_fedora = $settings[0]->sync_fedora;
    $selected_cmodels = fedora_commons_export_get_bundle_cmodels($settings[0]->id);
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $settings[0]->id,
    );
    $namespace =  $settings[0]->namespace;
  }
  else {
    $sync_fedora = FALSE;
    $selected_cmodels = array();
    $namespace = '';
  }

  $form['bundle'] = array(
    '#type' => 'value',
    '#value' => $bundle,
  );

  $form['sync_fedora'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sync nodes to Fedora'),
    '#default_value' => $sync_fedora,
  );

  $form['namespace'] = array(
    '#type' => 'textfield',
    '#title' => t('Namespace'),
    '#description' => t('Object ID namespace'),
    '#default_value' => $namespace,
  );

  $content_models = drupal_map_assoc(fedora_commons_export_get_content_models());

  $form['cmodel'] = array(
    '#type' => 'select',
    '#title' => t('Content model'),
    '#description' => t('The content models that this node type should be mapped to.'),
    '#options' => $content_models,
    '#multiple' => TRUE,
    '#states' => array(
      'invisible' => array(
        ':input[name="sync_fedora"]' => array('checked' => FALSE),
        ':input[name="sync_drupal"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => $selected_cmodels,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function fedora_commons_export_bundle_settings_form_validate($form, $form_state) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  if (!empty($form_state['values']['namespace']) && !islandora_is_valid_namespace($form_state['values']['namespace'])) {
    form_set_error('namespace', t('Please enter a valid namespace'));
  }
}

function fedora_commons_export_bundle_settings_form_submit($form, $form_state) {
  $record['sync_fedora'] = $form_state['values']['sync_fedora'];
  $record['entity_type'] = 'node';
  $record['bundle'] = $form_state['values']['bundle'];
  $record['namespace'] = $form_state['values']['namespace'];
  if (isset($form_state['values']['id'])) {
    $record['id'] = $form_state['values']['id'];
    drupal_write_record('fedora_commons_export_type_settings', $record, 'id');
  }
  else {
    drupal_write_record('fedora_commons_export_type_settings', $record);
  }

  $existing_cmodels = fedora_commons_export_get_bundle_cmodels($record['id']);

  // Insert new cmodels
  foreach ($form_state['values']['cmodel'] AS $cmodel) {
    if (!in_array($cmodel, $existing_cmodels)) {
      $cmodel_record = array('id' => $record['id'], 'cmodel' => $cmodel);
      drupal_write_record('fedora_commons_export_type_cmodel', $cmodel_record);
    }
  }

  // Delete cmodels
  $deleted_cmodels = array_diff($existing_cmodels, $form_state['values']['cmodel']);
  foreach ($deleted_cmodels AS $deleted) {
    db_query('DELETE FROM {fedora_commons_export_type_cmodel} WHERE id = :id AND cmodel = :cmodel',
      array(':id' => $record['id'], ':cmodel' => $deleted)
    );
  }

  drupal_set_message(t('Settings updated.'));
}