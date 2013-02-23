import sys

def printUsage():
    print '''Usage: python combine_name_and_points.py DB_FILE PTS_FILE OUT_FILE'''

def main():
    if len(sys.argv) < 4 :
        printUsage()
        exit(1)

    db_file = sys.argv[1]
    pts_file = sys.argv[2]
    out_file = sys.argv[3]

    dbf = open(db_file)
    ptf = open(pts_file)
    of = open(out_file, 'w')

    for db_line, pt_line in zip(dbf.readlines(), ptf.readlines()):
        image_name = db_line.split(',')[0]
        pts = pt_line.strip().split(' ')
        of.write(image_name + ' ' + ' '.join([str(p) for p in pts]) + "\n")

    print 'done'

if __name__ == '__main__':
    main()
