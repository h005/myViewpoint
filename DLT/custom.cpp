#include <iostream>
#include <math.h>
#include <gl/glut.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <time.h>
#include "custom.h"

using namespace std;

void printFloatv(int mode, char *title) {
	GLfloat v[16];
	printf("%s\n", title);
	glGetFloatv(mode, v);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", v[j * 4 + i]);
		}
		printf("\n");
	}
	printf("\n");
}

#define NDIM 3
void normalize_3D(float original[][NDIM], float processed[][NDIM], size_t n, float param[NDIM + 1]) {
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			processed[i][j] = original[i][j];
		}
	}

	// 计算质心，并作移中操作
	float X[3] = { 0, 0, 0 };
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			X[j] += processed[i][j];
		}
	}
	for (int j = 0; j < NDIM; j++) {
		X[j] /= n;
	}
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			processed[i][j] -= X[j];
		}
	}

	double var = 0;
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			var += processed[i][j] * processed[i][j];
		}
	}
	var /= n;
	double s = sqrt(NDIM) / sqrt(var);
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			processed[i][j] *= s;
		}
	}

	// 前面是质心坐标0，最后是缩放比
	for (int i = 0; i < NDIM; i++) {
		param[i] = X[i];
	}
	param[NDIM] = s;
}
#undef NDIM

#define NDIM 2
void normalize_2D(int original[][NDIM], float processed[][NDIM], size_t n, float param[NDIM + 1]) {
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			processed[i][j] = original[i][j];
		}
	}

	// 计算质心，并作移中操作
	float X[3] = { 0, 0, 0 };
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			X[j] += processed[i][j];
		}
	}
	for (int j = 0; j < NDIM; j++) {
		X[j] /= n;
	}
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			processed[i][j] -= X[j];
		}
	}

	double var = 0;
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			var += processed[i][j] * processed[i][j];
		}
	}
	var /= n;
	double s = sqrt(NDIM) / sqrt(var);
	for (size_t i = 0; i < n; i++) {
		for (int j = 0; j < NDIM; j++) {
			processed[i][j] *= s;
		}
	}

	// 前面是质心坐标0，最后是缩放比
	for (int i = 0; i < NDIM; i++) {
		param[i] = X[i];
	}
	param[NDIM] = s;
}
#undef NDIM

// 将结果矩阵中数值的末尾部分去掉
static cv::Mat removeEpsilon(cv::Mat &matrix) {
	assert(matrix.type() == CV_32F);
	cv::Mat result = cv::Mat::zeros(matrix.rows, matrix.cols, CV_32F);
	for (int i = 0; i < result.cols; i++) {
		for (int j = 0; j < result.cols; j++) {
			result.at<float>(i, j) = round(matrix.at<float>(i, j) * 1e5) / 1e5;
			if (abs(result.at<float>(i, j)) < 1e-5) {
				result.at<float>(i, j) = 0;
			}
		}
	}
	return result;
}

bool verifyModelViewMatrix(cv::Mat &modelView) {
	assert(modelView.type() == CV_32F);

	// 验证rotation是否是正交矩阵
	cv::Mat rotation = modelView(cv::Range(0, 3), cv::Range(0, 3));
	cv::Mat result1 = rotation.t() * rotation - cv::Mat::eye(3, 3, CV_32F);
	result1 = removeEpsilon(result1);

	// 选取相机坐标系下的一组单位正交向量，使用modelView矩阵的逆变换变换到模型坐标系下
	// 验证得到的三个向量是否还是单位正交
	cv::Mat aDir = cv::Mat::zeros(4, 1, CV_32F);
	aDir.at<float>(0, 0) = 1;
	cv::Mat bDir = cv::Mat::zeros(4, 1, CV_32F);
	bDir.at<float>(1, 0) = 1;
	cv::Mat cDir = cv::Mat::zeros(4, 1, CV_32F);
	cDir.at<float>(2, 0) = 1;
	aDir = modelView.inv() * aDir;
	bDir = modelView.inv() * bDir;
	cDir = modelView.inv() * cDir;

	cv::Mat MMM = cv::Mat::zeros(3, 3, CV_32F);
	aDir(cv::Range(0, 3), cv::Range::all()).copyTo(MMM.col(0));
	bDir(cv::Range(0, 3), cv::Range::all()).copyTo(MMM.col(1));
	cDir(cv::Range(0, 3), cv::Range::all()).copyTo(MMM.col(2));

	cv::Mat result2 = MMM.t() * MMM - cv::Mat::eye(3, 3, CV_32F);
	result2 = removeEpsilon(result2);

	if (cv::countNonZero(result1) + cv::countNonZero(result2) == 0) {
		return true;
	} else {
		cout << "modelView is not valid:" << endl << result1 << endl << result2 << endl;
		return false;
	}
}

bool getPointInModels(int x, int y, GLint viewport[4], GLdouble modelview[16], GLdouble projection[16],int iwidth, int iheight, float &mx, float &my, float &mz) {
	GLdouble oldx = x * 1.0 * viewport[2] / iwidth;
	GLdouble oldy = y * 1.0 * viewport[3] / iheight;
	GLdouble object_x, object_y, object_z;
	GLfloat winZ = 0;

	glReadBuffer(GL_BACK);
	glReadPixels(oldx, oldy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	if (abs(winZ - 1) < 1e-5) {
		return false;
	}

	gluUnProject((GLdouble)oldx, oldy, winZ, modelview, projection, viewport, &object_x, &object_y, &object_z);
	mx = object_x;
	my = object_y;
	mz = object_z;
	return true;
}

void randomSample(int range, int need, int result[]) {
	static bool inited = false;
	if (!inited) {
		srand(time(NULL));
		inited = true;
	}
	for (int i = 0; i < need; i++) {
		result[i] = rand() % range;
	}
}