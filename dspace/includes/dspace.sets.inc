<?php

function dspace_sets() {
    $form = array();

    //connect to dspace and display the sets. Check off the sets the user wishes to save to the database.
    $xml = get_xml_from_dspace("/request?verb=ListSets");

    if ($xml !== ""){
        $new_articles = 0;
        $form['dspace_sets'] = array(
            '#type' => 'fieldset',
            '#title' => t('DSpace Sets'),
            '#description' => "<p>Select the DSpace sets to use. Once saved,
            run cron to import data.<br><br>There are currently <b>".
                $new_articles ."</b> new articles left to import.</p>",
            '#collapsible' => FALSE,
            '#collapsed' => FALSE,
            '#tree' => TRUE,
        );

        $sets = entity_load(DSPACE_SET);

        /*List the different sets found in dspace */
        $set_options = array();
        $set_values = array();

        foreach ($xml->ListSets->set as $set) {
            $id = (string)$set->{'setSpec'};
            $name = (string)$set->{'setName'};

            $set_options[$id] = $name;

            if (array_key_exists($id, $sets)){
                if ($sets[$id]->status == "1"){
                    $set_values[$id] = $id;
                }

            }
        }

        $form['dspace_sets']['sets'] = array(
            '#type' => 'checkboxes',
            '#title' => t('Sets'),
            '#options' => $set_options,
            '#default_value' => $set_values,
        );

        $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
    }

    return $form;

}


function dspace_sets_submit($form, &$form_state) {
    $errors = FALSE;
    $sets = entity_load(DSPACE_SET, False, array(), TRUE);

    foreach ($form_state['values']['dspace_sets']['sets'] as $id => $value)
    {
        $status = ($value == '0') ? 0 : 1;
        $label = $form["dspace_sets"]["sets"]["#options"][$id];

        if (array_key_exists($id, $sets)){
            // Update existing set
            $set = $sets[$id];
            $set->status = $status;
            $set->label = $label;
        } else {
            // Create a new set
            $values = array('id' => $id,
                            'status' => $status,
                            'label' => $label,
                           );
            $set = entity_create(DSPACE_SET, $values);
        }

        $success = entity_save(DSPACE_SET, $set);

        if (!$success){
            drupal_set_message(t("There was an error saving set: $id."), 'error');
            $errors = TRUE;
        }
    }

    if (!$errors){
        drupal_set_message(t('DSpace sets have been saved.'));
    }
}
