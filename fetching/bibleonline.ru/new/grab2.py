import httplib
import re
import sys
import itertools

import createxml

lang = sys.argv[1]

bookcodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 're', ]
bookstart = 1


verse_re = re.compile(r'<li id="v(\d+)" value="\1" class="v"(?: title="[^"]*" gotitle=\'[^\']*\')?>(.*?)</li>')


for bookNo, bookCode in itertools.izip(itertools.count(bookstart), bookcodes):
    chapter = 0
    while True:
        chapter += 1

        url = '/bible/{0}/{1:02}/{2:02}/'.format(
            lang, bookNo, chapter
        )


        c = httplib.HTTPConnection('bibleonline.ru')
        c.request('GET', url, '', {
            'User-Agent': 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.38 Safari/535.11',
            'Referer': 'http://bibleonline.ru/bible/rus/01/01/'
        })
        content = c.getresponse().read()

        verses = verse_re.findall(content)

        if len(verses) == 0: break

        for n, v in itertools.izip(itertools.count(1), verses):
            if int(v[0]) != n: raise Exception('Strange verse number: {0} {1}:{2}'.format(bookCode, chapter, n))

        texts = [v[1].decode('utf-8') for v in verses]

        f = open('{0}/{1}_{2:03}.html'.format(
            lang, bookCode, chapter
        ), 'w')
        f.write(createxml.create_xml(texts).encode('utf-8'))
        f.close()

        print '{0}|{1}|{2}'.format(bookCode, chapter, len(verses))
        sys.stdout.flush()
