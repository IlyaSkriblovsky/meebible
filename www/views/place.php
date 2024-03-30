<?php
    require 'loadchapter.inc.php';



    function parseVerseSpec($spec)
    {
        $verseNums = array();

        $ranges = explode(',', $spec);

        foreach ($ranges as $range)
        {
            $edges = explode('-', $range);
            if (count($edges) == 1)
            {
                $v = intval($edges[0]);
                if ($v > 0 && $v <= 300)
                    $verseNums[] = $v;
            }
            else if (count($edges) == 2)
            {
                $from = intval($edges[0]);
                $to = intval($edges[1]);

                if ($from < 1 || $from > 300) continue;
                if ($to < 1 || $to > 300) continue;

                for ($i = $from; $i <= $to; $i++)
                    $verseNums[] = $i;
            }
        }

        $verseNums = array_unique($verseNums);
        sort($verseNums);

        return $verseNums;
    }


    function genVerseSpec($verseNums)
    {
        $cur = 0;

        $ranges = array();

        while ($cur < count($verseNums))
        {
            $begin = $cur;
            do {
                $cur++;
            } while ($cur < count($verseNums) && $verseNums[$cur-1] + 1 == $verseNums[$cur]);

            $end = $cur - 1;

            if ($begin == $end)
                $ranges[] = $verseNums[$begin];
            else
                $ranges[] = $verseNums[$begin] . '-' . $verseNums[$end];
        }

        return implode(',', $ranges);
    }


    if (! preg_match('/^\/t\/([a-zA-Z0-9_-]+)\/([a-zA-Z0-9_-]+)\/([a-z]+)\/(\d+)\/([0-9,-]+)/', $_SERVER['REQUEST_URI'], $matches))
    {
        $response = new Response('text/plain', 'Not all parameters specified');
        $response->add_header('HTTP/1.0 400 Bad Request');
        return $response;
    }


    $trans   = $matches[1];
    $lang    = $matches[2];
    $book    = $matches[3];
    $chapter = $matches[4];
    $verses  = $matches[5];

    $xml = loadChapterXML($trans, $lang, $book, $chapter);


    $verseNum = parseVerseSpec($verses);

    if (count($verseNum) == 0)
    {
        $response = new Response('text/plain', 'Strange verse specification');
        $response->add_header('HTTP/1.0 400 Bad Request');
        return $response;
    }

    $verseSpec = genVerseSpec($verseNum);

    if ($verses != $verseSpec)
    {
        return new Redirect("/t/$trans/$lang/$book/$chapter/$verseSpec");
    }


    $dom = new DOMDocument;
    $dom->loadHTML($xml);

    $xpath = new DomXPath($dom);


    $out = new DOMDocument;
    $outRoot = $out->appendChild($out->createElement('div'));



    $prevParent = null;
    $outParent = null;

    for ($i = 0; $i < count($verseNum); $i++)
    {
        if ($i > 0 && $verseNum[$i] != $verseNum[$i-1] + 1)
        {
            $hole = $out->createElement('span');
            $hole->setAttribute('class', 'hole');
            $hole->appendChild($out->createTextNode('[...]'));
            $outRoot->appendChild($hole);
        }

        $nodes = array();

        if ($verseNum[$i] == 1)
            foreach ($xpath->query(".//*[@class='superscript']") as $n)
                $nodes[] = $n;

        foreach ($xpath->query(".//*[@verse='" . $verseNum[$i] . "']") as $n)
            $nodes[] = $n;


        foreach ($nodes as $node)
        {
            if ($node->parentNode !== $prevParent)
            {
                $outParent = $outRoot->appendChild($out->importNode($node->parentNode, false));

                $prevParent = $node->parentNode;
            }

            $outParent->appendChild($out->importNode($node, true));
            $outParent->appendChild($out->createTextNode(' '));
        }
    }



    $versesHtml = trim($out->saveHTML());


    $versesText = '';
    $outXPath = new DomXPath($out);
    foreach ($outXPath->query(".//*[not(@class='verse-label')]/text()") as $node)
    {
        $versesText .= $node->nodeValue;
        $versesText .= ' ';
    }

    $versesText = trim($versesText);
    $versesText = preg_replace('/\s\s+/', ' ', $versesText);


    $bookName = bookName($trans, $lang, $book);

    $place = str_replace('-', '–', "$bookName $chapter:$verseSpec");


    return new TemplateResponse('place.tpl', array(
        'html' => $versesHtml,
        'text' => $versesText,

        'place' => $place,
    ));
?>
