{extends file="base.tpl"}

{block "js" append}
    <script src="http://code.jquery.com/jquery.min.js"></script>

    <script src="/static/js/phone.js"></script>
{/block}

{block body}
    <div id="content">
        <h1>MeeBible</h1>

        <p>
            The multilingual Bible reader for Nokia N9 smartphone.
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
                <li>Russian Synodal Translation</li>
                <li>Latvian Revised Bible</li>
                <li>Arabic Smith &amp; Van Dyke Version</li>
                <li>Svenska Folkbibeln (Swedish People's Bible)</li>
            </ul>
        </p>

        <p>
            Full-text unicode-aware search is supported. You can find Bible places by phrase or character's name.
        </p>


        <h2>Download from Nokia Store</h2>

        <p>
            <div class="store-links">
                <table>
                    <tr>
                        <td class="variant">
                            <a href="http://store.ovi.com/content/218744">
                                <img src="/static/img/meebible-free.png"/>
                                <div class="title">MeeBible Free</div>
                            </a>
                            <div class="desc">Fully-functional free version</div>
                        </td>
                        <td class="divider">
                            &larr; or &rarr;
                        </td>
                        <td class="variant">
                            <a href="http://store.ovi.com/content/215454">
                                <img src="/static/img/meebible.png"/>
                                <div class="title">MeeBible</div>
                            </a>
                            <div class="desc">Donate &euro;5 for further development</div>
                        </td>
                    </tr>
                </table>
            </div>
        </p>


        <h2>Source code</h2>

        <p>
            Full source code and bagtracking are available at <a href="https://projects.developer.nokia.com/meebible">MeeBible's page on Nokia Developer</a>.
        </p>

        <p>
            You may also fork me on <a href="https://bitbucket.org/IlyaSkriblovsky/meebible">BitBucket</a>.
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
