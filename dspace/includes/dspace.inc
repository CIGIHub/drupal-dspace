<?php

function getDSpaceUrl()
{
    $port = variable_get('dspace_port', NULL);
    $host = variable_get('dspace_url', NULL);
    $extension = variable_get('dspace_extension', NULL);

    return $host .":" . $port . "/" . $extension ;

}

/*
*helper function to get the Response code of the page
*/
function getHttpResponseCode($url)
{
    $ch = @curl_init($url);
    @curl_setopt($ch, CURLOPT_HEADER, TRUE);
    @curl_setopt($ch, CURLOPT_NOBODY, TRUE);
    @curl_setopt($ch, CURLOPT_FOLLOWLOCATION, FALSE);
    @curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);

    @curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 10); // 10 second timeout


    $status = array();
    $response = @curl_exec($ch);
    if ($response == FALSE){
        return $response;
    }

    preg_match('/HTTP\/.* ([0-9]+) .*/', $response, $status);

    if (count($status) > 1){
        return $status[1];
    }
}

function get_xml_from_dspace($path, $xlink_href=FALSE){
    $url = getDSpaceUrl() . $path;

    //check status of page
    if (getHttpResponseCode($url) == 200)
    {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);

        $xml_response = curl_exec($ch);
        $info = curl_getinfo($ch);
        curl_close($ch);
        if ($xlink_href){
            $xml_response = str_replace("xlink:href", "href", str_replace("mods:", "", $xml_response));
        }

        $xml_response = str_replace('xmlns=', 'ns=', $xml_response);

        $xml = new SimpleXMLElement($xml_response);
        return $xml;
    }
    else
    {
        $page_content = "URL used: " . $url . "<br><br>";
        drupal_set_message(t("There was an error connecting to the DSpace server. $page_content. Please check the configuration and try again."), "error");
        return "";
    }
}

function translate_dspace_date_for_db($datetime){
    $datetime = str_replace("T", " ", $datetime);
    $datetime = str_replace("Z", " ", $datetime);
    return $datetime;
}

function human_to_machine($human_name) {
    return strtolower(preg_replace(array(
        '/[^a-zA-Z0-9]+/',
        '/-+/',
        '/^-+/',
        '/-+$/',
    ), array('-', '-', '', ''), $human_name));
}
