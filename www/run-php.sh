#!/bin/sh

export PHP_FCGI_CHILDREN=8
export PHP_FCGI_MAX_REQUESTS=1000

exec php-cgi -b 127.0.0.1:10001
