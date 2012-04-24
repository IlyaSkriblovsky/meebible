# -*- coding: utf-8 -*-

import re
from itertools import *
import sqlite3

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]

bookname_re = re.compile(r'^(.*) (\d+)$')
verse_re = re.compile(ur'^(\d+) ?(¶)? ?(.*)$')

lines = [line.decode('latin1').strip() for line in open('bibschl.txt', 'r').readlines()]
lines = lines[1:]
lines.append('')

chaps = []
while len(lines) > 0:
    c = [l for l in takewhile(lambda l: len(l) > 0, lines)]
    chaps.append(c)
    lines = lines[len(c)+1 : ]


booknames = []

for c in chaps:
    m = bookname_re.match(c[0])
    if len(booknames) == 0:
        booknames.append({
            'name': m.group(1),
            'count': 1
        })
    else:
        if m.group(1) == booknames[-1]['name']:
            booknames[-1]['count'] += 1
        else:
            booknames.append({
                'name': m.group(1),
                'count': 1
            })


db = sqlite3.Connection('gsc51.sqlite')
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


citer = iter(chaps)
for book, bookNo in izip(booknames, count()):
    for chapterNo in xrange(1, book['count']+1):
        chap = citer.next()[1:]
        html = '<html>\n<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"/></head>\n<body>\n\n'
        for v, vn in izip(chap, count(1)):
            m = verse_re.match(v)
            if m is None: raise Exception('strage verse: "{}"'.format(v))
            if int(m.group(1)) != vn: raise Exception('strange verse no: {} !+ {}'.format(m.group(1), vn))

            if m.group(2) is not None:
                if vn != 1: html += u'</div>\n'
                html += u'<div class="par">\n'

            html += u'    <div class="verse" verse="{0}"><div class="verse-label">{0}</div>{1}</div>\n'.format(
                vn, m.group(3)
            )

        html += u'</div>\n\n</body>\n</html>\n'

        print '{}|{}|{}'.format(bookCodes[bookNo], chapterNo, len(chap))

        # with open('{0}_{1:03}.html'.format(bookCodes[bookNo], chapterNo), 'w') as f:
        #     f.write(html.encode('utf-8'))
        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            'x', bookCodes[bookNo], chapterNo, html
        ))

c.execute('vacuum')
db.commit()
