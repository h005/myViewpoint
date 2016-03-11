#ifndef CCSIFT_H
#define CCSIFT_H

#include <opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <features2d.hpp>
#include <opencv2/features2d/features2d.hpp>

class CCSift
{
public:
    CCSift();
    CCSift(cv::Mat &image);

    ~CCSift();

    void showSift(std::string windowName);

    cv::Mat& getDescriptors(std::vector<cv::KeyPoint> keypoints);
    cv::Mat& getDescriptors();
    cv::Mat& getImage();
    std::vector<cv::KeyPoint>& getKeyPoints();
private:
    cv::Mat image;
    cv::Mat descriptors;
    std::vector<cv::KeyPoint> keypoints;
};

#endif // CCSIFT_H
