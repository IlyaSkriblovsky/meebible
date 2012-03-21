import httplib, sys, re, sqlite3
from itertools import *

from createxml import create_xml

bookCodes = ('ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', )
bookUrls = ('genesis', 'exodo', 'levitico', 'numeros', 'deuteronomio', 'josue', 'jueces', 'rut', '1samuel', '2samuel', '1reyes', '2reyes', '1cronicas', '2cronicas', 'esdras', 'nehemias', 'ester', 'job', 'salmos', 'proverbios', 'eclesiastes', 'cantares', 'isaias', 'jeremias', 'lamentaciones', 'ezequiel', 'daniel', 'oseas', 'joel', 'amos', 'abdias', 'jonas', 'miqueas', 'nahum', 'habacuc', 'sofonias', 'hageo', 'zacarias', 'malaquias', 'mateo', 'marcos', 'lucas', 'juan', 'hechos', 'romanos', '1corintios', '2corintios', 'galatas', 'efesios', 'filipenses', 'colosenses', '1tesalonicenses', '2tesalonicenses', '1timoteo', '2timoteo', 'tito', 'filemon', 'hebreos', 'santiago', '1pedro', '2pedro', '1juan', '2juan', '3juan', 'judas', 'apocalipsis', )

http = httplib.HTTPConnection('www.su-iglesia.com')


verse_re = re.compile(r'(\d+)\.(\d+)\.(?:&nbsp;)*\s+(.*?)</span></p>', re.DOTALL)

db = sqlite3.Connection('rvr.sqlite')
cursor = db.cursor()
cursor
cursor.execute("DROP TABLE IF EXISTS html")
cursor.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

for bookCode, bookUrl in izip(bookCodes, bookUrls):
    content = file('{0}.htm'.format(bookUrl)).read().decode('iso-8859-1')

    raw_verses = verse_re.findall(content)
    # print raw_verses

    chapters = []

    chapterNo = 0
    expectedVerseNo = 1
    for rv in raw_verses:
        cn = int(rv[0])
        if cn != chapterNo:
            if cn != chapterNo + 1: raise Exception('Unexpected chapterNo: {0} {1}'.format(bookCode, cn))
            chapterNo = cn
            expectedVerseNo = 1
            chapters.append([])

        vn = int(rv[1])
        if vn != expectedVerseNo: raise Exception('Unexpected verseNo: {0} {1}:{2}'.format(bookCode, cn, vn))
        expectedVerseNo += 1

        chapters[cn-1].append(rv[2].replace('&nbsp', ' ').strip())


    for c, cn in izip(chapters, count(1)):
        cursor.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            's', bookCode, cn, create_xml(c)
        ))
        print "{0:6}, {1:03}, {2:03}".format(
            "'{0}'".format(bookCode),
            cn, len(c)
        )


cursor.execute('VACUUM')
db.commit()
