import sqlite3
import httplib
import re
import sys

from createxml import create_xml

trans = sqlite3.Connection('../../share/trans.sqlite')
c = trans.cursor()

c.execute("SELECT bookCode, bookName FROM books WHERE transCode='kjv' ORDER BY bookNo")
books = c.fetchall()

verse_re = re.compile(r"<a href=\"http://www.kingjamesbibleonline.org/[^\"]*?-(\d+?)/\" title='View more translations[^']*?'>(.*?)</a></p>")

for book in books:
    c.execute("SELECT count(*) FROM chapterSize WHERE transCode='kjv' AND bookCode=?", (book[0],))
    chapters = c.fetchall()[0][0]

    for chapterNo in xrange(1, chapters + 1):
        url = '/{0}-Chapter-{1}/'.format(
            book[1].replace(' ', '-'),
            chapterNo
        )

        http = httplib.HTTPConnection('www.kingjamesbibleonline.org')
        http.request("GET", url)
        response = http.getresponse()
        content = response.read()

        verses = [v[1] for v in verse_re.findall(content)]

        f = open('{0}_{1:003}.html'.format(book[0], chapterNo), 'w')
        f.write(create_xml(verses))
        f.close()

        print '{0}\t{1}\t{2}'.format(book[0], chapterNo, len(verses))
        sys.stdout.flush()
