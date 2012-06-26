import sqlite3, sys, re, os
from itertools import *

import xml.dom.minidom

from createxml import create_xml

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]


db = sqlite3.Connection("{0}.sqlite".format(sys.argv[2]))
c = db.cursor()

c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


def get_childs(parent, nodeName):
    res = []
    child = parent.firstChild
    while child:
        if child.nodeName == nodeName:
            res.append(child)
        child = child.nextSibling
    return res

xml = xml.dom.minidom.parse(sys.argv[1])

bookElements = get_childs(xml.documentElement, 'BIBLEBOOK')

for bookCode, bookElement in izip(bookCodes, bookElements):
    chapterElements = get_childs(bookElement, 'CHAPTER')

    for chapterNo, chapterElement in enumerate(chapterElements, 1):
        verseElements = get_childs(chapterElement, 'VERS')

        verses = []
        for verseNo, verseElement in enumerate(verseElements, 1):
            if verseNo != int(verseElement.attributes['vnumber'].value):
                raise Exception('Invalid verse no {0} {1}:{2}'.format(bookCode, chapterNo, verseNo))
            if verseElement.firstChild.nextSibling is not None:
                raise Exception('Seems like child tag in VERS {0} {1}:{2}'.format(bookCode, chapterNo, verseNo))

            verses.append(verseElement.firstChild.nodeValue)

        xml = create_xml(verses)

        # with open('{0}_{1}.html'.format(bookCode, chapterNo), 'w') as f:
        #     f.write(xml.encode('utf-8'))

        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            sys.argv[3], bookCode, chapterNo, xml
        ))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(verses))
        sys.stdout.flush()

c.execute('vacuum')
db.commit()
