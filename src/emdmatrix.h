#ifndef EMDMATRIX_H
#define EMDMATRIX_H 1

#include "db.h"
#include "emd.h"


using namespace std;

// Compute EMD distance matrix
// return 0 on success
int compute_distance_matrix(EmdDB &db, 
							double** mat); // 2D array to hold computation results. Must be preallocated to NxN

// Save matrix in CSV format, with the first line online containing N (matrix size - square)
int save_distance_matrix(string filename, double** mat, int size);

double** load_distance_matrix(string filename);

void release_matrix(double ** mat, int n);


#endif