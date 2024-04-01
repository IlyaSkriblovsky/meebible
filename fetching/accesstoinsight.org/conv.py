from bs4 import BeautifulSoup
import re
import sqlite3
import glob


def convert_chapter(book: str, chapter_no: int):
    html = BeautifulSoup(open(f'dl/{book}-{chapter_no:03}.html').read(), 'html.parser')
    chapter = html.find('div', attrs={'class': 'chapter'})

    verses = []
    ps = chapter.find_all('p', recursive=False)
    for p_no, p in enumerate(ps, 1):
        verses.append(f'<div class="par"><div class="verse" verse="{p_no}"><div class="verse-label">{p_no}</div>{p.text}</div></div>')

    return len(verses), '\n'.join([
        '<html>',
        '<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"/></head>',
        '<body layout="indented">',
        *verses,
        '</body>',
        '</html>',
    ])


db = sqlite3.Connection('tipitaka.sqlite')
c = db.cursor()

c.execute('DROP TABLE IF EXISTS html')
c.execute('CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY(langCode, bookCode, chapterNo))')

filename_re = re.compile(r'^dl/([a-z]+)-([0-9]+).html$')
for fn in glob.glob('dl/*.html'):
    match = filename_re.match(fn)
    book = match.group(1)
    chapter = int(match.group(2))
    verses_count, html = convert_chapter(book, chapter)
    c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", ('e', book, chapter, html))
    print(f"INSERT INTO chapterSize VALUES ('tipitaka', '{book}', {chapter}, {verses_count});")

db.commit()
c.execute('VACUUM')
