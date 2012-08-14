import re, os, sys, sqlite3
import xml.dom.minidom


bookCodes = ( 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're' )

transCode = sys.argv[1]


db = sqlite3.Connection('{0}.sqlite'.format(transCode))
c = db.cursor()

c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


heading_re = re.compile(r'<div class="titoli">(.*?)</div>')
pars_re = re.compile(r'<p[^>]*><sup>')
verses_re = re.compile('<sup>(\d+)</sup>(.*?)(?:</p>)?\n')


for bookCode in bookCodes:
    chapterNo = 0

    while True:
        chapterNo += 1

        filename = 'orig_{0}/{1}_{2:03}.html'.format(transCode, bookCode, chapterNo)
        if not os.path.exists(filename): break

        content = open(filename).read().decode('iso-8859-1')

        headings = [ ('heading', match) for match in heading_re.finditer(content) ]
        pars = [ ('par', match) for match in pars_re.finditer(content) ]
        verses = [ ('verse', match) for match in verses_re.finditer(content) ]

        all = headings + pars + verses
        all.sort(key = lambda m: m[1].start())

        html = u'<html>\n<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"/></head>\n<body>\n'

        par_opened = False
        i = 0
        for type, match in all:
            i += 1
            if type == 'heading':
                if par_opened:
                    html += u'</div>\n'
                    par_opened = False

                html += u'<div class="heading">{0}</div>\n'.format(match.group(1).replace('<br>', ''))

            elif type == 'par':
                if bookCode not in ['ps', 'la']:
                    if par_opened: html += u'</div>\n'
                    html += u'<div class="par">\n'
                    par_opened = True

            elif type == 'verse':
                verseNo = int(match.group(1))
                verse = match.group(2).strip()

                parBreakAfter = verse.endswith('<br>')

                verse = verse.replace(u'<p class="rientrato">', ' ').strip()

                def cleanend(line, tail):
                    if line.endswith(tail):
                        return line[ : -len(tail)].strip()
                    else:
                        return line

                cleaned = verse
                first = True
                while first or cleaned != verse:
                    first = False
                    verse = cleaned
                    cleaned = cleanend(cleaned, u'<br>')
                    # cleaned = cleanend(cleaned, u'<p class="rientrato">')
                    cleaned = cleanend(cleaned, u'<dd')
                    cleaned = cleanend(cleaned, u'<!versetto num=1><sup>1')

                verse = cleaned

                if verse.endswith(u'<br></i>'): verse = verse[:-8] + u'</i>'
                if verse.endswith(u'<i>'): verse = verse[:-3]
                if verse.find('<i>') != -1 and verse.find('</i>') == -1: verse += '</i>'

                while True:
                    if verse.find('</i>') != -1:
                        if verse.find('<i>') == -1 or verse.find('<i>') > verse.find('</i>'):
                            verse = verse.replace(u'</i>', '', 1)
                        else:
                            break
                    else:
                        break


                notags = verse
                notags = notags.replace(u'<br>', u'')
                notags = notags.replace(u'<i>', u'').replace(u'</i>', '')
                if notags.find(u'<') != -1:
                    raise Exception("Tag in {0} {1} {2}:{3}".format(transCode, bookCode, chapterNo, verseNo))

                text = verse.replace(u'<br>', u'<br/>')
                if bookCode in ['ps', 'la']:
                    html += u'    <div class="par"><div class="verse" verse="{0}"><div class="verse-label">{0}</div>{1}</div></div>\n'.format(verseNo, text)
                else:
                    html += u'    <div class="verse" verse="{0}"><div class="verse-label">{0}</div>{1}</div>\n'.format(verseNo, text)
                    if parBreakAfter and i < len(all):
                        if par_opened:
                            html += u'</div>\n'
                        else:
                            html = html.replace('<body>', '<body>\n<div class="par">')
                            html += u'</div>\n'
                        html += u'<div class="par">\n'
                        par_opened = True

        if par_opened: html += u'</div>\n'

        html += u'</body>\n</html>\n'

        html = html.replace(u'<div class="par">\n</div>\n', '')
        html = html.replace(u'<div class="heading"></div>\n', '')

        # print html

        with open('dest_{0}/{1}_{2:03}.html'.format(transCode, bookCode, chapterNo), 'w') as f:
            f.write(html.encode('utf-8'))

        xml.dom.minidom.parseString(html.encode('utf-8'))

        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            'i', bookCode, chapterNo, html
        ))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(verses))


c.execute('VACUUM')
db.commit()
db.close()
