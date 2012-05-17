import re, sys, sqlite3, glob, os
from itertools import *
from createxml import create_xml

transCode = 'tamil'

verse_re = re.compile(r'(\d+)\. (.*)<br><br>')

db = sqlite3.Connection("{0}.sqlite".format(transCode))
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

for fn in glob.glob('orig/*.html'):
    bookCode, chapterNo = os.path.basename(fn)[:-5].split('_')
    chapterNo = int(chapterNo)

    with open(fn, 'r') as f: content = f.read()

    verses = verse_re.findall(content)
    if len(verses) == 0: break

    for v, vn in izip(verses, count(1)):
        if int(v[0]) != vn: raise Exception('Strange verse no: {0} != {1}'.format(v[0], vn))

    verses = [v[1].decode('utf-8') for v in verses]

    xml = create_xml(verses)

    with open('result/{0}_{1:03}.html'.format(bookCode, chapterNo), 'w') as f: f.write(xml.encode('utf-8'))

    c.execute('INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)', (
        'tm', bookCode, chapterNo, xml
    ))

    print '{0}|{1}|{2}'.format(bookCode, chapterNo, len(verses))
    sys.stdout.flush()


c.execute('VACUUM')
db.commit()
