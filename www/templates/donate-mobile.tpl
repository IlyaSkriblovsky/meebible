{extends file="base.tpl"}

{block "css" append}
    <style>
        #paypal-form {
            margin-top: 30px;
        }
    </style>
{/block}

{block body}
    <div id="content">
        <h1>Donate to MeeBible</h1>

        <p>
            Click button below or open <tt style="font-family: monospace">http://meebible.org/donate</tt> on your computer
        </p>

        <form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top" id="paypal-form">
            <input type="hidden" name="cmd" value="_s-xclick">
            <input type="hidden" name="encrypted" value="-----BEGIN PKCS7-----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-----END PKCS7-----
            ">
            <input type="image" src="https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG_global.gif" border="0" name="submit" alt="PayPal – The safer, easier way to pay online.">
            <img alt="" border="0" src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif" width="1" height="1">
        </form>

        <p style="margin-top: 30px">
            Thank you!
        </p>
    </div>
{/block}
