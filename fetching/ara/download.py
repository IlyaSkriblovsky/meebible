engname = {
    "ge":   "Genesis",
    "ex":   "Exodus",
    "le":   "Leviticus",
    "nu":   "Numbers",
    "de":   "Deuteronomy",
    "jos":  "Joshua",
    "jg":   "Judges",
    "ru":   "Ruth",
    "1sa":  "1 Samuel",
    "2sa":  "2 Samuel",
    "1ki":  "1 Kings",
    "2ki":  "2 Kings",
    "1ch":  "1 Chronicles",
    "2ch":  "2 Chronicles",
    "ezr":  "Ezra",
    "ne":   "Nehemiah",
    "es":   "Esther",
    "job":  "Job",
    "ps":   "Psalms",
    "pr":   "Proverbs",
    "ec":   "Ecclesiastes",
    "ca":   "Song of Songs",
    "isa":  "Isaiah",
    "jer":  "Jeremiah",
    "la":   "Lamentations",
    "eze":  "Ezekiel",
    "da":   "Daniel",
    "ho":   "Hosea",
    "joe":  "Joel",
    "am":   "Amos",
    "ob":   "Obadiah",
    "jon":  "Jonah",
    "mic":  "Micah",
    "na":   "Nahum",
    "hab":  "Habakkuk",
    "zep":  "Zephaniah",
    "hag":  "Haggai",
    "zec":  "Zechariah",
    "mal":  "Malachi",
    "mt":   "Matthew",
    "mr":   "Mark",
    "lu":   "Luke",
    "joh":  "John",
    "ac":   "Acts",
    "ro":   "Romans",
    "1co":  "1 Corinthians",
    "2co":  "2 Corinthians",
    "ga":   "Galatians",
    "eph":  "Ephesians",
    "php":  "Philippians",
    "col":  "Colossians",
    "1th":  "1 Thessalonians",
    "2th":  "2 Thessalonians",
    "1ti":  "1 Timothy",
    "2ti":  "2 Timothy",
    "tit":  "Titus",
    "phm":  "Philemon",
    "heb":  "Hebrews",
    "jas":  "James",
    "1pe":  "1 Peter",
    "2pe":  "2 Peter",
    "1jo":  "1 John",
    "2jo":  "2 John",
    "3jo":  "3 John",
    "jude": "Jude",
    "re":   "Revelation",
}


import sqlite3
import httplib
import re
import sys

sys.path.append('..')
from createxml import create_xml

transCode = 'ara'

trans = sqlite3.Connection('../../share/trans.sqlite')
c = trans.cursor()

c.execute("SELECT bookCode, bookName FROM books WHERE transCode='{0}' ORDER BY bookNo".format(transCode))
books = c.fetchall()


payload_re = re.compile(r'<span class="arabictext">([^<]*?)<br>')


http = httplib.HTTPConnection('copticchurch.net')

for book in books:
    c.execute("SELECT count(*) FROM chapterSize WHERE transCode='{0}' AND bookCode=?".format(transCode), (book[0],))
    chapters = c.fetchall()[0][0]

    for chapterNo in xrange(1, chapters + 1):
        url = "/cgibin/bible/index.php?r={0}+{1}&version=SVD&showVN=1".format(
            engname[book[0]].replace(' ', '+'), chapterNo
        )

        http.request("GET", url)
        content = http.getresponse().read().decode('cp1256')
        payload = payload_re.search(content).group(1)

        verses = [v.strip().split(' ', 1)[1].strip() for v in payload.strip().split('\n')]

        f = open('{0}_{1:03}.html'.format(book[0], chapterNo), 'w')
        f.write(create_xml(verses).encode('utf-8'))
        f.close()

        print '{0}\t{1}\t{2}'.format(book[0], chapterNo, len(verses))
        sys.stdout.flush()
