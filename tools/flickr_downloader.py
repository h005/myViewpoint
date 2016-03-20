#-*-coding:utf-8-*-

# pip install pysocks
# pip install flickrapi
# pip install --upgrade urllib3
# pip install pyopenssl ndg-httpsclient pyasn1
# sudo apt-get install libffi-dev libssl-dev
# https://www.phodal.com/blog/python-pip-openssl-issue/
# https://urllib3.readthedocs.org/en/latest/security.html#snimissingwarning

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
flickr2 = flickrapi.FlickrAPI(api_key, api_secret)

if __name__ == '__main__':
    page_idx = 1
    need = 800

    while need > 0:
        raw_json = flickr.photos.search(text='sydney opera house', sort='interestingness-desc', page=page_idx, has_geo='1')
        #raw_json = flickr.photos.search(text='Burj Al Arab Hotel', sort='interestingness-desc', page=page_idx)
        #raw_json = flickr.photos.search(text='Louvre', sort='interestingness-desc', page=page_idx)	
		
        resp = json.loads(raw_json)

        page_idx = resp['photos']['page'] + 1
        total_page = resp['photos']['pages']

        for photo in resp['photos']['photo']:
            size_options = json.loads(flickr.photos.getSizes(photo_id=photo['id']))['sizes']['size']
            largest_size = size_options[-1]
            if largest_size['label'] != u'Original':
                continue

            print 'idx:', need
            print 'https://www.flickr.com/photos/%s/%s' % (photo['owner'], photo['id'])
            
            url = largest_size['source']
            print 'file url:', url

            # download image file
            r = requests.get(url)
            savefile = '%s@%s.jpg' % (photo['id'], photo['owner'])
            with open(savefile, 'wb') as f:
                f.write(r.content)

            try:
                tags = flickr2.photos_getExif(photo_id=photo['id']).getiterator('exif')
                with open('tags.xml', 'w') as tags_file:
                    tags_file.write("<?xml version='1.0' encoding='UTF-8'?>\n<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>\n")
                    for tag in tags:
                        tags_file.write('<%s:%s>%s</%s:%s>\n'%(tag.attrib['tagspace'], tag.attrib['tag'], tag.getchildren()[0].text.strip().encode('utf-8'), tag.attrib['tagspace'], tag.attrib['tag']))
                    tags_file.write('</rdf:RDF>\n')
                os.system('exiftool -overwrite_original -tagsfromfile tags.xml %s' % savefile)
            except:
                pass

            need -= 1
            if need == 0:
                break

        if page_idx > total_page:
            break
