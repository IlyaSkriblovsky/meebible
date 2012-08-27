import httplib
from itertools import *
import os


bookCodes = ( 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', )

amenUrls = ('AntiguoTestamento/Genesis/Genesis', 'AntiguoTestamento/Exodo/Exodo', 'AntiguoTestamento/Levitico/Levitico', 'AntiguoTestamento/Numeros/Numeros', 'AntiguoTestamento/Deuteronomio/Deuteronomio', 'AntiguoTestamento/Josue/Josue', 'AntiguoTestamento/Jueces/Jueces', 'AntiguoTestamento/Rut/Rut', 'AntiguoTestamento/1Samuel/1Samuel', 'AntiguoTestamento/2Samuel/2Samuel', 'AntiguoTestamento/1Reyes/1Reyes', 'AntiguoTestamento/2Reyes/2Reyes', 'AntiguoTestamento/1Cronicas/1Cronicas', 'AntiguoTestamento/2Cronicas/2Cronicas', 'AntiguoTestamento/Esdras/Esdras', 'AntiguoTestamento/Nehemias/Nehemias', 'AntiguoTestamento/Ester/Ester', 'AntiguoTestamento/Job/Job', 'AntiguoTestamento/Salmos/Salmo', 'AntiguoTestamento/Proverbios/Proverbios', 'AntiguoTestamento/Eclesiastes/Eclesiastes', 'AntiguoTestamento/Cantares/Cantares', 'AntiguoTestamento/Isaias/Isaias', 'AntiguoTestamento/Jeremias/Jeremias', 'AntiguoTestamento/Lamentaciones/Lamentaciones', 'AntiguoTestamento/Ezequiel/Ezequiel', 'AntiguoTestamento/Daniel/Daniel', 'AntiguoTestamento/Oseas/Oseas', 'AntiguoTestamento/Joel/Joel', 'AntiguoTestamento/Amos/Amos', 'AntiguoTestamento/Abdias/Abdias', 'AntiguoTestamento/Jonas/Jonas', 'AntiguoTestamento/Miqueas/Miqueas', 'AntiguoTestamento/Nahum/Nahum', 'AntiguoTestamento/Habacuc/Habacuc', 'AntiguoTestamento/Sofonias/Sofonias', 'AntiguoTestamento/Hageo/Hageo', 'AntiguoTestamento/Zacarias/Zacarias', 'AntiguoTestamento/Malaquias/Malaquias', 'NuevoTestamento/Mateo/Mateo', 'NuevoTestamento/Marcos/Marcos', 'NuevoTestamento/Lucas/Lucas', 'NuevoTestamento/Juan/Juan', 'NuevoTestamento/Hechos/Hechos', 'NuevoTestamento/Romanos/Romanos', 'NuevoTestamento/1Corintios/1Corintios', 'NuevoTestamento/2Corintios/2Corintios', 'NuevoTestamento/Galatas/Galatas', 'NuevoTestamento/Efesios/Efesios', 'NuevoTestamento/Filipenses/Filipenses', 'NuevoTestamento/Colosenses/Colosenses', 'NuevoTestamento/1Tesalonicenses/1Tesalonicenses', 'NuevoTestamento/2Tesalonicenses/2Tesalonicenses', 'NuevoTestamento/1Timoteo/1Timoteo', 'NuevoTestamento/2Timoteo/2Timoteo', 'NuevoTestamento/Tito/Tito', 'NuevoTestamento/Filemon/Filemon', 'NuevoTestamento/Hebreos/Hebreos', 'NuevoTestamento/Santiago/Santiago', 'NuevoTestamento/1Pedro/1Pedro', 'NuevoTestamento/2Pedro/2Pedro', 'NuevoTestamento/1Juan/1Juan', 'NuevoTestamento/2Juan/2Juan', 'NuevoTestamento/3Juan/3Juan', 'NuevoTestamento/Judas/Judas', 'NuevoTestamento/Apocalipsis/Apocalipsis', )


amenTransCode = 'RV1960'


try: os.mkdir('src')
except OSError: pass


for bookCode, amenUrl in izip(bookCodes, amenUrls):
    chapterNo = 0
    while True:
        chapterNo += 1

        url = '/{0}/{1}{2}.htm'.format(amenTransCode, amenUrl, chapterNo)

        http = httplib.HTTPConnection('www.amen-amen.net')
        http.request('GET', url)
        response = http.getresponse()
        if response.status != 200:
            break


        with open('src/{0}_{1:03}.html'.format(bookCode, chapterNo), 'w') as f:
            f.write(response.read())

        print '{0}|{1}'.format(bookCode, chapterNo)
