#-*- coding:utf-8 -*-
import sys
import optparse
import argparse
import StringIO

def prepare_for_SVM2K(label_file, f2d_files, f3d_files):
    label = {}
    length = -1
    with open(label_file, 'r') as labelfile:
        while True:
            key = labelfile.readline().strip()
            value = labelfile.readline().strip()
            if not key:
                break

            label[key] = value 
            # 检查向量的长度是否一致
            current_length = len(label[key].split(' '))
            if length < 0:
                length = current_length
            else:
                assert(length == current_length)

    f2d = {}
    for feature_file in f2d_files:
        length = -1
        with open(feature_file, 'r') as ff:
            while True:
                key = ff.readline().strip()
                features = ff.readline().strip()
                if not key:
                    break
                if key in f2d:
                    f2d[key] = f2d[key] + ' ' + features
                else:
                    f2d[key] = features
                
                # 检查向量的长度是否一致
                current_length = len(f2d[key].split(' '))
                if length < 0:
                    length = current_length
                else:
                    assert(length == current_length)

    f3d = {}
    for feature_file in f3d_files:
        length = -1
        with open(feature_file, 'r') as ff:
            while True:
                key = ff.readline().strip()
                features = ff.readline().strip()
                if not key:
                    break
                if key in f3d:
                    f3d[key] = f3d[key] + ' ' + features
                else:
                    f3d[key] = features
                
                # 检查向量的长度是否一致
                current_length = len(f3d[key].split(' '))
                if length < 0:
                    length = current_length
                else:
                    assert(length == current_length)

    assert len(f2d) == len(f3d) == len(label)

    output_filename = StringIO.StringIO()
    output_label = StringIO.StringIO()
    output_f2d = StringIO.StringIO()
    output_f3d = StringIO.StringIO()

    for key in sorted(label.keys()):
        output_filename.write(key + '\n')
        output_label.write(label[key] + "\n")
        output_f2d.write(f2d[key] + "\n")
        output_f3d.write(f3d[key] + "\n")

    return output_filename.getvalue(), output_label.getvalue(), output_f2d.getvalue(), output_f3d.getvalue()


if __name__ == '__main__':
    usage = '%(prog)s --label=hjw.label --2df=kxmmodel.2df --3df=kxmmodel.3df --output=kxm'
    parser =  argparse.ArgumentParser(usage=usage)
    parser.add_argument('--label', dest='label', help='labels for training')
    parser.add_argument('--output', dest='output', help='output data file name')
    parser.add_argument("--2df", dest='f2d', default=[], type=str, nargs='+', help="The 2D feature files to be loaded.")
    parser.add_argument("--3df", dest='f3d', default=[], type=str, nargs='+', help="The 3D feature files to be loaded.")
    options = parser.parse_args()

    a, b, c, d = prepare_for_SVM2K(options.label, options.f2d, options.f3d)
    with open(options.output + '.list', 'w') as o:
        o.write(a)
    with open(options.output + '.labelc', 'w') as o:
        o.write(b)
    with open(options.output + '.2dfc', 'w') as o:
        o.write(c)
    with open(options.output + '.3dfc', 'w') as o:
        o.write(d)
    print '[OK!]'
