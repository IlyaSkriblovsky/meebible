<?php
    $response = new TemplateResponse('404.tpl');
    $response->add_header('HTTP/1.0 404 Not found');
    return $response;
?>
