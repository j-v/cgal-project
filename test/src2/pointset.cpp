#include "pointset.h"

#include <cv.h>
#include <highgui.h>
#include <vector>

void addPointsFromSegment(cv::Point2f source, cv::Point2f target, std::vector<cv::Point2f> & pointSet, const double & pointSpacing) {
	float distX = target.x - source.x;
	float distY = target.y - source.y;
	float dist = sqrt(distX*distX + distY*distY); // Euclidean distance between the source and target points of the segment
	if (dist > pointSpacing) {
		cv::Point2f middle = cv::Point2f((source.x + target.x)/2.0, (source.y + target.y)/2.0);
		pointSet.push_back(middle);
		addPointsFromSegment(source, middle, pointSet, pointSpacing);
		addPointsFromSegment(middle, target, pointSet, pointSpacing);
	}
}

int generate_signature(const std::string & imagePath, signature_t & signature)
{
	cv::Mat src;
	src = cv::imread(imagePath, 1);

	if (!src.data) {
		printf("Unable to read image: %s \n", imagePath.c_str());
		return 1; // Fail
	}
	
	// Canny parameters
	const double threshold_low = 50.0;
	const double threshold_high = 150.0; // 2 or 3 times threshold_low is recommended
	const int apertureSize = 3; // Default
	const bool L2gradient = false; // Default
	
	// HoughLinesP parameters
	const double rho = 1.0; // 1 pixel, recommended
	const double theta = CV_PI/180.0; // 1 degree, recommended
	const int threshold = 10; // Recommended = 80
	const double minLineLength = src.rows * src.cols / 100000.0; // Recommended = 30, Default = 0
	const double maxLineGap = 3.0; // Recommended = 10, Default = 0
	const double pointSpacing = minLineLength * 1.8; // Extra parameter used in addPointsFromSegment 
	
	// Sobel parameters
	int scale = 1;
	int delta = 0;
	
	// Create a copy of the source image in gray color space
	cv::Mat gray_src;
	cv::cvtColor(src, gray_src, CV_BGR2GRAY);

	// Perform Canny edge detection
	cv::Mat edges;
	cv::Canny(src, edges, threshold_low, threshold_high, apertureSize, L2gradient);
	
	// Find line segments in the outputed edges using the probabilistic Hough transform
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(edges, lines, rho, theta, threshold, minLineLength, maxLineGap);
	
	// Create a "reduced" pointset from the lines
	std::vector<cv::Point2f> pointSet;
	for (std::size_t i = 0; i < lines.size(); i++) {
		cv::Point2f source = cv::Point2f(lines[i][0], lines[i][1]);
		cv::Point2f target = cv::Point2f(lines[i][2], lines[i][3]);
		// Add to the pointset the source and the target points of the segment
		pointSet.push_back(source);
		pointSet.push_back(target);
		// If the segment is not very small, addPointsFromSegment will add to the pointset some more points representing the segment
		addPointsFromSegment(source, target, pointSet, pointSpacing);
	}
	
	// Find X and Y partial intensity deriviatives using Sobel operator
	int ddepth = CV_16S;
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	cv::Sobel(gray_src, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(grad_x, abs_grad_x);
	cv::Sobel(gray_src, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(grad_y, abs_grad_y);

	// Compute total intensity gradient (approximate)
	cv::Mat grad;
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	// Create signature: the features are the points in pointSet, and a point's weight is its corresponding value on the intensity gradient magnitude image
	signature.n = pointSet.size();
	signature.Features = new feature_t[signature.n];
	signature.Weights = new float[signature.n];
	
	double totalWeight = 0; // Used for normalization
	for (int i = 0; i < signature.n; i++) {
		float x = pointSet[i].x;
		float y = pointSet[i].y;
		cv::Scalar pointIntensity = grad.at<uchar>((int) y, (int) x);
		feature_t f = {x, y};
		signature.Features[i] = f;
		signature.Weights[i] = pointIntensity[0];
		totalWeight += pointIntensity[0];
	}
	
	// Normalization of the weights
	for (int i = 0; i < signature.n; i++) {
		signature.Weights[i] = (signature.Weights[i]/totalWeight)*100.0;
	}
	
	// std::cout << "Size: " << signature.n << std::endl;
	// if (signature.n > MAX_SIG_SIZE) std::cout << "ACHTUNG!!!!!!!!!!!!!!!!!!!!!!!!!!!! Generated a signature bigger than the allowed size!" << std::endl;
	// std::cout << "Total Weight (before normalization): " << totalWeight << std::endl;
	
	return 0; // Success
}
