# -*- coding: utf-8 -*-

import sqlite3
import httplib
import urllib
from itertools import izip, count
import re
import sys

from createxml import create_xml


http = httplib.HTTPConnection('bibele.lv')

trans = sqlite3.Connection('../../share/trans.sqlite')
c = trans.cursor()

c.execute("SELECT bookCode FROM books WHERE transCode='blv' ORDER BY bookNo")
bookCodes = [r[0] for r in c.fetchall()]

verse_re = re.compile(r"<SPAN CLASS='normal'>(.*?)</SPAN>", re.DOTALL)
verse_no_re = re.compile(r'\d+\s+')

# for bookNo, bookCode in izip(count(1), bookCodes):
for bookNo, bookCode in [(44, 'ac')]:
    c.execute("SELECT count(*) FROM chapterSize WHERE transCode='blv' AND bookCode=?", (bookCode,))
    chapters = c.fetchall()[0][0]

    for chapterNo in xrange(1, chapters + 1):
        params = urllib.urlencode({
            'book': bookNo,
            'chapter': chapterNo
        })
        http.request("POST", "/bibele/bibele.php", params, {
            "Content-Type": "application/x-www-form-urlencoded"
        })
        response = http.getresponse()
        content = response.read()

        payload = verse_re.search(content).group(1)

        verses = [re.sub(r'\d+\s+', '', verse.strip()) for verse in payload.split('<br>') if len(verse.strip()) > 0]

        if bookCode == 'ac':
            if chapterNo == 24: verses.insert(6, '———')
            if chapterNo == 28: verses.insert(28, '———')

        f = open('{0}_{1:003}.html'.format(bookCode, chapterNo), 'w')
        f.write(create_xml(verses))
        f.close()

        print '{0}\t{1}\t{2}'.format(bookCode, chapterNo, len(verses))
        sys.stdout.flush()
