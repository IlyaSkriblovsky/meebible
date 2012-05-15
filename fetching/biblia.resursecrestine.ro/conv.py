# -* coding: utf-8 -*-

import subprocess, sys, os, sqlite3, glob, re

out_verse_re = re.compile(r'<div class="verse-label">(\d+)</div>')

try: os.mkdir('tmp')
except OSError: pass
try: os.mkdir('result')
except OSError: pass

db = sqlite3.Connection("rodc.sqlite")
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

for fn in glob.glob('orig/*.html'):
    bookCode, chapterNo = os.path.basename(fn)[:-5].split('_')
    chapterNo = int(chapterNo)

    with open(fn, 'r') as f: content = f.read()

    content = re.sub('&nbsp;', '&#160;', content)
    content = re.sub('<input[^>]*[^/]>', '', content)
    content = re.sub('<img[^>]*[^/]>', '', content)
    content = re.sub('<param[^>]*[^/]>', '', content)
    content = re.sub('& ', '', content)

    tmp_fn = 'result/{0}_{1:03}.html'.format(bookCode, chapterNo)

    with open(tmp_fn, 'w') as f: f.write(content)

    new_fn = 'result/{0}_{1:03}.html'.format(bookCode, chapterNo)

    result = subprocess.Popen(['xsltproc', '--nonet', 'brc.xslt', tmp_fn], stdout = subprocess.PIPE, stderr = None).communicate()[0]

    with open(new_fn, 'w') as f: f.write(result)

    actual_verses = len(result.split('class="verse"'))-1
    last_verse = int(out_verse_re.findall(result)[-1])
    if actual_verses != last_verse: raise Exception("verse count don't match ({0} != {1})".format(actual_verses, last_verse))

    c.execute('INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)', (
        'm', bookCode, chapterNo, result.decode('utf-8')
    ))

    print '{0}|{1}|{2}'.format(bookCode, chapterNo, last_verse)
    sys.stdout.flush()

c.execute('vacuum')
c.close()
db.commit()
