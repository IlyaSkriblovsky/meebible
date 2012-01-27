<?php

require_once('db.php');

$dom = new DomDocument("1.0");

if (empty($_REQUEST['trans']) || empty($_REQUEST['lang']))
{
    $error = $dom->appendChild($dom->createElement("error"));
    $error->appendChild($dom->createTextNode("trans and lang parameters are required"));
    return new XMLResponse($dom);
}


$trans = $_REQUEST['trans'];
$lang = $_REQUEST['lang'];



$db_trans = new SQLite3(db('trans.sqlite'));

$xml_translation = $dom->appendChild($dom->createElement("translation"));

$stmt_selectBooks = $db_trans->prepare("SELECT bookCode, bookName FROM books WHERE transCode=:transCode AND langCode=:langCode ORDER BY bookNo");
$stmt_chapterSize = $db_trans->prepare("SELECT chapterNo, verseCount FROM chapterSize WHERE transCode=:transCode AND bookCode=:bookCode ORDER BY chapterNo");

$stmt_selectBooks->bindValue(':transCode', $trans);
$stmt_selectBooks->bindValue(':langCode', $lang);
$r = $stmt_selectBooks->execute();

while ($b = $r->fetchArray())
{
    $xml_book = $xml_translation->appendChild($dom->createElement("book"));
    $xml_book->setAttribute('code', $b['bookCode']);
    $xml_book->setAttribute('name', $b['bookName']);

    $stmt_chapterSize->reset();
    $stmt_chapterSize->bindValue('transCode', $trans);
    $stmt_chapterSize->bindValue('bookCode', $b['bookCode']);
    $r2 = $stmt_chapterSize->execute();
    while ($ch = $r2->fetchArray())
    {
        $xml_chapter = $xml_book->appendChild($dom->createElement("chapter"));
        $xml_chapter->setAttribute("no", $ch['chapterNo']);
        $xml_chapter->setAttribute("verses", $ch['verseCount']);
    }
}


return new XMLResponse($dom);

?>
