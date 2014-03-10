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

#include "PotentialLine.h"
#include "PotentialPoint.h"
#include "Target.h"

using namespace std;
using namespace cv;

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
  //for ( int i=0; i<200; i++ ) {
  while (true) {
    cout<<"Frame grab"<<endl;
    Camera.grab();
    Camera.retrieve ( image);
    nCount++;
    int foundCount = 0;

    cv::Mat image_thres;

    //cv::threshold( image, image_thres, 128, 255, cv::THRESH_BINARY );

    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //blur( image, image, Size(3,3) );
    GaussianBlur( image, image, Size(3,3), 2, 2 );
  
    /// Detect edges using Threshold
    //threshold is rubbish, use canny
    //threshold( image, threshold_output, 0, 255, THRESH_BINARY+THRESH_OTSU );

    Canny( image, threshold_output, 100, 200, 3 );
  
    // Find contours
    findContours( threshold_output, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    // Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );
    
    //Point2f foundCenter [contours.size()];
    
    PotentialPoint potentialPoints [contours.size()];
    
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
    //cvtColor(image,drawing,CV_GRAY2RGB);
    cvtColor(threshold_output,drawing,CV_GRAY2RGB);
    
  
    /* 
    * Firstly;
    * Find Circles in circles
    */
    for( int i = 0; i< contours.size(); i++ ) {
        
      if (abs(center[i+1].x - center[i].x) < 1 && abs(center[i+1].y - center[i].y) < 1) {        
          
        /*Scalar color1 = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        Scalar color2 = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        circle( drawing, center[i], (int)radius[i], color1, 2, 8, 0 );
        circle( drawing, center[i+1], (int)radius[i+1], color2, 2, 8, 0 );*/
          
        if ((int)radius[i] > (int)radius[i+1]) {
          //potentialPoints[found] = new PotentialPoint(center[i], radius[i]);
          potentialPoints[found].setCenter(center[i]);
          potentialPoints[found].setRadius(radius[i]);
          //foundCenter[found] = center[i];
        } else {
          potentialPoints[found].setCenter(center[i+1]);
          potentialPoints[found].setRadius(radius[i+1]);
          //foundCenter[found] = center[i+1];
        }

        i++;
        found++;
      }
    }

    
    Target target;
    bool firstTarget = true;
    
    //Found 3 or more circles in circles   
    if (found >= 3) {
      //Make a 2d array to store all lengths   
      //float lengths [found][found];
      PotentialLine potentialLines [found][found];
      
      //Do the measurements between each point. some duplication here I think
      for (int k = 0; k < found; k++) {

        for (int j = k + 1; j < found; j++) {
          if (j != k) {
            /*//Two length of triangle
            float a = abs(foundCenter[j].x - foundCenter[k].x);
            float b = abs(foundCenter[j].y - foundCenter[k].y);
            //cout<<foundCenter[k]<<" "<<foundCenter[j]<<" = "<<a<<" , "<<b<<endl;
            float length = sqrt ( ( pow (a, 2.0) + pow (b, 2.0) ) );
            lengths[k][j] = length;
            lengths[j][k] = length;
            */
            
            potentialLines[j][k].setData(potentialPoints[j], potentialPoints[k]);
            potentialLines[k][j].setData(potentialPoints[k], potentialPoints[j]);

            //Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            //line( drawing, foundCenter[j], foundCenter[k], lineColor);

            //cout<<"("<<k<<","<<j<<") is "<<lengths[k][j]<<" and ("<<j<<","<<k<<") is "<<lengths[j][k]<<endl;
              
          }
        }
      }
        
      //cout<<"Calc done"<<endl;
        
      for (int l = 0; l < found; l++) {
        //cout <<"L is "<<l<<endl;
        int foundM = -1;
        for (int m = 0; m < found; m++) {
          if (m != l && m != foundM) {
            float dist = potentialLines[l][m].getLength();
            float distHalf = (dist / 3) * 2;
            float distDouble = (dist / 2) * 3;
            //cout<<l<<" to "<<m<< " is "<<dist<<"("<<distDouble<<","<<distHalf<<")"<<endl;
            //Go though the rest of the lines
              
            for (int n = 0; n < found; n++) {
              if (n != m && n != l) {
                float dist2 = potentialLines[l][n].getLength();
                float longSide = 0;
                bool foundPossible = false;
                
                PotentialPoint shortest;
                PotentialPoint longest;
                  
                //cout<<l<<" to "<<n<< " is "<<dist2<<endl;
                if ( abs(dist2 - distHalf) < 5 ) {
                  //Found another line from this point that is half the length of me
                  //cout<<"OK1"<<endl;
                  longSide = dist2;
                  foundPossible = true;
                  
                  longest = potentialPoints[m];
                  shortest = potentialPoints[n];
                  
                  /*
                  Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                  line( drawing, foundCenter[l], foundCenter[m], lineColor);
                  line( drawing, foundCenter[n], foundCenter[m], lineColor);
                  line( drawing, foundCenter[l], foundCenter[n], lineColor);

                  lCount++;*/
                    
                } else if (abs(dist2 - distDouble) < 5) {
                  //Found another line from this point that is double the length of me
                  longSide = distDouble;
                  foundPossible = true;
                  
                  longest = potentialPoints[n];
                  shortest = potentialPoints[m];
                  
                  //cout<<"OK2"<<endl;
                  /*
                  Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                  line( drawing, foundCenter[l], foundCenter[m], lineColor);
                  line( drawing, foundCenter[n], foundCenter[m], lineColor);
                  line( drawing, foundCenter[l], foundCenter[n], lineColor);
                  
                  lCount++;*/
                  
                }
                  
                if (foundPossible) {
                  float hypotenuse = potentialLines[n][m].getLength();
                  //cout<<"l: "<<l<<" m:"<<m<<" n:"<<n<<endl;
                  //cout<<"lm:"<<lengths[l][m]<<", "<<lengths[m][l]<<endl;
                  //cout<<"ln:"<<lengths[l][n]<<", "<<lengths[n][l]<<endl;
                  //cout<<"mn:"<<lengths[m][n]<<", "<<lengths[n][m]<<endl;
                  //cout<<"Line: "<<dist2<<" line:"<<dist<<" hyp:"<<hypotenuse<<endl;
                  
                  //cout<<"l = "<<l<<", m = "<<m<<", n ="<<n<<endl;
                  
                  
                  
                  //cout<<"Hyp: "<<(hypotenuse * hypotenuse)<<" dist: "<<(dist * dist)<<" dist2: "<<(dist2 * dist2)<<endl;
                  //cout<<"Foundy: "<< (float) ((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ))<<endl;
                  float diff = abs((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ) );
                 // cout<<"Diff: "<<diff<<" target diff:"<<target.getDiff()<<endl;
                  if ( diff < 500 && (firstTarget || diff < target.getDiff()) ) {
                    //cout<<"Setting foundM to :"<<n<<endl;
                    firstTarget = false;
                    foundM = n;
                   // cout<<"OK: "<<potentialPoints[l].getCenter()<<endl;
                    
                    Target t(potentialPoints[l], longest, shortest, diff);
                    target = t;
                    
                    
                    //Scalar lineColor = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                    /*Scalar lineColor = Scalar( 0, 0, 255);
                    line( drawing, potentialPoints[l].getCenter(), potentialPoints[m].getCenter(), lineColor);
                    line( drawing, potentialPoints[n].getCenter(), potentialPoints[m].getCenter(), lineColor);
                    line( drawing, potentialPoints[l].getCenter(), potentialPoints[n].getCenter(), lineColor);*/
                  
                    //cout<<"("<<l<<","<<m<<"):"<<lengths[l][m]<<"("<<l<<","<<n<<"):"<<lengths[l][n]<<"("<<m<<","<<n<<"):"<<lengths[m][n]<<endl;
                      
                    foundCount++;
                    
                  } else {
                    //Not within bounds, draw a yellow triangle for now
                    //cout<<"Too big: "<<abs((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ) )<<endl;
                    //Scalar lineColor = Scalar( 0, 255, 255);
                    //line( drawing, potentialPoints[l].getCenter(), potentialPoints[m].getCenter(), lineColor);
                    //line( drawing, potentialPoints[n].getCenter(), potentialPoints[m].getCenter(), lineColor);
                    //line( drawing, potentialPoints[l].getCenter(), potentialPoints[n].getCenter(), lineColor);
                  }
                    
                }
              }
            }
          }
        }
      }
      /*
      //print lengths 2d array
      cout<<"I have found "<<found<<" possible points"<<endl;
      cout<<"Here is the 2d array"<<endl;
      
      for (int i = 0; i < found; i++) {
        cout<<i<<endl;
        for (int j = 0; j < found; j++) {
          cout<<j<<": "<<lengths[i][j]<<" or "<<lengths[j][i]<<endl;
        }
      }
      */
    }
    
    //best target
    if (!firstTarget) {
      //Have set at least one target!
      target.draw(drawing);
      lCount++;
    }
     
       
    /* CIRCLES 
    vector<Vec3f> circles;
    GaussianBlur( image, image, Size(7,7), 2, 2 );
    threshold( image, image, 95, 255, THRESH_BINARY );
    vector<Vec3f> circles;
    // Apply the Hough Transform to find the circles
    HoughCircles( image, circles, CV_HOUGH_GRADIENT,
          1,   // accumulator resolution (size of the image / 2)
          5,  // minimum distance between two circleslengths
          200, // Canny high threshold
          50, // minimum number of votes
          0, 1000); // min and max radius

    // Draw the circles detected
  
    for( size_t i = 0; i < circles.size(); i++ ) {
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
   // cout<<"Found: "<<foundCount<<" points:"<<found<<endl;
    
    int c = 0;
    //do {
      c = cv::waitKey(10);
      if( (char)c == 27 ) { 
        break; // escape
      } else if ( (char)c == 112 ) {
        
      }
    //} while ( (char)c != 32 );
    
    if ( (char)c == 27) { break; }


  } //end while true
  
  cout<<"Stop camera..."<<endl;
  Camera.release();
    
    
  //show time statistics
  time ( &timer_end ); /* get current time; same as: timer = time(NULL)  */
  double secondsElapsed = difftime ( timer_end,timer_begin );
  cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames, "<< lCount <<" locks (" << (float)((float)lCount/(float)nCount) * 100 <<"%) : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;
}
