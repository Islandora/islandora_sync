<?php

function islandora_sync_xml_ds_page() {
  $datastreams = db_query('SELECT * FROM {islandora_sync_xml_ds}')->fetchAll();

  $header = array(t('Datastream'), t('Edit'), t('Delete'));
  $rows = array();
  foreach ($datastreams as $datastream) {
    $rows[] = array(
      check_plain($datastream->ds_id),
      l(t('Edit'), "admin/islandora/sync/xml-datastreams/$datastream->ds_id/edit"),
      l(t('Delete'), "admin/islandora/sync/xml-datastreams/$datastream->ds_id/delete"),
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function islandora_sync_xml_ds_add($form, $form_state) {
  $form = array();
  $form['#tree'] = TRUE;
  $form['ds_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Datastream ID'),
    '#description' => t('The DSID for this XML datastream'),
    '#required' => TRUE,
  );
  $form['root'] = array(
    '#type' => 'textfield',
    '#title' => t('Root Element'),
    '#description' => t('The root element of the XML document created for this datastream'),
    '#required' => TRUE,
  );
  $form['namespace_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Namespace URI'),
  );
  $form['namespaces'] = array(
    '#type' => 'fieldset',
    '#title' => t('Namespaces'),
  );
  if (empty($form_state['namespace_count'])) {
    $form_state['namespace_count'] = 1;
  }
  for ($i = 1; $i <= $form_state['namespace_count']; $i++) {
    $form['namespaces'][$i] = array(
      '#type' => 'fieldset',
      '#title' => t('Namespace @num', array('@num' => $i)),
    );
    $form['namespaces'][$i]['prefix'] = array(
      '#type' => 'textfield',
      '#title' => t('Prefix'),
    );
    $form['namespaces'][$i]['uri'] = array(
      '#type' => 'textfield',
      '#title' => t('URI'),
    );
  }
  $form['namespaces']['add_namespace'] = array(
    '#type' => 'submit',
    '#value' => t('Add Namespace'),
    '#submit' => array('islandora_sync_xml_ds_add_namespace_add'),
  );
  if ($form_state['namespace_count'] > 1) {
    $form['namespaces']['remove_namespace'] = array(
      '#type' => 'submit',
      '#value' => t('Remove Namespace'),
      '#submit' => array('islandora_sync_xml_ds_add_namespace_remove'),
      '#limit_validation_errors' => array(),
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function islandora_sync_xml_ds_add_namespace_add($form, &$form_state) {
  $form_state['namespace_count']++;
  $form_state['rebuild'] = TRUE;
}

function islandora_sync_xml_ds_add_namespace_remove($form, &$form_state) {
  $form_state['namespace_count']--;
  $form_state['rebuild'] = TRUE;
}

function islandora_sync_xml_ds_add_validate($form, $form_state) {
  $dsid_exists = db_query('SELECT 1 FROM {islandora_sync_xml_ds} WHERE ds_id = :ds_id',
    array('ds_id' => $form_state['values']['ds_id'])  //@TODO: handle ds_id case
  )->fetchAll();
  if ($dsid_exists) {
    form_set_error('ds_id', t('This datastream already exists.'));
  }
}

function islandora_sync_xml_ds_add_submit($form, &$form_state) {
  $values = $form_state['values'];
  $record = array('ds_id' => $values['ds_id'], 'root' => $values['root'], 'namespace_uri' => $values['namespace_uri']);
  drupal_write_record('islandora_sync_xml_ds', $record);

  foreach ($values['namespaces'] as $namespace) {
    if (is_array($namespace)) {
      $record = array('ds_id' => $values['ds_id'], 'prefix' => $namespace['prefix'], 'uri' => $namespace['uri']);
      drupal_write_record('islandora_sync_xml_ds_namespaces', $record);
    }
  }
  drupal_set_message(t('Datastream settings saved'));
  $form_state['redirect'] = 'admin/islandora/sync/xml-datastreams';
}

function islandora_sync_xml_ds_edit($form, $form_state, $ds_id) {

}

function islandora_sync_xml_ds_delete($form, $form_state, $ds_id) {
  $confirm_text = t('Delete datastream settings?');
  $form['ds_id'] = array(
    '#type' => 'value',
    '#value' => $ds_id,
  );
  return confirm_form($form, $confirm_text, 'admin/islandora/sync/xml-datastreams');
}

function islandora_sync_xml_ds_delete_submit($form, &$form_state) {
  db_query('DELETE FROM {islandora_sync_xml_ds} WHERE ds_id = :ds_id', array(':ds_id' => $form_state['values']['ds_id']));
  db_query('DELETE FROM {islandora_sync_xml_ds_namespaces} WHERE ds_id = :ds_id', array(':ds_id' => $form_state['values']['ds_id']));
  drupal_set_message(t('Datastream settings deleted'));
  $form_state['redirect'] = 'admin/islandora/sync/xml-datastreams';
}