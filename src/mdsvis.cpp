#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <math.h>

#include <cv.h>
#include <highgui.h>

#include "db.h"

using namespace std;
using namespace cv;

#define DIR_SEP "/"
#define EMDDB_INDEX "emdindex.csv"
#define MDS_PTS_FILE "mds_2d.txt"
#define MAT_SEP_CHAR ' '

#define DEFAULT_WIDTH 5000
#define DEFAULT_HEIGHT 5000
#define DEFAULT_SCALE 0.2

typedef pair<float, float> MyPoint;
typedef vector<MyPoint> Points;

void printUsage()
{
	cout << "mdsvis: Visualize the results of MDS (2D only)" << endl;
	cout << "Usage: ./mdsvis DB_PATH [MDS_PTS_FILE [WIDTH [HEIGHT [SCALE]]]]" << endl;
	cout << "MDS_PTS_FILE assumed to be in DB dir if omitted" << endl;
}

int read_points(string filename, Points &points)
{
	ifstream f(filename);
	string line, value;

	if (f.is_open())
	{
		float p1, p2;
		while (f.good())
		{
			getline(f, line);
			if (line.size() == 0)
				continue;
			stringstream ss(line);
			getline(ss, value, MAT_SEP_CHAR);
			
			p1 = atof(value.c_str());
			getline(ss, value, MAT_SEP_CHAR);
			p2 = atof(value.c_str());

			MyPoint p(p1, p2);
			points.push_back(p);
		}

		return 0;
	}
	else
	{
		cout << "Error reading points." << endl;
		return 1;
	}
}

void findExtremes(Points &pts, float &minX, float &minY, float &maxX, float &maxY)
{
	maxX = maxY = -INFINITY;
	minX = minY = INFINITY;

	Points::iterator it;
	for ( it = pts.begin(); it != pts.end(); ++it)
	{
		if (it->first < minX)
			minX = it->first;
		if (it->first > maxX)
			maxX = it->first;
		if (it->second < minY)
			minY = it->second;
		if (it->second > maxY)
			maxY = it->second;
	}
}

int visualizeMds(EmdDB & db, string img_dir, Points & pts, int width, int height, float img_scale)
{
	float minX, minY, maxX, maxY;
	findExtremes(pts, minX, minY, maxX, maxY);
	Mat canvas(height, width, CV_8UC3);
	canvas.setTo(Scalar(255, 255, 255));

	// coordinate scaling
	float pt_width = maxX - minX;
	float pt_height = maxY - minY;

	float canvas_ratio = (float)width / (float)height;
	float pt_space_ratio = (pt_width / pt_height);
	// assume square output image
	float x_ratio = (float)width / pt_width;
	float y_ratio = (float)height / pt_height;

	for (int i=0; i<pts.size(); i++)
	{
		string img_path;
		float x_cent, y_cent;
		int x_draw, y_draw;
		int scale_width, scale_height;
		try 
		{
			MyPoint pt = pts[i];
			entry_t e = db.getEntry(i);

			
			x_cent = x_ratio * ((-minX) + pt.first);
			y_cent = y_ratio * ((-minY) + pt.second);

			img_path = img_dir + DIR_SEP + e.filename;
			Mat pic = imread(img_path.c_str());
			scale_width = (int)((float)pic.cols * img_scale);
			scale_height = (int)((float)pic.rows * img_scale);
			x_draw = x_cent - (scale_width / 2);
			x_draw = x_draw < 0 ? 0 : x_draw;
			y_draw = y_cent - (scale_height / 2);
			y_draw = y_draw < 0 ? 0 : y_draw;

			Mat resized_pic;
			resize(pic, resized_pic, Size(scale_width, scale_width));

			Rect roi( Point( x_draw, y_draw ), Size( scale_width, scale_height));
			Mat destinationROI = canvas( roi );
			resized_pic.copyTo( destinationROI );
		}
		catch (Exception e)
		{
			cout << "ERROR: " << e.what() << endl; ;
		}
	}

	// TODO better way of doing/organizing image saving
	imwrite("MDS.jpg", canvas);
	cout << "Saved MDS.jpg" << endl;
	
	return 0;
}

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		printUsage();
		exit(1);
	}

	// Parse parameters

	string db_path(argv[1]);
	string db_filename = db_path + DIR_SEP + EMDDB_INDEX;

	string pts_filename;
	if (argc > 2)
	{
		pts_filename.assign(argv[2]);
	}
	else
	{
		pts_filename = db_path + DIR_SEP + MDS_PTS_FILE;
	}

	int width = (argc > 3) ? atoi(argv[3]) : DEFAULT_WIDTH;
	int height = (argc > 4) ? atoi(argv[4]) : DEFAULT_HEIGHT;
	float img_scale = (argc > 5) ? atof(argv[5]) : DEFAULT_SCALE;

	// Start the real work

	Points pts;
	int result = read_points(pts_filename, pts);
	if (result)
	{
		cout << "Cannot open points file " << pts_filename << ". Check the path." << endl;
		exit(1);
	}

	EmdDB db;
	result = db.load(db_filename);
	if (result)
	{
		cout << "Cannot open database at " << db_filename << ". Check the path." << endl;
		exit(1);
	}

	// We assume the Points vector and the db entries have an index-wise correspondance
	
	visualizeMds(db, db_path, pts, width, height, img_scale);

	return 0;
}