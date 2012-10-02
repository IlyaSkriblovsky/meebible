{extends file="base.tpl"}

{block "meta" append}
    <meta property="og:site_name" content="meebible.org"/>
    <meta property="og:type" content="article"/>
    <meta property="og:image" content="http://meebible.org/static/img/meebible.png"/>

    <meta property="og:title" content="MeeBible — Bible reader app"/>
    <meta property="og:description" content="Bible reading app for Symbian & MeeGo"/>
{/block}

{block "js" append}
    <script src="http://code.jquery.com/jquery.min.js"></script>

    <script src="/static/js/phone.js"></script>

    <script type="text/javascript">var d=document,h=d.getElementsByTagName("html")[0];h.setAttribute("xmlns:nokia","http://www.nokia.com/2011/dxml");</script>
    <script src="//dropsdk.nokia.com/latest/button/js/all.js" type="text/javascript"></script>
{/block}

{block body}
    <div id="content">
        <h1>MeeBible</h1>

        <p>
            <i>The multilingual Bible reader for Symbian^3 and MeeGo smartphones</i>
        </p>

        <div id="phone">
            <div id="screen-wrapper">
                <div class="screen visible current" id="screen-1"></div>
                <div class="screen" id="screen-2"></div>
                <div class="screen" id="screen-3"></div>
                <div class="screen" id="screen-4"></div>
                <div class="screen" id="screen-5"></div>
            </div>
        </div>

        <h2>Blog</h2>
            Official MeeBible blog: <a href="http://blog.meebible.org">blog.meebible.org</a>

            <div class="social">
                <div class="twitter">
                    {literal}
                        <a href="https://twitter.com/MeeBible_org" class="twitter-follow-button" data-show-count="false">Follow @MeeBible_org</a>
                        <script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0];if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src="//platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");</script>
                    {/literal}
                </div>
            </div>

        <h2>About</h2>

        <p>
            With MeeBible you can read God's Word everywhere with your phone. Bible texts are downloaded on the fly from the web and cached for further reading. You can also download whole Bible translation via Wi-Fi network and read it offline.
        </p>

        <p>
            MeeBible supports following Bible translations:
            <ul>
                <li>New World Translation in 42 languages<!-- (from <a href="http://watchtower.org">watchtower.org</a>) --></li>
                <li>King James Bible</li>
                <li>New International Bible</li>
                <li>English Standard Version</li>
                <li>New King James Bible</li>
                <li>Bible in Basic English</li>
                <li>The Message translation</li>
                <li>Russian Synodal Translation</li>
                <li>French Louis Segond Translation</li>
                <li>German Luther 1912 and Schlachter 1951 translations</li>
                <li>Italian Versione Diodati Riveduta</li>
                <li>Portuguese Almeida Corrigida e Revisada Translation</li>
                <li>Spanish Reina Valera Translation</li>
                <li>Latvian Revised Bible</li>
                <li>Svenska Folkbibeln (Swedish People's Bible)</li>
                <li>Finnish Bible 1912</li>
                <li>Ukranian translation by І. Огієнко</li>
                <li>Polish translations: Biblia Tysiąclecia, Biblia Warszawska, Biblia Warszawsko-Praska</li>
                <li>Bulgarian “Протестантски (1940)”</li>
                <li>Romanian translation by Dumitru Cornilescu</li>
                <li>Arabic Smith &amp; Van Dyke Version</li>
                <li>Indinesian Alkitab Terjemahan Baru</li>
            </ul>
        </p>

        <p>
            Full-text unicode-aware search is supported. You can find Bible places by phrase or character's name.
        </p>


        <h2>Download from Nokia Store</h2>

        <h3>Symbian^3</h3>

        <div class="indent">
            <div class="download-label">Fully-functional free version:</div>
            <!--
            <div class="store-link">
                <a href="http://store.ovi.com/content/262401"><img src="/static/img/meebible-small.png"/></a>
                <a href="http://store.ovi.com/content/262401"><span class="title">MeeBible for Symbian^3</span></a>
            </div>
            -->
            <nokia:store product_id="262401" size="small" lang="en"></nokia:store>
        </div>

        <h3>MeeGo</h3>

        <div class="indent">
            <div class="download-label">Fully-functional free version:</div>
            <!--
            <div class="store-link indent-left">
                <a href="http://store.ovi.com/content/218744"><img src="/static/img/meebible-free-small.png"/></a>
                <a href="http://store.ovi.com/content/218744"><span class="title">MeeBible Free</span></a>
            </div>
            -->
            <nokia:store product_id="218744" size="small" lang="en"></nokia:store>
        </div>
        <div class="indent">
            <div class="download-label">Donate €5 for further development:</div>
            <!--
            <div class="store-link indent-left">
                <a href="http://store.ovi.com/content/215454"><img src="/static/img/meebible-small.png"/></a>
                <a href="http://store.ovi.com/content/215454"><span class="title">MeeBible</span></a>
            </div>
            -->
            <nokia:store product_id="215454" size="small" lang="en"></nokia:store>
        </div>


        <h2>Source code</h2>

        <p>
            Full source code and bug tracking at <a href="https://projects.developer.nokia.com/meebible">MeeBible's page on Nokia Developer</a>.
        </p>

        <p>
            Alternative source hosting on <a href="https://bitbucket.org/IlyaSkriblovsky/meebible">BitBucket</a>.
        </p>

        <h2>Support</h2>

        <p>
            Please send your questions and comments at <a href="mailto:ilya@meebible.org">ilya@meebible.org</a>.
        </p>

        <div id="footer">
            &copy; 2011–2012 Ilya Skriblovsky
        </div>
    </div>


{/block}
