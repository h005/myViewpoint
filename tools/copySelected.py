#-*-coding:utf-8-*-
import sys
import shutil
import os

selected_items = []
while True:
    first = sys.stdin.readline()
    if not first:
        break
    first = first.strip()
    for i in xrange(8):
        sys.stdin.readline()
    selected_items.append(first)

import pprint
pprint.pprint(selected_items)

for item in selected_items:
    shutil.copyfile(item, os.path.join('selected', item))

with open(os.path.join('selected', 'selected.txt'), 'w') as f:
    f.write('\n'.join(selected_items))
