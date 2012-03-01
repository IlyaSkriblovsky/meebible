<?php

if (empty($_SERVER['PHP_AUTH_USER']) ||
    $_SERVER['PHP_AUTH_USER'] != 'ilya' ||
    $_SERVER['PHP_AUTH_PW'] != '<hidden>')
{
    $resp = new Response('text/html', 'Unauthorized');
    $resp->add_header('WWW-authenticate: basic realm="MeeBible"');
    $resp->add_header('HTTP/1.0 401 Unauthorized');
    return $resp;
}
else
{
    $db = new SQLite3("feedback.sqlite");
    $r = $db->query("SELECT * FROM feedback ORDER BY time DESC");
    $feedback = array();
    while ($row = $r->fetchArray())
        $feedback[] = $row;
    $db->close();

    $resp = new TemplateResponse('feedback-view.tpl', array(
        'feedback' => $feedback
    ));
    return $resp;
}

?>
