# -*- coding: utf-8 -*-

import httplib, sqlite3, re, sys
from itertools import *

from createxml import create_xml


trans = 'acf'



bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
theirBookCodes = [ 'gn', 'ex', 'lv', 'nm', 'dt', 'js', 'jz', 'rt', '1sm', '2sm', '1rs', '2rs', '1cr', '2cr', 'ed', 'ne', 'et', 'j%C3%B3', 'sl', 'pv', 'ec', 'ct', 'is', 'jr', 'lm', 'ez', 'dn', 'os', 'jl', 'am', 'ob', 'jn', 'mq', 'na', 'hc', 'sf', 'ag', 'zc', 'ml', 'mt', 'mc', 'lc', 'jo', 'atos', 'rm', '1co', '2co', 'gl', 'ef', 'fp', 'cl', '1ts', '2ts', '1tm', '2tm', 'tt', 'fm', 'hb', 'tg', '1pe', '2pe', '1jo', '2jo', '3jo', 'jd', 'ap',]

skipBooks = 0
bookCodes[0:skipBooks] = []
theirBookCodes[0:skipBooks] = []


verse_re = re.compile(r"<p class='(?:even|odd) verse' verse='(\d+)'>\n(.+)\n</p>")


http = httplib.HTTPConnection('www.bibliaonline.com.br')

for bookCode, theirBookCode in izip(bookCodes, theirBookCodes):
    chapterNo = 0
    while True:
        chapterNo += 1

        url = '/{0}/{1}/{2}'.format(trans, theirBookCode, chapterNo)

        http.request('GET', url)
        r = http.getresponse()
        content = r.read()
        if r.status != 200: break


        raw_verses = verse_re.findall(content)

        if bookCode == 'mt' and chapterNo == 2:
            raw_verses[15:16] = [
                ('16', 'Então, Herodes, vendo que tinha sido iludido pelos magos, irritou-se muito e mandou matar todos os meninos que havia em Belém e em todos os seus contornos, de dois anos para baixo, segundo o tempo que diligentemente inquirira dos magos.'),
                ('17', 'Então, se cumpriu o que foi dito pelo profeta Jeremias, que diz:'),
            ]
        if bookCode == 'mt' and chapterNo == 22:
            raw_verses[14:14] = [
                ('15', 'Então, retirando-se os fariseus, consultaram entre si como o surpreenderiam em alguma palavra.')
            ]
            raw_verses[22:22] = [
                ('23', 'No mesmo dia, chegaram junto dele os saduceus, que dizem não haver ressurreição, e o interrogaram,')
            ]
        if bookCode == 'mt' and chapterNo == 26:
            raw_verses[0:0] = [('1', 'E aconteceu que, quando Jesus concluiu todos esses discursos, disse aos seus discípulos:')]
            raw_verses[5:5] = [('6', 'E, estando Jesus em Betânia, em casa de Simão, o leproso,')]
            raw_verses[13:13] = [('14', 'Então, um dos doze, chamado Judas Iscariotes, foi ter com os príncipes dos sacerdotes')]
            raw_verses[16:16] = [('17', 'E, no primeiro dia da Festa dos Pães Asmos, chegaram os discípulos junto de Jesus, dizendo: Onde queres que preparemos a comida da Páscoa?')]
            raw_verses[25:25] = [('26', 'Enquanto comiam, Jesus tomou o pão, e, abençoando-o, o partiu, e o deu aos discípulos, e disse: Tomai, comei, isto é o meu corpo.')]
        if bookCode == '1pe' and chapterNo == 2:
            raw_verses[3:3] = [('4', 'E, chegando-vos para ele, a pedra viva, reprovada, na verdade, pelos homens, mas para com Deus eleita e preciosa,')]
            raw_verses[12:12] = [('13', 'Sujeitai-vos, pois, a toda ordenação humana por amor do Senhor; quer ao rei, como superior;')]
        if bookCode == '1pe' and chapterNo == 3:
            raw_verses[0:0] = [('1', 'Semelhantemente, vós, mulheres, sede sujeitas ao vosso próprio marido, para que também, se algum não obedece à palavra, pelo procedimento de sua mulher seja ganho sem palavra,')]
            raw_verses[7:7] = [('8', 'E, finalmente, sede todos de um mesmo sentimento, compassivos, amando os irmãos, entranhavelmente misericordiosos e afáveis,')]

        for v, vn in izip(raw_verses, count(1)):
            if int(v[0]) != vn: raise Exception('Strange verse num: {0} {1}:{2}'.format(bookCode, chapterNo, v[0]))


        verses = [v[1].decode('utf-8').strip() for v in raw_verses]

        with open('{0}_{1:03}.html'.format(bookCode, chapterNo), 'w') as f:
            f.write(create_xml(verses).encode('utf-8'))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(raw_verses))
        sys.stdout.flush()
