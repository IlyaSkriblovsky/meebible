from argparse import ArgumentParser
import os
from pathlib import Path
from bs4 import BeautifulSoup
import sqlite3

parser = ArgumentParser()
parser.add_argument('input_xml_filename')
parser.add_argument('lang_code')
parser.add_argument('trans_code')
parser.add_argument('-s', '--save-html', help='Save HTMLs to specified folder', metavar='dir')
parser.add_argument('--no-booknames', help="Don't get booknames from xml", action="store_true")
parser.add_argument('--add-studynote-css', help="Add studynote CSS rule", action="store_true")
args = parser.parse_args()

book_name2code = {
    "Genesis":         'ge',
    "Exodus":          'ex',
    "Leviticus":       'le',
    "Numbers":         'nu',
    "Deuteronomy":     'de',
    "Joshua":          'jos',
    "Judges":          'jg',
    "Ruth":            'ru',
    "1 Samuel":        '1sa',
    "2 Samuel":        '2sa',
    "1 Kings":         '1ki',
    "2 Kings":         '2ki',
    "1 Chronicles":    '1ch',
    "2 Chronicles":    '2ch',
    "Ezra":            'ezr',
    "Nehemiah":        'ne',
    "Esther":          'es',
    "Job":             'job',
    "Psalm":           'ps',
    "Proverbs":        'pr',
    "Ecclesiastes":    'ec',
    "Song of Solomon": 'ca',
    "Isaiah":          'isa',
    "Jeremiah":        'jer',
    "Lamentations":    'la',
    "Ezekiel":         'eze',
    "Daniel":          'da',
    "Hosea":           'ho',
    "Joel":            'joe',
    "Amos":            'am',
    "Obadiah":         'ob',
    "Jonah":           'jon',
    "Micah":           'mic',
    "Nahum":           'na',
    "Habakkuk":        'hab',
    "Zephaniah":       'zep',
    "Haggai":          'hag',
    "Zechariah":       'zec',
    "Malachi":         'mal',
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

verse_numbering_exceptions = [
    ('lxx', 'jer', 25, 34),
    ('lxx', 'jer', 30, 23),
    ('lxx', 'jer', 32, 15),
]

html_dir = None
if args.save_html:
    if not os.path.exists(args.save_html):
        os.mkdir(args.save_html)
    html_dir = Path(args.save_html)

studynote_css = '''<style>
  .studynote {
    background: rgba(192, 192, 192, 0.4);
    border-radius: 3px;
    font-style: italic;
    padding: 0 0.3em;
  }
</style>
'''

db = sqlite3.Connection(f'{args.trans_code}.sqlite')
c = db.cursor()
c.execute('DROP TABLE IF EXISTS html')
c.execute('CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY(langCode, bookCode, chapterNo))')
db.commit()

books = []
chapter_sizes = []

xml = BeautifulSoup(open(args.input_xml_filename).read(), 'xml')

for div in list(xml.find_all('DIV')):
    if len(div.contents) == 1:
        child = div.contents[0]
        if div.contents[0].name == 'NOTE':
            child.name = 'span'
            del child['type']
            child['class'] = ['studynote']
            div.insert_after(child.extract())
            div.extract()

for book_no, book in enumerate(xml.find_all('BIBLEBOOK'), 1):
    if not args.no_booknames:
        book_name = book['bname']
        book_code = book_name2code[book_name]
    else:
        book_name = f'book{book_no}'
        book_code = list(book_name2code.values())[book_no-1]
    books.append((book_code, book_no, book_name))
    for chapter_no, chapter in enumerate(book.find_all('CHAPTER'), 1):
        assert chapter_no == int(chapter['cnumber'])
        lines = []
        if args.add_studynote_css:
            lines.append(studynote_css)
        expected_verse_no = 1
        for verse in chapter.find_all('VERS'):
            verse_no = int(verse['vnumber'])
            try:
                assert verse_no == expected_verse_no, f'{book_name} {chapter_no}:{verse_no} ({expected_verse_no})'
            except AssertionError:
                if (args.trans_code, book_code, chapter_no, verse_no) not in verse_numbering_exceptions:
                    raise
            lines += [
                f'<div class="par">',
                f'  <div class="verse" verse="{verse_no}">',
                f'    <div class="verse-label">{verse_no}</div>',
                f'    {verse.decode_contents()}',
                f'  </div>',
                f'</div>',
            ]
            expected_verse_no = verse_no + 1
        html = '\n'.join(lines)
        if html_dir:
            with open(html_dir / f'{book_code}_{chapter_no:03}.html', 'w') as f:
                f.write('\n'.join(lines))

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
