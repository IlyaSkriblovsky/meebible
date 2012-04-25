# -*- coding: utf-8 -*-

import httplib, sqlite3, sys, re
from itertools import *

from createxml import create_xml

books = [line.strip().split('|') for line in open('books.txt').readlines()]

verse_re = re.compile(
    r'<A NAME="(\d+)">(\d+). </A>([^<]*)',
    re.IGNORECASE
)


db = sqlite3.Connection("fi38.sqlite")
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


for book in books:
    http = httplib.HTTPConnection('www.evl.fi')
    chapterNo = 0
    while True:
        chapterNo += 1

        http.request('GET', '/raamattu/1933,38/{0}.{1}.html'.format(
            book[1], chapterNo
        ))
        r = http.getresponse()
        if r.status != 200: break

        content = r.read().decode('iso-8859-1')

        verses = verse_re.findall(content)

        if book[0] == '2ch' and chapterNo == 23:
            verses[3] = ('4', '4', verses[3][2])
        if book[0] == 'ne' and chapterNo == 10:
            verses[0] = ('1', '1', verses[0][2] + u' ' + verses[1][2])
            verses[1:] = verses[2:]
        if book[0] == 'mt' and chapterNo == 17:
            verses[20:20] = [('21', '21', u'——')]
        if book[0] == 'mt' and chapterNo == 18:
            verses[10:10] = [('11', '11', u'——')]
        if book[0] == 'mt' and chapterNo == 23:
            verses[13:13] = [('14', '14', u'——')]
        if book[0] == 'mr' and chapterNo == 9:
            verses[43:43] = [('44', '44', u'——')]
            verses[45:45] = [('46', '46', u'——')]
        if book[0] == 'mr' and chapterNo == 11:
            verses[25:25] = [('26', '26', u'——')]
        if book[0] == 'mr' and chapterNo == 15:
            verses[27:27] = [('28', '28', u'——')]
        if book[0] == 'lu' and chapterNo == 17:
            verses[35:35] = [('36', '36', u'——')]
        if book[0] == 'lu' and chapterNo == 23:
            verses[16:16] = [('17', '17', u'——')]
        if book[0] == 'joh' and chapterNo == 5:
            verses[3:3] = [('4', '4', u'——')]
        if book[0] == 'ac' and chapterNo == 8:
            verses[36:36] = [('37', '37', u'——')]
        if book[0] == 'ac' and chapterNo == 15:
            verses[33:33] = [('34', '34', u'——')]
        if book[0] == 'ac' and chapterNo == 24:
            verses[6:6] = [('7', '7', u'——')]
        if book[0] == 'ac' and chapterNo == 28:
            verses[28:28] = [('29', '29', u'——')]

        for v, vn in izip(verses, count(1)):
            if v[0] != v[1]: raise Exception('verse no not consistent: {0} != {1}'.format(v[0], v[1]))
            if int(v[0]) != vn: raise Exception('invalid verse no: {0} (should be {1})'.format(v[0], vn))

        xml = create_xml(v[2].replace('\n', ' ').replace('  ', ' ') for v in verses)

        with open('{0}_{1:03}.html'.format(book[0], chapterNo), 'w') as f:
            f.write(xml.encode('utf-8'))

        c.execute("INSERT INTO html (langCode, bookCode, chapterno, html) VALUES (?, ?, ?, ?)", (
            'fi', book[0], chapterNo, xml
        ))

        print '{0}|{1}|{2}'.format(book[0], chapterNo, len(verses))
        sys.stdout.flush()

c.execute('vacuum')
db.commit()
