<?php
require_once("includes/dspace.variables.inc");
require_once("includes/dspace.items.inc");

function dspace_cron(){
    $queue = DrupalQueue::get(DSPACE_QUEUE, TRUE);

    $sets = entity_load(DSPACE_SET);

    foreach ($sets as $set){
        $queue->createItem($set);
    }
}

function dspace_cron_queue_info(){
    $queues[DSPACE_SYNC] = array(
      'worker callback' => 'dspace_synchronize_item',
      'time' => 60,
    );

    $queues[DSPACE_QUEUE] = array(
        'worker callback' => 'dspace_queue_items',
        'time' => 240,
    );

    return $queues;
}

function dspace_synchronize_item($record){
    dspace_process_record($record);
}

function dspace_queue_items($set){
    $last_record_process_request_array = variable_get(DSPACE_RECORD_PROCESS_REQUEST, array());
    dspace_queue_records_for_set($set, $last_record_process_request_array);
}
