import urllib
import re


for bookNo in xrange(1, 67):
    index = urllib.urlopen("http://bibleonline.ru/bible/rus/{0:02}/01".format(bookNo)).read()

    links = re.findall(r'http://bibleonline.ru/bible/rus/{0:02}/(\d+)/">'.format(bookNo), index)
    chapterCount = int(links[-1]) if len(links) > 0 else 1

    for chapterNo in xrange(1, chapterCount+1):
        chapterReq = urllib.urlopen("http://bibleonline.ru/bible/rus/{0:02}/{1:02}/".format(bookNo, chapterNo))
        if chapterReq.getcode() != 200:
            raise Exception("CANNOT LOAD: bookNo={0}, chapterNo={1}".format(bookNo, chapterNo))
        chapter = chapterReq.read()


        spans = re.findall(r'<span id="v(\d+)"', chapter)
        if map(lambda s: int(s), spans) != range(1, len(spans)+1):
            raise Exception("Strange verses in bookNo={0}, chapterNo={1}: {2}".format(bookNo, chapterNo, spans))

        print "INSERT INTO chapterSize VALUES ({0}, {1}, {2})".format(bookNo, chapterNo, len(spans))
