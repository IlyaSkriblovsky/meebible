import sqlite3
import re, os
from itertools import *

import xml.dom.minidom
from createxml import create_xml

bookCodes = ( 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', )

langCode = 's'
transCode = 'rvr'


db = sqlite3.Connection("{0}.sqlite".format(transCode))
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


verse_re = re.compile(r'(\d+):(\d+)(.*?)(?=\d+:\d+|</body>)', re.DOTALL)

font_re = re.compile(r'</?font[^>]*>')

known_tags_re = re.compile(r'</?i>|</?u>|</?b>|<br ?/>')


for bookCode in bookCodes:
    chapterNo = 0
    while True:
        chapterNo += 1


        filename = 'src/{0}_{1:03}.html'.format(bookCode, chapterNo)
        if not os.path.exists(filename): break

        content = open(filename).read().decode('latin-1')

        # Fixes
        if transCode == 'rvr':
            if bookCode == 'de'    and chapterNo == 1  : content = content.replace('1:1:41', '1:41')
            elif bookCode == '2ch' and chapterNo == 1  : content = content.replace('1: 5', '1:5')
            elif bookCode == 'ps'  and chapterNo == 115: content = content.replace('115:<i>3', '115:3 <i>')
            elif bookCode == 'isa' and chapterNo == 13 : content = content.replace(' Yo mand', '\n13:3 Yo mand')
            elif bookCode == 'isa' and chapterNo == 62 : content = content.replace('<center></center>', '')
            elif bookCode == 'eze' and chapterNo == 1  : content = content.replace('1:1:25', '1:25')
            elif bookCode == 'eze' and chapterNo == 47 : content = content.replace('<a href="http://www.iglesia.net/biblia/libros/apocalipsis.html#cap22">\n</a>', ' ')
            elif bookCode == 'ho'  and chapterNo == 1  : content = content.replace('<a href="http://www.iglesia.net/biblia/libros/2cronicas.html#cap28">\n</a>', ' ')
            elif bookCode == 'ob'  and chapterNo == 1  : content = content.replace('1:De', '1:16 De')
            elif bookCode == 'hab' and chapterNo == 2  : content = content.replace('<a href="http://www.iglesia.net/biblia/libros/isaias.html#cap11"><img height="14" alt="Isaias 11. 9" hspace="3" src="http://www.iglesia.net/biblia/libros/BLUE.GIF" width="14" border="0"></a>', '')
            elif bookCode == 'mt'  and chapterNo == 11 : content = content.replace('11:<font color="#000000">25</font>', '11:25')
            elif bookCode == 'mt'  and chapterNo == 12 : content = content.replace('<a href="http://www.iglesia.net/biblia/libros/isaias.html#cap42"><br>\n</a>', '\n')
            elif bookCode == 'ac'  and chapterNo == 7  : content = content.replace('<a href="http://atrevete.com/biblia/rv/genesis.shtml#cap17">\n</a>', ' ')
            elif bookCode == 'ac'  and chapterNo == 13 : content = content.replace('<a href="http://atrevete.com/biblia/rv/lucas.shtml#cap10">\n</a>', ' ')
            elif bookCode in ('ps', 'ca', 'la'):
                # content = re.sub(r'<br>\n(\d+:\d+)', r'</i><br/>\n<i>\1', content)
                content = re.sub(r'</?i>', '', content)


        content = re.sub(font_re, '', content)

        content = re.sub(r'<i>(\d+:\d+)(?:&nbsp;)?</i>', r'\1 ', content)
        content = re.sub(r'<i>(\d+:\d+)', r'\1 <i>', content)


        verses = verse_re.findall(content)

        texts = []

        for i, v in enumerate(verses, 1):
            vChapterNo = int(v[0])
            if vChapterNo != chapterNo:
                raise Exception('Invalid chapter no: {0} {1}:{2}'.format(bookCode, chapterNo, verseNo))

            verseNo = int(v[1])
            if verseNo != i:
                for v in verses: print v
                raise Exception('Invalid verse no: {0} {1}:{2} ({3})'.format(bookCode, chapterNo, verseNo, i))

            text = v[2].strip()
            text = text.replace('&nbsp;', ' ').replace('  ', ' ')
            text = text.replace('\n', ' ')
            if text.endswith('<br>'): text = text[:-4]
            if text.endswith('</p>'): text = text[:-4]
            text = text.strip()
            text = text.replace('<br>', '<br/>')


            withoutTags = re.sub(known_tags_re, '', text)
            if withoutTags.find('<') != -1:
                raise Exception('Unknown tag: {0} {1}:{2}'.format(bookCode, chapterNo, verseNo))

            texts.append(text)


            #print chapterNo, verseNo, text

        html = create_xml(texts)
        try:
            xml.dom.minidom.parseString(html.encode('utf-8'))
        except Exception as e:
            print html
            raise Exception('Failed to parse {0} {1} {2}'.format(bookCode, chapterNo, e))


        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            langCode, bookCode, chapterNo, html
        ))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(verses))



c.execute('VACUUM')
c.close()
db.commit()
