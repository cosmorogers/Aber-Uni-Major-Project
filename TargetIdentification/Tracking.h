/*
 * Tracking.h
 *
 *  Created on: 30 Apr 2014
 *      Author: chris
 */

#ifndef TRACKING_H_
#define TRACKING_H_

#include <opencv2/opencv.hpp>

class Tracking {
public:
	Tracking();
	virtual ~Tracking();
	static void process(cv::Mat image, cv::Mat& drawing);
	static bool in_circle (cv::Point2f circle, float radius, cv::Point2f point);
};

#endif /* TRACKING_H_ */
