/*
 * Target.cpp
 *
 *  Created on: 9 Mar 2014
 *      Author: chris
 */

#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Target.h"

Target::Target(PotentialPoint a, PotentialPoint b, PotentialPoint c,
		float diff) {
	point_a = a;
	point_b = b;
	point_c = c;
	pythagoras_dif = diff;
}

Target::~Target() {
	// TODO Auto-generated destructor stub
}

/*
 * A ------- C
 * |
 * |
 * |
 * P
 * |
 * |
 * |
 * B
 *
 * We want to have P in the center of the image
 */

void Target::setData(PotentialPoint a, PotentialPoint b, PotentialPoint c,
		float diff) {
	point_a = a;
	point_b = b;
	point_c = c;
	pythagoras_dif = diff;
	//abs((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ) )

}

void Target::draw(cv::Mat drawing) {
	//std::cout<<"Drawing "<<point_a.getCenter()<<std::endl;
	cv::Scalar red = cv::Scalar(0, 0, 255);
	cv::Scalar green = cv::Scalar(0, 255, 0);
	cv::line(drawing, getA().getCenter(), getC().getCenter(), red, 3); //AC, shortest side
	cv::line(drawing, getA().getCenter(), getB().getCenter(), green, 3); //AB, longest side
	//line( drawing, potentialPoints[l].getCenter(), potentialPoints[n].getCenter(), lineColor);

	float x2 = getA().getCenter().x;
	float y2 = getB().getCenter().y;

	cv::line(drawing, getA().getCenter(), cv::Point2f(x2, y2), cv::Scalar(255, 0, 0), 3);

	float centerx = (getA().getCenter().x + getB().getCenter().x) / 2;
	float centery = (getA().getCenter().y + getB().getCenter().y) / 2;

	//std::cout<<"center: "<<centerx<<","<<centery<<std::endl;

	int imageCenterX = drawing.cols / 2;
	int imageCenterY = drawing.rows / 2;

	float dx = abs(centerx - imageCenterX);
	float dy = abs(centery - imageCenterY);

	float bearingRad = atan(dy / dx);
	float bearingDeg = bearingRad * 180 / M_PI;

	if (centerx > imageCenterX) {
		//Center is to the left
		if (centery > imageCenterY) {
			//Center is to bottom
			bearingRad += ((3 * M_PI) / 2);
			bearingDeg += 270;
		} else if (centery < imageCenterY) {
			//Center is to the top
			bearingRad = ((3 * M_PI) / 2) - bearingRad;
			bearingDeg = 270 - bearingDeg;
		}
	} else if (centerx < imageCenterX) {
		//Center is to the right
		if (centery > imageCenterY) {
			//Center is to bottom
			bearingRad = (M_PI / 2) - bearingRad;
			bearingDeg = 90 - bearingDeg;
		} else if (centery < imageCenterY) {
			//Center is to the top
			bearingRad += M_PI / 2;
			bearingDeg += 90;
		}
	}

	float pitchMultiplier = cos(bearingRad);
	float rollMultiplier = sin(bearingRad);

	std::cout << "Bearing: " << bearingDeg << " pitch:" << pitchMultiplier
			<< " roll:" << rollMultiplier << std::endl;

	cv::circle(drawing, cv::Point2f(centerx, centery), 5,
			cv::Scalar(255, 0, 0), 3);
	cv::circle(drawing, cv::Point2f(imageCenterX, imageCenterY), 5,
			cv::Scalar(255, 0, 0), 3);

}
