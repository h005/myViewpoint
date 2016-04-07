#-*-coding:utf-8-*-
import subprocess
import re
import sys
import pprint

IN = 'kxm.data'
fname = 'kxm.fname'
if __name__ == '__main__':
    '''从一个已有文件之后筛选出单列属性进行预测'''

    fnames = {}
    with open(fname, 'r') as f:
        for idx, line in enumerate(f):
            line = line.strip()
            l = fnames.get(line, [])
            l.append(idx)
            fnames[line] = l
    pprint.pprint(fnames)
        
    with open(IN, 'r') as indata:
        v = indata.readline().strip()
        feature_count = len(v.split(' ')) - 1

    result = {}
    for key in fnames:
        with open('kxm.data', 'r') as indata:
            with open(r'C:\Users\mzd\Desktop\viewpoint\libsvm-3.20-auc-mod\kxm.data', 'w') as f:
                for line in indata:
                    elements = line.strip().split(' ')
                    f.write('%s' % elements[0])
                    for df in fnames[key]:
                        f.write(' %s' % elements[df + 1])
                    f.write('\n')
        
        # 运行easy.py
        command = 'python easy.py ../kxm.data'
        process = subprocess.Popen(command, cwd=r'C:\Users\mzd\Desktop\viewpoint\libsvm-3.20-auc-mod\tools', stdout=subprocess.PIPE)
        output = process.communicate()[0]
        sys.stderr.write('{0}\n'.format(key))
        sys.stderr.write(output)
        ex_name_re=re.search(r'CV rate=[\d]+.[\d]+', output)
        val = float(ex_name_re.group(0)[len('CV rate='):].strip())
        result[key] = val

    result = list(sorted(result.items(), key=lambda d:d[1], reverse=True))
    for item in result:
        print '\text{{{0}}}_{{{1}}} & {2} \\\\'.format(item[0], len(fnames[item[0]]), item[1])
