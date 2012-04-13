import sys
import struct
from itertools import *
import sqlite3

from createxml import create_xml

filename = sys.argv[1]
transCode = sys.argv[2]
langCode = sys.argv[3]

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]

content = open(filename).read()

pos = 0

key = '\x00n\x00p\x00a\x00s\x00a\x00l'
key2 = '\x00n\x00a\x00y\x00a\x00t'

chapterCount = []
verseCount = []

for bookNo in xrange(66):
    keypos = content.find(key, pos)

    key2pos = content.find(key2, pos)

    chapterCount.append(struct.unpack('>I', content[keypos + len(key) : keypos + len(key) + 4])[0])

    verseCount.append(struct.unpack('>{0}B'.format(chapterCount[-1]), content[key2pos + len(key2) : key2pos + len(key2) + chapterCount[-1]]))

    if filename == 'BibliaWarszawska.yes' and bookNo == 65:
        chapterCount[-1] -= 1
        verseCount[-1] = verseCount[-1][0:-1]

    pos = key2pos + 1

#    for v, cn in izip(verseCount[bookNo], count(1)):
#        print '{0}|{1}|{2}'.format(bookCodes[bookNo], cn, v)
    for v, cn in izip(verseCount[bookNo], count(1)):
        print '{0:6}, {1:03}, {2:03}'.format("'{0}'".format(bookCodes[bookNo]), cn, v)

textKey = 'teks________'
textpos = content.find(textKey)
text = content[textpos + len(textKey) + 4 : ]

verses = text.split('\n')


db = sqlite3.Connection(transCode + '.sqlite')
c = db.cursor()

c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

textpos = 0
for bookNo in xrange(66):
    for cn in range(chapterCount[bookNo]):
        xml = create_xml(
            v.decode('utf-8') for v in verses[textpos : textpos + verseCount[bookNo][cn]]
        )
        textpos += verseCount[bookNo][cn]
        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            langCode, bookCodes[bookNo], cn+1, xml
        ))

c.execute('vacuum')
db.commit()
