<?php

require_once('db.php');

$trans = $_REQUEST['trans'];
$lang = $_REQUEST['lang'];



$db_trans = new SQLite3(db('trans.sqlite'));

$stmt_selectBooks = $db_trans->prepare("SELECT bookCode, bookName FROM books WHERE transCode=:transCode AND langCode=:langCode ORDER BY bookNo");
// $stmt_chapterSize = $db_trans->prepare("SELECT chapterNo, verseCount FROM chapterSize WHERE transCode=:transCode AND bookCode=:bookCode ORDER BY chapterNo");
$stmt_chapterCount = $db_trans->prepare("SELECT count(chapterNo) FROM chapterSize WHERE transCode=:transCode AND bookCode=:bookCode");

$stmt_selectBooks->bindValue(':transCode', $trans);
$stmt_selectBooks->bindValue(':langCode', $lang);
$r = $stmt_selectBooks->execute();

$out = '';

while ($b = $r->fetchArray())
{
//    $verseCounts = array();
//
//    $stmt_chapterSize->reset();
//    $stmt_chapterSize->bindValue('transCode', $trans);
//    $stmt_chapterSize->bindValue('bookCode', $b['bookCode']);
//    $r2 = $stmt_chapterSize->execute();
//    while ($ch = $r2->fetchArray())
//    {
//        $verseCounts[] = $ch['verseCount'];
//    }
//
//    $out .= "$b[bookCode]|$b[bookName]|" . join(',', $verseCounts) . "\n";

    $stmt_chapterCount->reset();
    $stmt_chapterCount->bindValue('transCode', $trans);
    $stmt_chapterCount->bindValue('bookCode', $b['bookCode']);
    $r2 = $stmt_chapterCount->execute();
    $row = $r2->fetchArray();

    $out .= "$b[bookCode]|$b[bookName]|$row[0]\n";
}


$db_trans->close();

return new Response('text/plain; charset=utf-8', $out);

?>
