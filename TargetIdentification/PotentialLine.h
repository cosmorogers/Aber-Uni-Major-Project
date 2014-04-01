/*
 * PotentialLine.h
 *
 *  Created on: 1 Apr 2014
 *      Author: chris
 */

#include "PotentialPoint.h"
#include <opencv2/opencv.hpp>

#ifndef POTENTIALLINE_H_
#define POTENTIALLINE_H_

class PotentialLine {
private:
	PotentialPoint point_a;
	PotentialPoint point_b;
	float length;
public:
    PotentialLine() { }
    virtual ~PotentialLine();
    PotentialLine(PotentialPoint a, PotentialPoint b);
    void setData(PotentialPoint a, PotentialPoint b);

    float getLength() { return length; }
    PotentialPoint getA() { return point_a; }
    PotentialPoint getB() { return point_b; }
};

#endif /* POTENTIALLINE_H_ */
