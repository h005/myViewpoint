#-*- coding:utf-8 -*-
import sys
import optparse

if __name__ == '__main__':
    usage = 'Usage: %prog --2df=kxm.2df --3df=kxm.3df --label=kxm.label --output=kxm [--libsvm]'
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('--2df', dest='f2d', help='extracted 2d features')
    parser.add_option('--3df', dest='f3d', help='extracted 3d features')
    parser.add_option('--label', dest='label', help='labels for training')
    parser.add_option('--output', dest='output', help='output data file name')
    parser.add_option('--libsvm', dest='output_libsvm', action='store_true', help='output file with libsvm format')
    options, args = parser.parse_args()

    data = {}

    length = -1
    f2d = open(options.f2d, 'r')
    while True:
        key = f2d.readline().strip()
        features = f2d.readline().strip()
        if not key:
            break
        data[key] = features
        
        # 检查向量的长度是否一致
        current_length = len(data[key].split(' '))
        if length < 0:
            length = current_length
        else:
            assert(length == current_length)
    f2d.close()

    length = -1
    f3d = open(options.f3d, 'r')
    while True:
        key = f3d.readline().strip()
        features = f3d.readline().strip()
        if not key:
            break
        assert(key in data)
        data[key] = data[key] + " " + features

        # 检查向量的长度是否一致
        current_length = len(data[key].split(' '))
        if length < 0:
            length = current_length
        else:
            assert(length == current_length)
    f3d.close()

    length = -1
    label = open(options.label, 'r')
    while True:
        key = label.readline().strip()
        value = label.readline().strip()
        if not key:
            break
        assert(key in data)
        data[key] = value + " " + data[key] 

        # 检查向量的长度是否一致
        current_length = len(data[key].split(' '))
        if length < 0:
            length = current_length
        else:
            assert(length == current_length)
    label.close()

    if options.output_libsvm:
        output_data = open('%s.data' % options.output, 'w')
        output_list = open('%s.list' % options.output, 'w')
        # length的长度含有开头的label
        for key in data:
            output_list.write(key + "\n")
            elements = data[key].strip().split(' ')

            # 输出分类
            output_data.write(elements[0])
            # 输出特征, libsvm下标从1开始
            for idx, val in enumerate(elements[1:]):
                output_data.write(' %d:%s' % (idx+1, val))
            output_data.write("\n")
        output_data.close()
        output_list.close()
    else:
        output_data = open('%s.data' % options.output, 'w')
        output_list = open('%s.list' % options.output, 'w')
        # length的长度含有开头的label
        output_data.write('%d %d\n' % (len(data), length-1))
        for key in data:
            output_list.write(key + "\n")
            output_data.write(data[key] + "\n")
        output_data.close()
        output_list.close()

    print '[OK!]'
