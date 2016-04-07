# -*-coding:utf-8-*-
import urllib2, urllib
from bs4 import BeautifulSoup
import time
import sys
import requests
import os

def get_image_url_list(key_word, num, interval=1):
    result = []
    template = u'http://image.baidu.com/search/wisemiddetail?tn=wisemiddetail&ie=utf8&word=%s&fmpage=detail&pn=%d&gsm=11&size=small&pos=next'
    #template = u'http://image.baidu.com/i?tn=wisemiddetail&ie=utf8&word=%s&pn=%d&size=big&fr=wiseresult'
    for i in xrange(num):
        sys.stderr.write('Fetching url for image %d...... ' % i)
        url = template % (urllib.quote(key_word.encode('utf-8')), i)
        try:
            html = urllib2.urlopen(url, timeout=5).read().decode('utf-8')
            soup = BeautifulSoup(html)
            for a in soup.find_all('a'):
                if u'原图' in a.get_text():
                    img_url = a['href']
                    result.append(img_url)
            sys.stderr.write('OK\n')
            sys.stderr.write(img_url + '\n')

            # download image file
            r = requests.get(img_url)
            savefile = os.path.basename(img_url)
            with open(savefile, 'wb') as f:
                f.write(r.content)
        except:
            sys.stderr.write('FAILED\n')
        time.sleep(interval)

    return result


if __name__ == '__main__':
    img_urls = get_image_url_list(u'泰姬陵', 2000)
    print '\n'.join(img_urls)
