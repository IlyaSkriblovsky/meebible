<?php

$response = require("views/chapter.php");

$dom = new DomDocument();
$dom->loadHTML($response->content);

$body = $dom->getElementsByTagName('body')->item(0);;


function process($node) {
    $childResult = '';
    $child = $node->firstChild;
    while ($child) {
        $childResult .= process($child);
        $child = $child->nextSibling;
    }

    if ($node->hasAttributes() && $node->getAttribute("class") == "par")
        $result = $childResult . "|\n";
    elseif ($node->hasAttributes() && $node->getAttribute('class') == 'verse-label')
        $result = '{' . $childResult . '}';
    elseif ($node->hasAttributes() && $node->getAttribute('class') == 'z')
        $result = '^N' . $childResult . '^n';
    elseif ($node->hasAttributes() && $node->getAttribute('class') == 'small') {
        if ($childResult == 'OU')
            $result = '' . $childResult . ' ';
        else
            $result = ' ' . $childResult . ' ';
    }
    elseif ($node->nodeName == "b")
        $result = ' ^B' . $childResult . '^b ';
    elseif ($node->nodeName == "i")
        $result = ' ^I' . $childResult . '^i ';
    elseif ($node->nodeName == "u")
        $result = ' ^U' . $childResult . '^u ';
    elseif ($node->nodeType == XML_TEXT_NODE) {
        $result = trim($node->textContent);
        $result = str_replace('{', '\{', $result);
        $result = str_replace('|', '\|', $result);
    }
    else
        $result = $childResult;

    return $result;
}

$out = trim(process($body));

// post-process
$out = str_replace('^n^N', '^N', $out);
$out = str_replace('^n|', '|', $out);


return new Response('text/plain; charset=utf-8', $out); // . "\n\n\n" . $response->content);

?>
