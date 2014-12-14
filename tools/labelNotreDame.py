#-*- coding:utf-8 -*-
# 根据NotreDame的列表将文件以顺序重命名，并且将相机的内外参数
# 分离到到对应的文件中去
import os

l = open('list.txt', 'r')
i = 0
for line in l:
	i += 1
	line = line.strip()
	dirname = os.path.dirname(line)
	basename = os.path.basename(line)
	fname, fext = os.path.splitext(basename)

	img_file = os.path.join(dirname, '%s%s' % (fname, fext))
	rd_file = os.path.join(dirname, '%s.rd%s' % (fname, fext))
	new_img_file = os.path.join(dirname, 'exp%d%s' % (i, fext))
	new_rd_file = os.path.join(dirname, 'exp%d.rd%s' % (i, fext))

	if os.path.exists(img_file):
		print '%s --> \n\t%s' % (img_file, new_img_file)
		os.rename(img_file, new_img_file)
	if os.path.exists(rd_file):
		print '%s --> \n\t%s' % (rd_file, new_rd_file)
		os.rename(rd_file, new_rd_file)

l.close()

l = open('notredame.out', 'r')
l.readline()
a, b = l.readline().strip().split(' ')
a = int(a)
assert a == i
for i in xrange(a):
	out = open(os.path.join(dirname, 'exp%i.txt' % (i + 1)), 'w')
	r = []
	for j in xrange(5):
		content = l.readline().strip()
		r.append(content)
	out.write('\n'.join(r))
	out.close()
