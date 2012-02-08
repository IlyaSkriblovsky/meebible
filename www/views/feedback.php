<?php
    $name = $_POST['name'];
    $email = $_POST['email'];
    $message = $_POST['message'];

    sleep(3);

    error_log("FEEDBACK from $name");

    $db = new SQLite3("feedback.sqlite");
    $stmt = $db->prepare("INSERT INTO feedback (name, email, message) VALUES (:name, :email, :message)");
    $stmt->bindValue(":name", $name);
    $stmt->bindValue(":email", $email);
    $stmt->bindValue(":message", $message);
    $stmt->execute();

    # mail('ilyaskriblovsky@gmail.com', 'subject', 'body');

    $response = new Response('text/plain', 'ok');
    return $response;
?>
