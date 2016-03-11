#include "ccsift.h"

CCSift::CCSift()
{

}


CCSift::CCSift(cv::Mat &image)
{
    // input image is RGB
    image.copyTo(this->image);
    cv::cvtColor(this->image,this->image,CV_RGB2BGR);

    cv::Ptr<cv::Feature2D> f2d = cv::xfeatures2d::SIFT::create();
    // step 1: detect the keypoints
        // f2d->detect(image,keypoints);
// use good features to track
    // http://docs.opencv.org/3.0-beta/modules/features2d/doc/common_interfaces_of_feature_detectors.html
        cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(3000,0.01,1,3,true,0.04);
        gftt->detect(image,keypoints);
// use FAST feautres to track
    //  cv::FAST(image,keypoints,20);
    // step 2: calculate descriptors (features vectors)
    f2d->compute(image,keypoints,descriptors);
}

CCSift::~CCSift()
{
    image.release();
    descriptors.release();
}

void CCSift::showSift(std::string windowName)
{
    cv::Mat output;
    cv::drawKeypoints(image,
                      keypoints,
                      output,
                      cv::Scalar::all(-1),
                      cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cv::namedWindow(windowName);
    cv::imshow(windowName,output);
    cv::waitKey(0);
}

cv::Mat &CCSift::getDescriptors(std::vector<cv::KeyPoint> keypoints)
{
    this->keypoints = keypoints;
    descriptors.release();
    cv::Ptr<cv::Feature2D> f2d = cv::xfeatures2d::SIFT::create();
    f2d->compute(image,keypoints,descriptors);
    std::cout << "getDescriptors " << descriptors.rows << " " << descriptors.cols<<std::endl;
    return descriptors;
}

cv::Mat &CCSift::getDescriptors()
{
    return descriptors;
}

cv::Mat &CCSift::getImage()
{
    return image;
}

std::vector<cv::KeyPoint> &CCSift::getKeyPoints()
{
    return keypoints;
}


