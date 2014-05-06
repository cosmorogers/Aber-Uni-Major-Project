/*
 * Target.h
 *
 *  Created on: 9 Mar 2014
 *      Author: chris
 */

#include "PotentialPoint.h"
#include <opencv2/opencv.hpp>

#ifndef TARGET_H_
#define TARGET_H_

class Target {
private:
	PotentialPoint point_a;
	PotentialPoint point_b;
	PotentialPoint point_c;
	float pythagoras_dif;
public:
	Target() {}
	Target(PotentialPoint a, PotentialPoint b, PotentialPoint c, float diff);
	virtual ~Target();
	void setData(PotentialPoint a, PotentialPoint b, PotentialPoint c, float diff);
	float getRotation();
	void process(cv::Mat drawing);

	float getDiff() {
		return pythagoras_dif;
	}
	PotentialPoint getA() {
		return point_a;
	}
	PotentialPoint getB() {
		return point_b;
	}
	PotentialPoint getC() {
		return point_c;
	}

};

#endif /* TARGET_H_ */
