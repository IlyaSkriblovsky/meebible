# -*- coding: utf-8 -*-

import httplib
import re
import sys
from itertools import izip, count

def get_chapter_count(bookNo):
    c = httplib.HTTPConnection('bibele.lv')

    c.request('POST', '/bibele/bibele.php', 'book={0}&chapter={1}'.format(bookNo, 1), {
        'Content-Type': 'application/x-www-form-urlencoded'
    })

    r = c.getresponse()
    content = r.read()

    match = re.search(r"<SELECT name=chapter_select.*?>(.*?)</SELECT>", content, re.DOTALL)
    if match is None:
        return 1

    payload = match.group(1).strip()

    def check_chapter((no, chapter_name)):
        match = re.match(r"^(\d+)", chapter_name)
        if int(match.group(1)) != no:
            raise Exception('Strange chapter number: {0} {1}'.format(bookNo, chapter_name))

    chapters = re.findall(r'<option.*?>(.*?)</option>', payload, re.DOTALL)
    for chapter in izip(count(1), chapters):
        check_chapter(chapter)

    return len(chapters)



def get_verses(bookNo, chapterNo):
    c = httplib.HTTPConnection('bibele.lv')

    c.request('POST', '/bibele/bibele.php', 'book={0}&chapter={1}'.format(bookNo, chapterNo), {
        'Content-Type': 'application/x-www-form-urlencoded'
    })

    r = c.getresponse()
    content = r.read()

    payload = re.search(r"<SPAN CLASS='normal'>(.*?)</SPAN>", content, re.DOTALL).group(1).strip()

    def clean_verse((no, verse)):
        match = re.match(r"^(\d+) (.*)", verse)
        if int(match.group(1)) != no:
            for v in verses: print v
            raise Exception('Strange verse number: {0} {1}:{2}'.format(bookNo, chapterNo, no))
        return match.group(2).strip()

    verses = [verse.strip() for verse in payload.split('<br>') if len(verse.strip()) > 0]

    # Exception for Actions 24:7, Actions 28:29
    if bookNo == 44 and chapterNo == 24: verses[6:6] = ['7 ———']
    if bookNo == 44 and chapterNo == 28: verses[28:28] = ['29 ———']

    return map(clean_verse, izip(count(1), verses))


# verses = get_verses(66, 1)
# for no, verse in izip(count(1), verses): print '[{0}]\t{1}'.format(no, verse)
# print len(verses)


for bookNo in xrange(1, 67):
    for chapterNo in xrange(1, get_chapter_count(bookNo) + 1):
        verses = get_verses(bookNo, chapterNo)
        print '{0}\t{1}\t{2}'.format(bookNo, chapterNo, len(verses))
        sys.stdout.flush()
