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
  $fields_to_populate = db_query("SELECT * FROM {fedora_commons_export_fields} fcef
    LEFT JOIN {field_config} fc ON fc.field_name = fcef.field
    WHERE fcef.bundle = :bundle
    AND fc.type != 'field_collection'",
    array(':bundle' => $node->type)
  )->fetchAll();
  //@TODO: should exclude collection type fields

  foreach ($fields_to_populate as $field) {
    $values = get_field_values($field, $object);

    if ($field->field != 'title') {
      foreach ($values as $value) {
        if (!is_array($node->{$field->field})) {   //@TODO: prevent notice
          $node->{$field->field} = array();
        }
        $node->{$field->field}[$node->language][]['value'] = $value;   //@TODO: may not be 'value' key
      }
    }
    else {
      if ($values[0]) {
        $node->title = $values[0];
      }

    }

  }

  // @TODO: field_collection children has the bundle of its parent
  $field_collection_fields = db_query("SELECT * FROM {fedora_commons_export_fields} fcef
    LEFT JOIN {field_config_instance} fci ON fci.field_name = fcef.field
    LEFT JOIN {field_config} fc ON fc.field_name = fci.bundle
    WHERE fc.type = 'field_collection'"
  )->fetchAll();

  $values = array();
  foreach ($field_collection_fields as $field_collection_field) {
    $values[$field_collection_field->bundle][$field_collection_field->field] = get_field_values($field_collection_field, $object, array($field_collection_field->bundle));
  }

  foreach ($values as $bundle => $fields) {
    $collection_count = 0;
    foreach ($fields as $value) {
      if (count($value) > $collection_count) {
        $collection_count = count($value);
      }
    }

    if ($collection_count) {
      for ($i = 0; $i < $collection_count; $i++) {
        $field_collection_item = entity_create('field_collection_item', array('field_name' => $bundle));
        $field_collection_item->setHostEntity('node', $node);

        foreach ($fields as $k => $v) {
          if ($v[$i]) {
            $field_collection_item->{$k}[$node->language][]['value'] = $v[$i];
          }
        }
        $field_collection_item->save();
        $node->field_my_field_collection[$node->language][]['value'] = $field_collection_item->item_id;
      }
    }
  }
}

function get_field_values($field, $object, $parents = array()) {
  $values = array();

  if ($field->export_type == 'xml_value') {
    $read_path = '/mods:mods';   //@TODO: create root element
    $field_path = db_query("SELECT read_path FROM {fedora_commons_export_fields} WHERE field = :field AND bundle = :bundle",
      array(':field' => $field->field, ':bundle' => $field->bundle)
    )->fetchField();
    if ($field->read_context == 'parent') {
      $parent_path = fedora_commons_export_get_parent_path($field, $parents);
      $read_path .= $parent_path;
      if ($field_path) {
        $read_path .= '/' . $field_path; //@TODO: should text() be here?
      }
    }
    else {
      $read_path = $field_path;
    }

    $ds_content = $object[$field->dsid]->content;
    $doc = new DOMDocument();
    $doc->loadXML($ds_content);
    if ($doc) {
      $xpath = new DOMXPath($doc);
      $xml_values = $xpath->query($read_path);
      foreach ($xml_values as $xml_value) {
        $values[] = $xml_value->nodeValue;
      }
    }

  }
  else {
    // managed datastream
    $values[] = $object[$field->dsid]->content;
  }
  return $values;
}