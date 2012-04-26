import httplib, sys, re, sqlite3
from itertools import *

from createxml import create_xml


transUrl = 'indonesian_tb'
transCode = 'intb'
langCode = 'in'

# transUrl = 'german_l'
# transCode = 'glu'
# langCode = 'x'

encoding = 'windows-1252'


bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]

verse_re = re.compile(r'\n(\d+):(\d+) (.*)<br>')

db = sqlite3.Connection("{0}.sqlite".format(transCode))
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

for bookNo in xrange(66):
    chapterNo = 0
    while True:
        chapterNo += 1

        url = '/onlinebibles/{0}/{1:02}_{2:03}.htm'.format(transUrl, bookNo+1, chapterNo)
        while True:
            try:
                http = httplib.HTTPConnection('www.bibledbdata.org')
                http.request('GET', url)
                r = http.getresponse()
                break
            except httplib.BadStatusLine: pass

        if r.status != 200:
            if transCode != 'intb' or bookNo != 58 or chapterNo != 3:
                break

        content = r.read().decode(encoding)
        verses = verse_re.findall(content)

        if transCode == 'glu' and bookCodes[bookNo] == '3jo' and chapterNo == 1:
            verses = verses[:-1]

        for v, vn in izip(verses, count(1)):
            if int(v[0]) != chapterNo: raise Exception('strange chapterno at verse {}'.format(vn))
            if int(v[1]) != vn: raise Exception('strange verseno at verse {}'.format(vn))

        xml = create_xml([v[2] for v in verses])
        with open('{0}_{1:03}.html'.format(bookCodes[bookNo], chapterNo), 'w') as f:
            f.write(xml.encode('utf-8'))

        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            langCode, bookCodes[bookNo], chapterNo, xml
        ))

        print '{0}|{1}|{2}'.format(bookCodes[bookNo], chapterNo, len(verses))
        sys.stdout.flush()

c.execute("vacuum")
db.commit()
