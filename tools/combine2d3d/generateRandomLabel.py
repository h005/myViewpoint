#-*- coding:utf-8 -*-
import sys
import optparse
import random

if __name__ == '__main__':
	usage = 'Usage: %prog --2df=kxm.2df --output=kxm.label'
	parser = optparse.OptionParser(usage=usage)
	parser.add_option('--2df', dest='f2d', help='extracted 2d features')
	parser.add_option('--output', dest='output', help='output label file name')
	options, args = parser.parse_args()

	data = {}

	f2d = open(options.f2d, 'r')
	while True:
		key = f2d.readline().strip()
		features = f2d.readline().strip()
		if not key:
			break
		data[key] = features
	f2d.close()

	output = open(options.output, 'w')
	for key in data:
		output.write(key + '\n')
		output.write('%d\n' % random.randint(0, 1))
	output.close()
	print '[OK!]'
