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


#include <dirent.h>

#include "Tracking.h"

using namespace std;
using namespace cv;

void processDirectory();
void processSingle();

int main(int argc, char **argv) {

	processSingle();

}


void processDirectory() {
	Mat image, drawing;

	DIR * dirp = opendir("/home/chris/Data/PiTarget/pics_2014-03-30/");
	struct dirent * dp;

	while ((dp = readdir(dirp)) != NULL) {
		string file = "/home/chris/Data/PiTarget/pics_2014-03-30/targets/";
		cout << dp->d_name <<std::endl;
		file.append(dp->d_name);
		image = imread(file, CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

		if (!image.data) {
			cout << "Could not open or find the image" << file << std::endl;
			continue;
			//return -1;
		}

		Tracking::process(image, drawing);

		string fileout = "/home/chris/Data/PiTarget/pics_2014-03-30/targets/processed/";
		fileout.append(dp->d_name);

		imwrite( fileout, drawing );

	}

   closedir(dirp);
}

void processSingle() {
	Mat image, drawing;

	image = imread("/home/chris/Data/PiTarget/pics_2014-03-30/targets/lapse_0256.jpg", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

	if (!image.data) {
		cout << "Could not open or find the image" << std::endl;
		return;
	}

	Tracking::process(image, drawing);

	namedWindow("Target Identification", CV_WINDOW_NORMAL); // Create a window for display.
	imshow("Target Identification", drawing);

	waitKey(0);                            // Wait for a keystroke in the window
}
