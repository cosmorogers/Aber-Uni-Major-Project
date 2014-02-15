/**********************************************************
 Software developed by AVA ( Ava Group of the University of Cordoba, ava  at uco dot es) 
 Main author Rafael Munoz Salinas (rmsalinas at uco dot es)
 This software is released under BSD license as expressed below
-------------------------------------------------------------------
Copyright (c) 2013, AVA ( Ava Group University of Cordoba, ava  at uco dot es) 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   
   This product includes software developed by the Ava group of the University of Cordoba.
   
4. Neither the name of the University nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AVA ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AVA BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************/

#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;
//String window_name = "Capture - Face detection";

RNG rng(12345);

int main ( int argc,char **argv ) {
    raspicam::RaspiCam_Cv Camera;
    cv::Mat image;
 
   //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
    Camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    //Start capture

    while (true) {
        Camera.grab();
        Camera.retrieve ( image);
        
        cv::Mat image_thres;

      //cv::threshold( image, image_thres, 128, 255, cv::THRESH_BINARY );
        
          Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  blur( image, image_thres, Size(3,3) );
  
        /// Detect edges using Threshold
  threshold( image_thres, threshold_output, 100, 255, THRESH_BINARY );
  /// Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Approximate contours to polygons + get bounding rects and circles
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
  vector<Point2f>center( contours.size() );
  vector<float>radius( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       //boundRect[i] = boundingRect( Mat(contours_poly[i]) );
       minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
     }


  /// Draw polygonal contour + bonding rects + circles
  //Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  Mat drawing = image.clone();
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       //drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
       circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
     }

        
/*
 * 
 * CIRCLES
  vector<Vec3f> circles;

  /// Apply the Hough Transform to find the circles
  HoughCircles( image, circles, CV_HOUGH_GRADIENT, 1, image.rows/8, 200, 100, 0, 0 );

  /// Draw the circles detected
  
  for( size_t i = 0; i < circles.size(); i++ )
  {
     cout<<"Circle"<<endl;
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( image, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( image, center, radius, Scalar(0,0,255), 3, 8, 0 );
   }
   */
        
        cv::imshow( "ANAM", drawing );

        int c = cv::waitKey(10);
        if( (char)c == 27 ) { break; } // escape

    }
    cout<<"Stop camera..."<<endl;
    Camera.release();
}
