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
import os

AMT_DIR = r'D:\aws-mturk-clt-1.3.1'
JAVA_HOME = r'C:\Program Files\Java\jdk1.8.0_25'
def prepare_env():
    os.environ['JAVA_HOME'] = JAVA_HOME
    os.environ['PATH'] = os.path.join(JAVA_HOME, 'bin') + ';' + os.environ['PATH']
    classpath = []
    classpath.append(os.path.join(AMT_DIR, 'bin'))
    classpath.append(os.path.join(JAVA_HOME, 'lib'))
    classpath.append(os.path.join(JAVA_HOME, 'lib', 'tools.jar'))
    os.environ['CLASSPATH'] = ';'.join(classpath)
    os.environ['MTURK_CMD_HOME'] = AMT_DIR


def walker(dir_path, in_sandbox):
    if len(dir_path) == 0:
        dir_path = '.'
    
    count = 0
    for root, dirs, files in os.walk(dir_path):
        for name in files:
            fname, ext = os.path.splitext(name)
            if ext.lower() in ('.properties',):
                abs_path = os.path.abspath(root)
                pf = os.path.join(abs_path, fname + '.properties')
                qf = os.path.join(abs_path, fname + '.question')
                inf = os.path.join(abs_path, fname + '.input')

                sandbox_opt = '-sandbox' if in_sandbox else ''
                command = 'loadHITs -question %s -properties %s -input %s %s' % (qf, pf, inf, sandbox_opt)
                wdir = os.path.join(AMT_DIR, 'bin')

                print command

                import subprocess
                p = subprocess.Popen(command, cwd=wdir, stdout=subprocess.PIPE, shell=True)
                print p.communicate()[0]

                count += 1
                if count == 2:
                    break
                

if __name__ == '__main__':
    # For more details about optparser, Please visit:
    # https://docs.python.org/2/library/optparse.html#default-values

    usage = 'Usage: %prog --dir=. [--force]' 
    parser = optparse.OptionParser(usage = usage)
    parser.add_option('-d', '--dir', dest='dir', default = '.', help="source file's dir")
    parser.add_option('--production', dest='sandbox', default=True, action='store_false', help='run in production model')
    (options, args) = parser.parse_args()

    prepare_env()
    walker(os.path.dirname(options.dir), options.sandbox)
