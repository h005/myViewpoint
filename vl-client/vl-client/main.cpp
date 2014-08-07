#include <iostream>
#include <stdint.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/flann/flann.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

extern "C" {
#include <vl/generic.h>
}

#include "def.h"
#include "FeatureExtraction.h"

using namespace cv;
using namespace std;

static inline int convertRadianToDegree(double rad) {
	int deg = (int)(rad / M_PI * 180);

	// 对于负数和越界进行特殊处理，确保结果在[0, 360)中
	return (deg % 360 + 360) % 360;
}

// 将小明的提取的特征点转化成openCV中的KeyPoint格式
static void convertSingleFeatureToKeyPoint(const vector<singleFeature *> &feats, vector<KeyPoint> &keyPoints) {
	for (int i = 0; i < feats.size(); i++) {
		singleFeature *sf = feats[i];
		KeyPoint kp;
		kp.pt.x = sf->c.x;
		kp.pt.y = sf->c.y;
		kp.angle = convertRadianToDegree(sf->oritation);
		kp.size = (sf->scale >= 6) ? sf->scale : 6;
		keyPoints.push_back(kp);
	}
}

// 将小明的提取的特征描述符转化成openCV中的Mat格式
static void convertSingleFeatureToMat(const vector<singleFeature *> &feats, int descriptorDim, Mat &out) {
	// 将描述符中的特征整理到特征矩阵中
	// 注意和Mat(Size(.., ..), ..)的区别，它们的行列顺序不同
	Mat query = Mat::zeros(feats.size(), descriptorDim, CV_32F);
	for (int i = 0; i < query.rows; i++) {
		singleFeature *sf = feats[i];
		for (int j = 0; j < query.cols; j++) {
			query.at<float>(i, j) = (float)sf->descriptor[j];
		}
	}
	out = query;
}

// 将两幅图像横向拼接在一起
static void combineImage(const Mat &qImg, const Mat &tImg, Mat &output) {
	Mat panel(std::max<int>(qImg.rows, tImg.rows), qImg.cols + tImg.cols, qImg.type());
	panel.setTo(0);
	qImg.copyTo(panel(Rect(0, 0, qImg.cols, qImg.rows)));
	tImg.copyTo(panel(Rect(qImg.cols, 0, tImg.cols, tImg.rows)));

	output = panel;
}

// 使用给定的特征，对图像进行匹配
static void constructMatchPairs(const char *queryImagePath, const char *trainImagePath, detectTypes type, std::vector<Pair> &pairList) {
	pairList.clear();
	Mat qImg = imread(queryImagePath, 0);
	Mat tImg = imread(trainImagePath, 0);

	vector<KeyPoint> qKeyPoints;
	vector<KeyPoint> tKeyPoints;
	Mat query;
	Mat train;

	if (type == DETECT_USING_OPENCV_SIFT) {
		// 使用opencv中的SIFT
		SIFT siftDetector;
		siftDetector(qImg, cv::noArray(), qKeyPoints, query);
		siftDetector(tImg, cv::noArray(), tKeyPoints, train);
	} else if (type == DETECT_USING_OPENCV_SURF) {
		// 使用opencv中的SURF
		SURF surfDetector;
		surfDetector(qImg, cv::noArray(), qKeyPoints, query);
		surfDetector(tImg, cv::noArray(), tKeyPoints, train);
	} else {
		// 
		std::vector<singleFeature *> queryFeats;
		std::vector<singleFeature *> trainFeats;

		switch (type) {
		case DETECT_HARRIS_AFFINE:
		case DETECT_HESSIAN_AFFINE:
			generateFeats(queryImagePath, type, queryFeats);
			generateFeats(trainImagePath, type, trainFeats);
			break;
		case DETECT_VLFEAT_SIFT:
			generateSIFTFeats(queryImagePath, queryFeats);
			generateSIFTFeats(trainImagePath, trainFeats);
			break;
		default:
			assert(false);
		}

		// 转化为KeyPoint类型
		convertSingleFeatureToKeyPoint(queryFeats, qKeyPoints);
		convertSingleFeatureToKeyPoint(trainFeats, tKeyPoints);
		// 将特征描述符转化为Mat类型
		convertSingleFeatureToMat(queryFeats, ORIGINAL_DIM, query);
		convertSingleFeatureToMat(trainFeats, ORIGINAL_DIM, train);
	}

	// 在图上绘制出输出keypoints
	{
		Mat q;
		drawKeypoints(qImg, qKeyPoints, q, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		Mat t;
		drawKeypoints(tImg, tKeyPoints, t, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		Mat h;
		combineImage(q, t, h);
		imshow("keypoints", h);
	}

	// 使用KDTree作特征的匹配
	// KdTree with 5 random trees
	cv::flann::KDTreeIndexParams indexParams(5);
	cv::flann::Index kdtree(train, indexParams);
	Mat indices;
	Mat dists;
	kdtree.knnSearch(query, indices, dists, 2, cv::flann::SearchParams(64));

	for (int row = 0; row < indices.rows; row++){
		assert(indices.cols == 2);
		if (dists.at<float>(row, 0) < 0.75 * dists.at<float>(row, 1)) {
			int ai = row, bi = indices.at<int>(row, 0);
			KeyPoint ka = qKeyPoints[ai], kb = tKeyPoints[bi];

			Pair p;
			p.a.x = ka.pt.x;
			p.a.y = ka.pt.y;
			p.b.x = kb.pt.x;
			p.b.y = kb.pt.y;
			pairList.push_back(p);
		}
	}
}

// 将匹配点在图像中显示出来
static void explore_match(const Mat &qImg, const Mat &tImg, const std::vector<Pair> &pairList) {
	Mat panel;
	combineImage(qImg, tImg, panel);

	for (size_t i = 0; i < pairList.size(); i++) {
		Pair p = pairList[i];
		Point2i p1, p2;
		p1.x = (int)p.a.x;
		p1.y = (int)p.a.y;
		p2.x = (int)p.b.x + qImg.cols;
		p2.y = (int)p.b.y;

		Scalar color(0, 255, 0);
		circle(panel, p1, 2, color, -1);
		circle(panel, p2, 2, color, -1);

		line(panel, p1, p2, color, 1, CV_AA);
	}
	//imshow("aaa", panel);
}

int main()
{
	char *queryImg = "lugger2.ppm", *trainImg = "lugger1.jpg";
	std::vector<Pair> pairHarris;
	constructMatchPairs(queryImg, trainImg, DETECT_VLFEAT_SIFT, pairHarris);

	Mat qImg = imread(queryImg), tImg = imread(trainImg);
	explore_match(qImg, tImg, pairHarris);

	waitKey();
	destroyAllWindows();

	return 0;
}