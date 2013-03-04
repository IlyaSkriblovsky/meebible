import sqlite3, sys, re, os
from itertools import *

import xml.dom.minidom

from createxml import create_xml

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]


transCode = sys.argv[2]

db = sqlite3.Connection("{0}.sqlite".format(transCode))
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


def create_verse_tag(doc, num):
    v = doc.createElement('VERS')
    v.setAttribute('vnumber', str(num))
    return v


def insert_empty_verse(verses, doc, number):
    verses[number-1 : number-1] = [ create_verse_tag(doc, number) ]


xml = xml.dom.minidom.parse(sys.argv[1])

bookElements = get_childs(xml.documentElement, 'BIBLEBOOK')

for bookCode, bookElement in izip(bookCodes, bookElements):
    chapterElements = get_childs(bookElement, 'CHAPTER')

    for chapterNo, chapterElement in enumerate(chapterElements, 1):
        verseElements = get_childs(chapterElement, 'VERS')


        def fix_missing_verse(errBookCode, errChapterNo, errVerseNo):
            if bookCode == errBookCode and chapterNo == errChapterNo:
                insert_empty_verse(verseElements, xml, errVerseNo)


        def fix_missing_verses(errlist):
            for b, c, v in errlist:
                fix_missing_verse(b, c, v)


        if transCode == 'hindi_bsi':
            fix_missing_verses((
                ('ex' ,  4, 14),
                ('ex' ,  6,  7),
                ('ex' , 30, 16),
                ('le' , 23, 22),
                ('le' , 27,  8),
                ('nu' , 17,  8),
                ('de' , 19,  5),
                ('jos', 20,  6),
                ('jg' , 12,  4),
                ('jg' , 18, 14),
                ('1sa', 20, 41),
                ('2sa', 14,  7),
                ('1ch', 17,  9),
                ('1ch', 29, 20),
                ('2ch',  6, 27),
                ('isa', 54,  4),
                ('isa', 66,  5),
                ('isa', 66, 17),
                ('jer', 32, 29),
                ('jer', 33,  5),
                ('la' ,  1, 13),
                ('eze', 27, 11),
                ('eze', 29,  5),
                ('eze', 34, 13),
                ('eze', 48, 18),
                ('da' ,  2, 28),
                ('mr' , 16, 14),
                ('ac' ,  1, 11),
                ('ac' ,  7, 52),
                ('ac' , 23, 15),
                ('php',  2, 12),
                ('heb',  2,  9),
                ('heb',  8,  5),
                ('jas',  5, 11),
                ('1pe',  1,  2),
            ))


        verses = []
        for verseNo, verseElement in enumerate(verseElements, 1):
            if verseNo != int(verseElement.attributes['vnumber'].value):
                raise Exception('Invalid verse no {0} {1}:{2}'.format(bookCode, chapterNo, verseNo))
            if verseElement.firstChild is not None and verseElement.firstChild.nextSibling is not None:
                raise Exception('Seems like child tag in VERS {0} {1}:{2}'.format(bookCode, chapterNo, verseNo))

            if verseElement.firstChild is not None:
                verses.append(verseElement.firstChild.nodeValue)
            else:
                verses.append('')

        xml_text = create_xml(verses)

        # with open('{0}_{1}.html'.format(bookCode, chapterNo), 'w') as f:
        #     f.write(xml_text.encode('utf-8'))

        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            sys.argv[3], bookCode, chapterNo, xml_text
        ))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(verses))
        sys.stdout.flush()

c.execute('vacuum')
db.commit()
