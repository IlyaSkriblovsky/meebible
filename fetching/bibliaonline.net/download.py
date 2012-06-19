# -*- coding: utf-8 -*-

import httplib, sqlite3, sys, re
from itertools import *

from createxml import create_xml

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]

# names = [ 'Gênesis', 'Êxodo', 'Levítico', 'Números', 'Deuteronômio', 'Josué', 'Juízes', 'Rute', '1 Samuel', '2 Samuel', '1 Reis', '2 Reis', '1 Crônicas', '2 Crônicas', 'Esdras', 'Neemias', 'Ester', 'Jó', 'Salmos', 'Provérbios', 'Eclesiastes', 'Cantares', 'Isaías', 'Jeremias', 'Lamentações', 'Ezequiel', 'Daniel', 'Oséas', 'Joel', 'Amós', 'Obadias', 'Jonas', 'Miquéias', 'Naum', 'Habacuque', 'Sofonias', 'Ageu', 'Zacarías', 'Malaquias', 'Mateus', 'Marcos', 'Lucas', 'João', 'Atos', 'Romanos', '1 Coríntios', '2 Coríntios', 'Gálatas', 'Efésios', 'Filipenses', 'Colossenses', '1 Tessalonicenses', '2 Tessalonicenses', '1 Timóteo', '2 Timóteo', 'Tito', 'Filemon', 'Hebreus', 'Tiago', '1 Pedro', '2 Pedro', '1 João', '2 João', '3 João', 'Judas', 'Apocalipse' ]

versao = 1
langCode = 't'
transCode = 'para'
encoding = 'iso-8859-1'


verse_re = re.compile(r"<span id='t_(\d+)_(\d+)_(\d+)_(\d+)'>(.*?)</span>", re.DOTALL)


db = sqlite3.Connection('{0}.sqlite'.format(transCode))
c = db.cursor()

c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


http = httplib.HTTPConnection('www.bibliaonline.net')

for bookNo, bookCode in enumerate(bookCodes, 1):
    chapterNo = 0

    while True:
        chapterNo += 1
        pageNo = 0

        raw_verses = []

        while True:
            pageNo += 1
            url = '/biblia/?livro={0}&versao={1}&capitulo={2}&pag_ini={3}'.format(
                bookNo, versao, chapterNo, (pageNo-1) * 30
            )
            http.request('GET', url)
            r = http.getresponse()
            content = r.read()

            found = verse_re.findall(content)

            if len(found) > 0:
                raw_verses += found
            else:
                break

        if len(raw_verses) == 0:
            break

        for i, rv in enumerate(raw_verses, 1):
            if int(rv[0]) != versao or \
               int(rv[1]) != bookNo or \
               int(rv[2]) != chapterNo or \
               int(rv[3]) != i:
               raise Exception('Strange verse no: {0}'.format(rv))

        verses = [rv[4].decode(encoding).strip() for rv in raw_verses]
        xml = create_xml(verses)

        c.execute('INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)', (
            langCode, bookCode, chapterNo, xml
        ))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(raw_verses))
        sys.stdout.flush()

c.execute('VACUUM')
db.commit()
