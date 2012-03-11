# -*- coding: utf-8 -*-

import sqlite3
import httplib
import sys
import re
import os
import subprocess

meebible_bookcodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
yv_bookcodes = ['gen', 'exod', 'lev', 'num', 'deut', 'josh', 'judg', 'ruth', '1sam', '2sam', '1kgs', '2kgs', '1chr', '2chr', 'ezra', 'neh', 'esth', 'job', 'ps', 'prov', 'eccl', 'song', 'isa', 'jer', 'lam', 'ezek', 'dan', 'hos', 'joel', 'amos', 'obad', 'jonah', 'mic', 'nah', 'hab', 'zeph', 'hag', 'zech', 'mal', 'matt', 'mark', 'luke', 'john', 'acts', 'rom', '1cor', '2cor', 'gal', 'eph', 'phil', 'col', '1thess', '2thess', '1tim', '2tim', 'titus', 'phlm', 'heb', 'jas', '1pet', '2pet', '1john', '2john', '3john', 'jude', 'rev', ]

mee2yv = dict(zip(meebible_bookcodes, yv_bookcodes))

transCode = 'svf'
yv_transCode = 'sfb'

http = httplib.HTTPConnection('www.youversion.com')

for bookCode in meebible_bookcodes:
    chapter = 0
    while True:
        chapter += 1

        url = '/bible/{0}.{1}.{2}'.format(
            mee2yv[bookCode], chapter, yv_transCode
        )

        http.request('GET', url, '', {
            'User-Agent': 'Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_7; en-US) AppleWebKit/534.16 (KHTML, like Gecko) Chrome/10.0.648.205 Safari/534.16'
        })
        response = http.getresponse()
        if response.status != 200:
            if response.status == 504:
                print 'gateway timeout'
                chapter -= 1
                continue

            print '{0} {1}: err {2}'.format(
                bookCode, chapter, response.status
            )
            break

        content = response.read()

        # content = content.replace('>&nbsp;', '>')
        # content = content.replace('&copy;', '©')

        # content = re.sub(r'<meta name="description"[^>]*>', '', content)

        # content = content.replace(
        #     '<li><a href="http://blog.youversion.com/jobs">Jobs</a>',
        #     '<li><a href="http://blog.youversion.com/jobs">Jobs</a></li>'
        # )
        # content = content.replace(
        #     '<li><a href="http://www.youversion.com/press">Press</a>',
        #     '<li><a href="http://www.youversion.com/press">Press</a></li>'
        # )

        content = re.sub(r'&nbsp;', r'&#160;', content)
        content = re.sub(r'&copy;', r'©;', content)
        content = re.sub(r'<(meta[^>]*[^/])>', r'<\1/>', content)
        content = re.sub(r'<(link[^>]*[^/])>', r'<\1/>', content)
        content = re.sub(r'<(input[^>]*[^/])>', r'<\1/>', content)
        content = re.sub(r'<br\s*>', r'<br/>', content)
        content = re.sub(r'<option selected ', r'<option selected="1" ', content)

        # orig_filename = '{0}-{1}.html'.format(book[0], chapter)
        orig_filename = 'orig_{0}_{1:03}.html'.format(bookCode, chapter)
        new_filename = '{0}_{1:03}.html'.format(bookCode, chapter)

        f = open(orig_filename, 'w')
        f.write(content)
        f.close()

        result = subprocess.Popen(['xsltproc', '--nonet', 'yv.xslt', orig_filename], stdout = subprocess.PIPE, stderr = None).communicate()[0]

        # os.unlink(orig_filename)

        f = open(new_filename, 'w')
        f.write(result)
        f.close()

        print '{0} {1}'.format(bookCode, chapter)
        sys.stdout.flush()
