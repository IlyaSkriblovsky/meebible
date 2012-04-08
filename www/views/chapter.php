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
    (! preg_match('/^[0-9a-z]+$/', $book))  ||
    (! preg_match('/^[0-9]+$/', $chapter))
)
{
    $response = new Response('text/plain', 'Invalid parameters');
    $response->add_header('HTTP/1.0 400 Bad Request');
    return $response;
}


if ($trans != 'nwt')
{
    $db = new SQLite3("content/$trans.sqlite");
    $stmt = $db->prepare("SELECT html FROM html WHERE langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo");
    $stmt->reset();
    $stmt->bindValue(':langCode', $lang);
    $stmt->bindValue(':bookCode', $book);
    $stmt->bindValue(':chapterNo', $chapter, SQLITE3_INTEGER);
    $r = $stmt->execute();
    $row = $r->fetchArray();
    if (empty($row))
    {
        $response = new Response('text/plain', 'Chapter does not exists');
        $response->add_header('HTTP/1.0 400 Bad Request');
        return $response;
    }

    $html = $row[0];

    $db->close();

    return new Response('text/html', $html);
}
else
{
    $url_prefixes = array(
        "af" =>  "/af/bybel/",
        "al" =>  "/al/bible/",
        "bl" =>  "/bl/bible/",
        "cn" =>  "/cn/baibulo/",
        "chs" => "/chs/bible/",
        "ch" =>  "/ch/bible/",
        "cw" =>  "/cw/baibolo/",
        "c" =>   "/c/biblija/",
        "b" =>   "/b/bible/",
        "d" =>   "/d/bibelen/",
        "o" =>   "/o/bijbel/",
        "e" =>   "/e/bible/",
        "ef" =>  "/ef/bible/",
        "fi" =>  "/fi/raamattu/",
        "f" =>   "/f/bible/",
        "ge" =>  "/ge/bible/",
        "x" =>   "/x/bibel/",
        "g" =>   "/g/bible/",
        "h" =>   "/h/biblia/",
        "in" =>  "/in/alkitab/",
        "i" =>   "/i/bibbia/",
        "j" =>   "/j/bible/",
        "ko" =>  "/ko/bible/",
        "li" =>  "/li/biblia/",
        "mg" =>  "/mg/baiboly/",
        "mt" =>  "/mt/bibbja/",
        "n" =>   "/n/bibelen/",
        "oss" => "/oss/bible/",
        "p" =>   "/p/biblia/",
        "t" =>   "/t/biblia/",
        "m" =>   "/m/biblia/",
        "u" =>   "/u/bible/",
        "rea" => "/rea/bible/",
        "sb" =>  "/sb/bible/",
        "su" =>  "/su/bibele/",
        "sv" =>  "/sv/svetopismo/",
        "s" =>   "/s/biblia/",
        "sr" =>  "/sr/bijbel/",
        "sw" =>  "/sw/biblia/",
        "z" =>   "/z/bibeln/",
        "tg" =>  "/tg/bibliya/",
        "tn" =>  "/tn/baebele/",
        "tk" =>  "/tk/kutsalkitap/",
        "xo" =>  "/xo/ibhayibhile/",
        "yr" =>  "/yr/bibeli/",
        "zu" =>  "/zu/ibhayibheli/",
    );

    $wt_url = "http://www.watchtower.org" . $url_prefixes[$lang] . $book . "/chapter_" . str_pad($chapter, 3, '0', STR_PAD_LEFT) . ".htm";
    $content = file_get_contents($wt_url);

    $content = str_replace('&acute;', '', $content);
    $content = str_replace('&middot;', '', $content);
    $content = str_replace('&copy;', '©', $content);
    $content = str_replace('&nbsp;', '&#160;', $content);

    $in = new DOMDocument();
    $in->loadXML($content);

    $xslt = file_get_contents('content/nwt.xslt');
    $proc = new XSLTProcessor();
    $proc->importStylesheet(new SimpleXMLElement($xslt));
    $out = $proc->transformToXml($in);

    $response = new Response('text/html; charset=utf-8', $out);
    return $response;
}

?>
