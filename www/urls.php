<?php

return array(
    array(
        'url'   =>  '/',
        'view'  =>  'index'
    ),

    array(
        'url'   =>  '/privacy',
        'view'  =>  'privacy'
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
        'url'   =>  '/^\/chapter\?/',
        're'    =>  true,
        'view'  =>  'chapter'
    ),

    array(
        'url'   =>  '/^\/t\//',
        're'    =>  true,
        'view'  =>  'place'
    ),

    array(
        'url'   =>  '/feedback',
        'view'  =>  'feedback'
    ),

    array(
        'url'   =>  '/startup',
        'view'  =>  'startup'
    ),

    array(
        'url'   =>  '/fb',
        'view'  =>  'feedback-view'
    ),

    array(
        'url'   =>  '*',
        'view'  =>  '404'
    )
)

?>
