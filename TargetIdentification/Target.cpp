/*
 * Target.cpp
 *
 *  Created on: 9 Mar 2014
 *      Author: chris
 */

#include <iostream>
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Target.h"

Target::Target(PotentialPoint a, PotentialPoint b, PotentialPoint c,
		float d) {
	point_a = a;
	point_b = b;
	point_c = c;
	diff = d;
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
		float d) {
	point_a = a;
	point_b = b;
	point_c = c;
	diff = d;
	//abs((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ) )

}

void Target::process(cv::Mat drawing) {
	//std::cout<<"Drawing "<<point_a.getCenter()<<std::endl;
	cv::Scalar red = cv::Scalar(0, 0, 255);
	cv::Scalar green = cv::Scalar(0, 255, 0);
	cv::line(drawing, getA().getCenter(), getC().getCenter(), red, 3); //AC, shortest side
	cv::line(drawing, getA().getCenter(), getB().getCenter(), green, 3); //AB, longest side
	//line( drawing, potentialPoints[l].getCenter(), potentialPoints[n].getCenter(), lineColor);

	float x2 = getA().getCenter().x;
	float y2 = getB().getCenter().y;

	cv::line(drawing, getA().getCenter(), cv::Point2f(x2, y2), cv::Scalar(255, 0, 0), 3);

	float yawx = getA().getCenter().x - getB().getCenter().x;
	float yawy = getA().getCenter().y - getB().getCenter().y;

	float yawrad = atan2(yawy, yawx) + (M_PI / 2);
	float yawdeg = yawrad * 180 / M_PI;

	float centerx = (getA().getCenter().x + getB().getCenter().x) / 2;
	float centery = (getA().getCenter().y + getB().getCenter().y) / 2;

	float imageCenterX = drawing.cols / 2;
	float imageCenterY = drawing.rows / 2;

	float dx = centerx - imageCenterX;
	float dy = centery - imageCenterY;

	float bearingRad = atan2(dy, dx) + (M_PI / 2);
	float bearingDeg = bearingRad * 180 / M_PI;

	//float relativebearingrad = bearingRad - yawrad;
	//float relativebearingdeg = relativebearingrad * 180 / M_PI;



/*
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
	}*/

	float pitchMultiplier = cos(bearingRad);
	float rollMultiplier = sin(bearingRad);

	/* Simple proportional roll/pitch controller */

	//AB is 24cm on actual target
	float abDistance = sqrt( pow(yawx, 2) + pow(yawy, 2) );
	float pixelSize = 240 / abDistance;

	//max = 1900, normal = 1500, min = 1100
	int normalRC = 1500;
	int maxRC = 1900;
	int minRC = 1100;

	float gain = 0.5;
	float error = sqrt( pow( (dx * pixelSize), 2) + pow( (dy * pixelSize), 2) );
	float output = gain * error;

	int pitchRC = (int) (pitchMultiplier * output) + normalRC;
	int rollRC = (int) (rollMultiplier * output) + normalRC;

	if (pitchRC > maxRC) {
		pitchRC = maxRC;
	} else if (pitchRC < minRC) {
		pitchRC = minRC;
	}

	if (rollRC > maxRC) {
		rollRC = maxRC;
	} else if (rollRC < minRC) {
		rollRC = minRC;
	}

	/* Simple proportional yaw controller */
	float yawGain = 2.2;
	int yawRC = (int) (yawdeg * yawGain) + normalRC;

	if (yawRC > maxRC) {
		yawRC = maxRC;
	} else if (yawRC < minRC) {
		yawRC = minRC;
	}


	printf ("pitch: %f, roll: %f, yaw:%f, abs bearing: %f\n", pitchMultiplier, rollMultiplier, yawdeg, bearingDeg );
	printf ("distance: %f, pitchrc: %d, rollrc: %d, yawrc: %d\n", error, pitchRC, rollRC, yawRC );

	cv::circle(drawing, cv::Point2f(centerx, centery), 5, cv::Scalar(255, 0, 0), 3);
	cv::circle(drawing, cv::Point2f(imageCenterX, imageCenterY), 5, cv::Scalar(255, 0, 0), 3);

	std::stringstream ys, ps, rs, bs;
	ys << "yaw: " << yawdeg;
	ps << "pitch: " << pitchMultiplier;
	rs << "roll: " << rollMultiplier;
	bs << "bear: " << bearingDeg;

	cv::putText(drawing, ys.str(), cv::Point(20,50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,255,255),2);
	cv::putText(drawing, ps.str(), cv::Point(20,150), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,255,255),2);
	cv::putText(drawing, rs.str(), cv::Point(20,250), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,255,255),2);
	cv::putText(drawing, bs.str(), cv::Point(20,350), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,255,255),2);


}
