import httplib, os, sys
from itertools import *

bookCodes = ( 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're' )
itCodes = ( 'gn', 'es', 'lv', 'nm', 'dt', 'gs', 'gdc', 'rut', '1sam', '2sam', '1re', '2re', '1cr', '2cr', 'esd', 'ne', 'est', 'gb', 'sal', 'prv', 'qo', 'cant', 'is', 'ger', 'lam', 'ez', 'dan', 'os', 'gl', 'am', 'abd', 'gio', 'mi', 'na', 'abac', 'so', 'ag', 'za', 'ml', 'mt', 'mc', 'lc', 'gv', 'at', 'rm', '1cor', '2cor', 'gal', 'ef', 'fil', 'col', '1ts', '2ts', '1tm', '2tm', 'tt', 'fl', 'eb', 'gc', '1pt', '2pt', '1gv', '2gv', '3gv', 'giu', 'ap' )


transCode = sys.argv[1]

urlTransCode = {
    'it_cei08': 'Versione_CEI2008=3',
    'it_cei74': 'Versione_CEI74=1',
    'it_tilc' : 'Versione_TILC=2'
}


try: os.mkdir('orig_{0}'.format(transCode))
except OSError: pass


http = httplib.HTTPConnection('www.lachiesa.it')


for bookCode, itCode in izip(bookCodes, itCodes):
    chapterNo = 0

    while True:
        chapterNo += 1

        url = '/bibbia.php?ricerca=citazione&Citazione={0}%20{1}&{2}&VersettoOn=1&Cerca=Cerca'.format(
            itCode, chapterNo, urlTransCode[transCode]
        )
        http.request('GET', url)

        response = http.getresponse().read()

        if response.find('e la ricerca non ha dato risultati') != -1: break

        with open('orig_{0}/{1}_{2:03}.html'.format(transCode, bookCode, chapterNo), 'w') as f:
            f.write(response)

        print bookCode, chapterNo
