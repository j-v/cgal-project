import sys
import os
import shutil

''' usage: flattenize IN_PATH OUT_PATH '''

libpath = sys.argv[1]
outpath = sys.argv[2]

cats = os.listdir(libpath)

for cat in cats:
	items = os.listdir(os.path.join(libpath, cat))
	i = 1
	for item in items:
		srcpath = os.path.join(libpath, cat, item)
		filename, extension = os.path.splitext(item)
		destfile = cat + str(i) + extension
		destpath = os.path.join(outpath, destfile)
		shutil.copyfile(srcpath, destpath)
		
		print 'Copied %s to %s' % (srcpath, destpath)
		
		i += 1
		


