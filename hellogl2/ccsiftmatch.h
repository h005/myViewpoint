#ifndef CCSIFTMATCH_H
#define CCSIFTMATCH_H

#include "ccsift.h"
#include <QPointF>

class CCSiftMatch
{
public:
    CCSiftMatch();
    CCSiftMatch(cv::Mat image1,cv::Mat image2);
    ~CCSiftMatch();

    void match(int first = 0);
    void reMatch();
    std::vector<cv::Point2f>& getImagePoints();
    std::vector<cv::Point2f>& getModelPoints();
    cv::Mat& getMatchImg();
    cv::Mat& getRawMatchImg();
    void setKeyPointsCCsift1(std::vector<QPointF> keypoints);

    int getImWidth1();
    int getImWidth2();
    int getImHeight1();
    int getImHeight2();

private:
    // keypoints of image1
    std::vector<cv::KeyPoint> keypointsCCsift1;
    CCSift *ccsift1,*ccsift2;
    // left is image
    std::vector<cv::Point2f> m_LeftInlier;
    // right is 3D model
    std::vector<cv::Point2f> m_RightInlier;
    // match image
    cv::Mat imMatch;
    // match raw Image
    // with out match lines
    cv::Mat rawMatch;
    int imWidth1,imHeight1;
    int imWidth2,imHeight2;
};

#endif // CCSIFTMATCH_H
