<?php

require_once('core/Framework.php');

// Import template functions
foreach (glob("template_tags/*.php") as $template_func)
    require($template_func);

Framework::exec($_SERVER['REQUEST_URI']);

?>
