<?php

require_once('db.php');

$response = "";

$db_langs = new SQLite3(db('langs.sqlite'));
$r = $db_langs->query("SELECT * FROM langs ORDER BY engname");

while ($row = $r->fetchArray())
{
    $response .= "lang|$row[code]|$row[engname]|$row[selfname]\n";
}

$db_trans = new SQLite3(db('trans.sqlite'));
$r = $db_trans->query("SELECT * FROM translations");

$prep_transLangs = $db_trans->prepare("SELECT langCode, name FROM translationLangs WHERE transCode=:transCode");

while ($t = $r->fetchArray())
{
    $response .= "trans|$t[transCode]|$t[sourceUrl]|$t[copyright]|$t[rtl]\n";

    $prep_transLangs->reset();
    $prep_transLangs->bindValue(':transCode', $t['transCode']);
    $tl_r = $prep_transLangs->execute();
    while ($tl = $tl_r->fetchArray())
    {
        $response .= "transLang|$tl[langCode]|$tl[name]\n";
    }
}

$db_langs->close();
$db_trans->close();

return new Response('text/plain; charset=utf-8', $response);

?>
