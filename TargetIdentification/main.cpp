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

#include <opencv2/opencv.hpp>

#include <ctime>
#include <iostream>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "PotentialLine.h"
#include "PotentialPoint.h"
#include "Target.h"

using namespace std;
using namespace cv;

int lowThreshold = 70;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;

RNG rng(12345);

Mat process(Mat image);

int main(int argc, char **argv) {
	Mat image;

	image = imread("/home/chris/Data/PiTarget/pics_2014-03-30/lapse_0480.jpg",
			CV_LOAD_IMAGE_COLOR);   // Read the file

	cvtColor(image, image, CV_BGR2GRAY);

	if (!image.data) {
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	process(image);

	waitKey(0);                            // Wait for a keystroke in the window
	return 0;

}

Mat process(Mat image) {
	int foundCount = 0;

	Mat image_thres;
	Mat image_blured;

	//cv::threshold( image, image_thres, 128, 255, cv::THRESH_BINARY );

	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//blur( image, image, Size(3,3) );
	GaussianBlur(image, image_blured, Size(3, 3), 2, 2);

	/// Detect edges using Threshold
	//threshold is rubbish, use canny
	//threshold( image, threshold_output, 0, 255, THRESH_BINARY+THRESH_OTSU );

	//Canny( image, threshold_output, 100, 200, 3 );

	//threshold( image, image, 0, 255, THRESH_BINARY+THRESH_OTSU );
	//blur( image, image, Size(3,3) );
	Canny(image_blured, threshold_output, lowThreshold, lowThreshold * ratio,
			kernel_size);

	// Find contours
	findContours(threshold_output, contours, hierarchy, CV_RETR_LIST,
			CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());

	//Point2f foundCenter [contours.size()];

	PotentialPoint potentialPoints[contours.size()];

	int found = 0;

	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		//boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		minEnclosingCircle((Mat) contours_poly[i], center[i], radius[i]);
	}

	/// Draw polygonal contour + bonding rects + circles
	//Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
	//Mat drawing = image.clone();
	Mat drawing;
	//image.convertTo(drawing, CV_8UC3);
	//cvtColor(image,drawing,CV_GRAY2RGB);
	//cvtColor(threshold_output,drawing,CV_GRAY2RGB);
	cvtColor(image, drawing, CV_GRAY2RGB);

	/*
	 * Firstly;
	 * Find Circles in circles
	 */
	for (int i = 0; i < contours.size(); i++) {
		bool foundPair = false;
		//cout<<i<<" : "<<center[i]<<endl;

		for (int j = i; j < contours.size(); j++) {
			//cout<<"Checking "<<j<<" ("<<abs(center[j].x - center[i].x)<<","<<abs(center[j].y - center[i].y)<<")"<<endl;
			if (i != j && abs(center[j].x - center[i].x) < 2 && abs(center[j].y - center[i].y) < 2 ) {
				float doubleri = radius[i] * 2;
				float doublerj = radius[j] * 2;
				//cout<<"Possible "<<j<<"("<<abs(radius[i] - doublerj)<<","<<abs(radius[j] - doubleri)<<")"<<endl;
				if ( abs(radius[i] - doublerj) < 7 || abs(radius[j] - doubleri) < 7 ) {

					cout<<"Matching " << i << " to " << j << " radi: " << radius[i] << "," <<radius[j]<<endl;
					Scalar color1 = Scalar(0, 255, 0);
					//Scalar color2 = Scalar(255, 0, 0);
					circle(drawing, center[i], (int) radius[i], color1, 2, 8, 0);
					circle(drawing, center[j], (int) radius[j], color1, 2, 8, 0);

					if ((int) radius[i] > (int) radius[j]) {
						//potentialPoints[found] = new PotentialPoint(center[i], radius[i]);
						potentialPoints[found].setCenter(center[i]);
						potentialPoints[found].setRadius(radius[i]);
						//foundCenter[found] = center[i];
					} else {
						potentialPoints[found].setCenter(center[j]);
						potentialPoints[found].setRadius(radius[j]);
						//foundCenter[found] = center[i+1];
					}

					//i++;
					found++;
					foundPair = true;
					break;
				}
			}
		}

		if (!foundPair) {
			Scalar color = Scalar(255, 0, 0);
			//circle(drawing, center[i], (int) radius[i], color, 2, 8, 0);
		}
	}

	Target target;
	bool firstTarget = true;

	//Found 3 or more circles in circles
	if (found >= 3) {
		//Make a 2d array to store all lengths
		//float lengths [found][found];
		PotentialLine potentialLines[found][found];

		//Do the measurements between each point. some duplication here I think
		for (int k = 0; k < found; k++) {

			for (int j = k + 1; j < found; j++) {
				if (j != k) {
					potentialLines[j][k].setData(potentialPoints[j],
							potentialPoints[k]);
					potentialLines[k][j].setData(potentialPoints[k],
							potentialPoints[j]);
				}
			}
		}

		cout << "Calc done" << endl;

		for (int l = 0; l < found; l++) {
			cout << "L is " << l << endl;
			int foundM = -1;
			for (int m = 0; m < found; m++) {
				if (m != l && m != foundM) {
					float dist = potentialLines[l][m].getLength();
					float distHalf = (dist / 3) * 2;
					float distDouble = (dist / 2) * 3;
					cout << l << " to " << m << " is " << dist << "("
							<< distDouble << "," << distHalf << ")" << endl;
					//Go though the rest of the lines

					for (int n = 0; n < found; n++) {
						if (n != m && n != l) {
							float dist2 = potentialLines[l][n].getLength();
							float longSide = 0;
							bool foundPossible = false;

							PotentialPoint shortest;
							PotentialPoint longest;

							//cout<<l<<" to "<<n<< " is "<<dist2<<endl;
							if (abs(dist2 - distHalf) < 5) {
								//Found another line from this point that is half the length of me
								//cout<<"OK1"<<endl;
								longSide = dist2;
								foundPossible = true;

								longest = potentialPoints[m];
								shortest = potentialPoints[n];

								/* lCount++;*/

							} else if (abs(dist2 - distDouble) < 5) {
								//Found another line from this point that is double the length of me
								longSide = distDouble;
								foundPossible = true;

								longest = potentialPoints[n];
								shortest = potentialPoints[m];

								//cout<<"OK2"<<endl;

								 //lCount++;*/

							}

							if (foundPossible) {
								float hypotenuse = potentialLines[n][m].getLength();

								float a = potentialLines[l][n].getLength();
								float b = potentialLines[l][m].getLength();

								float cosine =  acos((a*a + b*b -hypotenuse*hypotenuse) / (2 * a * b) );
								cout<<"cosine: " << cosine<<endl;

								//cout<<"l: "<<l<<" m:"<<m<<" n:"<<n<<endl;
								//cout<<"lm:"<<lengths[l][m]<<", "<<lengths[m][l]<<endl;
								//cout<<"ln:"<<lengths[l][n]<<", "<<lengths[n][l]<<endl;
								//cout<<"mn:"<<lengths[m][n]<<", "<<lengths[n][m]<<endl;
								//cout<<"Line: "<<dist2<<" line:"<<dist<<" hyp:"<<hypotenuse<<endl;

								//cout<<"l = "<<l<<", m = "<<m<<", n ="<<n<<endl;

								//cout<<"Hyp: "<<(hypotenuse * hypotenuse)<<" dist: "<<(dist * dist)<<" dist2: "<<(dist2 * dist2)<<endl;
								//cout<<"Foundy: "<< (float) ((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ))<<endl;
								float diff = abs(cosine - M_PI/2);
								//cout<<"Diff: "<<diff<<" target diff:"<<target.getDiff()<<endl;
								if (diff < 0.1 && (firstTarget || diff < target.getDiff())) {
									//cout<<"Setting foundM to :"<<n<<endl;
									firstTarget = false;
									foundM = n;
									// cout<<"OK: "<<potentialPoints[l].getCenter()<<endl;

									Target t(potentialPoints[l], longest,
											shortest, diff);
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
									cout<<"Too big: "<<abs((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ) )<<endl;
									Scalar lineColor = Scalar( 0, 255, 255);
									line( drawing, potentialPoints[l].getCenter(), potentialPoints[m].getCenter(), lineColor);
									line( drawing, potentialPoints[n].getCenter(), potentialPoints[m].getCenter(), lineColor);
									line( drawing, potentialPoints[l].getCenter(), potentialPoints[n].getCenter(), lineColor);
								}

							}
						}
					}
				}
			}
		}
	}

	//best target
	if (!firstTarget) {
		//Have set at least one target!
		target.draw(drawing);
		//lCount++;
	}

	namedWindow("Target Identification", CV_WINDOW_NORMAL); // Create a window for display.
	imshow("Target Identification", drawing);

	return drawing;
}
