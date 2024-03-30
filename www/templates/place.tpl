{extends "base.tpl"}

{block title}{$place|escape:'html'}{/block}

{block meta append}

    <meta property="og:site_name" content="meebible.org"/>
    <meta property="og:type" content="article"/>
    <meta property="og:image" content="http://meebible.org/static/img/meebible.png"/>

    <meta property="og:title" content="{$place|escape:'html'}"/>
    <meta property="og:description" content="{$text|escape:'html'}"/>
    <meta property="article:tag" content="MeeBible"/>


    <meta name="viewport" content="width=device-width"/>

{/block}

{block css append}
    <link rel="stylesheet" href="/static/css/place.css" media="screen"/>
    <link rel="stylesheet" href="/static/css/place-desktop.css" media="screen and (min-device-width: 1024px)"/>
    <link rel="stylesheet" href="/static/css/place-mobile.css" media="screen and (max-device-width: 1023px)"/>
{/block}

{block js append}
<div id="fb-root"></div> <script>(function(d, s, id) { var js, fjs = d.getElementsByTagName(s)[0]; if (d.getElementById(id)) return; js = d.createElement(s); js.id = id; js.src = "//connect.facebook.net/ru_RU/all.js#xfbml=1"; fjs.parentNode.insertBefore(js, fjs); }(document, 'script', 'facebook-jssdk'));</script>
{/block}

{block body}

<div class="container-1">
    <div class="container-2">
        <div class="quote-block">
            <h2>{$place}</h2>
            {$html}
        </div>
    </div>
</div>


<div class="info">
    <a href="/"><img class="logo" src="/static/img/meebible-small.png"/></a>
    <a href="/">
        <div>MeeBible</div>
        <div>Mobile Reader</div>
    </a>
</div>


<div class="social">
    <div class="fb-like" data-send="false" data-layout="button_count" data-width="450" data-show-faces="false"></div>
    <a href="https://twitter.com/share" class="twitter-share-button" data-via="MeeBible_org">Tweet</a>
    {literal}<script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0];if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src="//platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");</script>{/literal}
</div>

{/block}
