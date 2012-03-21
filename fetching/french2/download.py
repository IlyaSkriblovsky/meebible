# -*- coding:utf-8 -*-
import httplib, sys, re, sqlite3, simplejson, urllib, subprocess, os
from itertools import *

from createxml import create_xml

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
boOrder = ( 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 're',)

verse_re = re.compile(r'<li id="v(\d+)" value="\1" class="v"(?: title="[^"]*")?(?: gotitle=\'[^\']*\')?>(.*?)</li>', re.DOTALL)

http = httplib.HTTPConnection('bibleonline.ru')

for bookCode in bookCodes:
    chapterNo = 0
    while True:
        chapterNo += 1

        url = '/bible/fra/{0:02}/{1:02}/'.format(
            boOrder.index(bookCode)+1,
            chapterNo
        )
        http.request('GET', url)
        content = http.getresponse().read()

        raw_verses = verse_re.findall(content)

        if len(raw_verses) == 0: break

        for v, vn in izip(raw_verses, count(1)):
            if int(v[0]) != vn: raise Exception('Strange verse no: {0}'.format(v[0]))

        verses = [v[1].decode('utf-8').strip() for v in raw_verses]

        with open('{0}_{1:03}.html'.format(bookCode, chapterNo), 'w') as f:
            f.write(create_xml(verses).encode('utf-8'))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(verses))
        sys.stdout.flush()
