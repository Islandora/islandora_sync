<?php

function islandora_sync_import_from_pid($pid, $type, $nid = FALSE) {
  $object = islandora_object_load($pid);

  if (!$nid) {
    $node = new stdClass();
    $node->type = $type;
    $node->language = LANGUAGE_NONE;
    node_object_prepare($node);
  }
  else {
    $node = node_load($nid);
    $node->revision = 1; // Is this good, many revisions seem to be created
    $node->log = 'Updated by islandora_sync';
  }

  $node->title = $object->label;

  $author = user_load_by_name($object->owner);
  if ($author) {
    $node->uid = $author->uid;
  }
  else {
    $node->uid = 1;
  }
  // Call node_save to get a nid
  node_save($node);
  islandora_sync_build_node_fields($node, $object);
  node_save($node);

  if (!$nid) {
    $map['pid'] = $object->id;
    $map['type'] = 'node';
    $map['bundle'] = $type;
    $map['entity_id'] = $node->nid;
    drupal_write_record('islandora_sync_map', $map);
  }

  return $node->nid;
}

function islandora_sync_build_node_fields(&$node, $object) {
  $fields_to_populate = db_query("SELECT fcef.*, fc.type FROM {islandora_sync_fields} fcef
    LEFT JOIN {field_config} fc ON fc.field_name = fcef.field
    WHERE fcef.bundle = :bundle",
    array(':bundle' => $node->type)
  )->fetchAll();

  foreach ($fields_to_populate as $field) {

    //@TODO: normalize $field->type?
    $return = module_invoke_all('islandora_sync_node_field_build', $node, $object, $field);

    if (!$return) {
      $values = get_field_values($field, $object);
      $node->{$field->field} = array();
      if ($field->field != 'title') {
        foreach ($values as $value) {
          islandora_sync_save_field($field, $node, $value);
        }
      }
      else {
        if ($values[0]) {
          $node->title = $values[0];
        }
      }
    }
  }
}

function get_field_values($field, $object, $parents = array()) {
  $values = array();

  switch ($field->export_type) {
    case 'xml_value':
      $read_path = '/mods:mods';   //@TODO: create root element, default to // if we can't get it
      $field_path = db_query("SELECT read_path FROM {islandora_sync_fields} WHERE field = :field AND bundle = :bundle",
        array(':field' => $field->field, ':bundle' => $field->bundle)
      )->fetchField();
      if ($field->read_context == 'parent') {
        $parent_path = islandora_sync_get_parent_path($field, $parents);
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
      break;

    case 'managed_datastream':
      $values[] = array('content' => $object[$field->dsid]->content, 'mimetype' => $object[$field->dsid]->mimetype);  //@TODO: stream to get around memory constraints?
      break;

    case 'relationship':
      $rels = $object->relationships->get($field->predicate_uri, $field->predicate);
      foreach ($rels as $rel) {
        $values[] = $rel['object']['value'];
      }
      break;
  }

  return $values;
}

function islandora_sync_save_field($field_info, &$entity, $value) {
  $lang = isset($entity->language) ? $entity->language : LANGUAGE_NONE;
  $id = isset($entity->nid) ? $entity->nid : user_password();
  $prepared_field = &$entity->{$field_info->field}[$lang];
  //@TODO: these should be plugins
  switch ($field_info->type) {
    case 'image':
      module_load_include('inc', 'islandora', 'mime_detect');
      $mime_detect = new MimeDetect();
      $extension = $mime_detect->getExtension($value['mimetype']);
      $file_name = $field_info->dsid . '-' . $entity->type . '-' . $id . '.' . $extension;
      $file = file_save_data($value['content'], 'public://' . $file_name, FILE_EXISTS_RENAME);
      $prepared_field[] = (array) $file;
      break;

    default:
      $prepared_field[]['value'] = $value;
  }
}

function islandora_sync_get_parent_path($field, $parents = array()) {
  $path = '';
  $parents = islandora_sync_get_parent_fields($field->field, $field->bundle, $parents);

  $parents = array_reverse($parents);
  foreach ($parents as $parent) {
    // @TODO: field could belong to multiple bundles, query with bundle
    $field_read_path = db_query("SELECT read_path FROM {islandora_sync_fields} WHERE field = :field",
      array(':field' => $parent)
    )->fetchField();
    // @TODO: if document read context, get it and end
    if ($field_read_path && $field_read_path != $path) {
      $path .= '/' . $field_read_path;
    }
  }

  return $path;

}

//@TODO: make recursive
function islandora_sync_get_parent_fields($field, $bundle, $parents) {
  $start_count = count($parents);
  $field_info = field_info_instance('node', $field, $bundle);

  // is part of field_collection? has to be a better way to do this
  // entity_type == field_collection_item ?? in field_config_instance
  // check if module exists?
  if ($field_info['widget']['module'] == 'field_collection') {

  }
  elseif (module_exists('field_group')) { // could be disabled
    // is part of field_group?
    // this is also terrible
    ctools_include('export');
    $groups = ctools_export_load_object('field_group', 'conditions', array(
      'entity_type' => 'node',
      'bundle' => $bundle,
    ));
    foreach ($groups as $group) {
      if (in_array($field, $group->data['children'])) {
        $parents[] = $group->group_name;
        if ($group->parent_name) {
          $parents[] = $group->parent_name;
        }
      }
    }
  }

  $current_count = count($parents);
  if ($current_count > $start_count) {
    $parents = islandora_sync_get_parent_fields($parents[$current_count - 1], $bundle, $parents);
  }

  return $parents;
}