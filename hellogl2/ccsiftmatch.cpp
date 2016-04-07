#include "ccsiftmatch.h"

CCSiftMatch::CCSiftMatch()
{
    ccsift1 = NULL;
    ccsift2 = NULL;
}

// input RGB Mat
CCSiftMatch::CCSiftMatch(cv::Mat image1, cv::Mat image2)
{
    imWidth1 = image1.cols;
    imWidth2 = image2.cols;
    imHeight1 = image1.rows;
    imHeight2 = image2.rows;
    ccsift1 = new CCSift(image1);
    ccsift2 = new CCSift(image2);
}

CCSiftMatch::~CCSiftMatch()
{
    delete ccsift1;
    delete ccsift2;
}

void CCSiftMatch::match(int first)
{
    // BF match
    cv::BFMatcher matcher;
    std::vector<cv::DMatch> matches;
    if(first == 0)
    {
        matcher.match(ccsift1->getDescriptors(),ccsift2->getDescriptors(),matches);
        std::cout << "call first time" << std::endl;
    }
    else
    {
        std::cout << "....................debug ..................." << std::endl;
        std::cout << "ccsift1 descriptors size "<< keypointsCCsift1.size() << std::endl;
        matcher.match(ccsift1->getDescriptors(keypointsCCsift1),ccsift2->getDescriptors(),matches);
        std::cout << "matches size "<< matches.size() << std::endl;
        std::cout << "call another time " <<std::endl;
    }



    // fundamental matrix match and RANSAC
    // ref http://blog.sina.com.cn/s/blog_4298002e01013w9a.html
    int ptCount = matches.size();
    std::cout << "matches size " << ptCount << std::endl;
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

    if(ptCount <8 )
        return;

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

    cv::drawMatches(ccsift1->getImage(),
                    key1,
                    ccsift2->getImage(),
                    key2,
                    m_InlierMatches,
                    imMatch);
    if(first)
    {
//        std::cout << "rematch .........." <<std::endl;
//        for(int i=0;i<key1.size();i++)
//            std::cout << key1[i].pt.x <<  " " << key1[i].pt.y << std::endl;
    }

    std::vector<cv::KeyPoint> rawKey1(0);
    std::vector<cv::KeyPoint> rawKey2(0);
    std::vector<cv::DMatch> raw_match(0);

    cv::drawMatches(ccsift1->getImage(),
                    rawKey1,
                    ccsift2->getImage(),
                    rawKey2,
                    raw_match,
                    rawMatch);
}

void CCSiftMatch::reMatch()
{
    match(1);
}

std::vector<cv::Point2f> &CCSiftMatch::getImagePoints()
{
    return m_LeftInlier;
}

std::vector<cv::Point2f> &CCSiftMatch::getModelPoints()
{
    return m_RightInlier;
}

cv::Mat& CCSiftMatch::getMatchImg()
{
    return imMatch;
}

cv::Mat &CCSiftMatch::getRawMatchImg()
{
    return rawMatch;
}

void CCSiftMatch::setKeyPointsCCsift1(std::vector<QPointF> keypoints)
{
    keypointsCCsift1.clear();
    // convert vector<QPointF> 2 vectro<cv::KeyPoint>
    for(int i=0;i<keypoints.size();i++)
        keypointsCCsift1.push_back(cv::KeyPoint(float(keypoints[i].x()),float(keypoints[i].y()),2.0));
    std::cout << "ccsiftmatch keypointsCCsift1 size "<< keypointsCCsift1.size() << std::endl;
}

int CCSiftMatch::getImWidth1()
{
    return imWidth1;
}

int CCSiftMatch::getImWidth2()
{
    return imWidth2;
}

int CCSiftMatch::getImHeight1()
{
    return imHeight1;
}

int CCSiftMatch::getImHeight2()
{
    return imHeight2;
}



