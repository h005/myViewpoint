#-*- coding:utf-8 -*-
import os
import shutil

l = open('list.txt', 'r')
i = 0
valid = []
for line in l:
	i += 1
	line = line.strip().split(' ')[0]
	valid.append(line)
l.close()

l = open('bundle.out', 'r')
l.readline()
a, b = l.readline().strip().split(' ')
a = int(a)
assert a == i
for i in xrange(a):
	for j in xrange(5):
		buf = l.readline()
	if float(buf.strip().split(' ')[0]) != 0:
		shutil.copyfile(valid[i], os.path.join('valid', valid[i]))
l.close()
