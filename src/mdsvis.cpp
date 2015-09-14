#define NOMINMAX

#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>

#include <cv.h>
#include <highgui.h>

#include "db.h"

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

#define DIR_SEP "/"
#define EMDDB_INDEX "emdindex.csv"
#define MDS_PTS_FILE "mds_2d.txt"
#define OUTPUT_IMAGE "MDS.jpg"
#define MAT_SEP_CHAR ' '

#define DEFAULT_WIDTH 5000
#define DEFAULT_HEIGHT 5000
#define DEFAULT_SCALE 0.2

typedef pair<float, float> MyPoint;
typedef vector<MyPoint> Points;

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

void printUsage()
{
	cout << "mdsvis: Visualize the results of MDS (2D only)" << endl;
	cout << "Usage: ./mdsvis DB_PATH [-o OUTPUT_IMAGE] [-m MDS_PTS_FILE] [-w WIDTH] [-h HEIGHT] [-s SCALE]" << endl;
	cout << "MDS_PTS_FILE assumed to be in DB dir if omitted" << endl;
}

int read_points(string filename, Points &points)
{
	ifstream f(filename.c_str());
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

int visualizeMds(EmdDB & db, string img_dir, Mat & output_image, Points & pts, int width, int height, float img_scale)
{
	float minX, minY, maxX, maxY;
	findExtremes(pts, minX, minY, maxX, maxY);
	output_image.create(height, width, CV_8UC3);
	output_image.setTo(Scalar(255, 255, 255));

	// coordinate scaling
	float pt_width = maxX - minX;
	float pt_height = maxY - minY;

	float output_image_ratio = (float)width / (float)height;
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
		Mat pic;
		try 
		{
			MyPoint pt = pts[i];
			entry_t e = db.getEntry(i);

			
			x_cent = x_ratio * ((-minX) + pt.first);
			y_cent = y_ratio * ((-minY) + pt.second);

			img_path = img_dir + DIR_SEP + e.filename;
			pic = imread(img_path.c_str());
			scale_width = (int)((float)pic.cols * img_scale);
			scale_height = (int)((float)pic.rows * img_scale);
			x_draw = x_cent - (scale_width / 2);
			x_draw = x_draw < 0 ? 0 : x_draw;
			y_draw = y_cent - (scale_height / 2);
			y_draw = y_draw < 0 ? 0 : y_draw;

			// make sure the image will be drawn inside the output image, kind of a hack
			if (scale_width > width || scale_height > height)
			{
				cout << "WARNING: Scaled image will not fit inside output image. Skipping " << endl;
			}
			if (x_draw < 0)
				x_draw = 0;
			else if (x_draw + scale_width > width - 1)
				x_draw = width - scale_width - 1;
			if (y_draw < 0)
				y_draw = 0;
			else if (y_draw + scale_height > height - 1)
				y_draw = height - scale_height - 1;

			Mat resized_pic;
			resize(pic, resized_pic, Size(scale_width, scale_width));

			Rect roi( Point( x_draw, y_draw ), Size( scale_width, scale_height));
			Mat destinationROI = output_image( roi );
			resized_pic.copyTo( destinationROI );
		}
		catch (Exception e)
		{
			cout << "ERROR img_path=" << img_path << " :" << e.what() << endl; ;
		}
	}


	
	return 0;
}

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		printUsage();
		exit(1);
	}

	if (cmdOptionExists(argv, argv+argc, "--help")) {
		printUsage();
		exit(0);
	}

	

	// Parse parameters

	string db_path(argv[1]);
	string db_filename = db_path + DIR_SEP + EMDDB_INDEX;

	char * pts_filename_cstr = getCmdOption(argv, argv+argc, "-m");
	string pts_filename;
	if (!pts_filename_cstr)
	{
		pts_filename = string(db_path) + DIR_SEP + MDS_PTS_FILE;
	}
	else
	{
		pts_filename.assign(pts_filename_cstr);
	}

	char * output_filename = getCmdOption(argv, argv+argc, "-o");
	if (!output_filename)
		output_filename = OUTPUT_IMAGE;

	char * width_opt = getCmdOption(argv, argv+argc, "-w");
	char * height_opt = getCmdOption(argv, argv+argc, "-h");
	char * scale_opt = getCmdOption(argv, argv+argc, "-s");
	int width = width_opt ? atoi(width_opt) : DEFAULT_WIDTH;
	int height = height_opt ? atoi(height_opt) : DEFAULT_HEIGHT;
	float img_scale = scale_opt ? atof(scale_opt) : DEFAULT_SCALE;

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
	
	Mat output_image;
	visualizeMds(db, db_path, output_image, pts, width, height, img_scale);

	// Write image to file
	imwrite(output_filename, output_image);
	cout << "Saved " << output_filename << endl;

	return 0;
}
