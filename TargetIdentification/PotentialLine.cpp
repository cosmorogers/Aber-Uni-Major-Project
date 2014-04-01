/*
 * PotentialLine.cpp
 *
 *  Created on: 1 Apr 2014
 *      Author: chris
 */

#include "PotentialLine.h"
#include "PotentialPoint.h"

PotentialLine::PotentialLine(PotentialPoint a, PotentialPoint b) {
	PotentialLine::setData(a,b);

}

PotentialLine::~PotentialLine() {
	// TODO Auto-generated destructor stub
}

void PotentialLine::setData(PotentialPoint a, PotentialPoint b) {
  point_a = a;
  point_b = b;

  //Calculate the length
  float side_x = abs(getA().getCenter().x - getB().getCenter().x);
  float side_y = abs(getA().getCenter().y - getB().getCenter().y);

  length = sqrt ( ( pow (side_x, 2.0) + pow (side_y, 2.0) ) );
}
