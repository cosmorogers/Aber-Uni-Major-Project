#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <iostream>
#include <stdio.h>

#include "Tracking.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv) {

	raspicam::RaspiCam_Cv Camera;

	Mat image, drawing;

	//set camera params
	Camera.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	//640x480
	//1280x960
	//2592x1944
	//1296x972
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//Open camera
	cout << "Opening Camera..." << endl;
	if (!Camera.open()) {
		cerr << "Error opening the camera" << endl;
		return -1;
	}
	//Start capture

	time_t timer_begin, timer_end;
	time(&timer_begin);

	int fCount=0;

	while (true) {
		cout << "Frame grab" << endl;
		Camera.grab();
		Camera.retrieve(image);
		fCount++;
		Tracking::process(image, drawing);

		namedWindow("Target Identification", CV_WINDOW_NORMAL); // Create a window for display.
		imshow("Target Identification", drawing);

		int c = 0;
		//do {
		c = cv::waitKey(10);
		if ((char) c == 27) {
			break; // escape
		} else if ((char) c == 112) {

		}
		//} while ( (char)c != 32 );

		if ((char) c == 27) {
			break;
		}

	} //end while true

	cout << "Stop camera..." << endl;
	Camera.release();

	//show time statistics
	time(&timer_end); /* get current time; same as: timer = time(NULL)  */
	double secondsElapsed = difftime(timer_end, timer_begin);
	cout << secondsElapsed << " seconds for " << fCount << "  frames, FPS = "
			<< (float) ((float) (fCount) / secondsElapsed) << endl;

}
