#include <opencv2/opencv.hpp>

#ifndef POTENTIAL_POINT_H
#define POTENTIAL_POINT_H

class PotentialPoint {
  private:
    cv::Point2f center;
    float radius;
  
  public:
    PotentialPoint() { }
    PotentialPoint(cv::Point2f c, float r);
    void setCenter(cv::Point2f c) { center = c; }
    void setRadius(float r) { radius = r; }
    cv::Point2f getCenter() { return center; }
    float getRadius() { return radius; }
};

#endif /* POTENTIAL_POINT_H */