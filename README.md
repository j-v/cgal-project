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

Output is a executable called "chsemd" (which stands for Canny Harris Sobel EMD)

### Execution

Building an image database:

	chsemd -builddb PATH-TO-DIR-WITH-IMAGES

The previous command creates a CSV index file in the same directory as the images.

Querying an image database:

	chsemd PATH-TO-QUERY-IMAGE PATH-TO-IMAGE-DB

PATH-TO-IMAGE-DB would be the same as PATH-TO-DIR-WITH-IMAGES in the first step.




	
