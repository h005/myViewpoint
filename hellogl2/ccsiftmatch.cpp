#include "ccsiftmatch.h"

CCSiftMatch::CCSiftMatch()
{
    ccsift1 = NULL;
    ccsift2 = NULL;
}

// input RGB Mat
CCSiftMatch::CCSiftMatch(cv::Mat image1, cv::Mat image2)
{
    ccsift1 = new CCSift(image1);
    ccsift2 = new CCSift(image2);
}

CCSiftMatch::~CCSiftMatch()
{
    delete ccsift1;
    delete ccsift2;
}

void CCSiftMatch::match()
{
    // BF match
    cv::BFMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(ccsift1->getDescriptors(),ccsift2->getDescriptors(),matches);
    // fundamental matrix match and RANSAC
    // ref http://blog.sina.com.cn/s/blog_4298002e01013w9a.html
    int ptCount = matches.size();
    cv::Mat p1(ptCount, 2, CV_32F);
    cv::Mat p2(ptCount, 2, CV_32F);

    std::vector<cv::KeyPoint> keypoints1 = ccsift1->getKeyPoints();
    std::vector<cv::KeyPoint> keypoints2 = ccsift2->getKeyPoints();
    cv::Point2f pt;
    for(int i=0;i<ptCount;i++)
    {
        pt = keypoints1[matches[i].queryIdx].pt;
        p1.at<float>(i,0) = pt.x;
        p1.at<float>(i,1) = pt.y;

        pt = keypoints2[matches[i].trainIdx].pt;
        p2.at<float>(i,0) = pt.x;
        p2.at<float>(i,1) = pt.y;
    }

    std::vector<uchar> m_RANSACStatus;

    cv::Mat m_Fundamental = cv::findFundamentalMat(p1,
                                                   p2,
                                                   m_RANSACStatus,
                                                   cv::FM_RANSAC);

    // outliers
    int OutLinerCount = 0;
    for(int i=0;i<ptCount;i++)
    {
        if(m_RANSACStatus[i] == 0)
            OutLinerCount++;
    }
    // 计算内点
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
    cv::drawMatches(ccsift1->getImage(),
                    key1,
                    ccsift2->getImage(),
                    key2,
                    m_InlierMatches,
                    imResult);

    cv::namedWindow("match");
    cv::imshow("match",imResult);
    cv::waitKey(0);
}

std::vector<cv::Point2f> &CCSiftMatch::getImagePoints()
{
    return m_LeftInlier;
}

std::vector<cv::Point2f> &CCSiftMatch::getModelPoints()
{
    return m_RightInlier;
}

