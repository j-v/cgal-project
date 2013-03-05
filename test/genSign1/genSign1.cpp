#include <cv.h>
#include <highgui.h>

#include <vector>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
  Mat image;
  // Canny parameters
  double threshold_low = 50.0;
  double threshold_high = 150.0;
  // MinEigenVal & Harris parameters
  int max_corners = 500;
  double quality_level = 0.04;
  double min_distance = 5.0;
  // Sobel parameters
  int scale = 1;
  int delta = 0;

  image = imread( argv[1], 1 );

  if( argc != 2 || !image.data ) {
	printf( "No image data \n" );
	return -1;
  }
  
  // Display source image
  namedWindow("Source", CV_WINDOW_AUTOSIZE);
  imshow("Source", image);
    
  // Perform Canny edge detection
  Mat edges;
  Canny(image, edges, threshold_low, threshold_high);

  namedWindow( "Canny Edge Detection", CV_WINDOW_AUTOSIZE );
  imshow( "Canny Edge Detection", edges );

  // Perform corner detection
  // goodFeaturesToTrack uses cornerMinEigenVal corner detection by default
  Mat gray_img;
  cvtColor(image, gray_img, CV_BGR2GRAY);
  vector<Point2f> corners;
  goodFeaturesToTrack(gray_img, corners, max_corners, quality_level, min_distance);

  // draw corners
  Mat copy = image.clone();
  for( int i = 0; i < corners.size(); i++ )
     { circle( copy, corners[i], 4, Scalar(255, 255, 0), 1, 8, 0 ); }

  namedWindow("MinEigenVal Corner Detection", CV_WINDOW_AUTOSIZE );
  imshow( "MinEigenVal Corner Detection", copy );
  
  //Perform corner detection again, now using Harris
  vector<Point2f> corners2;
  goodFeaturesToTrack(gray_img, corners2, max_corners, quality_level, min_distance, noArray(), 3, true);
  
  // draw corners2
  Mat copy2 = image.clone();
  for( int i = 0; i < corners2.size(); i++ )
     { circle( copy2, corners2[i], 4, Scalar(255, 255, 0), 1, 8, 0 ); }
  
  namedWindow("Harris Corner Detection", CV_WINDOW_AUTOSIZE );
  imshow( "Harris Corner Detection", copy2 );

  // refered to http://stackoverflow.com/questions/5787938/how-to-find-the-intensity-gradient-in-canny-edge-detector 
  // Use Sobel operator to get X and Y partial intensity derivatives, then create matrix of gradient intensity
  Mat grad; // gradient intensity
  int ddepth = CV_16S; // ?

  /// Generate grad_x and grad_y
  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  /// Gradient X
  //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  Sobel( gray_img, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_x, abs_grad_x );

  /// Gradient Y
  //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
  Sobel( gray_img, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_y, abs_grad_y );

  /// Total Gradient (approximate)
  addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

  // Display gradient intensity image
  namedWindow("Intensity Gradient Magnitude", CV_WINDOW_AUTOSIZE );
  imshow( "Intensity Gradient Magnitude", grad );

  // Find corners that correspond to edges
  vector<Point2f> edge_corners;
  vector<Point2f>::iterator pt_it;
  for (pt_it = corners.begin(); pt_it != corners.end(); ++pt_it)
  {
	  int x = (int)(*pt_it).x;
	  int y = (int)(*pt_it).y;
	  Scalar edge_val = edges.at<uchar>(y, x);
	  if (edge_val[0] != 0)
	  {
		  edge_corners.push_back( *pt_it);
	  }
  }

  // Draw corners which are edges, with intensity from gradient magnitude image
  Mat pt_img(image.rows, image.cols, grad.type());
  for (pt_it = edge_corners.begin(); pt_it != edge_corners.end(); ++pt_it)
  {
	  int x = (int)(*pt_it).x;
	  int y = (int)(*pt_it).y;
	  Scalar pt_intensity = grad.at<uchar>(y, x);
	  circle( pt_img, *pt_it, 1, pt_intensity, -1, 8, 0 ); 
  }

  // Display weighted point set image
  namedWindow("Weighted Point Set", CV_WINDOW_AUTOSIZE );
  imshow( "Weighted Point Set", pt_img );

  
  waitKey(0);
  return 0;
}
