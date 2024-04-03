<?php

require 'loadchapter.inc.php';

if (
    empty($_REQUEST['trans'])   ||
    empty($_REQUEST['lang'])    ||
    empty($_REQUEST['book'])    ||
    empty($_REQUEST['chapter'])
   )
{
    $response = new Response('text/plain', 'Not all parameters specified');
    $response->add_header('HTTP/1.0 400 Bad Request');
    return $response;
}

$trans = $_REQUEST['trans'];
$lang = $_REQUEST['lang'];
$book = $_REQUEST['book'];
$chapter = $_REQUEST['chapter'];

if (
    (! preg_match('/^[a-z0-9_]+$/', $trans))    ||
    (! preg_match('/^[a-z_]+$/', $lang))     ||
    (! preg_match('/^[0-9a-z]+$/', $book))  ||
    (! preg_match('/^[0-9]+$/', $chapter))
)
{
    $response = new Response('text/plain', 'Invalid parameters');
    $response->add_header('HTTP/1.0 400 Bad Request');
    return $response;
}


$xml = loadChapterXML($trans, $lang, $book, $chapter);

if (empty($xml))
{
    $response = new Response('text/plain', 'Chapter does not exists');
    $response->add_header('HTTP/1.0 400 Bad Request');
    return $response;
}

return new Response('text/html', $xml);

?>
