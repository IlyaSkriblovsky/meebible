<?php

require_once('db.php');

$db_langs = new SQLite3(db('langs.sqlite'));
$r = $db_langs->query("SELECT * FROM langs ORDER BY engname");

$dom = new DomDocument("1.0");
$xml_metainfo = $dom->appendChild($dom->createElement("metainfo"));

$xml_languages = $xml_metainfo->appendChild($dom->createElement("languages"));

while ($row = $r->fetchArray())
{
    $xml_language = $xml_languages->appendChild($dom->createElement("language"));
    $xml_language->setAttribute('code', $row['code']);
    $xml_language->setAttribute('name', $row['engname']);
    $xml_language->setAttribute('selfname', $row['selfname']);
}

$db_trans = new SQLite3($db('trans.sqlite'));
$r = $db_trans->query("SELECT * FROM translations");

$xml_translations = $xml_metainfo->appendChild($dom->createElement("translations"));

$prep_transLangs = $db_trans->prepare("SELECT langCode FROM translationLangs WHERE transCode=:transCode");

while ($t = $r->fetchArray())
{
    $xml_trans = $xml_translations->appendChild($dom->createElement("trans"));
    $xml_trans->setAttribute('code', $t['transCode']);
    $xml_trans->setAttribute('name', $t['name']);
    $xml_trans->setAttribute('parser', $t['parser']);
    $xml_trans->setAttribute('sourceUrl', $t['sourceUrl']);
    $xml_trans->setAttribute('copyright', $t['copyright']);
    $xml_trans->setAttribute('rtl', $t['rtl']);

    $prep_transLangs->reset();
    $prep_transLangs->bindValue(':transCode', $t['transCode']);
    $tl_r = $prep_transLangs->execute();
    while ($tl = $tl_r->fetchArray())
    {
        $xml_transLang = $xml_trans->appendChild($dom->createElement("transLang"));
        $xml_transLang->setAttribute('code', $tl['langCode']);
    }
}

return new XMLResponse($dom);

?>
