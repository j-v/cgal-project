import math
import sys

def printusage():
    print '''Compute and output the distance matrix given a list of points with dimension D
Usage: python dist.py infile outfile D'''

def dist(p1, p2):
    s = 0
    for i in range(len(p1)):
        if math.isnan(p2[i]) or math.isnan(p1[i]):
            continue
        d = p2[i] - p1[i]
        s += d * d
    return math.sqrt(s)

def make_distmat(filename, outfile, dims):
    f = open(filename)
    pts = []
    for l in f.readlines():
        pts.append(tuple(float(n) for n in l.split(' ')))

    f.close()

    mat = [[0]*len(pts) for n in range(len(pts))]
    for i in range(len(pts)):
        for j in range(i+1, len(pts)):
            mat[i][j] = dist(pts[i], pts[j])
            mat[j][i] = mat[i][j]

    f = open(outfile, 'w')
    for i in range(len(pts)):
        f.write(' '.join(str(n) for n in mat[i]))
        f.write("\n")
    f.close()


def main():
    if len(sys.argv) < 4:
        printusage()
        sys.exit(1)
    infile = sys.argv[1]
    outfile = sys.argv[2]
    d = int(sys.argv[3])
    make_distmat(infile, outfile, d)


if __name__ == '__main__':
    main()




