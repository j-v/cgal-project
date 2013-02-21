import sys

''' usage count_points.py PATH_TO_EMD_CSV_INDEX '''

filename = sys.argv[1]
f=open(filename)
lines = f.readlines()
data = [l.split(',') for l in lines]
pointcount = [(len(row)-1)/3 for row in data]

print 'max points:', max(pointcount)
print 'avg points:', sum(pointcount)/float(len(pointcount))
