import httplib
from itertools import *
import sys

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
seBooks = [ line.strip().split('|') for line in open('books').readlines() ]

seTransCode = sys.argv[1]

http = httplib.HTTPConnection('www.bibeln.se')

for bookCode, seBook in izip(bookCodes, seBooks):
    url = '/las/{0}/{1}?content=pure'.format(seTransCode, seBook[0])

    http.request('GET', url)
    r = http.getresponse()
    print bookCode, r.status

    content = r.read()

    with open('orig_{0}/{1}.html'.format(seTransCode, bookCode), 'w') as f:
        f.write('<chapter>' + content + '</chapter>')
