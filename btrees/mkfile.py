# -*- coding: utf-8 -*-

import sqlite3
import re

db = sqlite3.Connection('rus.sqlite')
c = db.cursor()

re1 = re.compile(r'<[^>]*>')
re2 = re.compile(ur'[.,;:—"“”„«»!?()\[\]{}\n†‡]')
re3 = re.compile(r'[0-9]')
re4 = re.compile('[ \xa0\r]+')

c.execute("SELECT html FROM html")

f = open('words.txt', 'w')

while True:
    row = c.fetchone()
    if row is None: break

    html = row[0]
    html = re.sub(re1, ' ', html)
    html = re.sub(re2, ' ', html)
    html = re.sub(re3, ' ', html)
    html = re.sub(re4, ' ', html)
    html = html.strip().replace(' ', '\n')
    f.write(html.encode('utf-8') + '\n')
