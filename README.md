CONTENTS OF THIS FILE
---------------------

 * summary
 * requirements
 * installation
 * configuration
 * troubleshooting
 * sponsors

SUMMARY
-------

Islandora Sync enables event-based synchronization of content between Fedora Commons and Drupal. Fedora content models may be mapped to Drupal node types and Fedora datastreams and their XML contents to Drupal fields. A more in-depth overview of the module's use and configuration can be found at https://github.com/discoverygarden/islandora_sync/wiki/Islandora-Sync-Use-and-Configuration

REQUIREMENTS
------------

Islandora

Field UI

Field

INSTALLATION
------------

Download and enable module.
Islandora Sync Relation provides integration with the Relation module (www.drupal.org/project/relation) for syncing Fedora object to object relationships as Drupalnode to node relationships.
Islandora Sync Field Collection integrates with the Field Collection module to allow for repeating groups of fields, similar to tabs of Islandora XML Forms.

CONFIGURATION
-------------

Node type mappings to Fedora content models and related settings may be configured at
/admin/structure/types/manage/<node_bundle_machine_name>/fedora
To sync XML datastreams, XML configuration is done at /admin/islandora/sync/xml-datastreams/

Features integration is provided to allow for programmatic deployment of configurations.

TROUBLESHOOTING
---------------

A Drupal issue may present problems when syncing managed datastreams from Fedora to Drupal, see
http://drupal.org/node/1443158 for a patch.

SPONSORS
--------

Development for Islandora Sync sponsored by UCLA Library.
