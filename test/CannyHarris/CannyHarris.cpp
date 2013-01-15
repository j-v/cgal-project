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
  // Harris parameters
  int max_corners = 500;
  double quality_level = 0.04;
  double min_distance = 5.0;

  image = imread( argv[1], 1 );

  if( argc != 2 || !image.data )
    {
      printf( "No image data \n" );
      return -1;
    }
    
  // Perform Canny edge detection
  Mat edges;
  Canny(image, edges, threshold_low, threshold_high);

  namedWindow( "Canny Edge Detection", CV_WINDOW_AUTOSIZE );
  imshow( "Canny Edge Detection", edges );

  // Perform corner detection
  // goodFeaturesToTrack uses Harris corner detection by default
  int channels = 1;
  Mat gray_img;
  cvtColor(image, gray_img, CV_BGR2GRAY);
  vector<Point2f> corners;
  goodFeaturesToTrack(gray_img, corners, max_corners, quality_level, min_distance);

  int r = 4;
  Mat copy = image.clone();
  for( int i = 0; i < corners.size(); i++ )
     { circle( copy, corners[i], r, Scalar(255, 255, 0), 1, 8, 0 ); }

  namedWindow("Harris Corner Detection", CV_WINDOW_AUTOSIZE );
  imshow( "Harris Corner Detection", copy );

  // refered to http://stackoverflow.com/questions/5787938/how-to-find-the-intensity-gradient-in-canny-edge-detector 
  // Use Sobel operator to get X and Y partial intensity derivatives, then create matrix of gradient intensity
  Mat grad; // gradient intensity
  int ddepth = CV_16S; // ?
  int scale = 1;
  int delta = 0;

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

  waitKey(0);

  return 0;
}
