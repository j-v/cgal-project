We want to find N such that P = MDS(D, N) minimizes the error | distmat(P) - EMDdistmat |

The following steps show how to perform one iteration of N:

Perform MDS on the matrix outputted by the emdmatrix executable, outputting a list of points, with space separated vector format:

	$ java -jar mdsj.jar
	MDSJ - Multidimensional Scaling for Java v0.2  (c) 2009 University of Konstanz
	  usage: MDSJ  [options]  infile  [outfile]        see manual for more details
	  options
	   -dD   output dimensions (default: D=2)
	   -eE   use exponential weight=dissimilarity^E (default: E=0)
	   -fF   data format: -fm matrix (default), -fl list, -fo optimized list
	   -q    quiet mode, do not print analysis results
	   -rR   stress minimization for at most R rounds
	   -sS   stress minimization until relative change is below 10^(-S)
	   -tT   stress minimization for up to T milliseconds
	  example:  MDSJ  -t2000  -s4 -d3  -q  in.txt  out.txt
	  -rR -sS -tT may be used jointly (termination once any condition holds)
	  output is written to stdout when no outfile specified

	$ java -jar mdsj.jar ..\shapes216\distmat.csv .\results\shapes216\mds_2d.txt
	MDSJ - Multidimensional Scaling for Java v0.2  (c) 2009 University of Konstanz
	  ...reading input file... done (170 ms)
	  read 218 rows, 217 columns
	  using 217 pivots for 2 dimensions
	  trace:  48392.350074
	  spectral range: [-8501.209052,  28689.871771]
	  --------------------------------------------------------------------
		dim    eigenvalue     relative (%)   cumulative (%) residual (%)
	  --------------------------------------------------------------------
		0   28689.835415      59.28589       59.28589       40.71411
		1   12393.914696      25.61131       84.8972        15.1028
	  --------------------------------------------------------------------
	  ...writing output file... done (33 ms)
	  wrote 218 rows, 2 columns


Compute the distance matrix of the point set:

	$ python dist.py
	Compute and output the distance matrix given a list of points with dimension D
	Usage: python dist.py infile outfile D
		
	$ python dist.py .\results\shapes216\mds_2d.txt .\results\shapes216\emd_dist_mat.mat 2

	$ cat .\results\shapes216\emd_dist_mat.mat
	0 26.5470708028 19.725763432 31.6418237508 28.9468753411 11.0345977978 31.7564350453 ...

Evaluation: calculate the difference (error) of the new distance matrix and original EMD distance matrix:

	$ python matcompare.py
	Calculate the matrix difference between two csv mat files with dimensions NxN
	Usage: matcompare.py FILE1 FILE2 N

	$  python .\matcompare.py .\results\shapes216\emd_dist_mat.mat ..\shapes216\distmat.csv 217
	total err=351002.342753
	avg err=7.454020

Output shown is sum of all error over all entries in matrix, and avg is sum/(NxN)

	
