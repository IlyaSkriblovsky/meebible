<?php

return array(
    array(
        'url'   =>  '/',
        'view'  =>  'index'
    ),

    array(
        'url'   =>  '/meta',
        'view'  =>  'meta'
    ),

    array(
        'url'   =>  '/^\/translation\?/',
        're'    =>  true,
        'view'  =>  'translation'
    ),

    array(
        'url'   =>  '*',
        'view'  =>  '404'
    )
)

?>
