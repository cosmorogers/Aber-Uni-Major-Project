#include "PotentialPoint.h"
#include <opencv2/opencv.hpp>

#ifndef POTENTIAL_LINE_H
#define POTENTIAL_LINE_H

class PotentialLine {
  private:
    PotentialPoint point_a;
    PotentialPoint point_b;
    float length;
  
  public:
    PotentialLine() { }
    PotentialLine(PotentialPoint a, PotentialPoint b);
    void setData(PotentialPoint a, PotentialPoint b);
    
    float getLength() { return length; }
    PotentialPoint getA() { return point_a; }
    PotentialPoint getB() { return point_b; }
};

#endif /* POTENTIAL_LINE_H */
