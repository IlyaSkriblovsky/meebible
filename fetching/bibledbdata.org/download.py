import httplib, sys, re, sqlite3
from itertools import *

from createxml import create_xml

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]

verse_re = re.compile(r'\n(\d+):(\d+) (.*)<br>')

db = sqlite3.Connection("glu.sqlite")
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

for bookNo in xrange(66):
    chapterNo = 0
    while True:
        chapterNo += 1

        url = '/onlinebibles/german_l/{0:02}_{1:03}.htm'.format(bookNo+1, chapterNo)
        while True:
            try:
                http = httplib.HTTPConnection('www.bibledbdata.org')
                http.request('GET', url)
                r = http.getresponse()
                break
            except httplib.BadStatusLine: pass
        if r.status != 200: break

        content = r.read().decode('windows-1251')
        verses = verse_re.findall(content)

        if bookCodes[bookNo] == '3jo' and chapterNo == 1:
            verses = verses[:-1]

        for v, vn in izip(verses, count(1)):
            if int(v[0]) != chapterNo: raise Exception('strange chapterno at verse {}'.format(vn))
            if int(v[1]) != vn: raise Exception('strange verseno at verse {}'.format(vn))

        xml = create_xml([v[2] for v in verses])
        with open('{0}_{1:03}.html'.format(bookCodes[bookNo], chapterNo), 'w') as f:
            f.write(xml.encode('utf-8'))

        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            'x', bookCodes[bookNo], chapterNo, xml
        ))

        print '{}|{}|{}'.format(bookCodes[bookNo], chapterNo, len(verses))
        sys.stdout.flush()

c.execute("vacuum")
db.commit()
