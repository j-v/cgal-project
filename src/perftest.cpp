#include <string>
#include <iostream>
#include <fstream>
#include <cv.h>

#include "pointset.h"
#include "emd.h"
#include "functions.h"

#define LOG_FILENAME "perftest_log.csv"

using namespace std;

int main( int argc, char** argv ) {

	int width = 100;
	int height = 100;

	int reps = 5;
	ofstream log_stream(LOG_FILENAME);

	// Write log header
	log_stream << "pts1,pts2,w1,w2,emd,emd_time" << endl;

	// Hold weight constant, vary number of points
	cout << "Holding weight constant..." << endl;
	int num_points[] = {1, 10, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500};
	float weight_sum = 100.0;
	int n = sizeof(num_points)/sizeof(num_points[0]);
	int pts1, pts2;
	for (int i=0; i < n; i++)
	{
		pts1 = num_points[i];
		for (int j=i; j < n; j++)
		{
			pts2 = num_points[j];
			cout << "pts1: " << pts1 << " pts2: " << pts2 << endl;
			for (int r=0; r < reps; r++)
			{
				signature_t s1;
				signature_t s2;
				random_point_set(s1, pts1, weight_sum, width, height);
				random_point_set(s2, pts2, weight_sum, width, height);

				int64 start = cvGetTickCount();
				float emd_dist = emd(&s1,&s2,euclid_dist2,NULL,NULL);
				double emd_time = ((double)cvGetTickCount() - (double)start)/cvGetTickFrequency();

				log_stream << pts1 << "," << pts2 << "," << weight_sum << "," << weight_sum << "," << emd_dist << "," << emd_time << endl;

				releaseSignatureMemory(s1);
				releaseSignatureMemory(s2);
			}
		}
	}

	// Hold number of points constant, while varying weights
	float weights[] = {1.0, 2.0, 3.0, 4.0, 5.0, 7.5, 10.0, 15.0, 20.0, 25.0, 50.0, 75.0, 100.0} ;
	int const_num_points = 150;
	n = sizeof(weights)/sizeof(weights[0]);
	float w1, w2;
	for (int i=0; i < n; i++)
	{
		w1 = weights[i];
		for (int j=i; j < n; j++)
		{
			w2 = weights[j];
			signature_t s1;
			signature_t s2;

			cout << "w1: " << w1 << " w2: " << w2 << endl;
			for (int r=0; r < reps; r++)
			{
				
				random_point_set(s1, const_num_points, w1, width, height);
				random_point_set(s2, const_num_points, w2, width, height);

				int64 start = cvGetTickCount();
				float emd_dist = emd(&s1,&s2,euclid_dist2,NULL,NULL);
				double emd_time = ((double)cvGetTickCount() - (double)start)/cvGetTickFrequency();

				log_stream << const_num_points << "," << const_num_points << "," << w1 << "," << w2<< "," << emd_dist << "," << emd_time << endl;

				releaseSignatureMemory(s1);
				releaseSignatureMemory(s2);
			}
		}
	}

	cout << "DONE" << endl;

	log_stream.close();

	return 0;
}