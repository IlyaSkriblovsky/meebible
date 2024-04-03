import sqlite3
import re
from bs4 import BeautifulSoup


verse_no_re = re.compile(r'^ *((\d+)(?:–(\d+))?)\.( .*)')
brackets_re = re.compile(r'\[\d+]')


db = sqlite3.Connection('mn_bodhi.sqlite')
c = db.cursor()
c.execute('DROP TABLE IF EXISTS html')
c.execute('CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY(langCode, bookCode, chapterNo))')


raw_unicode_replacements = {
    b'\xE1\xB8\x8D': b'\x64\xCC\xA3',  # ḍ -> ḍ
    b'\xE1\xB8\xB7': b'\x6C\xCC\xA3',  # ḷ
    b'\xE1\xB9\x87': b'\x6E\xCC\xA3',  # ṇ
    b'\xE1\xB9\xAD': b'\x74\xCC\xA3',  # ṭ
    # ↓ ṁ, we didn't found combination working on Symbian, so using css hack
    b'\xE1\xB9\x81': 'm<span class="m-dot-above"></span>'.encode(),
}

unicode_replacements = {
    before.decode(): after.decode()
    for before, after in raw_unicode_replacements.items()
}

inline_css = [
    "<style>",
    "  .par.indented { margin-left: 2em; }",
    "  .m-dot-above { display: inline-block; width: 0; }",
    "  .m-dot-above:before {",
    "    content: '.';",
    "    position: relative;",
    "    position: relative;",
    "    left: -0.5em;",
    "    top: -1.3ex;",
    "  }",
    "</style>",
]


def convert_chapter(chapter_no: int):
    html = BeautifulSoup(open(f'html/{chapter_no:03}.html').read(), 'html.parser')

    body = html.find('div', {'class': 'booksection'})

    for pageno in body.find_all('span', {'class': 'pageno'}):
        pageno.extract()

    for p in body.find_all('p', {'class': 'none'}):
        p.extract()

    for sup in body.find_all('sup'):
        if sup.contents[0].name == 'a' and sup.contents[0]['data-ohref']:
            sup.extract()

    for a in body.find_all('a'):
        if not a.contents:
            a.extract()

    for img in body.find_all('img'):
        if img['src'].startswith('data:'):
            img.extract()

    for text in body.find_all(string='10. The Ten Kasi˚as'):
        text.replace_with(text.text.replace('10. The Ten Kasi˚as', '10. The Ten Kasiṇas'))
    for text in body.find_all(string='11. The Four Jh›nas'):
        text.replace_with(text.text.replace('11. The Four Jh›nas', '11. The Four Jhānas'))

    for h1 in body.find_all('h1', {'class': 'chapter'}):
        h1.name = 'div'
        h1['class'] = 'heading'

    for h2 in body.find_all('h2', {'class': 'head1'}):
        h2.name = 'div'
        h2['class'] = 'heading'

    for block in body.find_all('div', {'class': 'block'}):
        block.name = 'p'
        block['class'] = ['indented']
        for p in list(block.find_all('p')):
            margin = 'verse1' in p['class']
            content = list(p.contents)
            p.extract()
            if margin:
                block.append(html.new_tag('br'))
            block.extend(content)
            block.append(html.new_tag('br'))

    max_verse_to = 0
    for p in body.find_all('p'):
        if not p.contents:
            continue

        p.name = 'div'
        if 'indented' in p['class']:
            p['class'].append('par')
        else:
            p['class'] = ['par']
        del p['id']

        subtags = {s.name for s in p.children if s.name is not None}
        assert not (subtags - {'small', 'i', 'br'}), subtags

        for text in p.find_all(string=brackets_re):
            text.replace_with(re.sub(brackets_re, ' ', text.text))

        verses = []
        verse_from = verse_to = None
        if p.contents[0].name is None:
            if match := verse_no_re.match(p.contents[0].text):
                p.contents[0].extract()
                verse_from = int(match[2])
                verse_to = int(match[3]) if match[3] else verse_from
                p.insert(0, match[4])

                for no in range(verse_from, verse_to):
                    verse = html.new_tag('div')
                    verse['class'] = 'verse'
                    verse['verse'] = no
                    verses.append(verse)

        verse = html.new_tag('div')
        verse['class'] = 'verse'
        if verse_to:
            verse['verse'] = verse_to
            label = html.new_tag('div')
            label['class'] = 'verse-label'
            label.append(match[1])
            verse.append(label)

        children = list(p.contents)
        p.clear()
        verse.extend(children)
        verses.append(verse)
        p.extend(verses)

        if verse_to:
            max_verse_to = verse_to

    html = ''.join(map(str, [
        *inline_css,
        body,
    ]))

    for before, after in unicode_replacements.items():
        html = html.replace(before, after)

    c.execute(
        'INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)',
        ('e_b', 'mn', chapter_no, html)
    )
    with open(f'out/{chapter_no:03}.html', 'w') as f:
        f.write(html)

    print(f'chapter {chapter_no: 3} has {max_verse_to: 3} verses')


for chapter_no in range(1, 152 +1):
    #if chapter_no != 1: continue
    convert_chapter(chapter_no)

db.commit()
c.execute('VACUUM')
