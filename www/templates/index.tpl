{extends file="base.tpl"}

{block "js" append}
    <script src="http://code.jquery.com/jquery.min.js"></script>

    <script src="/static/js/phone.js"></script>
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
                <li>Russian Synodal Translation</li>
                <li>French Louis Segond Translation</li>
                <li>German Luther 1912 and Schlachter 1951 translations</li>
                <li>Italian Versione Diodati Riveduta</li>
                <li>Portuguese Almeida Corrigida e Revisada Translation</li>
                <li>Spanish Reina Valera Translation</li>
                <li>Latvian Revised Bible</li>
                <li>Svenska Folkbibeln (Swedish People's Bible)</li>
                <li>Finnish Bible 1912</li>
                <li>Polish translations: Biblia Tysiąclecia, Biblia Warszawska, Biblia Warszawsko-Praska</li>
                <li>Bulgarian “Протестантски (1940)”</li>
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
            <div class="store-link">
                <a href="http://store.ovi.com/content/262401"><img src="/static/img/meebible-small.png"/></a>
                <a href="http://store.ovi.com/content/262401"><span class="title">MeeBible for Symbian^3</span></a>
            </div>
        </div>

        <h3>MeeGo</h3>

        <div class="indent">
            Fully-functional free version:
            <div class="store-link indent-left">
                <a href="http://store.ovi.com/content/218744"><img src="/static/img/meebible-free-small.png"/></a>
                <a href="http://store.ovi.com/content/218744"><span class="title">MeeBible Free</span></a>
            </div>
        </div>
        <div class="indent">
            Donate €5 for further development:
            <div class="store-link indent-left">
                <a href="http://store.ovi.com/content/215454"><img src="/static/img/meebible-small.png"/></a>
                <a href="http://store.ovi.com/content/215454"><span class="title">MeeBible</span></a>
            </div>
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
