import httplib, sys, re, sqlite3, subprocess

transCode = 'bbe'
bstCode = 'bbe'
langCode = 'e'



db = sqlite3.Connection("{0}.sqlite".format(transCode))
c = db.cursor()
c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

title_re = re.compile(r'<title>.*?</title>', re.DOTALL)

books = [line.strip().split('|') for line in open('books.txt').readlines()]

http = httplib.HTTPConnection('www.biblestudytools.com')

for book in books:
    chapterNo = 0
    while True:
        chapterNo += 1

        while True:
            http.request('GET', '/{0}/{1}/{2}.html'.format(
                bstCode, book[1], chapterNo
            ))
            try:
                r = http.getresponse()
                break
            except httplib.ResponseNotReady:
                http = httplib.HTTPConnection('www.biblestudytools.com')

        if r.status != 200: break

        content = r.read().decode('utf-8')

        content = re.sub(r'&nbsp;', r'&#160;', content)
        content = re.sub(r'<script async ', r'<script async="1" ', content)
        content = re.sub(r'<input name ', r'<input name="1" ', content)
        content = re.sub(r'&([^;]{5})', r'&amp;\1', content)
        content = re.sub(r'<g:plusone.*</g:plusone>', r'', content)
        content = re.sub(title_re, r'', content)
        content = re.sub(r'<<', r'&lt;&lt;', content)

        if bstCode == 'bbe':
            content = re.sub(r'"versenum">(\d+)</span>\x0d(\s*)<([A-Z][a-zA-Z0-9,. !?:]*)>?', r'"versenum">\1</span>\n\2[\3] ', content)
            content = re.sub(r'<A Psalm. Of Asaph.>', r'[A Psalm. Of Asaph.]', content)

        orig_fn = 'orig_{0}_{1:03}.html'.format(book[0], chapterNo)
        new_fn = '{0}_{1:03}.html'.format(book[0], chapterNo)

        with open(orig_fn, 'w') as f:
            f.write(content.encode('utf-8'))

        result = subprocess.Popen(['xsltproc', '--nonet', 'bst.xslt', orig_fn], stdout = subprocess.PIPE, stderr = None).communicate()[0]

        with open(new_fn, 'w') as f:
            f.write(result)

        c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
            langCode, book[0], chapterNo, result.decode('utf-8')
        ))

        print '{0}|{1}|{2}'.format(book[0], chapterNo, len(result.split('verse="')) - 1)
        sys.stdout.flush()

c.execute('vacuum')
db.commit()
