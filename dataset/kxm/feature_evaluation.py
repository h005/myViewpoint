#-*-coding:utf-8-*-
import combine

if __name__ == '__main__':
    # 搜集所有的df文件

    dfs = ['1.df', '2.df', '3.df']
    for df in dfs:
        input_file, = generate_file('kxm.label', [df], True)
        with open('../../kxm.data', 'w') as f:
            f.write(input_file)

        # 运行easy.py
