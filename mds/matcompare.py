import math
import sys

def printusage():
    print '''Calculate the matrix difference between two csv mat files with dimensions NxN
Usage: matcompare.py FILE1 FILE2 N'''
def load_mat(filename, d):
    f = open(filename)
    mat = []
    for l in f.readlines():
        mat.append(list(float(n) for n in l.split(' ')))
    return mat

def compare_mat(file1, file2, d):
    m1 = load_mat(file1, d)
    m2 = load_mat(file2, d)

    err = 0
    for i in range(d):
        for j in range(d):
            err += abs(m1[i][j] - m2[i][j])

    err_av = err / float(d * d)

    print "total err=%f" % (err)
    print "avg err=%f" % (err_av)


def main():
    if len(sys.argv) < 4:
        printusage()
        sys.exit(1)
    file1 = sys.argv[1]
    file2 = sys.argv[2]
    n = int(sys.argv[3])
    compare_mat(file1, file2, n)

if __name__ == '__main__':
    main()
