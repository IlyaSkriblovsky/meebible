# -*- coding: utf-8 -*-

import sqlite3
import httplib
import sys
import re
import os
import subprocess

meebible_bookcodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
niv_bookcodes = ['gen', 'exod', 'lev', 'num', 'deut', 'josh', 'judg', 'ruth', '1sam', '2sam', '1kgs', '2kgs', '1chr', '2chr', 'ezra', 'neh', 'esth', 'job', 'ps', 'prov', 'eccl', 'song', 'isa', 'jer', 'lam', 'ezek', 'dan', 'hos', 'joel', 'amos', 'obad', 'jonah', 'mic', 'nah', 'hab', 'zeph', 'hag', 'zech', 'mal', 'matt', 'mark', 'luke', 'john', 'acts', 'rom', '1cor', '2cor', 'gal', 'eph', 'phil', 'col', '1thess', '2thess', '1tim', '2tim', 'titus', 'phlm', 'heb', 'jas', '1pet', '2pet', '1john', '2john', '3john', 'jude', 'rev', ]

mee2niv = dict(zip(meebible_bookcodes, niv_bookcodes))

transCode = 'niv'

trans = sqlite3.Connection('../../share/trans.sqlite')
c = trans.cursor()

c.execute("SELECT bookCode, bookName FROM books WHERE transCode='{0}' ORDER BY bookNo".format(transCode))
books = c.fetchall()

http = httplib.HTTPConnection('www.youversion.com')

for book in books:
    c.execute("SELECT count(*) FROM chapterSize WHERE transCode=? AND bookCode=?", (transCode, book[0]))
    chapters = c.fetchall()[0][0]

    for chapter in xrange(1, chapters + 1):
        url = '/bible/chapter/niv/{0}/{1}'.format(
            mee2niv[book[0]], chapter
        )

        http.request('GET', url, '', {
            'User-Agent': 'Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_7; en-US) AppleWebKit/534.16 (KHTML, like Gecko) Chrome/10.0.648.205 Safari/534.16'
        })
        content = http.getresponse().read()

        content = content.replace('>&nbsp;', '>')
        content = content.replace('&copy;', '©')

        content = re.sub(r'<meta name="description"[^>]*>', '', content)

        content = content.replace(
            '<li><a href="http://blog.youversion.com/jobs">Jobs</a>',
            '<li><a href="http://blog.youversion.com/jobs">Jobs</a></li>'
        )
        content = content.replace(
            '<li><a href="http://www.youversion.com/press">Press</a>',
            '<li><a href="http://www.youversion.com/press">Press</a></li>'
        )

        # orig_filename = '{0}-{1}.html'.format(book[0], chapter)
        orig_filename = 'orig_{0}_{1:03}.html'.format(book[0], chapter)
        new_filename = '{0}_{1:03}.html'.format(book[0], chapter)

        f = open(orig_filename, 'w')
        f.write(content)
        f.close()

        result = subprocess.Popen(['xsltproc', '--nonet', 'niv.xslt', orig_filename], stdout = subprocess.PIPE, stderr = None).communicate()[0]

        # os.unlink(orig_filename)

        f = open(new_filename, 'w')
        f.write(result)
        f.close()

        print '{0} {1}'.format(book[1], chapter)
        sys.stdout.flush()
