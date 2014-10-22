#include <iostream>
#include <stdint.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

extern "C" {
#include <vl/generic.h>
}

#include "def.h"
#include "FeatureExtraction.h"
#include "calibration.h"
#include "utils.h"

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

// 使用给定的特征，对图像进行匹配
static void constructMatchPairs(const char *queryImagePath, const char *trainImagePath, detectTypes type, vector<KeyPoint> &qMatchPoints, vector<KeyPoint> &tMatchPoints, char *windowName = NULL) {
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
	} else if (type == DETECT_USING_MSER_SIFT || type == DETECT_USING_MSER_SURF) {
		MSER mser;
		vector<vector<Point>> qContours, tContours;
		mser(qImg, qContours);
		mser(tImg, tContours);

		Mat left = imread(queryImagePath);
		for (int i = 0; i < qContours.size(); i++) {
			RotatedRect box = fitEllipse(qContours[i]);
			box.angle = (float)CV_PI / 2 - box.angle;
			ellipse(left, box, Scalar(196, 255, 255), 2);
		}

		Mat right = imread(trainImagePath);
		for (int i = 0; i < tContours.size(); i++) {
			RotatedRect box = fitEllipse(tContours[i]);
			box.angle = (float)CV_PI / 2 - box.angle;
			ellipse(right, box, Scalar(196, 255, 255), 2);
		}

		Mat img;
		combineImage(left, right, img);
		imwrite("mm.png", img);
		printf("aaa\n");
		exit(-1);
	} else {
		// 
		std::vector<singleFeature *> queryFeats;
		std::vector<singleFeature *> trainFeats;

		switch (type) {
		case DETECT_HARRIS_AFFINE:
		case DETECT_HESSIAN_AFFINE:
			generateFeats(qImg, type, queryFeats);
			generateFeats(tImg, type, trainFeats);
			break;
		case DETECT_VLFEAT_SIFT:
			generateSIFTFeats(qImg, queryFeats);
			generateSIFTFeats(tImg, trainFeats);
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
	if (windowName) {
		Mat q;
		drawKeypoints(qImg, qKeyPoints, q, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		Mat t;
		drawKeypoints(tImg, tKeyPoints, t, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		Mat h;
		combineImage(q, t, h);
		imwrite(windowName, h);
	}

	// 使用KDTree作特征的匹配
	// KdTree with 5 random trees
	cv::flann::KDTreeIndexParams indexParams(5);
	cv::flann::Index kdtree(train, indexParams);
	Mat indices;
	Mat dists;
	kdtree.knnSearch(query, indices, dists, 2, cv::flann::SearchParams(64));

	qMatchPoints.clear();
	tMatchPoints.clear();
	for (int row = 0; row < indices.rows; row++){
		assert(indices.cols == 2);
		if (dists.at<float>(row, 0) < 0.75 * dists.at<float>(row, 1)) {
			int ai = row, bi = indices.at<int>(row, 0);
			KeyPoint ka = qKeyPoints[ai], kb = tKeyPoints[bi];
			qMatchPoints.push_back(ka);
			tMatchPoints.push_back(kb);
		}
	}

	assert(qMatchPoints.size() == tMatchPoints.size());
}

int main()
{
	
	/*char *queryImg = "2q.png", *trainImg = "2t.png";*/
	char *queryImg = "1q.ppm", *trainImg = "1t.png";
	/*char *queryImg = "et000.jpg", *trainImg = "et001.jpg";*/
	Mat qImg = imread(queryImg), tImg = imread(trainImg);

	vector<KeyPoint> qKeyPoints, tKeyPoints;

	/*{
		vector<KeyPoint> qHarrisPoints, tHarrisPoints;
		constructMatchPairs(queryImg, trainImg, DETECT_HARRIS_AFFINE, qHarrisPoints, tHarrisPoints, "Harris.png");
		qKeyPoints.insert(qKeyPoints.end(), qHarrisPoints.begin(), qHarrisPoints.end());
		tKeyPoints.insert(tKeyPoints.end(), tHarrisPoints.begin(), tHarrisPoints.end());
		explore_match(qImg, tImg, qHarrisPoints, tHarrisPoints, "Harris_match.png");
	}*/

	/*{
		vector<KeyPoint> qHessianPoints, tHessianPoints;
		constructMatchPairs(queryImg, trainImg, DETECT_HESSIAN_AFFINE, qHessianPoints, tHessianPoints, "Hessian.png");
		printf("%d %d\n", qHessianPoints.size(), tHessianPoints.size());
		qKeyPoints.insert(qKeyPoints.end(), qHessianPoints.begin(), qHessianPoints.end());
		tKeyPoints.insert(tKeyPoints.end(), tHessianPoints.begin(), tHessianPoints.end());
		explore_match(qImg, tImg, qHessianPoints, tHessianPoints, "Hessian_match.png");
	}*/

	{
		vector<KeyPoint> qSIFTPoints, tSIFTPoints;
		constructMatchPairs(queryImg, trainImg, DETECT_USING_OPENCV_SIFT, qSIFTPoints, tSIFTPoints, "SIFT.png");
		qKeyPoints.insert(qKeyPoints.end(), qSIFTPoints.begin(), qSIFTPoints.end());
		tKeyPoints.insert(tKeyPoints.end(), tSIFTPoints.begin(), tSIFTPoints.end());
		explore_match(qImg, tImg, qSIFTPoints, tSIFTPoints, "SIFT_match.png");
	}

	/*{
		vector<KeyPoint> qSURFPoints, tSURFPoints;
		constructMatchPairs(queryImg, trainImg, DETECT_USING_OPENCV_SURF, qSURFPoints, tSURFPoints, "SURF.png");
		qKeyPoints.insert(qKeyPoints.end(), qSURFPoints.begin(), qSURFPoints.end());
		tKeyPoints.insert(tKeyPoints.end(), tSURFPoints.begin(), tSURFPoints.end());
		explore_match(qImg, tImg, qSURFPoints, tSURFPoints, "SURF_match.png");
	}*/

	explore_match(qImg, tImg, qKeyPoints, tKeyPoints, "all_match.png");
	printf("%d %d\n", qKeyPoints.size(), tKeyPoints.size());

	FILE *pFile = fopen("pFile.dat", "w");
	vector<Point2f> qPoints, tPoints;
	for (int i = 0; i < qKeyPoints.size(); i++) {
		Point pq = qKeyPoints[i].pt;
		Point pt = tKeyPoints[i].pt;

		/*pq.y = qImg.size().height - pq.y;
		pt.y = tImg.size().height - pt.y;*/
		qPoints.push_back(pq);
		tPoints.push_back(pt);
		fprintf(pFile, "%d %d %d %d\n", pq.x, qImg.size().height - pq.y, pt.x, tImg.size().height - pt.y);
	}
	fclose(pFile);

	Mat mask;
	Mat F = findFundamentalMat(qPoints, tPoints, CV_FM_RANSAC, 1, 0.99, mask);
	
	vector<Point> qFilteredPoints, tFilteredPoints;
	vector<KeyPoint> qFilteredKeyPoints, tFilteredKeyPoints;
	for (int i = 0; i < mask.rows; i++) {
		if (mask.at<uchar>(i, 0)) {
			qFilteredPoints.push_back(qPoints[i]);
			tFilteredPoints.push_back(tPoints[i]);
			qFilteredKeyPoints.push_back(qKeyPoints[i]);
			tFilteredKeyPoints.push_back(tKeyPoints[i]);
		}
	}
	explore_match(qImg, tImg, qFilteredKeyPoints, tFilteredKeyPoints, "Ransaced.png");
	drawEpipolarLines(F, qImg, tImg, qPoints, tPoints, -1, "epipolar.png");

	waitKey();
	destroyAllWindows();

	return 0;
}