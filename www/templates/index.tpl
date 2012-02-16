{extends file="base.tpl"}

{block "js" append}
    <script src="http://code.jquery.com/jquery.min.js"></script>

    <script src="/static/js/phone.js"></script>
{/block}

{block "content"}
    <div id="phone">
        <div id="screen-wrapper">
            <div class="screen visible current" id="screen-1"></div>
            <div class="screen" id="screen-2"></div>
            <div class="screen" id="screen-3"></div>
        </div>
    </div>
{/block}
