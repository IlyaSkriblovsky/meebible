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
        'url'   =>  '/donate',
        'view'  =>  'donate'
    ),

    array(
        'url'   =>  '/donate-mobile',
        'view'  =>  'donate-mobile'
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
        'url'   =>  '/^\/nwtbible\?/',
        're'    => true,
        'view'  => 'nwtbible'
    ),

    array(
        'url'   =>  '*',
        'view'  =>  '404'
    )
)

?>
