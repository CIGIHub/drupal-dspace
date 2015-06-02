<?php
/**
 * @file
 * Verifies the dspace connection settings.
 */

function dspace_test_connection() {

  $xml = get_xml_from_dspace("/request?verb=Identify");

  if ($xml !== ""){
      $page_content =  $xml->asXML();
      drupal_set_message(t("You have successfully connected to the DSpace Server."), "status");
      return $page_content;
  }

  return "";
}
