#coding:utf-8   

import argparse
import sys
import numpy as np   
from sklearn.cluster import MeanShift, estimate_bandwidth, KMeans  
from sklearn.datasets.samples_generator import make_blobs   

if __name__ == '__main__':
    usage = '%(prog)s --input zyns.vp --output zyns.cluster --N 3000'
    parser =  argparse.ArgumentParser(usage=usage)
    parser.add_argument('--input', dest='input', help='Viewpoint input file')
    parser.add_argument('--output', dest='output', help='Cluster result file')
    parser.add_argument('--N', dest='N', type=int, help='Scene range')
    options = parser.parse_args()

    names = []
    points = []
    forwards = []
    ups = []
    N = options.N
    with open(options.input, 'r') as f:
        while True:
            name = f.readline().strip()
            pos = f.readline().strip()
            forward = f.readline().strip()
            up = f.readline().strip()
            if not name: 
                break

            pos = tuple(map(float, pos.split(' '))) 
            if -N < pos[0] < N and -N < pos[1] < N:
                names.append(name)
                points.append(pos)
                forwards.append(forward)
                ups.append(up)
    X = np.array(points)
    #X = X[:, (0, 1)]
      
    # 生成样本点   
    #centers = [[1, 1], [-1, -1], [1, -1]]   
    #X, _ = make_blobs(n_samples=10000, centers=centers, cluster_std=0.6)   
      
      
    # 通过下列代码可自动检测bandwidth值   
    bandwidth = estimate_bandwidth(X, quantile=0.2)   
    print bandwidth
      
    ms = MeanShift(bandwidth=bandwidth, bin_seeding=True)   
    #ms = KMeans(n_clusters=20)
    ms.fit(X)   
    labels = ms.labels_   
    cluster_centers = ms.cluster_centers_   
      
    labels_unique = np.unique(labels)   
    n_clusters_ = len(labels_unique)   
    new_X = np.column_stack((X, labels))   


    with open(options.output, 'w') as f:
        f.write('{} {}\n'.format(n_clusters_, len(names)))
        for idx, name in enumerate(names):
            f.write('{0} {1} {2} {3} {4} {5} {6}\n'.format(name, points[idx][0], points[idx][1], points[idx][2], forwards[idx], ups[idx], labels[idx]+1))
        for k in xrange(n_clusters_):
            f.write('{0} {1} 0 0 0 0 0 0 0\n'.format(cluster_centers[k, 0], cluster_centers[k, 1]))
        
      
    print("number of estimated clusters : %d" % n_clusters_)   
    print("Top 10 samples:",new_X[:10])   
      
    # 图像输出   
    import matplotlib.pyplot as plt   
    from itertools import cycle   
      
    plt.figure(1)   
    plt.clf()   
    axes = plt.gca()
    axes.set_xlim([-N,N])
    axes.set_ylim([-N,N])
     
    colors = cycle('bgrcmykbgrcmykbgrcmykbgrcmyk')   
    for k, col in zip(range(n_clusters_), colors):   
        my_members = labels == k   
        print np.count_nonzero(my_members), X.shape[0]
        if np.count_nonzero(my_members) < 0.02 * X.shape[0]:
            pass
            #continue
        cluster_center = cluster_centers[k]   
        plt.plot(X[my_members, 0], X[my_members, 1], col + '.')   
        plt.plot(cluster_center[0], cluster_center[1], 'o', markerfacecolor=col,   
                 markeredgecolor='k', markersize=14)   
    plt.title('Estimated number of clusters: %d' % n_clusters_)   
    plt.show()   
    sys.exit(0)

    from mpl_toolkits.mplot3d import Axes3D
    import matplotlib.pyplot as plt

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    x =X[:, 0]
    y =X[:, 1]
    z =X[:, 2]

    ax.scatter(x, y, z, c='r', marker='o')

    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')

    plt.show()
