#-*-coding:utf-8-*-
import math
import sys

def dotproduct(v1, v2):
    return sum((a*b) for a, b in zip(v1, v2))

def length(v):
    return math.sqrt(dotproduct(v, v))

def angle(v1, v2):
    return math.acos(dotproduct(v1, v2) / (length(v1) * length(v2)))

def clamp_angle(angle):
    while angle < 0:
        angle += 2 * math.pi
    while angle > 2 * math.pi:
        angle -= 2 * math.pi
    return angle


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

        print key.strip()
        
        # angle
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
        print angle1, angle2, angle3,

        line_angles = []
        for i in xrange(6, 9):
            # 一条线有两个向量
            line_angles.append(clamp_angle(float(vals[i])))
            line_angles.append(clamp_angle(float(vals[i]) + math.pi))
        line_angles.sort()
        # 求最接近pi / 2的那个向量
        start_idx, _ = sorted(enumerate(line_angles), key=lambda x:abs(math.pi / 2.0 - x[1]))[0]
        angle_diff = []
        for i in xrange(0, len(line_angles)):
            idx1 = (i + start_idx) % len(line_angles)
            idx2 = (i + 1 + start_idx) % len(line_angles)
            angle_diff.append(clamp_angle(line_angles[idx2] - line_angles[idx1]))
        assert len(angle_diff) == 6

        for i in xrange(0, 6, 2):
            print angle_diff[i] + angle_diff[i+1],
        print
