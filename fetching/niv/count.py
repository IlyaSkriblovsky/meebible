import httplib
import re
import sys

meebible_bookcodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
niv_bookcodes = [ 'gen', 'exod', 'lev', 'num', 'deut', 'josh', 'judg', 'ruth', '1sam', '2sam', '1kgs', '2kgs', '1chr', '2chr', 'ezra', 'neh', 'esth', 'job', 'ps', 'prov', 'eccl', 'song', 'isa', 'jer', 'lam', 'ezek', 'dan', 'hos', 'joel', 'amos', 'obad', 'jonah', 'mic', 'nah', 'hab', 'zeph', 'hag', 'zech', 'mal', 'matt', 'mark', 'luke', 'john', 'acts', 'rom', '1cor', '2cor', 'gal', 'eph', 'phil', 'col', '1thess', '2thess', '1tim', '2tim', 'titus', 'phlm', 'heb', 'jas', '1pet', '2pet', '1john', '2john', '3john', 'jude', 'rev', ]


c = httplib.HTTPConnection('www.youversion.com')

verse_re = re.compile(r'<span class="verse" id=')

for mee_bookcode, niv_bookcode in zip(meebible_bookcodes, niv_bookcodes):
    chapter = 0
    while True:
        chapter += 1

        url = '/bible/chapter/niv/{0}/{1}'.format(niv_bookcode, chapter)

        c.request('GET', url, '', {
            'User-Agent': 'Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_7; en-US) AppleWebKit/534.16 (KHTML, like Gecko) Chrome/10.0.648.205 Safari/534.16'
        })
        r = c.getresponse()

        if r.status != 200: break

        content = r.read()

        print '{0}|{1}|{2}'.format(
            mee_bookcode,
            chapter,
            len(verse_re.findall(content))
        )
        sys.stdout.flush()
