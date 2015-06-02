<?php

/**
 * @file
 * Creates the dspace connection settings.
 */

function dspace_admin() {
    $form = array();

    $form['dspace_server'] = array(
        '#type' => 'fieldset',
        '#title' => t('DSpace Connection'),
        '#description' => t('Enter in the appropriate info to connect to the DSpace server.'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
    );

    $form['dspace_server']['dspace_url'] = array(
        '#type' => 'textfield',
        '#title' => t('Url'),
        '#default_value' => variable_get('dspace_url', ''),
        '#size' => 80,
        '#maxlength' => 80,
        '#description' => t("Do not include the trailing slash. For example, http://www.example.com "),
        '#required' => TRUE,
    );

    $form['dspace_server']['dspace_port'] = array(
        '#type' => 'textfield',
        '#title' => t('Port'),
        '#size' => 6,
        '#maxlength' => 5,
        '#default_value' => variable_get('dspace_port', '80'),
        '#description' => t('The standard port is 80.'),
        '#required' => TRUE,
    );

    $form['dspace_server']['dspace_extension'] = array(
        '#type' => 'textfield',
        '#title' => t('Extention'),
        '#size' => 6,
        '#maxlength' => 80,
        '#default_value' => variable_get('dspace_extension', 'oai'),
        '#description' => t('The extension for pulling data using oai-phm. Default is oai.'),
        '#required' => FALSE,
    );

    $form['dspace_server']['dspace_metadata_prefix'] = array(
        '#type' => 'textfield',
        '#title' => t('Metadata Prefix'),
        '#size' => 6,
        '#maxlength' => 80,
        '#default_value' => variable_get('dspace_metadata_prefix', 'mets'),
        '#description' => t('The metadata prefix for oai-phm. Default is mets. Options include mets, oai_dc, rdf.'),
        '#required' => FALSE,

    );

    $form['dspace_server']['dspace_interactive_import_limit'] = array(
        '#type' => 'select',
        '#title' => t('Interactive Import Limit'),
        '#default_value' => variable_get('dspace_interactive_import_limit', '100'),
        '#options' => array(
            '0' => 0,
            '1' => 1,
            '2' => 2,
            '5' => 5,
            '10' => 10,
            '15' => 15,
            '20' => 20,
            '25' => 25,
            '50' => 50,
            '100' => 100,
            '250' => 250,
            '500' => 500,
        ),
        '#description' => t('The amount of dspace articles to pull on each cron run.'),
        '#required' => TRUE,
    );

    $form['dspace_server']['dspace_username'] = array(
        '#type' => 'textfield',
        '#title' => t('Username'),
        '#default_value' => variable_get('dspace_username', ''),
        '#size' => 80,
        '#maxlength' => 80,
        '#required' => FALSE,
        '#disabled' => TRUE,
    );

    $form['dspace_server']['dspace_password'] = array(
        '#type' => 'password',
        '#title' => t('Password'),
        '#size' => 80,
        '#maxlength' => 80,
        '#default_value' => variable_get('dspace_password', ''),
        '#required' => FALSE,
        '#disabled' => TRUE,
        '#suffix' => t("<a href='/admin/settings/dspace/delete?destination=/admin/settings/dspace'>Click here to reset ALL dspace data</a>"),
    );




    return system_settings_form($form);
}

/* validate the form */
function dspace_admin_validate($form, &$form_state) {
    $portnum = $form_state['values']['dspace_port'];
    if (!is_numeric( $portnum)) {
        form_set_error('dspace_port', t('You must enter an integer for the port number.'));
    }
    elseif ($portnum <= 0) {
        form_set_error('dspace_setup', t('Port number must be positive.'));
    }
}
