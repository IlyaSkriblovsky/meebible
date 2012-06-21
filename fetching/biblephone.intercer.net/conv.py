import sqlite3, sys

from createxml import create_xml


bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]


db = sqlite3.Connection('{0}.sqlite'.format(sys.argv[2]))
c = db.cursor()

c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


lines = open(sys.argv[1]).readlines()[1:]
lines = [line.strip(' \r\n') for line in lines]


prevBook = 0
prevChapter = 0

content = []

for line in lines:
    parts = line.split('\t')
    if parts[0][-1] not in ['O', 'N']: break

    bookNo = int(parts[0][:-1])
    chapterNo = int(parts[1])
    verseNo = int(parts[2])

    if bookNo != prevBook:
        content.append([[]])
    elif chapterNo != prevChapter:
        content[bookNo-1].append([])

    content[bookNo-1][chapterNo-1].append(parts[3].decode('utf-8'))

    assert len(content) == bookNo
    assert len(content[bookNo-1]) == chapterNo
    assert len(content[bookNo-1][chapterNo-1]) == verseNo


    prevBook = bookNo
    prevChapter = chapterNo



for bookNo, book in enumerate(content, 0):
    for chapterNo, chapter in enumerate(book, 1):
        xml = create_xml(chapter)

        c.execute('INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)', (
            sys.argv[3], bookCodes[bookNo], chapterNo, xml
        ))

        print '{0}|{1}|{2}'.format(
            bookCodes[bookNo], chapterNo, len(chapter)
        )
        sys.stdout.flush()


c.execute('VACUUM')
db.commit()
