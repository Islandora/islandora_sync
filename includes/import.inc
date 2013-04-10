<?php

function fedora_commons_export_batch_import_form($form, $form_state) {
  $form = array();

  $form['pid'] = array(
    '#type' => 'textfield',
    '#title' => t('Pid'),
  );

  $form['node_type'] = array(
    '#type' => 'radios',
    '#title' => t('Node type'),
    '#options' => node_type_get_names(),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function fedora_commons_export_batch_import_form_validate($form, $form_state) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  if (!islandora_is_valid_pid($form_state['values']['pid'])) {
    form_set_error('pid', t('Please enter a valid PID'));
  }
}

function fedora_commons_export_batch_import_form_submit($form, $form_state) {
  fedora_commons_export_import_batch(array($form_state['values']['pid']), $form_state['values']['node_type']);
}

function fedora_commons_export_import_batch($pids, $type) {

  foreach ($pids AS $pid) {
    $node_id = fedora_commons_export_import_from_pid($pid, $type);
    drupal_set_message(t('Imported object: @pid to node/@id', array('@pid' => $pid, '@id' => $node_id)));
  }
}

function fedora_commons_export_import_from_pid($pid, $type) {
  $object = islandora_object_load($pid);

  $node = new stdClass();
  $node->type = $type;
  $node->language = LANGUAGE_NONE;
  node_object_prepare($node);
  $node->title = $object->label;

  $author = user_load_by_name($object->owner);
  if ($author) {
    $node->uid = $author->uid;
  }
  else {
    $node->uid = 1;
  }
  fedora_commons_export_build_node_fields($node, $object);
  node_save($node);

  /*
  $object->relationships->add(ISLANDORA_RELS_EXT_URI, 'drupalEntity', $node->nid, TRUE);
  */
  return $node->nid;
}

function fedora_commons_export_build_node_fields(&$node, $object) {
  $fields_to_populate = db_query('SELECT * FROM {fedora_commons_export_fields} WHERE bundle = :bundle', array(':bundle' => $node->type))->fetchAll();

  foreach ($fields_to_populate AS $field) {
    $values = get_field_values($field, $object);

    foreach ($values AS $value) {

    }
    //$node->{$field->field}
  }
}

function get_field_values($field, $object) {
  $values = array();
  switch ($field->export_type) {
    case 'xml_value':

  }
  return $values;
}