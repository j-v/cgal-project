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

  waitKey(0);

  return 0;
}
