#include <cv.h>
#include <highgui.h>
#include <vector>

using namespace cv;
using namespace std;

vector<Point2f> pointSet;
double pointSpacing;

void addPointsFromSegment(Point2f source, Point2f target) {
	float distX = target.x - source.x;
	float distY = target.y - source.y;
	float dist = sqrt(distX*distX + distY*distY); // Euclidean distance between the source and target points of the segment
	if (dist > pointSpacing) {
		Point2f middle = Point2f((source.x + target.x)/2.0, (source.y + target.y)/2.0);
		pointSet.push_back(middle);
		addPointsFromSegment(source, middle);
		addPointsFromSegment(middle, target);
	}
}

int main( int argc, char** argv ) {
	
	Mat src;
	src = imread( argv[1], 1 );
	if( argc != 2 || !src.data ) {
	  printf( "No image data \n" );
	  return -1;
	}
	
	// Canny parameters
	const double threshold_low = 50.0;
	const double threshold_high = 150.0; // 2 or 3 times threshold_low is recommended
	const int apertureSize = 3; // Default
	const bool L2gradient = false; // Default
	
	// HoughLinesP parameters
	const double rho = 1.0; // 1 pixel, recommended
	const double theta = CV_PI/180.0; // 1 degree, recommended

	//core params
	const int threshold = 30; // Recommended = 80
	const double minLineLength = 10.0; // Recommended = 30, Default = 0 - most important param?
	const double maxLineGap = 2.0; // Recommended = 10, Default = 0
	pointSpacing = minLineLength * 2.0; // Extra parameter used in addPointsFromSegment 
	
	// Sobel parameters
	int scale = 1;
	int delta = 0;
	
	// Create a copy of the source image in gray color space
	Mat gray_src;
	cvtColor(src, gray_src, CV_BGR2GRAY);

	// Perform Canny edge detection
	Mat edges;
	Canny(src, edges, threshold_low, threshold_high, apertureSize, L2gradient);
	
	/* Canny input alternatives
	// Option 1
	blur(gray_src, edges, Size(3,3));
	Canny(edges, edges, threshold_low, threshold_high, apertureSize, L2gradient);
	// Option 2
	Canny(gray_src, edges, threshold_low, threshold_high, apertureSize, L2gradient);
	*/
	
	// Find line segments in the outputed edges using the probabilistic Hough transform
	vector<Vec4i> lines;
	HoughLinesP(edges, lines, rho, theta, threshold, minLineLength, maxLineGap);
	
	// Draw in red the detected lines over the Canny's detected edges
	Mat bgr_edges;
	cvtColor(edges, bgr_edges, CV_GRAY2BGR);
	for (size_t i = 0; i < lines.size(); i++) {
		line(bgr_edges, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 2, 8);
	}
	
	// Create a reduced pointset from the lines
	for (size_t i = 0; i < lines.size(); i++) {
		Point2f source = Point2f(lines[i][0], lines[i][1]);
		Point2f target = Point2f(lines[i][2], lines[i][3]);
		// Add to the pointset the source and the target points of the segment
		pointSet.push_back(source);
		pointSet.push_back(target);
		// If the segment is not very small, addPointsFromSegment will add to the pointset some more points representing the segment
		addPointsFromSegment(source, target);
	}
	
	// Use Sobel operator to get X and Y partial intensity derivatives, then create matrix of gradient intensity
	Mat grad; // Gradient intensity
	int ddepth = CV_16S; // ?

	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	Sobel(gray_src, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x );

	/// Gradient Y
	Sobel(gray_src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	// Draw the source and target points of the detected segments
	Mat segmentPoints(src.rows, src.cols, bgr_edges.type());
	for (size_t i = 0; i < lines.size(); i++) {
		circle(segmentPoints, Point(lines[i][0], lines[i][1]), 1, Scalar(0,0,255), -1, 8, 0);
		circle(segmentPoints, Point(lines[i][2], lines[i][3]), 1, Scalar(0,0,255), -1, 8, 0);
	}
	
	// Draw the pointset with weights corresponding to the intensity from the gradient magnitude image
	Mat weighted_ps(src.rows, src.cols, grad.type());
	for (vector<Point2f>::iterator pointSetIter = pointSet.begin(); pointSetIter != pointSet.end(); ++pointSetIter) {
		int x = (int) (*pointSetIter).x;
		int y = (int) (*pointSetIter).y;
		Scalar pointIntensity = grad.at<uchar>(y, x);
		circle(weighted_ps, *pointSetIter, 1, pointIntensity, -1, 8, 0);
	}

	cout << pointSet.size() << endl ;

	// Display source image
	namedWindow("Source", CV_WINDOW_AUTOSIZE);
	imshow("Source", src);
	
	// Display Canny edge detection result
	namedWindow("Canny Edge Detection", CV_WINDOW_AUTOSIZE);
	imshow("Canny Edge Detection", edges);
	
	// Display the detected lines using HoughLinesP
	namedWindow("Detected Lines", CV_WINDOW_AUTOSIZE);
	imshow("Detected Lines", bgr_edges);
	
	// Display gradient intensity image
	namedWindow("Intensity Gradient Magnitude", CV_WINDOW_AUTOSIZE);
	imshow("Intensity Gradient Magnitude", grad);
	
	// Display the source and target points of the detected segments
	namedWindow("Segment Points", CV_WINDOW_AUTOSIZE);
	imshow("Segment Points", segmentPoints);
	
	// Display weighted point set image
	namedWindow("Weighted Point Set", CV_WINDOW_AUTOSIZE);
	imshow("Weighted Point Set", weighted_ps);
	

	waitKey(0);
	return 0;
}
