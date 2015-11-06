#-*- coding:utf-8 -*-
import sys
import optparse

if __name__ == '__main__':
    usage = 'Usage: %prog --label=kxm.label --output=kxm --libsvm kxm.2df kxm.3df [kxm2.3df]'
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('--label', dest='label', help='labels for training')
    parser.add_option('--output', dest='output', help='output data file name')
    parser.add_option('--libsvm', dest='output_libsvm', action='store_true', help='output file with libsvm format')
    options, feature_files = parser.parse_args()

    data = {}

    length = -1
    with open(options.label, 'r') as label:
        while True:
            key = label.readline().strip()
            value = label.readline().strip()
            if not key:
                break

            data[key] = value 
            # 检查向量的长度是否一致
            current_length = len(data[key].split(' '))
            if length < 0:
                length = current_length
            else:
                assert(length == current_length)

    for feature_file in feature_files:
        length = -1
        with open(feature_file, 'r') as ff:
            while True:
                key = ff.readline().strip()
                features = ff.readline().strip()
                if not key:
                    break
                assert(key in data)
                data[key] = data[key] + ' ' + features
                
                # 检查向量的长度是否一致
                current_length = len(data[key].split(' '))
                if length < 0:
                    length = current_length
                else:
                    assert(length == current_length)

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
