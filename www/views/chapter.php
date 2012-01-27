<?php

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
    (! preg_match('/^[a-z]+$/', $trans))    ||
    (! preg_match('/^[a-z]+$/', $lang))     ||
    (! preg_match('/^[a-z]+$/', $book))     ||
    (! preg_match('/^[0-9]+$/', $chapter))
)
{
    $response = new Response('text/plain', 'Invalid parameters');
    $response->add_header('HTTP/1.0 400 Bad Request');
    return $response;
}

$filename = "chapters/$trans/$lang/${book}_" . str_pad($chapter, 3, '0', STR_PAD_LEFT) . '.html';

if (file_exists($filename))
    return new Response('text/html', file_get_contents($filename));

$response = new Response('text/plain', 'Chapter does not exists');
$response->add_header('HTTP/1.0 400 Bad Request');
return $response;

?>
