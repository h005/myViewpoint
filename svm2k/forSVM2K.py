#-*- coding:utf-8 -*-
import sys
import optparse
import argparse
import StringIO

def prepare_for_SVM2K(label_file, f2d_files, f3d_files, mask=None):
    label = {}
    length = -1
    
    if label_file:
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

    output_filename = StringIO.StringIO()
    output_label = StringIO.StringIO()
    output_f2d = StringIO.StringIO()
    output_f3d = StringIO.StringIO()

    for idx, key in enumerate(sorted(f3d.keys())):
        if mask is not None and idx not in mask:
            continue
        output_filename.write(key + '\n')
        output_label.write(label.get(key, '1') + "\n")
        output_f2d.write(f2d.get(key, '') + "\n")
        output_f3d.write(f3d.get(key, '') + "\n")

    return output_filename.getvalue(), output_label.getvalue(), output_f2d.getvalue(), output_f3d.getvalue()

def SVM2K_write(label, f2d, f3d, output, mask=None):
    a, b, c, d = prepare_for_SVM2K(label, f2d, f3d, mask)
    with open(output + '.list', 'w') as o:
        o.write(a)
    with open(output + '.labelc', 'w') as o:
        o.write(b)
    with open(output + '.2dfc', 'w') as o:
        o.write(c)
    with open(output + '.3dfc', 'w') as o:
        o.write(d)

if __name__ == '__main__':
    usage = '%(prog)s --label=hjw.label --2df=kxmmodel.2df --3df=kxmmodel.3df --output=kxm'
    parser =  argparse.ArgumentParser(usage=usage)
    parser.add_argument('--label', dest='label', help='labels for training')
    parser.add_argument('--output', dest='output', help='output data file name')
    parser.add_argument("--2df", dest='f2d', default=[], type=str, nargs='+', help="The 2D feature files to be loaded.")
    parser.add_argument("--3df", dest='f3d', default=[], type=str, nargs='+', help="The 3D feature files to be loaded.")
    options = parser.parse_args()
    SVM2K_write(options.label, options.f2d, options.f3d, options.output)

    print '[OK!]'
