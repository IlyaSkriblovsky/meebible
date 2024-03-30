import httplib, urllib
import json
from multiprocessing import Pool


cookie = None


def downloadBook(bookNo):
    chapterNo = 0

    http = httplib.HTTPConnection('www.bibleserver.com')

    result = []
    bookname = None

    while True:
        chapterNo += 1

        http.request('GET', '/ajax/textContent.php?' + urllib.urlencode({ 'loadData': 2, 'ref': '{0}{1:03}000'.format(bookNo, chapterNo) }), '', { 'Cookie': cookie })
        content = http.getresponse().read()

        j = json.loads(content)
        if j['config']['verseMax'] == 0:
            break

        bookname = j['data'][0]['bookname_trl']
        result.append(j['data'][0]['content'])


    return bookname, result




if __name__ == '__main__':
    bs_translation_no = 2

    http = httplib.HTTPConnection('www.bibleserver.com')
    params = urllib.urlencode({ "loadData": 0, "defaultTranslation": bs_translation_no })
    http.request('POST', '/ajax/textContent.php', params, { 'Content-Type': 'application/x-www-form-urlencoded' })
    r = http.getresponse()
    r.read()

    cookie = r.getheader('Set-Cookie').split(';', 1)[0]


    result = Pool(20).map(downloadBook, range(1, 67))

    for bookNo, bookrow in enumerate(result, 1):
        print bookrow[0].encode('utf-8')

        for chapterNo, content in enumerate(bookrow[1], 1):
            with open('orig/{0:02}_{1:03}.html'.format(bookNo, chapterNo), 'w') as f:
                f.write(content.encode('utf-8'))
