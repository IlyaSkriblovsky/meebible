import httplib
import re

c = httplib.HTTPConnection('bibele.lv')

c.request('POST', '/bibele/bibele.php', 'book=66&chapter=1', {
    'Content-Type': 'application/x-www-form-urlencoded'
})

r = c.getresponse()
content = r.read()

payload = re.search(r"<SPAN CLASS='normal'>(.*?)</SPAN>", content, re.DOTALL).group(1).strip()

verses = [verse.strip() for verse in payload.split('<br>') if len(verse.strip()) > 0]
print verses
print len(verses)
