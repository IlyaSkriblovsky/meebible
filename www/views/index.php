<?php

require_once('db.php');

$db_langs = new SQLite3(db('langs.sqlite'));
$r = $db_langs->query("SELECT * FROM langs ORDER BY selfname");
$langs = array();
while ($lang = $r->fetchArray())
{
    $lang["trans"] = array();
    $langs[$lang["code"]] = $lang;
}

$db_trans = new SQLite3(db('trans.sqlite'));
$r = $db_trans->query("SELECT * FROM translationLangs");
$trans_count = 0;
while ($trans = $r->fetchArray())
{
    $langs[$trans["langCode"]]["trans"][] = $trans;
    $trans_count++;
}

function has_translations($lang)
{
    return count($lang["trans"]) > 0;
}

$langs = array_filter($langs, "has_translations");

return new TemplateResponse('index.tpl', array(
    'langs' => $langs,
    'trans_count' => $trans_count
));

?>
