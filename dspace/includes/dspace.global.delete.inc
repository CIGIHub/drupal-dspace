<?php
/**
 * @file
 * Resets all dspace data.
 */

require_once("dspace.variables.inc");

/**
 *
 */
function dspace_global_delete() {
  
  $form = array();

  $question = 'Are you sure you want to delete all DSpace data, including imported DSpace articles and DSpace authors?';
		 

  return confirm_form($form, check_plain($question), "admin/settings/dspace/setup", NULL, "Delete" );
}

/**
 * Clear the filter's cache when configuration settings are saved.
 */
function dspace_global_delete_submit($form, &$form_state) {
  #TODO: this takes too long to process.
  $sync_queue = DrupalQueue::get(DSPACE_SYNC, TRUE);
  $sync_queue->deleteQueue();

  $sync_queue = DrupalQueue::get(DSPACE_SYNC, TRUE);
  $sync_queue->createQueue();

  $dspace_queue = DrupalQueue::get(DSPACE_QUEUE, TRUE);
  $dspace_queue->deleteQueue();

  $dspace_queue = DrupalQueue::get(DSPACE_QUEUE, TRUE);
  $dspace_queue->createQueue();

  foreach (array(DSPACE_ITEM, DSPACE_AUTHOR, DSPACE_SET, DSPACE_PUBLISHER, DSPACE_PROCESS_RECORD) as $entity_type){
    $entities = entity_load($entity_type);
    $entity_ids = array_keys($entities);
    entity_delete_multiple($entity_type, $entity_ids);
  }

  drupal_set_message(t('All DSpace data has been deleted.'));
  cache_clear_all();
}
