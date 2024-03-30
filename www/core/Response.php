<?php

class Response {
    public function __construct($mime, $content) {
        $this->mime = $mime;
        $this->content = $content;
        $this->headers = array();
    }

    public function render() {
        header("Content-Type: $this->mime");
        header("Content-Length: " . strlen($this->content));
        foreach ($this->headers as $header)
            header($header);

        print $this->content;
    }

    public function add_header($header) {
        $this->headers[] = $header;
    }
}


class TemplateResponse extends Response {
    public function __construct($template, $params = null, $mime = "text/html") {
        $smarty = new Smarty();
        $smarty->setTemplateDir('templates');
        $smarty->setCacheDir('cache');
        $smarty->setCompileDir('compile');

        if (! empty($params))
            foreach ($params as $var => $value)
                $smarty->assign($var, $value);

        $content = $smarty->fetch($template);

        parent::__construct($mime, $content);
    }
}

class XMLResponse extends Response {
    public function __construct($dom) {
        parent::__construct('application/xml', $dom->saveXML());
    }
}


class Redirect extends Response {
    public function __construct($url) {
        parent::__construct('text/plain', "HTTP/1.1 301 Moved Permanently\n$url");

        $this->add_header("HTTP/1.1 301 Moved Permanently");
        $this->add_header("Location: $url");
    }
}

?>
