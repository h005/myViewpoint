#include "utils.h"

using namespace cv;

typedef struct _color{
	float red;
	float green;
	float blue;
} color;

static color pointColor[12] = {
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

// 将两幅图像横向拼接在一起
void combineImage(const Mat &qImg, const Mat &tImg, Mat &output) {
	Mat panel(std::max<int>(qImg.rows, tImg.rows), qImg.cols + tImg.cols, qImg.type());
	panel.setTo(0);
	qImg.copyTo(panel(Rect(0, 0, qImg.cols, qImg.rows)));
	tImg.copyTo(panel(Rect(qImg.cols, 0, tImg.cols, tImg.rows)));

	output = panel;
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

// 将变换后的点在train图像中显示出来
void explore_point_homograhy(const Mat &qImg, const Mat &tImg, char *qPointsFile, const Mat &H, char *fileName) {
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

// 将匹配点在图像中显示出来
void explore_match(const Mat &qImg, const Mat &tImg, const vector<KeyPoint> &qMatchPoints, const vector<KeyPoint> &tMatchPoints, char *fileName) {
	if (fileName) {
		assert(qMatchPoints.size() == tMatchPoints.size());

		Mat panel;
		combineImage(qImg, tImg, panel);

		int n = sizeof(pointColor) / sizeof(color);

		for (size_t i = 0; i < qMatchPoints.size(); i++) {
			Point2i p1 = qMatchPoints[i].pt, p2 = tMatchPoints[i].pt;
			p2.x += qImg.cols;

			Scalar color(pointColor[i % n].blue * 255, pointColor[i % n].green * 255, pointColor[i % n].red * 255);
			circle(panel, p1, 2, color, -1);
			circle(panel, p2, 2, color, -1);

			line(panel, p1, p2, color, 1, CV_AA);
		}
		imwrite(fileName, panel);
	}
}

void convertLineEquationToPoints(double a, double b, double c, cv::Size imgSize, cv::Point &pa, cv::Point &pb) {
#define END_N 300
	if (abs(a) < abs(b)) {
		pa = cv::Point(0, -c / b);
		pb = cv::Point(imgSize.width, (-c - imgSize.width * a) / b);
	}
	else {
		pa = cv::Point(-c / a, 0);
		pb = cv::Point((-c - imgSize.height * b) / a, imgSize.height);
	}
}