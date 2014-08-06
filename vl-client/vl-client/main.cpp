#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\flann\flann.hpp>
#include <stdint.h>

extern "C" {
#include <vl/generic.h>
}

#include "def.h"
#include "FeatureExtraction.h"

using namespace cv;
using namespace std;

static void constructMatchPairs(const char *queryImagePath, const char *trainImagePath, detectTypes type, std::vector<Pair> &pairList) {
	pairList.clear();

	std::vector<singleFeature *> queryFeats;
	std::vector<singleFeature *> trainFeats;

	// 根据选定的特征类型提取两幅图片的特征
	switch (type) {
	case DETECT_HARRIS_AFFINE:
	case DETECT_HESSIAN_AFFINE:
		generateFeats(queryImagePath, type, queryFeats);
		generateFeats(trainImagePath, type, trainFeats);
		break;
	case DETECT_SIFT:
		generateSIFTFeats(queryImagePath, queryFeats);
		generateSIFTFeats(trainImagePath, trainFeats);
		break;
	default:
		assert(false);
	}

	VL_PRINT("%s with %s: %d %d\n", queryImagePath, trainImagePath, queryFeats.size(), trainFeats.size());

	// 注意和Mat(Size(.., ..), ..)的区别，它们的行列顺序不同
	Mat query(queryFeats.size(), ORIGINAL_DIM, CV_32F);
	for (int i = 0; i < query.rows; i++) {
		singleFeature *sf = queryFeats[i];
		for (int j = 0; j < query.cols; j++) {
			query.at<float>(i, j) = (float)sf->descriptor[j];
		}
	}
	Mat train(trainFeats.size(), ORIGINAL_DIM, CV_32F);
	for (int i = 0; i < train.rows; i++) {
		singleFeature *sf = trainFeats[i];
		for (int j = 0; j < train.cols; j++) {
			train.at<float>(i, j) = (float)sf->descriptor[j];
		}
	}
	printf("q: %d, %d t: %d, %d\n", query.rows, query.cols, train.rows, train.cols);

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
			Pair p;
			int ai = row;
			int bi = indices.at<int>(row, 0);
			singleFeature *fa = queryFeats[ai], *fb = trainFeats[bi];
			p.a = fa->c;
			p.b = fb->c;
			pairList.push_back(p);
		}
	}
}

static void explore_match(const Mat &qImg, const Mat &tImg, const std::vector<Pair> &pairList) {
	Mat panel(std::max<int>(qImg.rows, tImg.rows), qImg.cols + tImg.cols, qImg.type());
	panel.setTo(0);
	qImg.copyTo(panel(Rect(0, 0, qImg.cols, qImg.rows)));
	tImg.copyTo(panel(Rect(qImg.cols, 0, tImg.cols, tImg.rows)));

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
	imshow("aaa", panel);
}

int main()
{
	char *queryImg = "lugger2.ppm", *trainImg = "lugger1.jpg";
	std::vector<Pair> pairHarris;
	constructMatchPairs(queryImg, trainImg, DETECT_SIFT, pairHarris);

	Mat qImg = imread(queryImg), tImg = imread(trainImg);
	printf("%d %d\n", pairHarris.size(), qImg.type());
	explore_match(qImg, tImg, pairHarris);

	waitKey();
	destroyAllWindows();

	return 0;
}