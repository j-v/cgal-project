#include <iostream>
#include <algorithm>
#include <highgui.h>

#include "db.h"
#include "functions.h"
#include "pointset.h"


#define DIR_SEP "/"
#define EMDDB_INDEX "emdindex.csv"

using namespace std;
using namespace cv;

void printUsage() {
	cout << "Compute EMD on two images in a database and visualize the flow" << endl;
	cout << "Usage: ./emdvis DB_DIR IMAGE1 IMAGE2" << endl;
}

float max_val(float *arr, int n)
{
	float curr_max = 0;
	for (int i=0; i<n; i++)
	{
		if (arr[i] > curr_max) 
			curr_max = arr[i];
	}
	return curr_max;
}


int main( int argc, char** argv ) {
	if (argc < 4)
	{
		// invalid parameters: print usage and exit
		printUsage();
		exit(1);
	}

	int result = 0;

	string db_path(argv[1]);
	string image1_name(argv[2]);
	string image2_name(argv[3]);

	// Load database
	EmdDB db;	
	result = db.load(db_path + DIR_SEP + EMDDB_INDEX);
	if (result)
	{
		cout << "Error loading database: " << db_path << ". Please check the path is entered correctly." << endl;
		exit(1);
	}

	// Get signatures
	entry_t entry1, entry2;
	int e1found = 0, e2found = 0;
	for (int i=0; i < db.numEntries; i++)
	{
		entry_t entry = db.getEntry(i);
		if (e1found == 0)
		{
			if (entry.filename.compare(image1_name) == 0)
			{
				e1found = 1;
				entry1 = entry;
			}
		}
		if (e2found == 0)
		{
			if (entry.filename.compare(image2_name) == 0)
			{
				e2found = 1;
				entry2 = entry;
			}
		}
		if (e2found && e1found)
			break;
	}
	if (!e1found)
	{
		cout << "Unable to find entry for " << image1_name << " in database" << endl;
	}
	if (!e2found)
	{
		cout << "Unable to find entry for " << image2_name << " in database" << endl;
	}
	if (!(e2found && e1found))
		exit(1);

	// normalize by centroid
	double s1_cx, s1_cy, s2_cx, s2_cy;
	get_centroid(entry1.signature, s1_cx, s1_cy);
	get_centroid(entry2.signature, s2_cx, s2_cy);
	normalize_by_centroid(entry1.signature);
	normalize_by_centroid(entry2.signature);


	// Perform EMD		
	int flowSize;
	flow_t * flow = new flow_t[entry1.signature.n * entry2.signature.n];
	float emd_dist = emd(&entry1.signature, &entry2.signature, euclid_dist2, flow, &flowSize);

	// Output text results
	cout << "e=" << emd_dist << endl;
	cout << "flowsize=" << flowSize << endl;
	for (int i=0; i< flowSize; i++)
	{
		cout << flow[i].from << "\t" << flow[i].to << "\t" << flow[i].amount << endl;
	}

	// Visualize results
	// we need to know how big the images are
	
	string impath1 = db_path + DIR_SEP + image1_name;
	string impath2 = db_path + DIR_SEP + image2_name;
	Mat pic1 = cvLoadImage(impath1.c_str());
	Mat pic2 = cvLoadImage(impath2.c_str());
	/*int rows = (int)max(pic1.size().height, pic2.size().height);
	int cols = (int)max(pic1.size().width, pic2.size().width);;*/
	int rows = (int)(pic1.size().height + pic2.size().height);
	int cols = (int)(pic1.size().width + pic2.size().width);
	Mat im(rows, cols, CV_8UC3);



	rectangle(im, Point2d(0, 0), Point2d(cols, rows), Scalar(255,255,255), CV_FILLED); // background

	Scalar color1(50, 190, 250);
	Scalar color2(250, 190, 50);

	// Draw flows

	//double FLOW_SCALE = 0.05;
	float max_weight = max( max_val(entry1.signature.Weights, entry1.signature.n),
							max_val(entry2.signature.Weights, entry2.signature.n));
	double FLOW_SCALE = (min(rows,cols) / 40.0)/(255.0*(max_weight/255.0)); // normalize for image size and max weight
	Scalar flowColor(150,150,150);
	for (int i=0; i< flowSize; i++)
	{
		flow_t f = flow[i];
		feature_t f1 = entry1.signature.Features[f.from];
		feature_t f2 = entry2.signature.Features[f.to];
		line(im,
			//Point2d(f1.X, f1.Y),
			//Point2d(f2.X, f2.Y),
			Point2d(f1.X + s1_cx + s2_cx, f1.Y + s1_cy + s2_cy),
			Point2d(f2.X + s2_cx + s1_cx, f2.Y + s2_cy + s1_cy),
			flowColor,
			(int)(1 + f.amount * FLOW_SCALE));
	}

	// Draw points

	double WEIGHT_SCALE = FLOW_SCALE;
	for (int i=0; i<entry1.signature.n; i++)
	{
		circle(im, 
			Point2d(entry1.signature.Features[i].X + s1_cx + s2_cx, entry1.signature.Features[i].Y + s1_cy + s2_cy),
			(int) 1 + entry1.signature.Weights[i] * WEIGHT_SCALE,
			color1,
			CV_FILLED);
		circle(pic1, 
			Point2d(entry1.signature.Features[i].X + s1_cx, entry1.signature.Features[i].Y + s1_cy),
			(int) 1 + entry1.signature.Weights[i] * WEIGHT_SCALE,
			color1,
			CV_FILLED);
	}
	for (int i=0; i<entry2.signature.n; i++)
	{
		circle(im, 
			Point2d(entry2.signature.Features[i].X + s2_cx + s1_cx, entry2.signature.Features[i].Y + s2_cy + s1_cy),
			(int) (1 + entry2.signature.Weights[i] * WEIGHT_SCALE),
			color2,
			CV_FILLED);
		circle(pic2, 
			Point2d(entry2.signature.Features[i].X + s2_cx, entry2.signature.Features[i].Y + s2_cy),
			(int) (1 + entry2.signature.Weights[i] * WEIGHT_SCALE),
			color2,
			CV_FILLED);
	}

	
	namedWindow("EMDVis", CV_WINDOW_AUTOSIZE);
	imshow("EMDVis", im);
	namedWindow("Image1", CV_WINDOW_AUTOSIZE);
	imshow("Image1", pic1);
	namedWindow("Image2", CV_WINDOW_AUTOSIZE);
	imshow("Image2", pic2);

	waitKey(0);
	
	return 0;
}
