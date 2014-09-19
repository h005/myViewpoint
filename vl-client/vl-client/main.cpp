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

using namespace cv;
using namespace std;

typedef struct _color{
	float red;
	float green;
	float blue;
} color;

color pointColor[12] = {
	{ 1.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 1.0 },
	{ 1.0, 1.0, 0.0 },
	{ 1.0, 0.0, 1.0 },
	{ 0.0, 1.0, 1.0 },
	{ 0.5, 0.0, 0.0 },
	{ 1.0, 0.6, 0.0 },
	{ 0.8, 0.6, 1.0 },
	{ 0.0, 0.6, 0.2 },
	{ 0.0, 0.6, 1.0 },
	{ 0.6, 0.0, 1.0 },
};

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

// 将匹配点在图像中显示出来
static void explore_match(const Mat &qImg, const Mat &tImg, const vector<KeyPoint> &qMatchPoints, const vector<KeyPoint> &tMatchPoints, char *fileName) {
	if (fileName) {
		assert(qMatchPoints.size() == tMatchPoints.size());

		Mat panel;
		combineImage(qImg, tImg, panel);

		for (size_t i = 0; i < qMatchPoints.size(); i++) {
			Point2i p1 = qMatchPoints[i].pt, p2 = tMatchPoints[i].pt;
			p2.x += qImg.cols;

			Scalar color(0, 255, 0);
			circle(panel, p1, 2, color, -1);
			circle(panel, p2, 2, color, -1);

			line(panel, p1, p2, color, 1, CV_AA);
		}
		imwrite(fileName, panel);
	}
}

static bool loadTXT(char* filename, vector<Point2f> &plist, int height)
{
	FILE* fp;
	fp = fopen(filename, "rb");
	if (!fp) {
		perror(filename);
		return false;
	}

	// feof很不好用，很容易出现最后一行读两次的情况
	// http://stackoverflow.com/questions/5431941/while-feof-file-is-always-wrong
	// 这样处理之后，*_im_norm.txt文件的最后一行可以为空
	Point2f p;
	int x, y;
	while (fscanf(fp, "%d %d", &x, &y) != EOF) {
		// 文件中的点坐标系在左下角，需要转化为opencv中的点坐标
		p.x = x;
		p.y = height - y;
		plist.push_back(p);
	}
	fclose(fp);
	return true;
}

static void explore_point_homograhy(const Mat &qImg, const Mat &tImg, char *qPointsFile, const Mat &H, char *fileName) {
	// 读取query图像中标定的点
	vector<Point2f> plist;
	loadTXT(qPointsFile, plist, qImg.size().height);

	// 将点在query图像中表示出来
	Mat left;
	qImg.copyTo(left);
	for (int i = 0; i < plist.size(); i++) {
		Scalar pColor(pointColor[i].blue * 255, pointColor[i].green * 255, pointColor[i].red * 255);
		circle(left, plist[i], 4, pColor, -1);
	}

	// 使用homography变换plist中的点
	vector<Point2f> mlist;
	perspectiveTransform(plist, mlist, H);

	// 将变换后的点在train图像中表示出来
	// 如果点在train图像中也能精确对应建筑物上的相应点，则表示homography效果好
	// 不过一般不存在这种情况，至多一个平面上的点精确对应
	Mat right;
	tImg.copyTo(right);
	for (int i = 0; i < mlist.size(); i++) {
		Scalar pColor(pointColor[i].blue * 255, pointColor[i].green * 255, pointColor[i].red * 255);
		circle(right, mlist[i], 4, pColor, -1);
	}

	// 将左右的图像画在一起
	Mat panel;
	combineImage(left, right, panel);
	imwrite(fileName, panel);
}

int main()
{
	char *queryImg = "luggg.png", *trainImg = "luggg1.png";
	Mat qImg = imread(queryImg), tImg = imread(trainImg);

	vector<KeyPoint> qKeyPoints, tKeyPoints;

	{
		vector<KeyPoint> qHarrisPoints, tHarrisPoints;
		constructMatchPairs(queryImg, trainImg, DETECT_HARRIS_AFFINE, qHarrisPoints, tHarrisPoints, "Harris.png");
		qKeyPoints.insert(qKeyPoints.end(), qHarrisPoints.begin(), qHarrisPoints.end());
		tKeyPoints.insert(tKeyPoints.end(), tHarrisPoints.begin(), tHarrisPoints.end());
		explore_match(qImg, tImg, qHarrisPoints, tHarrisPoints, "Harris_match.png");
	}

	{
		vector<KeyPoint> qHessianPoints, tHessianPoints;
		constructMatchPairs(queryImg, trainImg, DETECT_HESSIAN_AFFINE, qHessianPoints, tHessianPoints, "Hessian.png");
		printf("%d %d\n", qHessianPoints.size(), tHessianPoints.size());
		qKeyPoints.insert(qKeyPoints.end(), qHessianPoints.begin(), qHessianPoints.end());
		tKeyPoints.insert(tKeyPoints.end(), tHessianPoints.begin(), tHessianPoints.end());
		explore_match(qImg, tImg, qHessianPoints, tHessianPoints, "Hessian_match.png");
	}

	{
		vector<KeyPoint> qSIFTPoints, tSIFTPoints;
		constructMatchPairs(queryImg, trainImg, DETECT_USING_OPENCV_SIFT, qSIFTPoints, tSIFTPoints, "SIFT.png");
		qKeyPoints.insert(qKeyPoints.end(), qSIFTPoints.begin(), qSIFTPoints.end());
		tKeyPoints.insert(tKeyPoints.end(), tSIFTPoints.begin(), tSIFTPoints.end());
		explore_match(qImg, tImg, qSIFTPoints, tSIFTPoints, "SIFT_match.png");
	}

	{
		vector<KeyPoint> qSURFPoints, tSURFPoints;
		constructMatchPairs(queryImg, trainImg, DETECT_USING_OPENCV_SURF, qSURFPoints, tSURFPoints, "SURF.png");
		qKeyPoints.insert(qKeyPoints.end(), qSURFPoints.begin(), qSURFPoints.end());
		tKeyPoints.insert(tKeyPoints.end(), tSURFPoints.begin(), tSURFPoints.end());
		explore_match(qImg, tImg, qSURFPoints, tSURFPoints, "SURF_match.png");
	}

	explore_match(qImg, tImg, qKeyPoints, tKeyPoints, "all_match.png");
	printf("%d %d\n", qKeyPoints.size(), tKeyPoints.size());

	vector<Point2f> qPoints, tPoints;
	for (int i = 0; i < qKeyPoints.size(); i++) {
		Point pq = qKeyPoints[i].pt;
		Point pt = tKeyPoints[i].pt;

		/*pq.y = qImg.size().height - pq.y;
		pt.y = tImg.size().height - pt.y;*/
		qPoints.push_back(pq);
		tPoints.push_back(pt);
	}

	Mat mask;
	Mat H = findHomography(qPoints, tPoints, CV_RANSAC, 3, mask);

	vector<KeyPoint> qFilteredKeyPoints, tFilteredKeyPoints;
	for (int i = 0; i < mask.rows; i++) {
		if (mask.at<uchar>(i, 0)) {
			qFilteredKeyPoints.push_back(qKeyPoints[i]);
			tFilteredKeyPoints.push_back(tKeyPoints[i]);
		}
	}
	explore_match(qImg, tImg, qFilteredKeyPoints, tFilteredKeyPoints, "Ransaced.png");
	
	cout << H << endl;
	Mat output(qImg.size(), qImg.type());
	warpPerspective(qImg, output, H, output.size());
	imshow("warpPerspective", output);
	imwrite("warpPerspective.png", output);

	explore_point_homograhy(qImg, tImg, "luggg_im_norm.txt", H, "twp.png"); 

	waitKey();
	destroyAllWindows();

	return 0;
}