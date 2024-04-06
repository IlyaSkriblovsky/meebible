from argparse import ArgumentParser
from bs4 import BeautifulSoup
import sqlite3

parser = ArgumentParser()
parser.add_argument('input_xml_filename')
parser.add_argument('lang_code')
parser.add_argument('trans_code')
args = parser.parse_args()
print(args)

book_name2code = {
    "Genesis":     'ge',
    "Exodus":      'ex',
    "Leviticus":   'le',
    "Numbers":     'nu',
    "Deuteronomy": 'de',
# 'jos',
# 'jg',
# 'ru',
# '1sa',
# '2sa',
# '1ki',
# '2ki',
# '1ch',
# '2ch',
# 'ezr',
# 'ne',
# 'es',
# 'job',
# 'ps',
# 'pr',
# 'ec',
# 'ca',
# 'isa',
# 'jer',
# 'la',
# 'eze',
# 'da',
# 'ho',
# 'joe',
# 'am',
# 'ob',
    "Jonah": 'jon',
# 'mic',
# 'na',
# 'hab',
# 'zep',
# 'hag',
# 'zec',
# 'mal',
    "Matthew":         'mt',
    "Mark":            'mr',
    "Luke":            'lu',
    "John":            'joh',
    "Acts":            'ac',
    "Romans":          'ro',
    "1 Corinthians":   '1co',
    "2 Corinthians":   '2co',
    "Galatians":       'ga',
    "Ephesians":       'eph',
    "Philippians":     'php',
    "Colossians":      'col',
    "1 Thessalonians": '1th',
    "2 Thessalonians": '2th',
    "1 Timothy":       '1ti',
    "2 Timothy":       '2ti',
    "Titus":           'tit',
    "Philemon":        'phm',
    "Hebrews":         'heb',
    "James":           'jas',
    "1 Peter":         '1pe',
    "2 Peter":         '2pe',
    "1 John":          '1jo',
    "2 John":          '2jo',
    "3 John":          '3jo',
    "Jude":            'jude',
    "Revelation":      're',
}

db = sqlite3.Connection(f'{args.trans_code}.sqlite')
c = db.cursor()
c.execute('DROP TABLE IF EXISTS html')
c.execute('CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY(langCode, bookCode, chapterNo))')
db.commit()

books = []
chapter_sizes = []

xml = BeautifulSoup(open(args.input_xml_filename).read(), 'xml')
for book_no, book in enumerate(xml.find_all('BIBLEBOOK'), 1):
    book_name = book['bname']
    book_code = book_name2code[book_name]
    books.append((book_code, book_no, book_name))
    for chapter_no, chapter in enumerate(book.find_all('CHAPTER'), 1):
        assert chapter_no == int(chapter['cnumber'])
        lines = []
        for verse_no, verse in enumerate(chapter.find_all('VERS'), 1):
            assert verse_no == int(verse['vnumber'])
            lines += [
                f'<div class="par">',
                f'  <div class="verse" verse="{verse_no}">',
                f'    <div class="verse-label">{verse_no}</div>',
                f'    {verse.text}',
                f'  </div>',
                f'</div>',
            ]
        html = '\n'.join(lines)
        # with open(f'out/{book_code}_{chapter_no:03}.html', 'w') as f:
        #     f.write('\n'.join(lines))

        c.execute(
            'INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)',
            (args.lang_code, book_code, chapter_no, html)
        )

        chapter_sizes.append((book_code, chapter_no, verse_no))

db.commit()
c.execute('VACUUM')

def q(s):
    return f"'{s}'"

print('\n'.join([
    'INSERT INTO books VALUES',
    ',\n'.join(
        f"    ('{args.trans_code}', '{args.lang_code}', {q(book_code):6}, {book_no:2}, '{book_name}')"
        for book_code, book_no, book_name in books
    ),
    ';',
]))

print()

print('\n'.join([
    'INSERT INTO chapterSize VALUES',
    ',\n'.join(
        f"    ('{args.trans_code}', {q(book_code):6}, {chapter_no:3}, {verse_count:3})"
        for book_code, chapter_no, verse_count in chapter_sizes
    ),
    ';',
]))
