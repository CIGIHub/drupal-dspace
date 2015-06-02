<?php

function dspace_items() {
    $form = array();

    $queue = DrupalQueue::get("dspace_sync", TRUE);
    $count = $queue->numberOfItems();

    $form['dspace_items'] = array(
        '#type' => 'fieldset',
        '#title' => t('DSpace Items'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
        '#description' => t("There are $count items to import, update, or delete."),
    );

    $form['queue_submit'] = array('#type' => 'submit', '#value' => t('Queue Imports'), '#name' => 'queue');
    $form['import_submit'] = array('#type' => 'submit', '#value' => t('Start Import'), '#name' => 'import');
    return $form;
}

function dspace_items_submit($form, &$form_state) {
    if ($form_state['triggering_element']['#name'] == 'queue'){
        dspace_queue_records_for_processing();
    }
    elseif ($form_state['triggering_element']['#name'] == 'import'){
        dspace_process_import_records();
    }

}

function dspace_get_title($itemMetaData){
    return (string)$itemMetaData->titleInfo;
}

function dspace_get_alternate_title($itemMetaData){
    $xml_nodes = $itemMetaData->xpath('//titleInfo[@type="alternative"]');
    if (count($xml_nodes) > 0){
        return (string)($xml_nodes[0]);
    }
    return "";
}

function dspace_get_authors($itemMetaData){
    $authors = array();
    $names = $itemMetaData->name;
    foreach ($names as $name)
    {
        $text_name = (string)$name->namePart;

        if ($text_name != "" && $text_name != NULL){
            $author = entity_load(DSPACE_AUTHOR, FALSE, array('full_name' => $text_name), TRUE);

            if (!$author){
                $author = entity_create(DSPACE_AUTHOR, array());
            }else {
                $author_array = array_values($author);
                $author = $author_array[0];
            }

            $name_parts = explode(",", $text_name);

            $author->full_name = $text_name;
            $author->last_name = $name_parts[0];
            if (count($name_parts) > 1){
                $author->first_name = $name_parts[1];
            }


            $success = entity_save(DSPACE_AUTHOR, $author);

            if ($success){
                $authors[] = $author->id;
            } else {
                drupal_set_message("Error saving author", "error");
            }
        }

    }

    return $authors;
}

function dspace_get_genre($itemMetaData){
    return (string)$itemMetaData->genre;
}

function dspace_get_extent($itemMetaData){
    return (string)$itemMetaData->physicalDescription->extent;
}

function dspace_get_abstract($itemMetaData){
    return (string)$itemMetaData->abstract;
}

function dspace_get_url($itemMetaData){
    $xml_nodes = $itemMetaData->xpath('//fileGrp[@USE="ORIGINAL"]/file/FLocat[@LOCTYPE="URL"]/@href');
    if (count($xml_nodes) > 0){
        $href = $xml_nodes[0];
        return (string)$href;
    }
    return "";
}

function dspace_get_full_text($itemMetaData){
  $xml_nodes = $itemMetaData->xpath('//fileGrp[@USE="TEXT"]/file/FLocat[@LOCTYPE="URL"]/@href');
  if (count($xml_nodes) > 0){
    $href = $xml_nodes[0];

    $ch2 = curl_init();
    curl_setopt($ch2, CURLOPT_URL, $href);
    curl_setopt($ch2, CURLOPT_RETURNTRANSFER, TRUE);
    curl_setopt($ch2, CURLOPT_CONNECTTIMEOUT, 10); // 10 second timeout
    curl_setopt($ch2, CURLOPT_FOLLOWLOCATION, true);
    $text = curl_exec($ch2);

    if($text === false)
    {
      watchdog ("dspace", "cURL Error: " . curl_error($ch2) , NULL, WATCHDOG_ERROR , NULL);
      return "Error";
    }

    curl_close($ch2);

    return (string)$text;
  }
  return "";
}

function dspace_get_issued_date($itemMetaData){
    $xml_nodes = $itemMetaData->xpath('//originInfo/dateIssued');
    if (count($xml_nodes) > 0){
        return translate_dspace_date_for_db((string)($xml_nodes[0]));
    }
    return "";
}

function dspace_get_created_date($itemMetaData){
  $xml_nodes = $itemMetaData->xpath('//originInfo/dateCreated');
  if (count($xml_nodes) > 0){
    return (string)($xml_nodes[0]);
//    return translate_dspace_date_for_db((string)($xml_nodes[0]));
  }
  return "";
}

function dspace_get_publisher($itemMetaData){

    $publishers = array();
    $names = $itemMetaData->xpath('//originInfo/publisher');
    foreach ($names as $name)
    {
      $text_name = (string)$name;

      if ($text_name != "" && $text_name != NULL){
        $publisher = entity_load(DSPACE_PUBLISHER, FALSE, array('name' => $text_name), TRUE);

        if (!$publisher){
          $publisher = entity_create(DSPACE_PUBLISHER, array());
        }else {
          $publisher_array = array_values($publisher);
          $publisher = $publisher_array[0];
        }

        $publisher->name = $text_name;

        $success = entity_save(DSPACE_PUBLISHER, $publisher);

        if ($success){
          $publishers[] = $publisher->id;
        } else {
          drupal_set_message("Error saving publisher", "error");
        }
      }

    }

    return $publishers;

}

function dspace_get_isbn($itemMetaData){
    $isbn_nodes = $itemMetaData->xpath('//identifier[@type="isbn"]');
    if (count($isbn_nodes) > 0){
        return (string)($isbn_nodes[0]);
    }
    return "";
}

function dspace_get_issn($itemMetaData){
    $isbn_nodes = $itemMetaData->xpath('//identifier[@type="issn"]');
    if (count($isbn_nodes) > 0){
        return (string)($isbn_nodes[0]);
    }
    return "";
}

function dspace_get_copyright($itemMetaData){
    $xml_nodes = $itemMetaData->xpath('//accessCondition[@type="copyright"]');
    if (count($xml_nodes) > 0){
        return (string)($xml_nodes[0]);
    }
    return "";
}

function dspace_get_last_modified_timestamp($xml){
    return translate_dspace_date_for_db((string)$xml->GetRecord->record->header->datestamp);
}

function dspace_get_languages($itemMetaData){
  $vocabulary = taxonomy_vocabulary_machine_name_load(DSPACE_ITEM_LANGUAGE);
  $xml_nodes = $itemMetaData->xpath('//language/languageTerm');
  return _dspace_get_taxonomy_terms($xml_nodes, $vocabulary);
}

function dspace_get_subjects($itemMetaData){
    $vocabulary = taxonomy_vocabulary_machine_name_load(DSPACE_ITEM_SUBJECT);
    $xml_nodes = $itemMetaData->xpath('//subject[@authority="lcsh"]');
    return _dspace_get_taxonomy_terms($xml_nodes, $vocabulary);
}

function dspace_get_topics($itemMetaData){
    $vocabulary = taxonomy_vocabulary_machine_name_load(DSPACE_ITEM_TOPIC);
    $xml_nodes = $itemMetaData->xpath('//topic');
    return _dspace_get_taxonomy_terms($xml_nodes, $vocabulary);
}

function dspace_get_regions($itemMetaData){
    $vocabulary = taxonomy_vocabulary_machine_name_load(DSPACE_ITEM_REGION);
    $xml_nodes = $itemMetaData->xpath('//geographic');
    return _dspace_get_taxonomy_terms($xml_nodes, $vocabulary);
}

function _dspace_get_taxonomy_terms($xml_nodes, $vocabulary){
    $terms = array();
    if (count($xml_nodes) > 0){

        foreach ($xml_nodes as $term_node){
            $term_name = (string)$term_node;
//            $machine_term = human_to_machine($term_name);
            $machine_term = $term_name;

            $existing_terms = taxonomy_get_term_by_name($machine_term, $vocabulary->machine_name);
            if (count($existing_terms) == 0){
                $term = new stdClass();
                $term->name = $machine_term;
                $term->vid = $vocabulary->vid;
                $term->description = $term_name;

                taxonomy_term_save($term);
            } else {
                $term_array = array_values($existing_terms);
                $term = $term_array[0];
            }

            $terms[] = $term->tid;
        }
    }
    return $terms;
}

function dspace_get_existing_record($id){
    $query = new EntityFieldQuery();
    $query->entityCondition("entity_type", DSPACE_ITEM, "=")
        ->propertyCondition("dspace_id", $id, "=");

    $result_list = $query->execute();

    if ($result_list){
        $results_array = array_values($result_list);
        $first_result = $results_array[0];
        $articles_array = array_values($first_result);
        $article_result = $articles_array[0];
        $article = entity_load(DSPACE_ITEM, array($article_result->id));
        $article_entity_array = array_values($article);
        $article = $article_entity_array[0];

        return $article;
    }
    return NULL;
}

function set_is_active($set_id){
  $sets = entity_load(DSPACE_SET, array($set_id));
  if ($sets){
    $set_array = array_values($sets);
    $set = $set_array[0];
    return ($set->status == 1);
  }
  return FALSE;
}

function dspace_process_record($record){
    $id = $record->id;

    $set_id = $record->set_id;
    if (!set_is_active($set_id)){
      return TRUE;
    }

    $article = dspace_get_existing_record($id);

    if ($record->status == 1){
        if ($article != NULL){
            entity_delete(DSPACE_ITEM, $article->id);

            watchdog('Item deletion', "The article $article->id was marked for deletion.");
        }
    } else {
        $metadataPrefix = variable_get('dspace_metadata_prefix', NULL);
        $xml = get_xml_from_dspace("/request?verb=GetRecord&metadataPrefix=$metadataPrefix&identifier=$id", TRUE);
        if (!isset($xml) || empty($xml)){
          return FALSE;
        }
        $itemMetaData = $xml->GetRecord->record->metadata->mets->dmdSec->mdWrap->xmlData;


        $title = dspace_get_title($itemMetaData);
        $alt_title = dspace_get_alternate_title($itemMetaData);
        $authors = dspace_get_authors($itemMetaData);
        $genre = dspace_get_genre($itemMetaData);
        $extent = dspace_get_extent($itemMetaData);
        $abstract = dspace_get_abstract($itemMetaData);
        $url = dspace_get_url($itemMetaData);
        $date_issued = dspace_get_issued_date($itemMetaData);
        $date_created = dspace_get_created_date($itemMetaData);
        $publishers = dspace_get_publisher($itemMetaData);
        $languages = dspace_get_languages($itemMetaData);
        $isbn = dspace_get_isbn($itemMetaData);
        $issn = dspace_get_issn($itemMetaData);
        $copyright = dspace_get_copyright($itemMetaData);
        $last_modified = dspace_get_last_modified_timestamp($xml);
        $subjects = dspace_get_subjects($itemMetaData);
        $topics = dspace_get_topics($itemMetaData);
        $regions = dspace_get_regions($itemMetaData);
        $full_text = dspace_get_full_text($itemMetaData);
        if ($full_text == "Error"){
          $full_text = dspace_get_full_text($itemMetaData);
          if ($full_text == "Error"){
            return FALSE;
          }
        }

        if ($article == NULL) {
            $article = entity_create(DSPACE_ITEM, array());
        }

        $article->last_modified = $last_modified;
        $article->date_issued = $date_issued;
//



        $article = entity_metadata_wrapper(DSPACE_ITEM, $article);

        $article->dspace_id->set($id);
        $article->title->set($title);
        $article->alt_title->set($alt_title);
        $article->genre->set($genre);
        $article->extent->set($extent);
        $article->abstract->set($abstract);
        $article->full_text->set($full_text);
        $article->url->set($url);
        $article->isbn->set($isbn);
        $article->issn->set($issn);
        $article->copyright->set($copyright);
        $article->date_created->set($date_created);

        if (!empty($publishers)){
          $article->publisher->set($publishers);
        }

        if (!empty($authors)){
          $article->author->set($authors);
        }

        if (!empty($subjects)){
          $article->dspace_subject->set($subjects);
        }
        if (!empty($topics)){
          $article->dspace_topic->set($topics);
        }
        if (!empty($regions)){
          $article->dspace_region->set($regions);
        }
        if (!empty($languages)){
          $article->dspace_language->set($languages);
        }
        $article->save();
    }

    entity_delete(DSPACE_PROCESS_RECORD, $record->id);

    return TRUE;
}

function dspace_process_import_records(){
    $max_records_to_process = variable_get('dspace_interactive_import_limit');
    $processed = 0;

    $queue = DrupalQueue::get(DSPACE_SYNC, TRUE);

    while(($item = $queue->claimItem()) &&
        $processed < $max_records_to_process) {

        //Try saving the data.
        if(dspace_process_record($item->data)) {
            //Good, we succeeded.  Delete the item as it is no longer needed.
            $queue->deleteItem($item);
        }
        else {
            watchdog("dspace_sync", "Unable to process $item->data->id.");
        }
        $processed++;
    }

}

function dspace_queue_records_for_set($set, $last_record_process_request_array){
    if ($set->status == "1"){
        $metadataPrefix = variable_get('dspace_metadata_prefix', NULL);

        $set_id = $set->id;

        if (array_key_exists($set_id, $last_record_process_request_array)){
            $last_record_process_request = $last_record_process_request_array[$set_id];
        } else{
            $last_record_process_request = '1970-01-01T00:00:00Z';
        }

        $xml = get_xml_from_dspace(
            "/request?verb=ListIdentifiers&metadataPrefix=$metadataPrefix&set=$set_id&from=$last_record_process_request");

        $original_response_date = (string)$xml->{'responseDate'};
        $response_date = str_replace("T", " ", $original_response_date);
        $response_date = str_replace("Z", " ", $response_date);

        if (isset($xml->ListIdentifiers)){

            $queue = DrupalQueue::get("dspace_sync", TRUE);

            foreach ($xml->ListIdentifiers->header as $header) {
                $id = (string)$header->{'identifier'};
                $datestamp = (string)$header->{'datestamp'};
                $datestamp = str_replace("T", " ", $datestamp);
                $datestamp = str_replace("Z", " ", $datestamp);
                $delete = (string)$header->attributes()->status == "deleted" ? 1 : 0;

                $record = entity_load(DSPACE_PROCESS_RECORD, array($id));
                if (!$record){
                    $values = array('id' => $id,
                        'set_id' => $set_id,
                        'status' => $delete,
                        'last_modified' => $datestamp,
                        'queued' => $response_date,
                    );

                    $record = entity_create(DSPACE_PROCESS_RECORD, $values);
                    $success = entity_save(DSPACE_PROCESS_RECORD, $record);

                    if ($success){
                        $queue->createItem($record);
                    }

                }
            }
        }

        $last_record_process_request_array[$set_id] = $original_response_date;
        variable_set(DSPACE_RECORD_PROCESS_REQUEST, $last_record_process_request_array);
    }
}

function dspace_queue_records_for_processing(){

    $sets = entity_load(DSPACE_SET);

    $last_record_process_request_array = variable_get(DSPACE_RECORD_PROCESS_REQUEST, array());

    foreach ($sets as $set){
        dspace_queue_records_for_set($set, $last_record_process_request_array);
    }

}
