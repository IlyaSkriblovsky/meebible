import httplib
import re
import sys

meebible_bookcodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
yv_bookcodes = [ 'gen', 'exod', 'lev', 'num', 'deut', 'josh', 'judg', 'ruth', '1sam', '2sam', '1kgs', '2kgs', '1chr', '2chr', 'ezra', 'neh', 'esth', 'job', 'ps', 'prov', 'eccl', 'song', 'isa', 'jer', 'lam', 'ezek', 'dan', 'hos', 'joel', 'amos', 'obad', 'jonah', 'mic', 'nah', 'hab', 'zeph', 'hag', 'zech', 'mal', 'matt', 'mark', 'luke', 'john', 'acts', 'rom', '1cor', '2cor', 'gal', 'eph', 'phil', 'col', '1thess', '2thess', '1tim', '2tim', 'titus', 'phlm', 'heb', 'jas', '1pet', '2pet', '1john', '2john', '3john', 'jude', 'rev', ]

yv_trans = 'esv'


c = httplib.HTTPConnection('www.youversion.com')

verse_re = re.compile(r'<span class="verse" id=')

for mee_bookcode, yv_bookcode in zip(meebible_bookcodes, yv_bookcodes):
    chapter = 0
    while True:
        chapter += 1

        url = '/bible/chapter/{0}/{1}/{2}'.format(yv_trans, yv_bookcode, chapter)

        c.request('GET', url, '', {
            'User-Agent': 'Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_7; en-US) AppleWebKit/534.16 (KHTML, like Gecko) Chrome/10.0.648.205 Safari/534.16'
        })
        r = c.getresponse()

        if r.status != 200: break

        content = r.read()


        verse_count = len(verse_re.findall(content))

        if mee_bookcode == 'ac' and chapter == 15:
            verse_count += 1    # v 34
        if mee_bookcode == 'ac' and chapter == 24:
            verse_count += 1    # v 7
        if mee_bookcode == 'ac' and chapter == 28:
            verse_count += 1    # v 29
        if mee_bookcode == 'ac' and chapter == 8:
            verse_count += 1    # v 37
        if mee_bookcode == 'joh' and chapter == 5:
            verse_count += 1    # v 4
        if mee_bookcode == 'lu' and chapter == 17:
            verse_count += 1    # v 36
        if mee_bookcode == 'lu' and chapter == 23:
            verse_count += 1    # v 17
        if mee_bookcode == 'mr' and chapter == 11:
            verse_count += 1    # v 26
        if mee_bookcode == 'mr' and chapter == 15:
            verse_count += 1    # v 28
        if mee_bookcode == 'mr' and chapter == 7:
            verse_count += 1    # v 16
        if mee_bookcode == 'mr' and chapter == 9:
            verse_count += 2    # v 44, 46
        if mee_bookcode == 'mt' and chapter == 12:
            verse_count += 1    # v 47
        if mee_bookcode == 'mt' and chapter == 17:
            verse_count += 1    # v 21
        if mee_bookcode == 'mt' and chapter == 18:
            verse_count += 1    # v 11
        if mee_bookcode == 'mt' and chapter == 23:
            verse_count += 1    # v 14
        if mee_bookcode == 'ro' and chapter == 16:
            verse_count += 1    # v 24

        print '{0}|{1}|{2}'.format(
            mee_bookcode,
            chapter,
            verse_count
        )
        sys.stdout.flush()
