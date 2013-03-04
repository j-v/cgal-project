# CGAL Project : Earth Mover's Distance

### Freie Universität Berlin - Winter Semester 2012-13

### Authors

* Martí Griera
* Jonathon Volkmar

### Requirements

* OpenCV
* dirent.h (if on Windows, available with a Google search)
* CMake

CGAL not (yet) required...

### Compiling

Instructions for *nix 

	cd src
	cmake .
	make

Output is a executable called "chsemd" (which stands for Canny Harris Sobel EMD), and various other executables.

### Execution

Building an image database:

	./chsemd -builddb PATH-TO-DIR-WITH-IMAGES

The previous command creates a CSV index file in the same directory as the images.

Querying an image database:

	./chsemd PATH-TO-QUERY-IMAGE PATH-TO-IMAGE-DB

PATH-TO-IMAGE-DB would be the same as PATH-TO-DIR-WITH-IMAGES in the first step.

### MDS Visualization

Build the EMD distance matrix and output to file, in directory of the image DB:
	
	emdmatrix DB_PATH

Perform MDS using the MDSJ executable, save resulting point set to file in directory of image DB:

	cd ../mds
	java -jar mdsj.jar -d3 DB_PATH/emd_dist_mat.csv DB_PATH/mds_points_3d.txt

Create file with names of images and MDS point coordinates for input to MDS visualizer:

	cd ../scripts
	python combine_names_and_points DB_PATH/emdindex.csv DB_PATH/mds_points_3d.txt DB_PATH/names_and_points_3d.txt

Run MDSVis, choosing the names_and_points_3d.txt file created in the last time when the file picker is presented at startup. The app can be started from Processing by loading mds/MDSVis/MDSVis.pde, or execute the included binary:
	
	../mds/MDSVis/application.linux32/MDSVis

### Other executables

Further parameters for the above described programs and parameters for emdvis can be shown by running the programs without arguments. You can play around with some of the python scripts in the ./scripts directory (e.g. eval.py for classifier evaluation)


