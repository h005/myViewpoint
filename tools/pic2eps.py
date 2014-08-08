#-*- coding:utf-8 -*-
import cv2
import numpy
import optparse
import os
import sys
import shutil
import pprint
import time
import subprocess

def walker(dir_path, force):
	if len(dir_path) == 0:
		dir_path = '.'

	for root, dirs, files in os.walk(dir_path):
		for name in files:
			fname, ext = os.path.splitext(name)
			if ext.lower() in ('.png', '.jpg'):
				png_file = os.path.join(root, name)
				png2eps(png_file, force)
	

def png2eps(image_path, force):
	basename = os.path.basename(image_path)
	fname, ext = os.path.splitext(basename)
	eps_filepath = os.path.join(os.path.dirname(image_path), '%s.eps' % fname)
	if (force \
		or not os.path.exists(eps_filepath) \
		or os.path.getmtime(image_path) > os.path.getmtime(eps_filepath)):

		print '%s --> %s' % (image_path, eps_filepath),
		print os.popen('bmeps -p 3 -c %s %s' % (image_path, eps_filepath)).read()

if __name__ == '__main__':
	# For more details about optparser, Please visit:
	# https://docs.python.org/2/library/optparse.html#default-values

	usage = 'Usage: %prog --dir=. [--force]' 
	parser = optparse.OptionParser(usage = usage)
	parser.add_option('-d', '--dir', dest='dir', default = '.', help="image file's dir")
	parser.add_option('-f', '--force', action='store_true', dest='force_generate', help="force re-generate eps file(s)")
	(options, args) = parser.parse_args()

	walker(os.path.dirname(options.dir), options.force_generate)
