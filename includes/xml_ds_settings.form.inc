<?php

function fedora_commons_export_xml_ds_page() {
  $datastreams = db_query('SELECT * FROM {fedora_commons_export_xml_ds}')->fetchAll();

  $header = array(t('Datastream'), t('Edit'), t('Delete'));
  $rows = array();
  foreach ($datastreams AS $datastream) {
    $rows[] = array(
      check_plain($datastream->ds_id),
      l(t('Edit'), "admin/islandora/fedora-export/xml-datastreams/$datastream->ds_id/edit"),
      l(t('Delete'), "admin/islandora/fedora-export/xml-datastreams/$datastream->ds_id/delete"),
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function fedora_commons_export_xml_ds_add($form, $form_state) {
  $form = array();

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
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function fedora_commons_export_xml_ds_add_validate($form, $form_state) {
  watchdog('mitch', 'validate form?');
  $dsid_exists = db_query('SELECT 1 FROM {fedora_commons_export_xml_ds} WHERE ds_id = :ds_id',
    array('ds_id' => $form_state['values']['ds_id'])  //@TODO: handle ds_id case
  )->fetchAll();
  if ($dsid_exists) {
    form_set_error('ds_id', t('This datastream already exists.'));
  }
}

function fedora_commons_export_xml_ds_add_submit($form, $form_state) {
  $values = $form_state['values'];
  $record = array('ds_id' => $values['ds_id'], 'root' => $values['root'], 'namespace_uri' => $values['namespace_uri']);
  watchdog('mitch', 'submit form?');
  drupal_write_record('fedora_commons_export_xml_ds', $record);

}

function fedora_commons_export_xml_ds_edit($form, $form_state, $ds_id) {

}

function fedora_commons_export_xml_ds_delete($form, $form_state, $ds_id) {

}