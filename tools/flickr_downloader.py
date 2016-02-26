#-*-coding:utf-8-*-

# monkey patching for socket and DNS resolve
# pip install flickrapi
# pip install pysocks
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
import piexif

api_key = u'97ee5883c16a5c2811262eca5818cb42'
api_secret = u'000dd6f561ce506b'
flickr = flickrapi.FlickrAPI(api_key, api_secret, format='json')

if __name__ == '__main__':
    page_idx = 1
    need = 5

    while need > 0:
        raw_json = flickr.photos.search(text='sydney opera house', sort='interestingness-desc', page=page_idx, has_geo='1')
        #raw_json = flickr.photos.search(text='Burj Al Arab Hotel', sort='interestingness-desc', page=page_idx)
        #raw_json = flickr.photos.search(text='Louvre', sort='interestingness-desc', page=page_idx)	
		
        resp = json.loads(raw_json)

        page_idx = resp['photos']['page'] + 1
        total_page = resp['photos']['pages']

        for photo in resp['photos']['photo']:
            print 'idx:', need
            print 'https://www.flickr.com/photos/%s/%s' % (photo['owner'], photo['id'])
            
            size_options = json.loads(flickr.photos.getSizes(photo_id=photo['id']))['sizes']['size']
            i = 1
            while i <= len(size_options):
                largest_size = size_options[-i]
                if int(largest_size['height']) <= 800:
                    break
                i += 1
            url = largest_size['source']
            print 'file url:', url

            #print flickr.photos.getExif(photo_id=photo['id'])

            # download image file
            r = requests.get(url)
            savefile = '%s@%s.jpg' % (photo['id'], photo['owner'])
            with open(savefile, 'wb') as f:
                f.write(r.content)

            need -= 1
            if need == 0:
                break

        if page_idx > total_page:
            break
