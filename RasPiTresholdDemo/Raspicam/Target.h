#include "PotentialPoint.h"
#include <opencv2/opencv.hpp>

#ifndef TARGET_H
#define TARGET_H

class Target {
  private:
    PotentialPoint point_a;
    PotentialPoint point_b;
    PotentialPoint point_c;
    float pythagoras_dif;
  
  public:
    Target() { }
    Target(PotentialPoint a, PotentialPoint b, PotentialPoint c, float diff);
    void setData(PotentialPoint a, PotentialPoint b, PotentialPoint c, float diff);
    float getRotation();
    void draw(cv::Mat drawing);
    
    float getDiff() { return pythagoras_dif; }
    PotentialPoint getA() { return point_a; }
    PotentialPoint getB() { return point_b; }
    PotentialPoint getC() { return point_c; }
    
};

#endif /* POTENTIAL_LINE_H */
