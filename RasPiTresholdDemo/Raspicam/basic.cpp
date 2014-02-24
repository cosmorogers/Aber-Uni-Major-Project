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

#include <ctime>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;
//String window_name = "Capture - Face detection";

RNG rng(12345);

int main ( int argc,char **argv ) {
    raspicam::RaspiCam_Cv Camera;
    cv::Mat image;
 
   //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
    //640x480
    //1280x960
    Camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    //Start capture

    
    time_t timer_begin,timer_end;
    time ( &timer_begin );
    int nCount=0;
    int lCount=0;
    //for ( int i=0; i<nCount; i++ ) {
    while (true) {
      
        Camera.grab();
        Camera.retrieve ( image);
        nCount++;
        
        cv::Mat image_thres;

      //cv::threshold( image, image_thres, 128, 255, cv::THRESH_BINARY );
      
          Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  //blur( image, image, Size(3,3) );
    GaussianBlur( image, image, Size(3,3), 2, 2 );
  
        /// Detect edges using Threshold
  threshold( image, threshold_output, 100, 255, THRESH_BINARY );
  /// Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Approximate contours to polygons + get bounding rects and circles
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
  vector<Point2f>center( contours.size() );
  vector<float>radius( contours.size() );
  
  Point2f foundCenter [contours.size()];
  int found = 0;
  
  for( int i = 0; i < contours.size(); i++ ) { 
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    //boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
  }


  /// Draw polygonal contour + bonding rects + circles
  //Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  //Mat drawing = image.clone();
  Mat drawing;
  //image.convertTo(drawing, CV_8UC3);
  cvtColor(image,drawing,CV_GRAY2RGB);
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       //drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
       circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
       //if ((int)radius[i] > 10) {
         
         if (abs(center[i+1].x - center[i].x) < 1 && abs(center[i+1].y - center[i].y) < 1) {        
           
           //Scalar color1 = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
           //Scalar color2 = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
           //circle( drawing, center[i], (int)radius[i], color1, 2, 8, 0 );
           //circle( drawing, center[i+1], (int)radius[i+1], color2, 2, 8, 0 );
          
           
           
           if ((int)radius[i] > (int)radius[i+1]) {
             foundCenter[found] = center[i];
           } else {
             foundCenter[found] = center[i+1];
           }

           i++;
           found++;
           //cout<<radius[i]<<endl;
         }
       //}
     }

     cout<<"Found "<<found<<endl;
     
     Point2f target [3];
     
     if (found >= 3) {
       
       float lengths [found][found];
       
       for (int k = 0; k < found; k++) {
         //Find length between each
         
         for (int j = k + 1; j < found; j++) {
           if (j != k) {
             //Two length of triangle
             float a = abs(foundCenter[j].x - foundCenter[k].x);
             float b = abs(foundCenter[j].y - foundCenter[k].y);
             //cout<<foundCenter[k]<<" "<<foundCenter[j]<<" = "<<a<<" , "<<b<<endl;
             float length = sqrt ( ( pow (a, 2.0) + pow (b, 2.0) ) );
             lengths[k][j] = length;
             lengths[j][k] = length;
             
             Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
             line( drawing, foundCenter[j], foundCenter[k], lineColor);
             
             cout<<"("<<k<<","<<j<<") is "<<lengths[k][j]<<" and ("<<j<<","<<k<<") is "<<lengths[j][k]<<endl;
             
           }
         }
       }
       
       //cout<<"Calc done"<<endl;
       //cout<<lengths<<endl;
       
       for (int l = 0; l < found; l++) {
         for (int m = l+1; m < found; m++) {
           float dist = lengths[l][m];
           float distHalf = (dist / 3) * 2;
           float distDouble = (dist / 2) * 3;
           cout<<l<<" to "<<m<< " is "<<dist<<"("<<distDouble<<","<<distHalf<<")"<<endl;
           //Go though the rest of the lines
           for (int n = 0; n < found; n++) {
             if (n != m && n != l) {
              float dist2 = lengths[l][n];
              float longSide = 0;
              bool found = false;
              
              cout<<l<<" to "<<n<< " is "<<dist2<<endl;
              if ( abs(dist2 - distHalf) < 5 ) {
                //Found another line from this point that is half the length of me
                cout<<"OK1"<<endl;
                longSide = dist2;
                found = true;
                /*
                Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                line( drawing, foundCenter[l], foundCenter[m], lineColor);
                line( drawing, foundCenter[n], foundCenter[m], lineColor);
                line( drawing, foundCenter[l], foundCenter[n], lineColor);
                
                lCount++;*/
                
              } else if (abs(dist2 - distDouble) < 5) {
                //Found another line from this point that is double the length of me
                longSide = distDouble;
                found = true;
                cout<<"OK2"<<endl;
                /*
                Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                line( drawing, foundCenter[l], foundCenter[m], lineColor);
                line( drawing, foundCenter[n], foundCenter[m], lineColor);
                line( drawing, foundCenter[l], foundCenter[n], lineColor);
                
                lCount++;*/
                
              }
              
              if (found) {
                float hypotenuse = lengths[n][m];
                cout<<"l: "<<l<<" m:"<<m<<" n:"<<n<<endl;
                cout<<"lm:"<<lengths[l][m]<<", "<<lengths[m][l]<<endl;
                cout<<"ln:"<<lengths[l][n]<<", "<<lengths[n][l]<<endl;
                cout<<"mn:"<<lengths[m][n]<<", "<<lengths[n][m]<<endl;
                cout<<"Line: "<<dist2<<" line:"<<dist<<" hyp:"<<hypotenuse<<endl;
                cout<<"Foundy: "<< (float) ((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ))<<endl;
                if ( abs((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (distDouble * distDouble) ) ) < 3) {
                  
                  Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                  line( drawing, foundCenter[l], foundCenter[m], lineColor);
                  line( drawing, foundCenter[n], foundCenter[m], lineColor);
                  line( drawing, foundCenter[l], foundCenter[n], lineColor);
                  
                  lCount++;
                }
                
              }
              
             }
           }
         }
       }
       
       /*
       Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       line( drawing, foundCenter[0], foundCenter[1], lineColor);
       line( drawing, foundCenter[1], foundCenter[2], lineColor);
       line( drawing, foundCenter[2], foundCenter[0], lineColor);
       */
     }
     
        
/*
 * 
 * CIRCLES*
  //vector<Vec3f> circles;
GaussianBlur( image, image, Size(7,7), 2, 2 );
threshold( image, image, 95, 255, THRESH_BINARY );
    vector<Vec3f> circles;
  /// Apply the Hough Transform to find the circles
  HoughCircles( image, circles, CV_HOUGH_GRADIENT,
          1,   // accumulator resolution (size of the image / 2)
          5,  // minimum distance between two circles
          200, // Canny high threshold
          50, // minimum number of votes
          0, 1000); // min and max radius

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
    
    
    //show time statistics
    time ( &timer_end ); /* get current time; same as: timer = time(NULL)  */
    double secondsElapsed = difftime ( timer_end,timer_begin );
    cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames, "<< lCount <<" locks (" << (float)((float)lCount/(float)nCount) * 100 <<"%) : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;
}
