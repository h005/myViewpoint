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
import filecmp
import re
import time

def getTestCase(dir_path):
    res = []
    for root, dirs, files in os.walk(dir_path):
        for name in files:
            file_path = os.path.join(root, name)
            fname, ext = os.path.splitext(name)
            if ext.lower().startswith('.3df'):
                res.append(file_path)
    return res

if __name__ == '__main__':
    # For more details about optparser, Please visit:
    # https://docs.python.org/2/library/optparse.html#default-values

    usage = 'Usage: %prog --dir=. -output=3df.out'  
    parser = optparse.OptionParser(usage = usage)
    parser.add_option('--dir', dest='dir', default = '.', help="3df file's dir")
    parser.add_option('--output', dest='output', default = '3df.out', help="The output file path")
    (options, args) = parser.parse_args()

    print 'Searching %s For 3df files' % options.dir
    files = getTestCase(options.dir)
    print 'files:\n%s' % '\n'.join(files)

    result = {}
    for f in files:
        with open(f, 'r') as fp:
            while True:
                a = fp.readline().strip()
                b = fp.readline().strip()
                if not a or not b:
                    break

                assert(a not in result or result[a] == b)
                result[a] = b

    with open(options.output, 'w') as fp:
        for key, val in result.items():
            fp.write(key + '\n' + val + '\n')

    print 'output: %s' % options.output
        

