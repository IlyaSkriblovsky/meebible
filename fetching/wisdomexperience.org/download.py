from glob import glob
from multiprocessing import Pool
import requests

def download_url(chapter_no: int, url: str):
    print(f'downloading {chapter_no}')
    response = requests.get(url, headers={
        'cookie': '; '.join([
            'wfwaf-authcookie-5799888efde81e08c57b6262fa1a90a7=42087%7Csubscriber%7Cread%7C29fd602f204e4a9b4aea8c357eef6d79750b881aeafb7a8162b1babe287ca700',
            '_gcl_au=1.1.539637281.1712052895',
            'sbjs_migrations=1418474375998%3D1',
            'sbjs_current_add=fd%3D2024-04-02%2010%3A14%3A55%7C%7C%7Cep%3Dhttps%3A%2F%2Fwisdomexperience.org%2Febook%2Fthe-middle-length-discourses-of-the-buddha%2Fpart-one-the-root-fifty-discourses-mulapa%25e1%25b9%2587%25e1%25b9%2587asapa%25e1%25b8%25b7i%2F1-the-division-of-the-discourse-on-the-root-mulapariyayavagga%2F1-mulapariyaya-sutta-the-root-of-all-things%2F%7C%7C%7Crf%3D%28none%29',
            'sbjs_first_add=fd%3D2024-04-02%2010%3A14%3A55%7C%7C%7Cep%3Dhttps%3A%2F%2Fwisdomexperience.org%2Febook%2Fthe-middle-length-discourses-of-the-buddha%2Fpart-one-the-root-fifty-discourses-mulapa%25e1%25b9%2587%25e1%25b9%2587asapa%25e1%25b8%25b7i%2F1-the-division-of-the-discourse-on-the-root-mulapariyayavagga%2F1-mulapariyaya-sutta-the-root-of-all-things%2F%7C%7C%7Crf%3D%28none%29',
            'sbjs_current=typ%3Dtypein%7C%7C%7Csrc%3D%28direct%29%7C%7C%7Cmdm%3D%28none%29%7C%7C%7Ccmp%3D%28none%29%7C%7C%7Ccnt%3D%28none%29%7C%7C%7Cid%3D%28none%29%7C%7C%7Ctrm%3D%28none%29%7C%7C%7Cmtke%3D%28none%29',
            'sbjs_first=typ%3Dtypein%7C%7C%7Csrc%3D%28direct%29%7C%7C%7Cmdm%3D%28none%29%7C%7C%7Ccmp%3D%28none%29%7C%7C%7Ccnt%3D%28none%29%7C%7C%7Cid%3D%28none%29%7C%7C%7Ctrm%3D%28none%29%7C%7C%7Cmtke%3D%28none%29',
            '_gid=GA1.2.632464821.1712052896',
            '_fbp=fb.1.1712052895967.817247843',
            '__stripe_mid=d2442ee3-3181-4a42-9b35-9764a0d1d197e6bbdf',
            'wordpress_test_cookie=WP%20Cookie%20check',
            'wordpress_sec_927da0000d6167babad638c05dd8b4db=emmgee%40vivaldi.net%7C1712225737%7C1GMM80s5KxWh7xbma6bPtR7mmBWBAxhOikune5CEU2y%7C72ffac53fd458800c0d0a16235f370df66f04942d91205d2c464609ab9109db6',
            'wordpress_logged_in_927da0000d6167babad638c05dd8b4db=emmgee%40vivaldi.net%7C1712225737%7C1GMM80s5KxWh7xbma6bPtR7mmBWBAxhOikune5CEU2y%7Ce0f6cb22c075ecfb447d05cc8536b119b611783f8f8ca2dfe9ab82ee37296347',
            'wp_automatewoo_visitor_927da0000d6167babad638c05dd8b4db=cyxwyypnljumjg8ymvvg',
            'wp_automatewoo_session_started=1',
            'sbjs_udata=vst%3D2%7C%7C%7Cuip%3D%28none%29%7C%7C%7Cuag%3DMozilla%2F5.0%20%28X11%3B%20Linux%20x86_64%29%20AppleWebKit%2F537.36%20%28KHTML%2C%20like%20Gecko%29%20Chrome%2F123.0.0.0%20Safari%2F537.36',
            '__stripe_sid=b64aa422-8c5f-46eb-a871-7f69afad30e723a77f',
            'readerfontsize=21.44',
            'wfwaf-authcookie-5799888efde81e08c57b6262fa1a90a7=42087%7Csubscriber%7Cread%7Cd89bf7b44bae7b46870c1b79d57a49cce1b0e5c58b9e2fdf19bba1ba8f23632f',
            'uptopage=page_321',
            'sbjs_session=pgs%3D15%7C%7C%7Ccpg%3Dhttps%3A%2F%2Fwisdomexperience.org%2Febook%2Fthe-middle-length-discourses-of-the-buddha%2Fpart-one-the-root-fifty-discourses-mulapa%25e1%25b9%2587%25e1%25b9%2587asapa%25e1%25b8%25b7i%2F1-the-division-of-the-discourse-on-the-root-mulapariyayavagga%2F1-mulapariyaya-sutta-the-root-of-all-things%2F%23',
            '_ga_265ECE5Y0D=GS1.1.1712058928.2.1.1712060659.0.0.0',
            '_ga=GA1.2.97028485.1712052895'
        ])
    })
    response.raise_for_status()
    with open(f'html/{chapter_no:03}.html', 'w') as f:
        f.write(response.text)

links = [link.strip() for link in open('links.txt')]
numbers_and_links = list(enumerate(links, 1))
existing_chapters = [int(fn[5:-5]) for fn in glob('html/*.html')]
to_download = [
    (chapter_no, url)
    for chapter_no, url in numbers_and_links
    if chapter_no not in existing_chapters
]

####
# to_download = [(chapter_no, url) for chapter_no, url in to_download if chapter_no == 46]
####

pool = Pool(10)
pool.starmap(download_url, to_download)
