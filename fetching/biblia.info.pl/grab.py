# -*- coding: utf-8 -*-

# translations = [ 'bt', 'bw', 'br', 'bg' ]
# translations = [ 'bw', 'br', 'bg' ]
translations = [ 'bw', 'bg' ]


books = [
    {
        'code': line.strip().split('|')[1],
        'biplCode': line.strip().split('|')[2],
    }
    for line in open('books').readlines()
]


import sys, httplib, urllib, re, itertools

c = httplib.HTTPConnection('www.biblia.info.pl')


verse_re = re.compile(r'<SPAN class="nrWersetu">\((.+)\)</SPAN> (.*)\n')

for trans in translations:
    for book in books:
        chapter = 0
        while True:
            chapter += 1

            c.request('POST', '/cgi-bin/biblia-nawigator.cgi', urllib.urlencode({
                'tlumaczenie': trans,
                'st': 'i',
                'nw': 'tak',
                'ks': book['biplCode'],
                'rozdzial': chapter,
            }), {
                'Content-Type': 'application/x-www-form-urlencoded',
            })
            r = c.getresponse()
            content = r.read()

            if r.status != 200: print 'HTTP STATUS {0}'.format(r.status)

            # if content.find('<SPAN class="blad1">') != -1: break


            verses = verse_re.findall(content)

            if len(verses) == 0: break

            # <EXCEPTIONS>
            if trans == 'bt' and book['code'] == 'jos' and chapter == 15:
                verses[58:60] = [
                    (59, verses[58][1] + ' ' + verses[59][1])
                ]
            if trans == 'bt' and book['code'] == 'jg' and chapter == 13:
                verses[15:17] = [
                    (16, verses[16][1] + ' ' + verses[15][1])
                ]
            if trans == 'bt' and book['code'] == 'jg' and chapter == 20:
                verses[21], verses[22] = verses[22], verses[21]
            if trans == 'bt' and book['code'] == '1ki' and chapter == 15:
                print verses
                verses[5:5] = [(6, '')]
                print verses

            if trans == 'br' and book['code'] == '2sa' and chapter == 19:
                verses[8:10] = [
                    (9, verses[8][1] + ' ' + verses[9][1])
                ]
            # </EXCEPTIONS>

            for v, i in itertools.izip(verses, itertools.count(1)):
                try:
                    if int(v[0]) != i: raise Exception('Strange verse number: {0} ({1})'.format(v[0], book['biplCode']))
                except ValueError:
                    print book['biplCode']
                    print v
                    raise

            print '{0}|{1}|{2}|{3}'.format(
                trans, book['code'], chapter, len(verses)
            )
            sys.stdout.flush()


            # <EXCEPTIONS>
            if book['code'] == 'ob': break
            if book['code'] == 'phm': break
            if book['code'] == '2jo': break
            if book['code'] == '3jo': break
            if book['code'] == 'jude': break
            # </EXCEPTIONS>
