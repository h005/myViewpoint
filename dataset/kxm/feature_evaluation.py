#-*-coding:utf-8-*-
import subprocess
import re
import sys
import pprint

IN = 'kxm.data'
if __name__ == '__main__':
    '''从一个已有文件之后筛选出单列属性进行预测'''

    with open(IN, 'r') as indata:
        v = indata.readline().strip()
        feature_count = len(v.split(' ')) - 1

    result = {}
    for df in xrange(feature_count):
        with open('kxm.data', 'r') as indata:
            with open(r'C:\Users\mzd\Desktop\viewpoint\libsvm-3.20-auc-mod\kxm.data', 'w') as f:
                for line in indata:
                    elements = line.strip().split(' ')
                    f.write('%s %s\n' % (elements[0], elements[df+1]))
        
        # 运行easy.py
        command = 'python easy.py ../kxm.data'
        process = subprocess.Popen(command, cwd=r'C:\Users\mzd\Desktop\viewpoint\libsvm-3.20-auc-mod\tools', stdout=subprocess.PIPE)
        output = process.communicate()[0]
        sys.stderr.write('%d\n' % df)
        sys.stderr.write(output)
        ex_name_re=re.search(r'CV rate=[\d]+.[\d]+', output)
        val = float(ex_name_re.group(0)[len('CV rate='):].strip())
        result[df] = val

    pprint.pprint(list(sorted(result.items(), key=lambda d:d[1], reverse=True)))
