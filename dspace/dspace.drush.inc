<?php
require_once("includes/dspace.variables.inc");
require_once("includes/dspace.inc");
require_once("includes/dspace.items.inc");

/**
 * Implements hook_drush_command().
 */
function dspace_drush_command() {

    $items['dspace-sync'] = array(
        'description' => 'Queues items for import, then processes import queue.',
        'aliases' => array('dss'),
    );

    $items['dspace-enable-set'] = array(
      'description' => 'Enables a DSpace set for import.',
      'aliases' => array('des'),
      'arguments' => array(
        'name' => 'The name of the set to enable',
      ),
    );

    $items['dspace-disable-set'] = array(
      'description' => 'Disables a DSpace set for import.',
      'aliases' => array('dds'),
      'arguments' => array(
        'set_name' => 'The name of the set to disable',
      ),
    );

    return $items;
}

function drush_dspace_sync(){

    echo "Queueing items for processing." . PHP_EOL;

    $sets = entity_load(DSPACE_SET);

    $last_record_process_request_array = variable_get(DSPACE_RECORD_PROCESS_REQUEST, array());

    foreach ($sets as $set){
        dspace_queue_records_for_set($set, $last_record_process_request_array);
    }

    $processed = 0;

    $queue = DrupalQueue::get(DSPACE_SYNC, TRUE);
    $to_process = $queue->numberOfItems();

    echo "$to_process items are queued for processing." . PHP_EOL;


    while($item = $queue->claimItem()) {

      try{

        //Try saving the data.
        if(dspace_process_record($item->data)) {
            //Good, we succeeded.  Delete the item as it is no longer needed.
            $queue->deleteItem($item);
        }
        else {
            //You might want to log to watchdog and delete the item
            //anyway.  We'll just ignore the failure for our example.
        }
        $processed++;

        if ($processed % 100 == 0){
           $remaining = $queue->numberOfItems();
           echo "Processed $processed items so far. $remaining remaining." . PHP_EOL;
        }

      } catch (PDOException $e){
        $queue->releaseItem($item);
        sleep(2);
      }
    }

    echo "Processed $processed items." . PHP_EOL;

}

function drush_dspace_enable_set($set_name){
  _set_set_status($set_name, 1);
}

function drush_dspace_disable_set($set_name){
  _set_set_status($set_name, 0);
}

function _set_set_status($set_name, $enable){
  $xml = get_xml_from_dspace("/request?verb=ListSets");
  if ($xml !== ""){
    $sets = entity_load(DSPACE_SET);

    /*List the different sets found in dspace */

    foreach ($xml->ListSets->set as $set_details) {
      $id = (string)$set_details->{'setSpec'};
      $name = (string)$set_details->{'setName'};


      if ($name == $set_name){
        $set = entity_load(DSPACE_SET, array($id));

        if (!$set){
          $set = entity_create(DSPACE_SET, array());
        }else {
          $set = array_shift(array_values($set));
        }

        $set->id = $id;
        $set->label = $name;
        $set->status = $enable;

        entity_save(DSPACE_Set, $set);
      }
    }
  }

}

