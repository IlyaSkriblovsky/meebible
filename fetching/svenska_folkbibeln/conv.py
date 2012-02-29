# -*- coding: utf-8 -*-

import re
from itertools import *

from createxml import create_xml

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're' ]

bookNames = []
chapterCounts = []


verse_re = re.compile(r'^<p>(?:<sup>\d+</sup>)?(?:<font color=blue>\(\d+:\d+\)</font>)? ?(.*)</p>$')


setuplines = [line.strip() for line in open('setup.ini').readlines()]

for i in xrange(len(setuplines)):
    if setuplines[i].startswith('PathName = '):
        # bookNo = int(setuplines[i][len('PathName = ') : -4])
        bookNames.append(setuplines[i+1][len('FullName = ') : ])
        chapterCounts.append(int(setuplines[i+3][len('ChapterQty = ') : ]))



import sqlite3
conn = sqlite3.Connection('svf.sqlite')
c = conn.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

tmp1 = open('tmp1', 'w')
tmp2 = open('tmp2', 'w')


for bookNo, bookCode, bookName, chapterCount in izip(count(1), bookCodes, bookNames, chapterCounts):
    lines = [line.decode('latin-1').strip() for line in open('{0:02}.htm'.format(bookNo)).readlines()]

    chapterNo = 0

    for line in lines:
        if line.startswith('<h2>') or line.startswith('</body>'):
            if chapterNo > 0:
                xml = create_xml(verses)
                c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES ('z', ?, ?, ?)", (
                    bookCode, chapterNo, xml
                ))
                print '{0:6}, {1:03}, {2:03}'.format("'{0}'".format(bookCode), chapterNo, len(verses))

            chapterNo += 1
            verses = []
        elif line.startswith('<p>'):
            m = verse_re.match(line)
            if m is not None:
                verses.append(m.group(1))
                tmp1.write('{0:02}.htm:{1}\n'.format(bookNo, line.encode('latin1')))
                tmp2.write('{0:02} {1} {2}: {3}\n'.format(bookNo, chapterNo, len(verses), m.group(1).encode('latin1')))
            else:
                raise Exception('Strange verse: {0} {1}:{2}'.format(bookNo, chapterNo, len(verses)+1))

c.execute("vacuum")

tmp1.close()
tmp2.close()
