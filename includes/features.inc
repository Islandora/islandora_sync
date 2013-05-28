<?php

function field_datastreams_features_export($data, &$export, $module_name) {
  foreach ($data AS $component) {
    $export['features']['field_datastreams'][$component] = $component;
  }
  return array();
}

function field_datastreams_features_export_options() {
  $options = array();
  $fields = db_query("SELECT * FROM {islandora_sync_fields}")->fetchAll();
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
    $field = db_query("SELECT * FROM {islandora_sync_fields} WHERE bundle = :bundle AND field = :field",
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
  field_datastreams_features_disable_feature($module_name);
  $fields = module_invoke($module_name, 'field_datastreams_defaults');
  if (!empty($fields)) {
    foreach ($fields as $field) {
      drupal_write_record('islandora_sync_fields', $field);
    }
  }
}

function field_datastreams_features_enable_feature($module_name) {
  field_datastreams_features_rebuild($module_name);
}

function field_datastreams_features_disable_feature($module_name) {
  $fields = module_invoke($module_name, 'field_datastreams_defaults');
  foreach($fields as $field) {
    db_delete('islandora_sync_fields')->condition('bundle', $field['bundle'])->condition('field', $field['field'])->execute();
  }
}

function islandora_sync_bundle_settings_features_export($data, &$export, $module_name) {
  foreach ($data as $component) {
    $export['features']['islandora_sync_bundle_settings'][$component] = $component;
  }
  return array();
}

function islandora_sync_bundle_settings_features_export_options() {
  $options = array();
  $bundles = db_query("SELECT * FROM {islandora_sync_type_settings}")->fetchAll();
  foreach ($bundles as $bundle) {
    $name = $bundle->bundle;
    $options[$name] = $name;
  }
  return $options;
}

function islandora_sync_bundle_settings_features_export_render($module_name, $data, $export = NULL) {
  $code = array();
  $code[] = '$islandora_sync_bundle_settings = array();';
  foreach ($data as $name) {
    $bundle = db_query("SELECT * FROM {islandora_sync_type_settings} ists",
      array(':bundle' => $name)
    )->fetchObject();
    $code[] = "  \$islandora_sync_bundle_settings['{$name}'] = " . features_var_export($bundle) .";";
    $bundle_cmodels = db_query("SELECT * FROM {islandora_sync_type_cmodel} WHERE bundle = :bundle",
      array(':bundle' => $name)
    )->fetchAll();
    $code[] = "  \$islandora_sync_bundle_settings['{$name}']['cmodels'] = " . features_var_export($bundle_cmodels) .";";
  }
  $code[] = "return \$islandora_sync_bundle_settings;";
  $code = implode("\n", $code);
  return array('islandora_sync_bundle_settings_defaults' => $code);
}

function islandora_sync_bundle_settings_features_revert($module_name) {
  islandora_sync_bundle_settings_features_rebuild($module_name);
}

function islandora_sync_bundle_settings_features_rebuild($module_name) {
  islandora_sync_bundle_settings_features_disable_feature($module_name);
  $bundles = module_invoke($module_name, 'islandora_sync_bundle_settings_defaults');
  if (!empty($bundles)) {
    foreach ($bundles as $bundle) {
      foreach ($bundle['cmodels'] as $cmodel) {
        $bundle_cmodel = array('bundle' => $bundle['bundle'], 'cmodel' => $cmodel['cmodel']);
        drupal_write_record('islandora_sync_type_cmodel', $bundle_cmodel);
      }
      unset($bundle['cmodels']);
      drupal_write_record('islandora_sync_type_settings', $bundle);
    }
  }
}

function islandora_sync_bundle_settings_features_enable_feature($module_name) {
  islandora_sync_bundle_settings_features_rebuild($module_name);
}

function islandora_sync_bundle_settings_features_disable_feature($module_name) {
  $bundles = module_invoke($module_name, 'islandora_sync_bundle_settings_defaults');
  foreach ($bundles as $bundle) {
    foreach ($bundle['cmodels'] as $cmodel) {
      db_delete('islandora_sync_type_cmodel')->condition('bundle', $bundle['bundle'])->condition('cmodel', $cmodel['cmodel'])->execute();
    }
    db_delete('islandora_sync_type_settings')->condition('bundle', $bundle['bundle'])->execute();
  }
}
