{extends "base.tpl"}

{block body}
<div id="content">
    {literal}
    <style>
        .feedback {
            width: 100%;
            margin: 30px auto;

            font-size: 80%;
        }

        .feedback td {
            padding: 10px;
        }

        .feedback th {
            font-weight: bold;
        }
    </style>
    {/literal}

    <h2>
        Feedback: {count($feedback)}
    </h2>

    <table class="feedback">
        <tr>
            <th>id</th>
            <th>time</th>
            <th>name</th>
            <th>email</th>
            <th>message</th>

            {foreach item=f from=$feedback}
                <tr>
                    <td>{$f.id}</td>
                    <td>{$f.time}</td>
                    <td>{$f.name}</td>
                    <td>{$f.email}</td>
                    <td>{$f.message}</td>
                </tr>
            {/foreach}
        </tr>
    </table>
</div>
{/block}
