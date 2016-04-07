#-*- coding:utf-8 -*-
import sys
import optparse
import random

if __name__ == '__main__':
    usage = 'Usage: %prog --2df=kxm.2df --output=kxm.label'
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('--list', dest='names', help='list file')
    parser.add_option('--predicted', dest='predicted', help='predicted classification file')
    parser.add_option('--output', dest='output', help="output file")
    options, args = parser.parse_args()

    data = {}

    a = open(options.names, 'r')
    b = open(options.predicted, 'r')
    while True:
        name = a.readline().strip()
        label = b.readline().strip()
        if not name:
            break
        data[name] = label
    b.close()
    a.close()

    output = open(options.output, 'w')
    for key in sorted(data.keys()):
        output.write(key + '\n')
        output.write(data[key] + '\n')
    output.close()
    print '[OK!]'
