<?php

include_once("includes/dspace.variables.inc");

function dspace_entity_info(){
    $entities[DSPACE_SET] = array(
        'label' => t('DSpace Set'),
        'plural label' => t('DSpace Sets'),
        'description' => t('Sets imported from DSpace.'),
        'entity class' => 'Entity',
        'controller class' => 'EntityAPIController',
        'base table' => 'dspace_set',
        'fieldable' => TRUE,
        'view modes' => array(
            'full' => array(
                'label' => t('Full DSpace Set'),
                'custom settings' =>  FALSE,
            ),
        ),
        'entity keys' => array(
            'id' => 'id',
            'label' => 'label',
        ),
        'label_callback' => 'entity_class_label',
        'uri callback' => 'dspace_set_uri',
        'module' => 'dspace',
        'access callback' => 'dspace_access',
//        'admin' => array(
//            'path' => 'admin/structure/dspace/manage/sets',
//            'real path' => 'admin/structure/dspace/manage/sets',
//            'access arguments' => array('administer ingredients'),
//        ),


    );
    $entities[DSPACE_PROCESS_RECORD] = array(
        'label' => t('DSpace Process Record'),
        'controller class' => 'EntityAPIController',
        'base table' => 'dspace_process_record',
        'fieldable' => TRUE,
        'entity keys' => array(
            'id' => 'id',
            'label' => 'id',
        ),
        'view modes' => array(
            'full' => array(
                'label' => t('Full DSpace Process Record'),
                'custom settings' =>  FALSE,
            ),
        ),
        'label_callback' => 'entity_class_label',
        'uri callback' => 'dspace_process_record_uri',
        'module' => 'dspace',
        'access callback' => 'dspace_access',
    );

    $entities[DSPACE_AUTHOR] = array(
        'label' => t('Library Author'),
        'controller class' => 'EntityAPIController',
        'base table' => 'dspace_author',
        'fieldable' => TRUE,
        'entity keys' => array(
            'id' => 'id',
            'label' => 'full_name',
        ),
        'view modes' => array(
            'full' => array(
                'label' => t('Full Library Author'),
                'custom settings' =>  FALSE,
            ),
        ),
        'label_callback' => 'entity_class_label',
        'uri callback' => 'dspace_author_uri',
        'module' => 'dspace',
        'access callback' => 'dspace_access',
    );

    $entities[DSPACE_PUBLISHER] = array(
      'label' => t('Library Publisher'),
      'controller class' => 'EntityAPIController',
      'base table' => 'dspace_publisher',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'id',
        'label' => 'name',
      ),
      'view modes' => array(
        'full' => array(
          'label' => t('Full Library Publisher'),
          'custom settings' =>  FALSE,
        ),
      ),
      'label_callback' => 'entity_class_label',
      'uri callback' => 'dspace_publisher_uri',
      'module' => 'dspace',
      'access callback' => 'dspace_access',
    );

    $entities[DSPACE_ITEM] = array(
        'label' => t('Library Article'),
        'controller class' => 'EntityAPIController',
        'base table' => 'dspace_item',
        'fieldable' => TRUE,
        'entity keys' => array(
            'id' => 'id',
            'label' => 'title',
        ),
        'view modes' => array(
            'full' => array(
                'label' => t('Full Library Article'),
                'custom settings' =>  FALSE,
            ),
        ),
        'label_callback' => 'entity_class_label',
        'uri callback' => 'dspace_item_uri',
        'module' => 'dspace',
        'access callback' => 'dspace_access',
    );

    return $entities;
}

function dspace_access($op, $profile = NULL, $account = NULL) {
  switch($op){
    case "view":
      return user_access('access content', $account);
    case "create":
    case "edit":
    case "delete":
      return user_access('administer dspace', $account);
  }
}

function dspace_item_uri($item){
  return array(
    'path' => 'dspace/item/' . $item->id,
  );
}

function dspace_author_uri($item){
  return array(
    'path' => 'dspace/author/' . $item->id,
  );
}

function dspace_publisher_uri($item){
  return array(
    'path' => 'dspace/publisher/' . $item->id,
  );
}

function dspace_set_uri($item){
  return array(
    'path' => 'dspace/set/' . $item->id,
  );
}

function dspace_process_record_uri($item){
  return array(
    'path' => 'dspace/process_record/' . $item->id,
  );
}

/**
 * Implements hook_entity_property_info_alter().
 */
function dspace_entity_property_info_alter(&$info) {
    $item_properties = &$info['dspace_item']['properties'];

    $item_properties['dspace_id'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['title'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['alt_title'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['genre'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['extent'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['abstract'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['full_text'] += array(
      'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['url'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['isbn'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['issn'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['copyright'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['date_created'] += array(
      'setter callback' => 'entity_property_verbatim_set',
    );
    $item_properties['date_issued'] = array(
      'entity views field' => TRUE,
      'type' => 'date',
      'setter callback' => 'entity_property_verbatim_set',
      'getter callback' => 'entity_property_verbatim_date_get',
      'label' => 'Date Issued',
      'description' => 'Date item was added to DSpace.',
      'schema field' => 'date_issued',

    );
    $item_properties['last_modified'] = array(
      'entity views field' => TRUE,
      'type' => 'date',
      'setter callback' => 'entity_property_verbatim_set',
      'getter callback' => 'entity_property_verbatim_date_get',
      'label' => 'Last Modified',
      'description' => 'Date item was synchronized into Drupal.',
      'schema field' => 'last_modified',

    );

    $item_properties = &$info['dspace_author']['properties'];

    $item_properties['full_name'] += array(
        'setter callback' => 'entity_property_verbatim_set',
    );
}
