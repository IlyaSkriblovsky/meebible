import sqlite3
import glob

db = sqlite3.Connection("esv.sqlite")
c = db.cursor()

c.execute("DROP TABLE IF EXISTS html")
c.execute("CREATE TABLE html (langCode, bookCode, chapterNo, html, PRIMARY KEY (langCode, bookCode, chapterNo))")

for f in glob.glob("*.html"):
    bookCode, chapterNo = f[:-5].split('_')
    chapterNo = int(chapterNo)
    print bookCode, chapterNo

    c.execute("INSERT INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
        'e', bookCode, chapterNo, open(f).read().decode("utf-8")
    ))

c.execute("VACUUM")
