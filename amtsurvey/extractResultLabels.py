#-*-coding:utf-8-*-

if __name__ == '__main__':
    import optparse
    usage = 'Usage: %prog a.csv [b.csv c.csv d.csv]'  

    parser = optparse.OptionParser(usage = usage)
    (options, args) = parser.parse_args()

    labels = {}
    for filename in args:
        with open(filename, 'r') as f:
            # eat header
            f.readline()
            while True:
                line = f.readline()
                if not line:
                    break
                results = line.strip().split(',')[ord('I') - ord('A'):]
                for res in results:
                    if not res:
                        continue
                    a, b = res.rstrip(']').split('[')
                    if b not in labels:
                        labels[b] = []
                    labels[b].append(1 if a == 'good' else -1)
    #import pprint
    #pprint.pprint(labels)

    for key, val in labels.items():
        for v in val:
            print key
            print v

