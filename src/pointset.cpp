#include "pointset.h"

#include <cv.h>
#include <highgui.h>
#include <vector>

int generate_signature(const std::string & imagePath, signature_t & signature)
{
	cv::Mat image;
	// Canny parameters
	double threshold_low = 50.0;
	double threshold_high = 150.0;
	// Harris parameters
	int max_corners = 500;
	double quality_level = 0.04;
	double min_distance = 5.0;
	// Sobel parameters
	int scale = 1;
	int delta = 0;

	image = cv::imread(imagePath, 1);

	if (!image.data)
	{
		printf("Unable to read image: %s \n", imagePath.c_str());
		return 1; // Fail
	}

	// Create grayscale copy
	cv::Mat gray_img;
	cv::cvtColor(image, gray_img, CV_BGR2GRAY);
	

	// Perform Canny edge detection
	cv::Mat edge_img;
	cv::Canny(image, edge_img, threshold_low, threshold_high);

	// Perform Harris corner detection (goodFeaturesToTrack usesHarris by default)
	std::vector<cv::Point2f> corners;
	goodFeaturesToTrack(gray_img, corners, max_corners, quality_level, min_distance);

	// Find X and Y partial intensity deriviatives using Sobel operator
	int ddepth = CV_16S;
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	cv::Sobel( gray_img, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs( grad_x, abs_grad_x );
	cv::Sobel( gray_img, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs( grad_y, abs_grad_y );

	// Compute total intensity gradient (approximate)
	cv::Mat grad;
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

	// Find corners that correspond to edges
	std::vector<cv::Point2f> edge_corners;
	std::vector<cv::Point2f>::iterator pt_it;
	for (pt_it = corners.begin(); pt_it != corners.end(); ++pt_it)
	{
		int x = (int)(*pt_it).x;
		int y = (int)(*pt_it).y;
		cv::Scalar edge_val = edge_img.at<uchar>(y, x);
		if (edge_val[0] != 0)
		{
			edge_corners.push_back( *pt_it);
		}
	}

	// Create signature: each "edge corner" is a point, and a point's weight is its
	// corresponding value on the intensity gradient magnitude image
	signature.n = edge_corners.size();
	signature.Features = new feature_t[signature.n];
	signature.Weights = new float[signature.n];

	int i;
	for (i=0; i < signature.n; i++)
	{
		float x = edge_corners[i].x;
		float y = edge_corners[i].y;
		cv::Scalar pt_intensity = grad.at<uchar>((int)y, (int)x);
		feature_t f = {x, y};
		signature.Features[i] = f;
		signature.Weights[i] = pt_intensity[0];
	}
	
	return 0; // Success
}

void get_centroid(signature_t & s, double & cent_x, double & cent_y)
{
	double x_sum = 0, y_sum = 0, weight_sum = 0;
	for (unsigned int i = 0; i < s.n; i++)
	{
		weight_sum += s.Weights[i];
		x_sum += (double)s.Features[i].X  * s.Weights[i];
		y_sum += (double)s.Features[i].Y  * s.Weights[i];
	}

	cent_x = x_sum / weight_sum;
	cent_y = y_sum / weight_sum;
}

void normalize_by_centroid(signature_t & s)
{
	double cx, cy;
	get_centroid(s, cx, cy);

	for (int i=0; i<s.n; i++)
	{
		s.Features[i].X -= (int)cx;
		s.Features[i].Y -= (int)cy;
	}

}