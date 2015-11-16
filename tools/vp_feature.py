#-*-coding:utf-8-*-
import math

def dotproduct(v1, v2):
  return sum((a*b) for a, b in zip(v1, v2))

def length(v):
  return math.sqrt(dotproduct(v, v))

def angle(v1, v2):
  return math.acos(dotproduct(v1, v2) / (length(v1) * length(v2)))


def cmp_points(a, b):
    v1 = cmp(a[0], b[0])
    if v1 != 0:
        return v1
    else:
        return cmp(a[1], b[1])

with open('vp.out', 'r') as f:
    while True:
        key = f.readline()
        val = f.readline()
        if not key:
            break
        
        vals = val.strip().split(' ')
        vp_points = []
        for i in xrange(0, 6, 2):
            vp_points.append((float(vals[i]), float(vals[i+1])))

        vp_points.sort(cmp = cmp_points)
        a = vp_points[0]
        b = vp_points[1]
        c = vp_points[2]

        angle1 = angle((b[0] - a[0], b[1] - a[1]), (c[0] - a[0], c[1] - a[1]))
        angle2 =  angle((b[0] - c[0], b[1] - c[1]), (a[0] - c[0], a[1] - c[1]))
        angle3 = angle((a[0] - b[0], a[1] - b[1]), (c[0] - b[0], c[1] - b[1]))
        print key.strip()
        print angle1, angle2, angle3
