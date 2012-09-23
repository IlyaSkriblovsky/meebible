from xml.dom import minidom

import sqlite3


def PROCESS_BOOK(bookCode, content):

    def copyContent(input, output):
        for ch in input.childNodes:
            if ch.nodeType == minidom.Node.TEXT_NODE:
                text = output.appendChild(output.ownerDocument.createTextNode(ch.nodeValue))
            else:
                raise Exception('Unknown node in copyContent: {0}'.format(ch))


    def convert(input):

        outputs = []

        def newChapter():
            outDoc = minidom.Document()
            outHtml = outDoc.appendChild(outDoc.createElement('html'))
            outHead = outHtml.appendChild(outDoc.createElement('head'))
            outMeta = outHead.appendChild(outDoc.createElement('meta'))
            outMeta.setAttribute('http-equiv', 'Content-Type')
            outMeta.setAttribute('content', 'text/html; charset=utf-8')
            outBody = outHtml.appendChild(outDoc.createElement('body'))

            return outDoc, outBody


        curVerse = None
        verseCount = 0
        curVerseNo = 0

        def newVerse(span, par, verseCount, curVerseNo):
            curVerse = par.appendChild(par.ownerDocument.createElement('div'))
            curVerse.setAttribute('class', 'verse')
            curVerse.setAttribute('verse', span.firstChild.nodeValue)
            label = curVerse.appendChild(curVerse.ownerDocument.createElement('div'))
            label.setAttribute('class', 'verse-label')
            label.appendChild(label.ownerDocument.createTextNode(span.firstChild.nodeValue))

            verseCount += 1

            curVerseNo = span.firstChild.nodeValue

            return curVerse, verseCount, curVerseNo


        body = None
        par = None


        for child in input.childNodes:

            if child.nodeName in ('h2', 'h1'):
                if not body or (child.attributes.has_key('class') and child.attributes['class'].value in ('ch_pos', 'ch ch_pos')):
                    doc, body = newChapter()
                    outputs.append(doc)

                heading = body.appendChild(body.ownerDocument.createElement('div'))
                heading.setAttribute('class', 'heading')
                copyContent(child, heading)

            elif child.nodeName == 'p':
                if body:
                    par = body.appendChild(body.ownerDocument.createElement('div'))
                    par.setAttribute('class', 'par')


                for sb in child.childNodes:
                    if sb.nodeName == 'span' and sb.attributes['class'].value == 'ch': pass
                    elif sb.nodeName == 'span' and sb.attributes['class'].value in ('ch_pos', 'ch ch_pos'):
                        doc, body = newChapter()
                        outputs.append(doc)
                        curVerse = None
                        par = body.appendChild(body.ownerDocument.createElement('div'))
                        par.setAttribute('class', 'par')

                    elif sb.nodeName == 'span' and sb.attributes['class'].value == 'v':
                        if par is None:
                            if body is None:
                                doc, body = newChapter()
                                outputs.append(doc)
                            par = body.appendChild(body.ownerDocument.createElement('div'))
                            par.setAttribute('class', 'par')
                        curVerse, verseCount, curVerseNo = newVerse(sb, par, verseCount, curVerseNo)

                    elif sb.nodeType == minidom.Node.TEXT_NODE:
                        if curVerse:
                            curVerse.appendChild(curVerse.ownerDocument.createTextNode(sb.nodeValue))
                        else:
                            if sb.nodeValue.strip() != '':
                                raise Exception('text node in <p> while curVerse is None: {0}'.format(sb))

                    elif sb.nodeName == 'span' and sb.attributes['class'].value == 'damage':
                        i = curVerse.appendChild(curVerse.ownerDocument.createElement('i'))
                        copyContent(sb, i)

                    elif sb.nodeName == 'em':
                        i = curVerse.appendChild(curVerse.ownerDocument.createElement('i'))
                        copyContent(sb, i)

                    else:
                        print sb.attributes['class'].value
                        raise Exception('Unknown node in <p>: {0}'.format(sb))

            elif child.nodeName == 'div' and child.attributes['class'].value == 'poetry':
                for sb in child.childNodes:
                    if sb.nodeName == 'div' and sb.attributes['class'].value in ('line', 'line2'):
                        bodyJustCreated = False
                        if not body:
                            doc, body = newChapter()
                            outputs.append(doc)
                            bodyJustCreated = True

                        z = body.appendChild(body.ownerDocument.createElement('div'))
                        if sb.attributes['class'].value == 'line':
                            z.setAttribute('class', 'z')
                        v = None

                        for ssb in sb.childNodes:
                            if ssb.nodeType == minidom.Node.TEXT_NODE:
                                if ssb.nodeValue.strip() != '':
                                    if v is None:
                                        v = z.appendChild(z.ownerDocument.createElement('div'))
                                        v.setAttribute('class', 'verse')
                                        v.setAttribute('verse', curVerseNo)
                                    v.appendChild(v.ownerDocument.createTextNode(ssb.nodeValue))

                            elif ssb.nodeName == 'em' or (ssb.nodeName == 'span' and ssb.attributes['class'].value == 'damage'):
                                if v is None:
                                    v = z.appendChild(z.ownerDocument.createElement('div'))
                                    v.setAttribute('class', 'verse')
                                    v.setAttribute('verse', curVerseNo)
                                i = v.appendChild(v.ownerDocument.createElement('i'))
                                copyContent(ssb, i)

                            elif ssb.nodeName == 'span' and ssb.attributes['class'].value == 'ch': pass
                            elif ssb.nodeName == 'span' and ssb.attributes['class'].value in ('ch_pos', 'ch ch_pos'):
                                if not bodyJustCreated:
                                    doc, body = newChapter()
                                    outputs.append(doc)
                                    curVerse = None
                                    z = body.appendChild(body.ownerDocument.createElement('div'))

                            elif ssb.nodeName == 'span' and ssb.attributes['class'].value == 'v':
                                curVerse, verseCount, curVerseNo = newVerse(ssb, z, verseCount, curVerseNo)
                                v = curVerse
                            else:
                                raise Exception('Unknown verse in poetry: {0}'.format(ssb))

#                        if len(sb.childNodes) == 1 and sb.firstChild.nodeType == minidom.Node.TEXT_NODE:
#                            if sb.firstChild.nodeValue.strip() != '':
#                                # z = curVerse.appendChild(curVerse.ownerDocument.createElement('div'))
#                                # z.setAttribute('class', 'z')
#                                # copyContent(sb, z)
#                                z = body.appendChild(body.ownerDocument.createElement('div'))
#                                if sb.attributes['class'].value == 'line':
#                                    z.setAttribute('class', 'z')
#                                v = z.appendChild(z.ownerDocument.createElement('div'))
#                                v.setAttribute('verse', curVerseNo)
#                                copyContent(sb, v)
#
#                        else:
#                            firstNotText = sb.firstChild
#                            while firstNotText.nodeType == minidom.Node.TEXT_NODE:
#                                firstNotText = firstNotText.nextSibling
#
#                            if firstNotText.nodeName == 'span' and firstNotText.attributes['class'].value == 'v':
#                                z = body.appendChild(body.ownerDocument.createElement('div'))
#                                z.setAttribute('class', 'z')
#
#                                curVerse, verseCount, curVerseNo = newVerse(firstNotText, z, verseCount, curVerseNo)
#
#                                next = firstNotText.nextSibling
#                                while next:
#                                    if next.nodeType == minidom.Node.TEXT_NODE:
#                                        curVerse.appendChild(curVerse.ownerDocument.createTextNode(next.nodeValue))
#                                    elif next.nodeName == 'span' and next.attributes['class'].value == 'damage':
#                                        i = curVerse.appendChild(curVerse.ownerDocument.createElement('i'))
#                                        copyContent(next, i)
#                                    elif next.nodeName == 'em':
#                                        i = curVerse.appendChild(curVerse.ownerDocument.createElement('i'))
#                                        copyContent(next, i)
#                                    else:
#                                        raise Exception('Unknown tag in <div class="line">: {0}'.format(next))
#                                    next = next.nextSibling
#
#                            elif firstNotText.nodeName == 'span' and firstNotText.attributes['class'].value in ('ch', 'ch_pos', 'ch ch_pos'):
#                                doc, body = newChapter()
#                                outputs.append(doc)
#                                curVerse = None
#
#                            elif firstNotText.nodeName == 'span' and firstNotText.attributes['class'].value == 'damage':
#                                z = body.appendChild(body.ownerDocument.createElement('div'))
#                                z.setAttribute('class', 'z')
#                                v = z.appendChild(z.ownerDocument.createElement('div'))
#                                v.setAttribute('verse', curVerseNo)
#
#                                i = v.appendChild(v.ownerDocument.createElement('i'))
#                                copyContent(firstNotText, i)
#
#                            elif firstNotText.nodeName == 'em':
#                                next = firstNotText
#                                while next:
#                                    if next.nodeName == 'em':
#                                        i = curVerse.appendChild(curVerse.ownerDocument.createElement('i'))
#                                        copyContent(next, i)
#                                    elif next.nodeType == minidom.Node.TEXT_NODE:
#                                        curVerse.appendChild(curVerse.ownerDocument.createTextNode(next.nodeValue))
#                                    else:
#                                        print next.attributes['id'].value
#                                        raise Exception('Unknown tag [3]: {0}'.format(next))
#
#                                    next = next.nextSibling
#
#                            else:
#                                raise Exception('Unknown node in poetry[2]: {0}'.format(firstNotText))
#
                    elif sb.nodeType == minidom.Node.TEXT_NODE and sb.nodeValue.strip() == '':
                        pass
                    else:
                        raise Exception('Unknown node in poetry: {0}'.format(sb))

            elif child.nodeName == 'hr':
                body.appendChild(body.ownerDocument.createElement('hr'))

            elif child.nodeName == 'h3' and child.attributes['class'].value == 'speaker':
                div = body.appendChild(body.ownerDocument.createElement('div'))
                div.setAttribute('class', 'speaker')
                b = div.appendChild(div.ownerDocument.createElement('b'))
                copyContent(child, b)

            elif child.nodeType == minidom.Node.TEXT_NODE:
                if child.nodeValue.strip() != '':
                    raise Exception('Not empty text node in convert: {0}'.format(child))

            else:
                raise Exception('Unknown node in convert: {0}'.format(child))

        return outputs


    content = content.decode('utf-8').replace(u'\u00ad', u'').encode('utf-8')

    dom = minidom.parseString(content)

    chapters = convert(dom.documentElement)
    for chapterNo, chapter in enumerate(chapters, 1):
        def find(node):
            r = []
            if node.nodeName == 'div' and node.attributes.has_key('verse'):
                r.append(node.attributes['verse'].value)
            if node.childNodes:
                for ch in node.childNodes:
                    r += find(ch)

            return r


        vns = find(chapter.documentElement)

        if bookCode == 'ex' and chapterNo == 7:
            vns[3:3] = ['4']

        # v = 1
        # for vn in vns:
        #     if int(vn) == v: pass
        #     elif int(vn) == v+1: v = v + 1
        #     else:
        #         raise Exception('strange verse no: {0} {1}'.format(chapterNo, vn))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(set(vns)))
        # print chapter.toprettyxml().encode('utf-8')

    return [
        chapter.toprettyxml().replace(u'<?xml version="1.0" ?>\n', u'')
        for chapter in chapters
    ]


bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]

import sys
transCode = sys.argv[1]


db = sqlite3.Connection("se_{0}.sqlite".format(transCode))
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")


for bookCode in bookCodes:
    # print bookCode
    for chapterNo, xml in enumerate(PROCESS_BOOK(bookCode, open('orig_{0}/{1}.html'.format(transCode, bookCode)).read()), 1):
        c.execute('INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)', (
            'z', bookCode, chapterNo, xml
        ))


c.close()
db.commit()

# bc = 'am'
# PROCESS_BOOK(bc, open('orig_2k/{0}.html'.format(bc)).read())
