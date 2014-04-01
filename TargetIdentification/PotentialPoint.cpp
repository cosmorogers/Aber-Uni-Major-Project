/*
 * PotentialPoint.cpp
 *
 *  Created on: 1 Apr 2014
 *      Author: chris
 */

#include "PotentialPoint.h"

PotentialPoint::PotentialPoint(cv::Point2f c, float r) {
	center = c;
	radius = r;
}

PotentialPoint::~PotentialPoint() {
	// TODO Auto-generated destructor stub
}

