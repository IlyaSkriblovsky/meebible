import sys
import httplib
from itertools import *
import re

from createxml import create_xml

bookcodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 're', ]


verse_re = re.compile(r'<br /><br /><b>(\d+). </b>(.*?)(?=<br /><br />)')


import sqlite3
db = sqlite3.Connection('bbg.sqlite')
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


http = httplib.HTTPConnection('blagodat-bg.net')

for bookNo, bookCode in izip(count(1), bookcodes):
    chapter = 0
    while True:
        chapter += 1

        http.request('GET', '/main.php?menu=bible&act=1&nc=50&district={0}&chapter={1}'.format(
            bookNo, chapter
        ))

        content = http.getresponse().read()

        verses = verse_re.findall(content)

        if len(verses) == 0: break

        for n, v in izip(count(1), verses):
            if int(v[0]) != n: raise Exception('Strange verse number: {0} {1}:{2}'.format(bookCode, chapter, n))


        xml = create_xml(v[1].decode('windows-1251') for v in verses)

        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            'bl', bookCode, chapter, xml
        ))
        c.execute('select count(*) from html')
        print c.fetchone()

        print '{0}|{1}|{2}'.format(
            bookCode, chapter, len(verses)
        )
        sys.stdout.flush()

c.execute('vacuum')
