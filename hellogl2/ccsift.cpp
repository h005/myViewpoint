#include "ccsift.h"

CCSift::CCSift()
{

}


CCSift::CCSift(cv::Mat &image)
{
    this->image = image;

    cv::Ptr<cv::Feature2D> f2d = cv::xfeatures2d::SIFT::create();
    // step 1: detect the keypoints
    f2d->detect(image,keypoints);
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
    cv::cvtColor(output,output,CV_BGR2RGB);
    cv::namedWindow(windowName);
    cv::imshow(windowName,output);
    cv::waitKey(0);
}

void CCSift::match(CCSift *sift)
{
    // BF match
    cv::BFMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors,sift->getDescriptors(),matches);
    // fundamental matrix match and RANSAC
    int ptCount = matches.size();
    cv::Mat p1(ptCount, 2, CV_32F);
    cv::Mat p2(ptCount, 2, CV_32F);

    std::vector<cv::KeyPoint> keypoints2 = sift->getKeyPoints();
    cv::Point2f pt;
    for(int i=0;i<ptCount;i++)
    {
        pt = keypoints[matches[i].queryIdx].pt;
        p1.at<float>(i,0) = pt.x;
        p1.at<float>(i,1) = pt.y;

        pt = keypoints2[matches[i].trainIdx].pt;
        p2.at<float>(i,0) = pt.x;
        p2.at<float>(i,1) = pt.y;
    }

    std::vector<uchar> m_RANSACStatus;

    cv::Mat m_Fundamental  = cv::findFundamentalMat(p1,
                                                    p2,
                                                    m_RANSACStatus,
                                                    cv::FM_RANSAC);
    // outliers
    int OutLinerCount = 0;
    for(int i=0;i<ptCount;i++)
    {
        if(m_RANSACStatus[i]==0)
            OutLinerCount++;
    }
    // 计算内点
    std::vector<cv::Point2f> m_LeftInlier;
    std::vector<cv::Point2f> m_RightInlier;
    std::vector<cv::DMatch> m_InlierMatches;
    // 上面三个变量用于保存内点和匹配关系
    int InlinerCount = ptCount - OutLinerCount;
    m_InlierMatches.resize(InlinerCount);
    m_LeftInlier.resize(InlinerCount);
    m_RightInlier.resize(InlinerCount);
    InlinerCount = 0;
    for (int i=0;i<ptCount;i++)
    {
        if(m_RANSACStatus[i] != 0)
        {
            m_LeftInlier[InlinerCount].x = p1.at<float>(i,0);
            m_LeftInlier[InlinerCount].y = p1.at<float>(i,1);
            m_RightInlier[InlinerCount].x = p2.at<float>(i,0);
            m_RightInlier[InlinerCount].y = p2.at<float>(i,1);
            m_InlierMatches[InlinerCount].queryIdx = InlinerCount;
            m_InlierMatches[InlinerCount].trainIdx = InlinerCount;
            InlinerCount++;
        }
    }

    // 把内点转换为drawMatches可以使用的格式
    std::vector<cv::KeyPoint> key1(InlinerCount);
    std::vector<cv::KeyPoint> key2(InlinerCount);
    cv::KeyPoint::convert(m_LeftInlier,key1);
    cv::KeyPoint::convert(m_RightInlier,key2);

    cv::Mat imResult;
    cv::drawMatches(image,
                    key1,
                    sift->getImage(),
                    key2,
                    m_InlierMatches,
                    imResult);

//    cv::Mat imResult;
//    cv::drawMatches(image,
//                    keypoints,
//                    sift->getImage(),
//                    sift->getKeyPoints(),
//                    matches,
//                    imResult);
    cv::cvtColor(imResult,imResult,CV_BGR2RGB);
    cv::namedWindow("match");
    cv::imshow("match",imResult);
    cv::waitKey(0);
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


