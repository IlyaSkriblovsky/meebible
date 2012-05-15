# -*- coding: utf-8 -*-

import httplib, sys
from itertools import *

bookCodes = ( 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', )
theirBookCodes = ('geneza', 'exodul', 'leviticul', 'numeri', 'deuteronomul', 'iosua', 'judecatorii', 'rut', '1-samuel', '2-samuel', '1-imparati', '2-imparati', '1-cronici', '2-cronici', 'ezra', 'neemia', 'estera', 'iov', 'psalmii', 'proverbele', 'eclesiastul', 'cantarea-cantarilor', 'isaia', 'ieremia', 'plangerile-lui-ieremia', 'ezechiel', 'daniel', 'osea', 'ioel', 'amos', 'obadia', 'iona', 'mica', 'naum', 'habacuc', 'tefania', 'hagai', 'zaharia', 'maleahi', 'matei', 'marcu', 'luca', 'ioan', 'faptele-apostolilor', 'romani', '1-corinteni', '2-corinteni', 'galateni', 'efeseni', 'filipeni', 'coloseni', '1-tesaloniceni', '2-tesaloniceni', '1-timotei', '2-timotei', 'tit', 'filimon', 'evrei', 'iacov', '1-petru', '2-petru', '1-ioan', '2-ioan', '3-ioan', 'iuda', 'apocalipsa', )

start = int(sys.argv[1])

http = httplib.HTTPConnection('biblia.resursecrestine.ro')

for bookNo, bookCode, theirBookCode in izip(count(start), bookCodes[start:], theirBookCodes[start:]):
    chapterNo = 0
    while True:
        chapterNo += 1

        url = '/{0}/{1}'.format(theirBookCode, chapterNo)
        print bookNo, url
        http.request('GET', url)
        r = http.getresponse()
        if r.status != 200:
            r.read()
            break

        with open('orig/{0}_{1:03}.html'.format(bookCode, chapterNo), 'w') as f:
            f.write(r.read())

        print '{0}|{1}'.format(bookCode, chapterNo)
        sys.stdout.flush()
