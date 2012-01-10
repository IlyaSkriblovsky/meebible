import sys

trans = sys.argv[1]


missed = [
    ('deu', 1, 32, 9),
    ('deu', 2, 22, 30),
    ('deu', 5, 23, 9),
    ('deu', 9, 21, 7),
    ('deu', 14, 2, 9),

    ('por', 1, 36, 5),
    ('por', 1, 43, 5),
    ('por', 4, 5, 6),
    ('por', 4, 25, 8),
    ('por', 12, 18, 34),

    ('tur', 1, 1, 15),
    ('tur', 1, 1, 18),
    ('tur', 1, 2, 7),
    ('tur', 1, 2, 8),
    ('tur', 1, 3, 1),
    ('tur', 1, 3, 9),
    ('tur', 1, 4, 6),
    ('tur', 1, 4, 9),
    ('tur', 1, 6, 3),
    ('tur', 1, 6, 5),
    ('tur', 1, 7, 1),
    ('tur', 1, 7, 3),
    ('tur', 1, 7, 9),
    ('tur', 1, 7, 22),
    ('tur', 1, 8, 16),
    ('tur', 1, 9, 10),
    ('tur', 1, 10, 9),
    ('tur', 1, 10, 12),
    ('tur', 1, 10, 14),
    ('tur', 1, 10, 16),
    ('tur', 1, 10, 17),
    ('tur', 1, 10, 18),
    ('tur', 1, 10, 27),
    ('tur', 1, 10, 28),
    ('tur', 1, 10, 29),
    # and more

    ('zho', 1, 24, 31),
    ('zho', 2, 38, 16),
    ('zho', 3, 3, 16),
    ('zho', 4, 1, 22),
    ('zho', 4, 1, 24),
    ('zho', 4, 1, 26),
    ('zho', 4, 1, 28),
    ('zho', 4, 1, 30),
    ('zho', 4, 1, 32),
    ('zho', 4, 1, 34),
    ('zho', 4, 1, 36),
    ('zho', 4, 1, 38),
    ('zho', 4, 1, 40),
    ('zho', 4, 1, 42),
    ('zho', 4, 1, 44),
    ('zho', 4, 1, 47),

    ('spa', 23, 13, 3),

    ('spa', 3, 15,  1),
    ('spa', 3, 15,  2),
    ('spa', 3, 15,  3),
    ('spa', 3, 15,  4),
    ('spa', 3, 15,  5),
    ('spa', 3, 15,  6),
    ('spa', 3, 15,  7),
    ('spa', 3, 15,  8),
    ('spa', 3, 15,  9),
    ('spa', 3, 15, 10),
    ('spa', 3, 15, 11),
    ('spa', 3, 15, 12),
    ('spa', 3, 15, 13),
    ('spa', 3, 15, 14),
    ('spa', 3, 15, 15),
    ('spa', 3, 15, 16),
    ('spa', 3, 15, 17),
    ('spa', 3, 15, 18),
    ('spa', 3, 15, 19),
    ('spa', 3, 15, 20),
    ('spa', 3, 15, 21),
    ('spa', 3, 15, 22),
    ('spa', 3, 15, 23),
    ('spa', 3, 15, 24),
    ('spa', 3, 15, 25),
    ('spa', 3, 15, 26),
    ('spa', 3, 15, 27),
    ('spa', 3, 15, 28),
    ('spa', 3, 15, 29),
    ('spa', 3, 15, 30),
    ('spa', 3, 15, 31),
    ('spa', 3, 15, 32),
    ('spa', 3, 15, 33),
    ('spa', 3, 15, 34),
    ('spa', 3, 15, 35),
    ('spa', 3, 15, 36),
    ('spa', 3, 15, 37),
    ('spa', 3, 15, 38),
    ('spa', 3, 15, 39),
    ('spa', 3, 15, 40),
    ('spa', 3, 15, 41),
    ('spa', 3, 15, 42),
    ('spa', 3, 15, 43),
    ('spa', 3, 15, 44),
    ('spa', 3, 15, 45),
    ('spa', 3, 15, 46),
    ('spa', 3, 15, 47),
    ('spa', 3, 15, 48),
    ('spa', 3, 15, 49),
    ('spa', 3, 15, 50),
    ('spa', 3, 15, 51),
    ('spa', 3, 15, 52),
    ('spa', 3, 15, 53),
    ('spa', 3, 15, 54),
    ('spa', 3, 15, 55),
    ('spa', 3, 15, 56),

    ('spa', 5, 12,  1),
    ('spa', 5, 12,  2),
    ('spa', 5, 12,  3),
    ('spa', 5, 12,  4),
    ('spa', 5, 12,  5),
    ('spa', 5, 12,  6),
    ('spa', 5, 12,  7),
    ('spa', 5, 12,  8),
    ('spa', 5, 12,  9),
    ('spa', 5, 12, 10),
    ('spa', 5, 12, 11),
    ('spa', 5, 12, 12),
    ('spa', 5, 12, 13),
    ('spa', 5, 12, 14),
    ('spa', 5, 12, 15),
    ('spa', 5, 12, 16),
    ('spa', 5, 12, 17),
    ('spa', 5, 12, 18),
    ('spa', 5, 12, 19),
    ('spa', 5, 12, 20),
    ('spa', 5, 12, 21),
    ('spa', 5, 12, 22),
    ('spa', 5, 12, 23),
    ('spa', 5, 12, 24),
    ('spa', 5, 12, 25),
    ('spa', 5, 12, 26),
    ('spa', 5, 12, 27),
    ('spa', 5, 12, 28),
    ('spa', 5, 12, 29),
    ('spa', 5, 12, 30),
    ('spa', 5, 12, 31),

]


import httplib, re
from itertools import izip, count

verse_re = re.compile(r'<li id="v(\d+)" value="(\d+)" class="v">(.*?)</li>')

c = httplib.HTTPConnection("bibleonline.ru")

for book_no in xrange(1, 67):
    chapter = 0
    while True:
        chapter += 1

        c.request('GET', '/bible/{0}/{1:02}/{2:02}/'.format(trans, book_no, chapter), None, {
            'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/535.7 (KHTML, like Gecko) Chrome/16.0.912.63 Safari/535.7',
        })
        r = c.getresponse()
        content = r.read()

        matches = verse_re.findall(content)

        if len(matches) == 0: break

        for mis in missed:
            if trans == mis[0] and book_no == mis[1] and chapter == mis[2]:
                matches.insert(mis[3] - 1, (mis[3], mis[3], '---'))

        for m, i in izip(matches, count(1)):
            if int(m[0]) != i or int(m[1]) != i: raise Exception("Strange verse number: b{0} c{1} v{2}".format(book_no, chapter, m[0]))

        print '{0}|{1}|{2}'.format(book_no, chapter, len(matches))
        sys.stdout.flush()
