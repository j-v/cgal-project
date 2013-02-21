#include "emdmatrix.h"

#include "functions.h"
#include "pointset.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include <cv.h>

#define DIR_SEP "/"
#define EMDDB_INDEX "emdindex.csv"
#define MAT_SEP_CHAR ' '
#define DEFAULT_OUT_FILE "emd_dist_mat.csv"
#define DEFAULT_LOG_FILE "build_mat_log.csv"

ofstream * log_stream;

void printUsage()
{
	cout << "Usage: emdmatrix DB_PATH [OUT_FILE [LOG_FILE]]" << endl;
	cout << "OUT_FILE: defaults to " << DEFAULT_OUT_FILE << " in directory of DB_PATH" << endl;
	cout << "LOG_FILE: outputs CSV performance log - defaults to " << DEFAULT_LOG_FILE << " in DB_PATH" << endl;
}


int compute_distance_matrix(EmdDB &db, double** mat)
{
	// init diagonal to 0
	for (int i=0; i<db.numEntries; i++)
	{
		mat[i][i] = 0;
	}

	cout << "Computing distance matrix. " << db.numEntries << " entries." << endl;

	for (int i=0; i<db.numEntries; i++)
	{
		for (int j=i+1; j<db.numEntries; j++)
		{
			entry_t e1 = db.getEntry(i);
			entry_t e2 = db.getEntry(j);
			normalize_by_centroid(e1.signature);
			normalize_by_centroid(e2.signature);

			int64 emd_start = cvGetTickCount();
			float e = emd(&e1.signature, &e2.signature, euclid_dist2, NULL, NULL);
			int64 emd_end = cvGetTickCount();
			double emd_time = ((double)emd_end - (double)emd_start) / cvGetTickFrequency();

			mat[i][j] = e;
			mat[j][i] = e;

			//cout << i << ", " << j << " : " << e << endl;

			*log_stream << db.getPath() << "," << e1.filename << "," << e2.filename << "," 
				<< e1.signature.n << "," << e2.signature.n << "," 
				<< signature_weight_sum(e1.signature) << "," << signature_weight_sum(e2.signature)
				<< "," << emd_time << endl;
		}
	}

	return 0;
}

int save_distance_matrix(string filename, double** mat, int size)
{
	ofstream f(filename.c_str());

	for (int i=0; i<size; i++) {
		if (i>0)
			f << endl << mat[i][0];
		else
			f << mat[i][0];

		for (int j=1; j<size; j++) {
			f << MAT_SEP_CHAR << mat[i][j];
		}
	}

	f.close();

	return 0;	
}

double** load_distance_matrix(string filename)
{
	ifstream f(filename.c_str());
	string line, value;
	if (f.is_open())
	{
		getline(f, line);
		int n = atoi(line.c_str());
		double ** mat = new double*[n];

		for (int i=0; i<n; i++) {
			mat[i] = new double[n];
			getline(f, line);
			stringstream ss(line);
			for (int j=0; j<n; j++) {
				getline(ss, value, MAT_SEP_CHAR);
				mat[i][j] = atof(value.c_str());
			}
		}

		return mat;
	}
	else
	{
		cout << "Error saving distance matrix." << endl;
		return NULL;
	}
}

void release_matrix(double ** mat, int n)
{
	for (int i=0; i<n; i++)
	{
		delete[] mat[i];
	}
	delete[] mat;
}

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		cout << "Incorrect parameters" << endl;
		printUsage();
		exit(1);
	}

	string db_path(argv[1]);
	string db_file = db_path + DIR_SEP + EMDDB_INDEX;
	string out_filepath;
	if (argc > 2)
	{
		out_filepath = argv[2];
	}
	else
	{
		out_filepath = db_path + DIR_SEP + DEFAULT_OUT_FILE;
	}

	string log_path;
	if (argc > 3)
	{
		log_path = argv[3];
	}
	else
	{
		log_path = db_path + DIR_SEP + DEFAULT_LOG_FILE;
	}
	log_stream = new ofstream(log_path);
	*log_stream << "db_path,img1,img2,sig1_points,sig2_points,sig1_weight_sum,sig2_weight_sum,emd_time" << endl;

	EmdDB db;
	int result = db.load(db_file);
	if (result)
	{
		cout << "Unable to open db at " << db_file << ". Check the path is correct" << endl;
		exit(1);
	}

	double ** mat = new double*[db.numEntries];
	for (int i=0; i<db.numEntries; i++)
		mat[i] = new double[db.numEntries];

	cout << "computing distance matrix..." << endl;
	result = compute_distance_matrix(db, mat);
	if (result)
	{
		cout << "failed to compute distance matrix" << endl;
		exit(1);
	}

	cout << "saving distance matrix..." << endl;
	result = save_distance_matrix(out_filepath, mat, db.numEntries);

	// We don't need to load matrix. This fails.
	// TODO load matrix without header
	//cout << "loading distance matrix..." << endl;
	//double ** mat2 = load_distance_matrix(out_filepath);
	//if (mat2 == NULL)
	//{
	//	cout << "Failed to load matrix" << endl;
	//	exit(1);
	//}

	// TODO check doesn't work due to output accuracy
	//cout << "checking matrix..." << endl;
	//for (int i=0; i<db.numEntries; i++)
	//	for (int j=0; j<db.numEntries; j++)
	//	{
	//		if (mat[i][j] != mat2[i][j])
	//		{
	//			cout << "Bad matrix values at " << i << "," << j << endl;
	//			exit(1);
	//		}
	//	}

	log_stream->close();
	release_matrix(mat, db.numEntries);
	//release_matrix(mat2, db.numEntries);

	return 0;
}
