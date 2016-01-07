#-*-coding:utf-8-*-

# monkey patching for socket and DNS resolve
import urllib2
import socket
import socks
def getaddrinfo(*args):
    return [(socket.AF_INET, socket.SOCK_STREAM, 6, '', (args[0], args[1]))]
socks.set_default_proxy(socks.SOCKS5, "localhost", 1080)
socket.socket = socks.socksocket
socket.getaddrinfo = getaddrinfo

import flickrapi
import json
import sys
import requests

api_key = u'97ee5883c16a5c2811262eca5818cb42'
api_secret = u'000dd6f561ce506b'
flickr = flickrapi.FlickrAPI(api_key, api_secret, format='json')

if __name__ == '__main__':
    page_idx = 1
    need = 1000

    while need > 0:
        raw_json = flickr.photos.search(text='sydney opera house', sort='interestingness-desc', page=page_idx, has_geo='1')
        #raw_json = flickr.photos.search(text='Burj Al Arab', sort='interestingness-desc')
        resp = json.loads(raw_json)

        page_idx = resp['photos']['page'] + 1
        total_page = resp['photos']['pages']

        for photo in resp['photos']['photo']:
            print need
            url = 'https://farm%d.staticflickr.com/%s/%s_%s_b.jpg' % (photo['farm'], photo['server'], photo['id'], photo['secret'])
            print 'https://www.flickr.com/photos/%s/%s' % (photo['owner'], photo['id'])
            print url
            need -= 1

            # download image file
            r = requests.get(url)
            if r.url.endswith('png'):
                print 'no big size image exists, download normal size'
                url = 'https://farm%d.staticflickr.com/%s/%s_%s.jpg' % (photo['farm'], photo['server'], photo['id'], photo['secret'])
                r = requests.get(url)
            savefile = '%s@%s.jpg' % (photo['id'], photo['owner'])
            with open(savefile, 'wb') as f:
                f.write(r.content)

        if page_idx > total_page:
            break
