# -*- coding: utf-8 -*-

import re, sys, httplib, sqlite3, subprocess
from itertools import *

bookCodes = [ 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', ]
ebBookCodes = [ 'genesis', 'exodus', 'leviticus', 'numbers', 'deuteronomy', 'joshua', 'judges', 'ruth', '1samuel', '2samuel', '1kings', '2kings', '1chronicles', '2chronicles', 'ezra', 'nehemiah', 'esther', 'job', 'psalms', 'proverbs', 'ecclesiastes', 'songofsolomon', 'isaiah', 'jeremiah', 'lamentations', 'ezekiel', 'daniel', 'hosea', 'joel', 'amos', 'obadiah', 'jonah', 'micah', 'nahum', 'habakkuk', 'zephaniah', 'haggai', 'zechariah', 'malachi', 'matthew', 'mark', 'luke', 'john', 'acts', 'romans', '1corinthians', '2corinthians', 'galatians', 'ephesians', 'philippians', 'colossians', '1thessalonians', '2thessalonians', '1timothy', '2timothy', 'titus', 'philemon', 'hebrews', 'james', '1peter', '2peter', '1john', '2john', '3john', 'jude', 'revelation', ]

ebTransCode = 'amp'
transCode = 'amp'

script_re = re.compile(r'<script.*?</script>', re.DOTALL)

for bookCode, ebBookCode in izip(bookCodes, ebBookCodes):

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
        content = re.sub(r'(?<=[^!])--(?=[^>])', '—', content)
        content = re.sub(r' itemscope[^=]', ' ', content)
        content = re.sub(r'Media Kit</a>\n', 'Media Kit</a></li>\n', content)
        content = re.sub(r"<sub class='tip-tip-click.*?</sub>", ' ', content)


        orig_filename = 'orig_{0}_{1:03}.html'.format(bookCode, chapterNo)
        new_filename  = '{0}_{1:03}.html'.format(bookCode, chapterNo)

        with open(orig_filename, 'w') as f:
            f.write(content)

        result = subprocess.Popen(['xsltproc', '--nonet', 'ebible.xslt', orig_filename], stdout = subprocess.PIPE, stderr = None).communicate()[0]

        with open(new_filename, 'w') as f:
            f.write(result)

        print '{0}|{1}|{2}'.format(
            bookCode, chapterNo, len(result.split('class="verse"'))-1
        )
        sys.stdout.flush()
