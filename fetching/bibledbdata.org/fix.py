import sqlite3, glob, os

for fn in glob.glob('fix/*.html'):
    bn = os.path.basename(fn)
    transCode, langCode, bookCode, chapterNo = bn[:-5].split('_')
    chapterNo = int(chapterNo)
    print transCode, langCode, bookCode, chapterNo

    db = sqlite3.Connection("{0}.sqlite".format(transCode))
    c = db.cursor()
    c.execute("REPLACE INTO html (langCode, bookCode, chapterNo, html) VALUES (?, ?, ?, ?)", (
        langCode, bookCode, chapterNo, open(fn).read().decode('utf-8')
    ))
    c.close()
    db.commit()
