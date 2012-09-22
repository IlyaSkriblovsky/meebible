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
        'url'   =>  '/meta.j2me',
        'view'  =>  'meta.j2me'
    ),

    array(
        'url'   =>  '/^\/translation\?/',
        're'    =>  true,
        'view'  =>  'translation'
    ),

    array(
        'url'   =>  '/^\/translation.j2me\?/',
        're'    =>  true,
        'view'  =>  'translation.j2me'
    ),

    array(
        'url'   =>  '/^\/chapter\?/',
        're'    =>  true,
        'view'  =>  'chapter'
    ),

    array(
        'url'   =>  '/^\/chapter.j2me\?/',
        're'    =>  true,
        'view'  =>  'chapter.j2me'
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
