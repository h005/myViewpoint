#-*- coding:utf-8 -*-
import cv2
import numpy
import optparse
import os
import shutil
import pprint

def walker(dir_path):
	if len(dir_path) == 0:
		dir_path = '.'

	for root, dirs, files in os.walk(dir_path):
		for name in files:
			fname, ext = os.path.splitext(name)
			if ext.lower() == '.ppm':
				ppm_file = os.path.join(root, name)
				update_coordinate(ppm_file)
	

def update_coordinate(image_path):
	ppm = cv2.imread(image_path)
	iheight, iwidth, channel = ppm.shape

	basename = os.path.basename(image_path)
	fname, ext = os.path.splitext(basename)
	txtfile = os.path.join(os.path.dirname(image_path), '%s_im.txt' % fname)
	new_txtfile = os.path.join(os.path.dirname(image_path), '%s_im_norm.txt' % fname)

	if os.path.exists(txtfile):
		print  '%s --> %s' % (txtfile, new_txtfile)
		fp = open(txtfile, 'r')
		tmp = open(new_txtfile, 'w')

		for line in fp:
			x, y = line.strip().split()
			x = float(x) *  iwidth / 500
			y = float(y) * iheight / 500
			x = int(round(x))
			y = int(round(y))
			tmp.write('%d %d\n' % (x, y))

		fp.close()
		tmp.close()
if __name__ == '__main__':
	parser = optparse.OptionParser()
	parser.add_option('-i', '--image', dest='image', help="related image file")
	(options, args) = parser.parse_args()

	walker(os.path.dirname(options.image))
	#update_coordinate(options.image)

