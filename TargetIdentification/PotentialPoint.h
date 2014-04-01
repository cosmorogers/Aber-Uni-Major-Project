/*
 * PotentialPoint.h
 *
 *  Created on: 1 Apr 2014
 *      Author: chris
 */

#include <opencv2/opencv.hpp>

#ifndef POTENTIALPOINT_H_
#define POTENTIALPOINT_H_

class PotentialPoint {
private:
	cv::Point2f center;
	float radius;

public:
	virtual ~PotentialPoint();
    PotentialPoint() { }
    PotentialPoint(cv::Point2f c, float r);
    void setCenter(cv::Point2f c) { center = c; }
    void setRadius(float r) { radius = r; }
    cv::Point2f getCenter() { return center; }
    float getRadius() { return radius; }
};

#endif /* POTENTIALPOINT_H_ */
