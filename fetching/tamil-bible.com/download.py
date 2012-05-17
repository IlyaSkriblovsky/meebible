import httplib, re, sqlite3, sys
from itertools import *

bookCodes = ( 'ge', 'ex', 'le', 'nu', 'de', 'jos', 'jg', 'ru', '1sa', '2sa', '1ki', '2ki', '1ch', '2ch', 'ezr', 'ne', 'es', 'job', 'ps', 'pr', 'ec', 'ca', 'isa', 'jer', 'la', 'eze', 'da', 'ho', 'joe', 'am', 'ob', 'jon', 'mic', 'na', 'hab', 'zep', 'hag', 'zec', 'mal', 'mt', 'mr', 'lu', 'joh', 'ac', 'ro', '1co', '2co', 'ga', 'eph', 'php', 'col', '1th', '2th', '1ti', '2ti', 'tit', 'phm', 'heb', 'jas', '1pe', '2pe', '1jo', '2jo', '3jo', 'jude', 're', )
theirBookCodes = ( 'Genesis', 'Exodus', 'Leviticus', 'Numbers', 'Deuteronomy', 'Joshua', 'Judges', 'Ruth', 'I Samuel', 'II Samuel', 'I Kings', 'II Kings', 'I Chronicles', 'II Chronicles', 'Ezra', 'Nehemiah', 'Esther', 'Job', 'Psalms', 'Proverbs', 'Ecclesiastes', 'Song of Solomon', 'Isaiah', 'Jeremiah', 'Lamentations', 'Ezekiel', 'Daniel', 'Hosea', 'Joel', 'Amos', 'Obadiah', 'Jonah', 'Micah', 'Nahum', 'Habakkuk', 'Zephaniah', 'Haggai', 'Zechariah', 'Malachi', 'Matthew', 'Mark', 'Luke', 'John', 'Acts', 'Romans', 'I Corinthians', 'II Corinthians', 'Galatians', 'Ephesians', 'Philippians', 'Colossians', 'I Thessalonians', 'II Thessalonians', 'I Timothy', 'II Timothy', 'Titus', 'Philemon', 'Hebrews', 'James', 'I Peter', 'II Peter', 'I John', 'II John', 'III John', 'Jude', 'Revelation', )

total_re = re.compile(r'has Total of <b>(\d+)</b> Chapters')

http = httplib.HTTPConnection('www.tamil-bible.com')

def fetch(url):
    global http
    while True:
        http.request('GET', url)
        r = http.getresponse()
        if r.status == 200: return r.read()
        print r.status
        http = httplib.HTTPConnection('www.tamil-bible.com')

start = int(sys.argv[1])
for bookNo, bookCode, theirBookCode in izip(count(start), bookCodes[start:], theirBookCodes[start:]):

    url = '/lookup.php?Book={0}&Chapter=200'.format(theirBookCode).replace(' ', '+')
    print url
    errpage = fetch(url)

    m = total_re.search(errpage)
    print '{0} chapters'.format(int(m.group(1)))

    for chapterNo in xrange(1, int(m.group(1)) + 1):
        print bookNo, bookCode

        url = '/lookup.php?Book={0}&Chapter={1}'.format(theirBookCode, chapterNo).replace(' ', '+')
        print url
        content = fetch(url)

        with open('orig/{0}_{1:03}.html'.format(bookCode, chapterNo), 'w') as f: f.write(content)
