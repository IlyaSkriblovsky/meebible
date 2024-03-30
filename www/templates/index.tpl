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

    <script>
        $(function () {
            $('#show-translations').on('click', function () {
                $('.translations-list').slideDown()
                $('#show-translations').remove()
                return false
            })
        })
    </script>
{/block}

{block "css" append}
    <style>
        .announcement {
            border-left: 3px solid #c4443b;
            padding: 15px 0 15px 25px;
        }
    </style>
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

        <!--
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
        -->

        <!--
        <h2 class="red">Important announcement</h2>
        <div class="announcement">
            <p>
                2 years have passed since Nokia Store was closed and 2&nbsp;years since the last version of&nbsp;MeeBible was released.
                Sad to say, but in modern world such a long time without updates means that software is effectively dead.
            </p>
            <p>
                But meebible.org website that is needed for MeeBible to work is still running.
                This&nbsp;site hosts all Bible translations that MeeBible downloads for&nbsp;reading on&nbsp;smartphones.
                Hosting of the site costs around $70/year or $5.80/month.
            </p>
            <p>
                I think it would be a good test for whether MeeBible is still needed by people: if&nbsp;your donations will be
                able to cover operational costs of meebible.org then it&nbsp;will&nbsp;continue to live. If you continue to use MeeBible
                please consider making a&nbsp;donation to&nbsp;support its infrastructure. MeeBible have always been free, so if
                it&nbsp;was useful for you, please support it with your "two mites" (Mark 12:43).
            </p>
            <p>
                Sincerely yours,<br/>
                &nbsp;&nbsp;&nbsp;&nbsp;Ilya Skriblovsky
            </p>
            <p style="margin-top: 2em">
                <form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
                    <input type="hidden" name="cmd" value="_s-xclick">
                    <input type="hidden" name="encrypted" value="-----BEGIN PKCS7-----MIIHNwYJKoZIhvcNAQcEoIIHKDCCByQCAQExggEwMIIBLAIBADCBlDCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20CAQAwDQYJKoZIhvcNAQEBBQAEgYAiRoc4MTOgPYTbWwtriJdekSy6PXiLRize39FXnhj0OZNNFwsOqq1+oGi0iqnYY/ItkC1R8t05Y0Yoj/46EpgQPo30HNgkUdhNigvPfDNPsPGGXe03ZZhYl7JCbBVRIQ9ZQz2OIL/6V8U4C6GwuZuag7B+JG0m6Ks1oQuvIC5sNzELMAkGBSsOAwIaBQAwgbQGCSqGSIb3DQEHATAUBggqhkiG9w0DBwQI7wVyeUMNxfaAgZAcY4vApOplmtDqqRAXKx9xo6sYlq2xV2lfoqIky9rzlDER6oqNalQ86C26i0H4D9a3XrFgNRQxmTlgrG5DWLe4FUqd/rT8dLubSAzNXZRCI6PFXLVlfsFtDZJgzSnlUlzt1QshXAF6HvWseAHL0t+bA/8pDYStFu0csD4FMCeTDKbEOhF0O7S+x8oW8OUrEhagggOHMIIDgzCCAuygAwIBAgIBADANBgkqhkiG9w0BAQUFADCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20wHhcNMDQwMjEzMTAxMzE1WhcNMzUwMjEzMTAxMzE1WjCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAMFHTt38RMxLXJyO2SmS+Ndl72T7oKJ4u4uw+6awntALWh03PewmIJuzbALScsTS4sZoS1fKciBGoh11gIfHzylvkdNe/hJl66/RGqrj5rFb08sAABNTzDTiqqNpJeBsYs/c2aiGozptX2RlnBktH+SUNpAajW724Nv2Wvhif6sFAgMBAAGjge4wgeswHQYDVR0OBBYEFJaffLvGbxe9WT9S1wob7BDWZJRrMIG7BgNVHSMEgbMwgbCAFJaffLvGbxe9WT9S1wob7BDWZJRroYGUpIGRMIGOMQswCQYDVQQGEwJVUzELMAkGA1UECBMCQ0ExFjAUBgNVBAcTDU1vdW50YWluIFZpZXcxFDASBgNVBAoTC1BheVBhbCBJbmMuMRMwEQYDVQQLFApsaXZlX2NlcnRzMREwDwYDVQQDFAhsaXZlX2FwaTEcMBoGCSqGSIb3DQEJARYNcmVAcGF5cGFsLmNvbYIBADAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUAA4GBAIFfOlaagFrl71+jq6OKidbWFSE+Q4FqROvdgIONth+8kSK//Y/4ihuE4Ymvzn5ceE3S/iBSQQMjyvb+s2TWbQYDwcp129OPIbD9epdr4tJOUNiSojw7BHwYRiPh58S1xGlFgHFXwrEBb3dgNbMUa+u4qectsMAXpVHnD9wIyfmHMYIBmjCCAZYCAQEwgZQwgY4xCzAJBgNVBAYTAlVTMQswCQYDVQQIEwJDQTEWMBQGA1UEBxMNTW91bnRhaW4gVmlldzEUMBIGA1UEChMLUGF5UGFsIEluYy4xEzARBgNVBAsUCmxpdmVfY2VydHMxETAPBgNVBAMUCGxpdmVfYXBpMRwwGgYJKoZIhvcNAQkBFg1yZUBwYXlwYWwuY29tAgEAMAkGBSsOAwIaBQCgXTAYBgkqhkiG9w0BCQMxCwYJKoZIhvcNAQcBMBwGCSqGSIb3DQEJBTEPFw0xNjAxMjgyMDI1NTFaMCMGCSqGSIb3DQEJBDEWBBR+oiBNhilmLPqz35Gca9Mc/k25nzANBgkqhkiG9w0BAQEFAASBgKaZLv9lWOjbvB1tjh69zgCa0DJmAQpbgyvPhanfv05PsM+CFMy9cmtwo+iKsD201XUaeQ7jbfwgcSpk3aCB4UsIjlDRrjvsxDmnGL+9RRQ8qBwY7ok+IGOf0+w/iBBbmrmlJphSux0F3o7OMPvN7AZ+IXH9Oa6Wllnse9JRb3mC-----END PKCS7-----
                    ">
                    <input type="image" src="https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
                    <img alt="" border="0" src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif" width="1" height="1">
                </form>
            </p>
        </div>
        -->

        <h2>About</h2>

        <p>
            With MeeBible you can read God's Word everywhere with your phone. Bible texts are downloaded on the fly from the web and cached for further reading. You can also download whole Bible translation via Wi-Fi network and read it offline.
        </p>

        <p>
            MeeBible supports <b><i>{$trans_count}</i></b> Bible translations in <b><i>{$langs|@count}</i></b> languages. <a href="#" id="show-translations">See the full list.</a>
        </p>

        <div class="translations-list" style="display: none">
            {foreach $langs as $langCode => $lang}
                {if $lang.trans}
                    <div class="language" code="{$langCode}">
                        <h3>{$lang.selfname}<span class="lang-separator">|</span><span class="engname">{$lang.engname}</span></h3>
                        {foreach $lang.trans as $trans}
                            <div class="translation" code="{$trans.transCode}">
                                {$trans.name}
                            </div>
                        {/foreach}
                    </div>
                {/if}
            {/foreach}
        </div>

        <p>
            Full-text unicode-aware search is supported. You can find Bible places by phrase or character's name.
        </p>


        <h2>Download from Nokia Store</h2>

        <h3>Symbian^3</h3>

        <div class="indent">
            <div class="store-link">
                <a href="/static/meebible_3.5.7_signed.sis"><img src="/static/img/meebible-small.png"/></a>
                <a href="/static/meebible_3.5.7_signed.sis"><span class="title">MeeBible for Symbian^3</span></a>
            </div>
        </div>

        <h3>MeeGo</h3>

        <div class="indent">
            <div class="store-link indent-left">
                <a href="/static/meebible-free_3.5.5_armel.deb"><img src="/static/img/meebible-free-small.png"/></a>
                <a href="/static/meebible-free_3.5.5_armel.deb"><span class="title">MeeBible for MeeGo</span></a>
            </div>
        </div>


        <h2>Source code</h2>

        <p>
            Full source code is on <a href="https://github.com/IlyaSkriblovsky/meebible">GitHub</a>.
        </p>

        <h2>Support</h2>

        <p>
            Please send your questions and comments at <a href="mailto:ilyaskriblovsky@gmail.com">ilyaskriblovsky@gmail.com</a>.
        </p>

        <h2>Donate</h2>
        <div id="donate-button-container">
        <div id="donate-button"></div>
        <script src="https://www.paypalobjects.com/donate/sdk/donate-sdk.js" charset="UTF-8"></script>
        <script>
        PayPal.Donation.Button({
        env:'production',
        hosted_button_id:'97APBNSQFP8HJ',
        image: {
        src:'https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif',
        alt:'Donate with PayPal button',
        title:'PayPal - The safer, easier way to pay online!',
        }
        }).render('#donate-button');
        </script>
        </div>


        <h2>Privacy policy</h2>

        <p>
            By using this site and MeeBible application you agree with our <a href="/privacy">privacy policy</a>. We will never share your personal information.
        </p>

        <div id="footer">
            &copy; 2011–2013 Ilya Skriblovsky
        </div>
    </div>
{/block}
