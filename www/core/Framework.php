<?php

require_once('smarty/Smarty.class.php');

require_once('core/Response.php');

class Framework {
    public static function urls() {
        static $urls = null;
        if ($urls == null)
            $urls = require('urls.php');

        return $urls;
    }

    public static function match_url($url) {
//        foreach (Framework::urls() as $urlspec) {
//            if ($urlspec['url'] == $url || $urlspec['url'] == '*')
//                return $urlspec;
//        }

        foreach (Framework::urls() as $urlspec) {
            if (!empty($urlspec['re']))
            {
                if (preg_match($urlspec['url'], $url))
                    return $urlspec;
            }
            else
            {
                if ($urlspec['url'] == $url)
                    return $urlspec;
            }

            if ($urlspec['url'] == '*')
                return $urlspec;
        }

        return null;
    }


    public static function exec($url) {
        // ob_start();
        // echo "SERVER";
        // print_r($_SERVER);
        // echo "REQUEST";
        // print_r($_REQUEST);
        // die(ob_get_contents());

        $urlspec = Framework::match_url($_SERVER['REQUEST_URI']);

        if ($urlspec == null)
        {
            print("Cannot find view");
            header("HTTP/1.0 500 Cannot find view");
            return;
        }

        /*
        $params = array();
        foreach ($matches as $no => $match)
            if (!empty($urlspec[$no]))
                $params[$urlspec[$no]] = $match;
        */

        $response = require("views/$urlspec[view].php");
        $response->render();
    }
}

?>
