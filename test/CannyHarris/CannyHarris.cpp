#include <cv.h>
#include <highgui.h>

using namespace cv;

int main( int argc, char** argv )
{
  Mat image;
  image = imread( argv[1], 1 );

  if( argc != 2 || !image.data )
    {
      printf( "No image data \n" );
      return -1;
    }
    
  Mat edges;
  Canny(image, edges, 10.0, 30.0);

  namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
  imshow( "Display Image", edges );

  waitKey(0);

  return 0;
}
