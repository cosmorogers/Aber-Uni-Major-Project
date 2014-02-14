#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
String window_name = "Capture - Face detection";

/** @function main *
int main( void )
{
    VideoCapture capture;
    Mat frame;

    //-- 2. Read the video stream
    capture.open( -1 );
    if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }

    while ( capture.read(frame) )
    {
        if( frame.empty() )
        {
            printf(" --(!) No captured frame -- Break!");
            break;
        }

        //-- 3. Apply the classifier to the frame
        detectAndDisplay( frame );

        int c = waitKey(10);
        if( (char)c == 27 ) { break; } // escape
    }
    return 0;
}

/** @function detectAndDisplay *
void detectAndDisplay( Mat frame )
{
    std::vector<Rect> faces;
    Mat frame_gray;
    Mat frame_thres;
    
    cvtColor( frame, frame_gray, CV_RGB2GRAY );

    threshold( frame_gray, frame_thres, 128, 255,THRESH_BINARY );
  
    //-- Show what you got
    imshow( window_name, frame_thres );
}
*/
void Threshold_Demo( int, void* );
Mat src, src_gray, dst;

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";

int threshold_value = 0;
int threshold_type = 3;;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;



int main( int argc, char** argv )
{
  /// Create a window to display results
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Create Trackbar to choose type of Threshold
  createTrackbar( trackbar_type,
                  window_name, &threshold_type,
                  max_type, Threshold_Demo );

  createTrackbar( trackbar_value,
                  window_name, &threshold_value,
                  max_value, Threshold_Demo );

  
  
  
  /// Call the function to initialize
  //Threshold_Demo( 0, 0 );


  VideoCapture capture;
      capture.open( -1 );
    if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }
  while ( capture.read(src) )
    {
        if( src.empty() )
        {
            printf(" --(!) No captured frame -- Break!");
            break;
        }

        //-- 3. Apply the classifier to the frame
        //detectAndDisplay( frame );
        Threshold_Demo( 0, 0 );

        int c = waitKey(10);
        if( (char)c == 27 ) { break; } // escape
    }
    return 0;

}


/**
 * @function Threshold_Demo
 */
void Threshold_Demo( int, void* )
{
  /* 0: Binary
     1: Binary Inverted
     2: Threshold Truncated
     3: Threshold to Zero
     4: Threshold to Zero Inverted
   */
  
  cvtColor( src, src_gray, CV_RGB2GRAY );

  threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );

  Mat M = dst.clone();
 /* 
  double sum=0;
  int cols = M.cols, rows = M.rows;
  if(M.isContinuous())
  {
      cols *= rows;
      rows = 1;
  }
  for(int i = 0; i < rows; i++)
  {
      const double* Mi = M.ptr<double>(i);
      for(int j = 0; j < cols; j++)
          sum += std::max(Mi[j], 0.);
  }

// cout << sum;*/
  
  imshow( window_name, M );
}