#ifndef CCSIFT_H
#define CCSIFT_H

#include <opencv.hpp>
#include <opencv2/xfeatures2d.hpp>

class CCSift
{
public:
    CCSift();
    CCSift(cv::Mat &image);

    ~CCSift();

    void showSift(std::string windowName);

    void match(CCSift *sift);

    cv::Mat& getDescriptors();
    cv::Mat& getImage();
    std::vector<cv::KeyPoint>& getKeyPoints();
private:
    cv::Mat image;
    cv::Mat descriptors;
    std::vector<cv::KeyPoint> keypoints;
};

#endif // CCSIFT_H
