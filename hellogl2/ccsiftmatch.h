#ifndef CCSIFTMATCH_H
#define CCSIFTMATCH_H

#include "ccsift.h"

class CCSiftMatch
{
public:
    CCSiftMatch();
    CCSiftMatch(cv::Mat image1,cv::Mat image2);
    ~CCSiftMatch();

    void match();
    std::vector<cv::Point2f>& getImagePoints();
    std::vector<cv::Point2f>& getModelPoints();

private:
    CCSift *ccsift1,*ccsift2;
    // left is image
    std::vector<cv::Point2f> m_LeftInlier;
    // right is 3D model
    std::vector<cv::Point2f> m_RightInlier;
};

#endif // CCSIFTMATCH_H
