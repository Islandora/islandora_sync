<?php

function field_datastreams_features_export($data, &$export, $module_name) {
  foreach ($data AS $component) {
    $export['features']['field_datastreams'][$component] = $component;
  }
  return array();
}

function field_datastreams_features_export_options() {
  $options = array();
  $fields = db_query("SELECT * FROM {fedora_commons_export_fields}")->fetchAll();
  foreach ($fields as $field) {
    $name = $field->bundle . ':' . $field->field;
    $options[$name] = $name;
  }
  return $options;
}

function field_datastreams_features_export_render($module_name, $data, $export = NULL) {
  $code = array();
  $code[] = '$field_datastreams = array();';
  foreach ($data as $name) {
    list($bundle, $field_name) = explode(':', $name);
    $field = db_query("SELECT * FROM {fedora_commons_export_fields} WHERE bundle = :bundle AND field = :field",
      array(':bundle' => $bundle, ':field' => $field_name)
    )->fetchObject();
    $code[] = "  \$field_datastreams['{$name}'] = " . features_var_export($field) .";";
  }
  $code[] = "return \$field_datastreams;";
  $code = implode("\n", $code);
  return array('field_datastreams_defaults' => $code);
}

function field_datastreams_features_revert($module_name) {
  field_datastreams_features_rebuild($module_name);
}

function field_datastreams_features_rebuild($module_name) {
  watchdog('mitch', 'field_datastreams_features_rebuild?');
  $fields = module_invoke($module_name, 'field_datastreams_defaults');
  if (!empty($fields)) {
    foreach ($fields as $field) {
      drupal_write_record('fedora_commons_export_fields', $field);

    }
  }
}
