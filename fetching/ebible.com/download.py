# -*- coding: utf-8 -*-

import re, sys, httplib, sqlite3, subprocess
from itertools import *

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
ebBookCodes = [ 'genesis', 'exodus', 'leviticus', 'numbers', 'deuteronomy', 'joshua', 'judges', 'ruth', '1samuel', '2samuel', '1kings', '2kings', '1chronicles', '2chronicles', 'ezra', 'nehemiah', 'esther', 'job', 'psalms', 'proverbs', 'ecclesiastes', 'songofsolomon', 'isaiah', 'jeremiah', 'lamentations', 'ezekiel', 'daniel', 'hosea', 'joel', 'amos', 'obadiah', 'jonah', 'micah', 'nahum', 'habakkuk', 'zephaniah', 'haggai', 'zechariah', 'malachi', 'matthew', 'mark', 'luke', 'john', 'acts', 'romans', '1corinthians', '2corinthians', 'galatians', 'ephesians', 'philippians', 'colossians', '1thessalonians', '2thessalonians', '1timothy', '2timothy', 'titus', 'philemon', 'hebrews', 'james', '1peter', '2peter', '1john', '2john', '3john', 'jude', 'revelation', ]

ebTransCode = 'msg'
transCode = 'msg'
langCode = 'e'

script_re = re.compile(r'<script.*?</script>', re.DOTALL)

out_verse_re = re.compile(r'<div class="verse-label">(\d+)</div>')

db = sqlite3.Connection("{0}.sqlite".format(transCode))
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

start = 0
for bookCode, ebBookCode in izip(bookCodes[start:], ebBookCodes[start:]):

    http = httplib.HTTPConnection('ebible.com')

    chapterNo = 0
    while True:
        chapterNo += 1

        url = '/{0}/{1}/{2}'.format(
            ebTransCode, ebBookCode, chapterNo
        )

        http.request('GET', url)
        response = http.getresponse()

        if response.status != 200:
            break

        content = response.read()

        content = re.sub(script_re, '', content)
        content = re.sub(r'<meta[^>]*[^/]>', '', content)
        content = re.sub(r'&nbsp;', '&#160;', content)
        content = re.sub(r'&mdash;', '—', content)
        content = re.sub(r'(?<=[^!])--(?=[^>])', '—', content)
        content = re.sub(r' itemscope[^=]', ' ', content)
        content = re.sub(r'Media Kit</a>\n', 'Media Kit</a></li>\n', content)
        content = re.sub(r"<sub class='tip-tip-click.*?</sub>", ' ', content)
        content = re.sub(r'<\$F', '', content)
        content = re.sub(r'\$E>', '', content)
        content = re.sub(r'[]', 'and', content)

        if ebTransCode == 'amp' and bookCode == 'ps' and chapterNo in (125, 129, 133):
            content = re.sub(r'</p>\n\s*(<h3>.*?</h3>)\n\s*<p>', r'\1', content)

        if ebTransCode == 'amp' and bookCode == 'mr' and chapterNo == 9:
            content = re.sub(r'(<sup id="41009045" >45</sup>)', r'<sup>44</sup><span class="verse "></span>\n\1', content)
            content = re.sub(r'(<sup id="41009047" >47</sup>)', r'<sup>46</sup><span class="verse "></span>\n\1', content)


        orig_filename = 'orig_{0}_{1:03}.html'.format(bookCode, chapterNo)
        new_filename  = '{0}_{1:03}.html'.format(bookCode, chapterNo)

        with open(orig_filename, 'w') as f:
            f.write(content)

        result = subprocess.Popen(['xsltproc', '--nonet', 'ebible.xslt', orig_filename], stdout = subprocess.PIPE, stderr = None).communicate()[0]

        with open(new_filename, 'w') as f:
            f.write(result)

        actual_verses = len(result.split('class="verse"'))-1
        last_verse = int(out_verse_re.findall(result)[-1])
        if ebTransCode != 'msg':
            if actual_verses != last_verse: raise Exception("verse count doesn't match")

        c.execute('INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)', (
            langCode, bookCode, chapterNo, result.decode('utf-8')
        ))

        print '{0}|{1}|{2}'.format(bookCode, chapterNo, last_verse)
        sys.stdout.flush()

c.execute('vacuum')
c.close()
db.commit()
