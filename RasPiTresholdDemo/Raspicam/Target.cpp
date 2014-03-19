#include <iostream>

#include "Target.h"
#include "PotentialLine.h"
#include "PotentialPoint.h"

// Target constructor
Target::Target(PotentialPoint a, PotentialPoint b, PotentialPoint c, float diff) {
    point_a = a;
  point_b = b;
  point_c = c;
  pythagoras_dif = diff;
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

void Target::setData(PotentialPoint a, PotentialPoint b, PotentialPoint c, float diff) {
  point_a = a;
  point_b = b;
  point_c = c;
  pythagoras_dif = diff;
  //abs((hypotenuse * hypotenuse) - ( (dist2 * dist2) + (dist * dist) ) )
  
}  

void Target::draw(cv::Mat drawing) {
  //std::cout<<"Drawing "<<point_a.getCenter()<<std::endl;
  cv::Scalar red = cv::Scalar( 0, 0, 255);
  cv::Scalar green = cv::Scalar( 0, 255, 0);
  cv::line( drawing, getA().getCenter(), getC().getCenter(), red ); //AC, shortest side
  cv::line( drawing, getA().getCenter(), getB().getCenter(), green);//AB, longest side
  //line( drawing, potentialPoints[l].getCenter(), potentialPoints[n].getCenter(), lineColor);
  
  float x2 = getA().getCenter().x;
  float y2 = getB().getCenter().y;
  
  cv::line( drawing, getA().getCenter(), cv::Point2f(x2, y2), cv::Scalar(255,0,0) );
  
  float centerx = ( getA().getCenter().x + getB().getCenter().x ) / 2;
  float centery = ( getA().getCenter().y + getB().getCenter().y ) / 2;
  
  
  std::cout<<"center: "<<centerx<<","<<centery<<std::endl;
  
  cv:circle( drawing, cv::Point2f(centerx, centery), 10, cv::Scalar(255,0,0) );
  
}