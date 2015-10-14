#-*- coding:utf-8 -*-
import sys
import optparse
import random

if __name__ == '__main__':
    usage = 'Usage: %prog --input=kxm.data --output=kxm_sub.data'
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('--input', dest='input', help='data file with all features')
    parser.add_option('--ratio', dest='ratio', type='float', default=0.8, help='percentage of features to be left, e.g. 0.8')
    parser.add_option('--output', dest='output', help='output data file after feature selection')
    options, args = parser.parse_args()

    data_file = open(options.input, 'r')

    # 统计所有的属性
    feature_set = set()
    while True:
        line = data_file.readline().strip()
        if not line:
            break
        elements = line.split(' ')
        for e in elements[1:]:
            idx, val = e.split(':')
            idx = int(idx)
            feature_set.add(idx)

    # 筛选出所需的特征
    sampled_feature = random.sample(feature_set, max(1, int(len(feature_set) * options.ratio)))

    output_file = open(options.output, 'w')
    data_file.seek(0)
    while True:
        cache = []
        line = data_file.readline().strip()
        if not line:
            break
        elements = line.split(' ')
        cache.append(elements[0])

        for e in elements[1:]:
            idx, val = e.split(':')
            idx = int(idx)
            if idx in sampled_feature:
                cache.append(e)

        output_file.write('%s\n' % ' '.join(cache))

    data_file.close()
    output_file.close()

    print '[OK!]'
