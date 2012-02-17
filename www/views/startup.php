<?php
    $os = $_POST['os'];
    $device_id = $_POST['device_id'];
    $lang = $_POST['lang'];
    $trans = $_POST['trans'];

    error_log("STARTUP: os=$os id=$device_id lang=$lang trans=$trans");

    $db = new SQLite3("startup.sqlite");
    $stmt = $db->prepare("INSERT INTO startup (time, device_id, os, lang, trans) VALUES (datetime('now'), :device_id, :os, :lang, :trans)");
    $stmt->bindValue(":device_id", $device_id);
    $stmt->bindValue(":os", $os);
    $stmt->bindValue(":lang", $lang);
    $stmt->bindValue(":trans", $trans);
    $stmt->execute();
    $db->close();

    $response = new Response('text/plain', 'ok');
    return $response;
?>
