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
