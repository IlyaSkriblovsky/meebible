<?php
    $name = $_POST['name'];
    $email = $_POST['email'];
    $message = $_POST['message'];

    error_log("FEEDBACK from $name");

    $db = new SQLite3("feedback.sqlite");
    $stmt = $db->prepare("INSERT INTO feedback (time, name, email, message) VALUES (datetime('now'), :name, :email, :message)");
    $stmt->bindValue(":name", $name);
    $stmt->bindValue(":email", $email);
    $stmt->bindValue(":message", $message);
    $stmt->execute();
    $db->close();

    # mail('ilyaskriblovsky@gmail.com', 'subject', 'body');

    $response = new Response('text/plain', 'ok');
    return $response;
?>
