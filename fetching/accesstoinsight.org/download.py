import glob
import multiprocessing
import re
import requests


mn_chapters = 152

def download_mn_chapter(chapter_no: int):
    print(chapter_no)
    url = f'https://www.accesstoinsight.org/tipitaka/mn/mn.{chapter_no:03}.than.html'
    response = requests.get(url)
    response.raise_for_status()
    with open(f'dl/mn-{chapter_no:03}.html', 'w') as f:
        f.write(response.text)

pool = multiprocessing.Pool(10)
filename_re = re.compile(r'dl/mn-(\d+).html')
chapters_to_download = set(range(1, mn_chapters+1)) - {int(filename_re.match(g).group(1)) for g in glob.glob('dl/mn*.html')}
pool.map(download_mn_chapter, chapters_to_download)
