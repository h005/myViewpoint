#include <stdio.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "glm.h"
#include "DLT.h"
#include "custom.h"

using namespace std;

int baseline = 0, index = 0;

#define NDIM 3
static void normalize_3D(float original[][NDIM], float processed[][NDIM], size_t n, float param[NDIM + 1]) {
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
static void normalize_2D(int original[][NDIM], float processed[][NDIM], size_t n, float param[NDIM + 1]) {
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
			if (abs(result.at<float>(i, j)) < 1e-3) {
				result.at<float>(i, j) = 0;
			}
		}
	}
	return result;
}

static bool verifyModelViewMatrix(const cv::Mat &modelView) {
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
	}
	else {
		cout << "modelView is not valid:" << endl << result1 << endl << result2 << endl;
		return false;
	}
}

static cv::Mat constructProjectionMatrix(const cv::Mat &K, GLfloat n, GLfloat f, int iwidth, int iheight) {
	// Hacked from this: http://www.songho.ca/opengl/gl_projectionmatrix.html

	// 先将NDC中的z和w分量计算好，其中w = -z
	cv::Mat A = cv::Mat::zeros(4, 4, CV_32F);
	A.at<float>(0, 0) = 1;
	A.at<float>(1, 1) = 1;
	A.at<float>(2, 2) = -(f + n) / (f - n);
	A.at<float>(2, 3) = -2 * f * n / (f - n);
	A.at<float>(3, 2) = -1;

	// 使用K投影和平移x和y分量
	cv::Mat B = cv::Mat::zeros(4, 4, CV_32F);
	B.at<float>(0, 0) = K.at<float>(0, 0);
	B.at<float>(0, 1) = K.at<float>(0, 1);
	B.at<float>(0, 3) = K.at<float>(0, 2);
	B.at<float>(1, 1) = K.at<float>(1, 1);
	B.at<float>(1, 3) = K.at<float>(1, 2);
	B.at<float>(2, 2) = 1;
	B.at<float>(3, 3) = 1;

	// 将x和y分量规范化到NDC坐标系中
	cv::Mat C = cv::Mat::eye(4, 4, CV_32F);
	C.at<float>(0, 0) = 2.0 / iwidth;
	C.at<float>(1, 1) = 2.0 / iheight;
	C.at<float>(0, 3) = -1;
	C.at<float>(1, 3) = -1;

	cv::Mat result = C * B * A;
	return result;
}

static cv::Mat constructProjectionMatrixWithoutPrinciplePoint(cv::Mat &K, GLfloat n, GLfloat f, int iwidth, int iheight) {
	// Hacked from this: http://www.songho.ca/opengl/gl_projectionmatrix.html

	// 先将NDC中的z和w分量计算好，其中w = -z
	cv::Mat A = cv::Mat::zeros(4, 4, CV_32F);
	A.at<float>(0, 0) = 1;
	A.at<float>(1, 1) = 1;
	A.at<float>(2, 2) = -(f + n) / (f - n);
	A.at<float>(2, 3) = -2 * f * n / (f - n);
	A.at<float>(3, 2) = -1;

	// 使用K投影和平移x和y分量
	cv::Mat B = cv::Mat::zeros(4, 4, CV_32F);
	B.at<float>(0, 0) = K.at<float>(0, 0);
	B.at<float>(0, 1) = K.at<float>(0, 1);
	B.at<float>(1, 1) = K.at<float>(1, 1);
	B.at<float>(2, 2) = 1;
	B.at<float>(3, 3) = 1;

	// 将x和y分量规范化到NDC坐标系中
	cv::Mat C = cv::Mat::eye(4, 4, CV_32F);
	C.at<float>(0, 0) = 2.0 / iwidth;
	C.at<float>(1, 1) = 2.0 / iheight;

	cv::Mat result = C * B * A;
	return result;
}

cv::Mat phase1CalculateP(int imClick, int objClick, int imCords[][2], float objCords[][3]) {
	float *cords3d = new float[imClick * 3];
	float *cords2d = new float[imClick * 2];
	float param2d[3], param3d[4];

	// 将选定的点进行正规化，提高SVD分解的数值精度
	// 正规化使用的参数需要记录下来，以后还原使用
	normalize_2D(imCords, (float(*)[2])cords2d, imClick, param2d);
	normalize_3D(objCords, (float(*)[3])cords3d, imClick, param3d);

	/*printf("%f %f %f\n", param2d[0], param2d[1], param2d[2]);
	printf("%f %f %f %f\n", param3d[0], param3d[1], param3d[2], param3d[3]);
	for (int i = 0; i < imClick; i++) {
	printf("%f %f %f %f %f\n", cords3d[i * 3 + 0], cords3d[i * 3 + 1], cords3d[i * 3 + 2], cords2d[i * 2 + 0], cords2d[i * 2 + 1]);
	}*/

	cv::Mat M = cv::Mat::zeros(3 * imClick, 12 + imClick, CV_32F);
	for (int i = 0; i < imClick; i++) {
		for (int j = 0; j < 12; j++) {
			if (j % 4 == 3) {
				// 3D齐次坐标中的最后一维
				M.at<float>(3 * i + j / 4, j) = 1;
			}
			else {
				// 3D齐次坐标的前三维
				M.at<float>(3 * i + j / 4, j) = cords3d[i * 3 + j % 4];
			}

		}
		M.at<float>(3 * i + 0, 12 + i) = -cords2d[i * 2 + 0];
		M.at<float>(3 * i + 1, 12 + i) = -cords2d[i * 2 + 1];
		M.at<float>(3 * i + 2, 12 + i) = -1;
	}

	// 执行SVD分解
	cv::SVD thissvd(M, cv::SVD::FULL_UV);
	cv::Mat U = thissvd.u;
	cv::Mat S = thissvd.w;
	cv::Mat VT = thissvd.vt;

	// 在||v|| = 1约束下, 求出使得((Mv)T * (Mv))最小的v
	// 但是v和-v用于计算二范数是一样的，所以选择λ全部大于零的那个v
	cv::Mat v = VT.row(S.rows - 1);
	if (cv::countNonZero(v(cv::Range(0, 1), cv::Range(12, v.cols)) > 0) < (imClick / 2)) {
		v = -v;
	}

	/*std::cout << "SV: " << S.t() << std::endl;
	std::cout << "v: " << v << std::endl;
	cv::Mat z = M * v.t();
	std::cout << "LSR: " << cv::norm(z, cv::NORM_L2) << std::endl;*/

	// 教程中使用的公式是
	// λx = PX，但是里面的x和X都是正规化后的，我们需要找到真实的P'
	// λ(NL * x) = P * (NR * X) => λx = NL(-1) * P * NR * X
	// 所以 P' = NL(-1) * P * NR

	// 将用于正规化的矩阵构造出来
	cv::Mat NR = cv::Mat::eye(4, 4, CV_32F);
	NR.at<float>(0, 3) = -param3d[0];
	NR.at<float>(1, 3) = -param3d[1];
	NR.at<float>(2, 3) = -param3d[2];
	NR *= param3d[3];
	NR.at<float>(3, 3) = 1;

	cv::Mat NL = cv::Mat::eye(3, 3, CV_32F);
	NL.at<float>(0, 2) = -param2d[0];
	NL.at<float>(1, 2) = -param2d[1];
	NL *= param2d[2];
	NL.at<float>(2, 2) = 1;

	// λ(NL * x) = P * (NR * X)
	cv::Mat P = v(cv::Range(0, 1), cv::Range(0, 12)).reshape(0, 3);
	// 求出P'
	// 因为 λx = NL(-1) * P * NR * X
	// 所以 P' = NL(-1) * P * NR
	P = NL.inv() * P * NR;

	// 清理空间
	delete cords2d;
	delete cords3d;

	return P;
}

void phase2ExtractParametersFromP(cv::Mat &P, cv::Mat &modelView, cv::Mat &K) {
	// 求解旋转矩阵
	cv::Mat A = P(cv::Range::all(), cv::Range(0, 3));
	cv::Mat A1 = A.row(0).t();
	cv::Mat A2 = A.row(1).t();
	cv::Mat A3 = A.row(2).t();

	cv::Mat t3 = A3;
	float f = cv::norm(t3, cv::NORM_L2);
	cv::Mat R3 = t3 / f;

	float e = A2.dot(R3);
	cv::Mat t2 = A2 - e * R3;
	float d = cv::norm(t2, cv::NORM_L2);
	cv::Mat R2 = t2 / d;

	float c = A1.dot(R3);
	float b = A1.dot(R2);
	cv::Mat t1 = A1 - b * R2 - c * R3;
	float a = cv::norm(t1, cv::NORM_L2);
	cv::Mat R1 = t1 / a;

	modelView = cv::Mat::zeros(4, 4, CV_32F);
	modelView(cv::Range(0, 1), cv::Range(0, 3)) = R1.t();
	modelView(cv::Range(1, 2), cv::Range(0, 3)) = R2.t();
	modelView(cv::Range(2, 3), cv::Range(0, 3)) = R3.t();
	modelView.at<float>(3, 3) = 1;

	K = cv::Mat::zeros(3, 3, CV_32F);
	K.at<float>(0, 0) = a;
	K.at<float>(0, 1) = b;
	K.at<float>(0, 2) = c;
	K.at<float>(1, 1) = d;
	K.at<float>(1, 2) = e;
	K.at<float>(2, 2) = f;

	cv::Mat A4 = P(cv::Range::all(), cv::Range(3, 4));
	cv::Mat T = K.inv() * A4;
	T.copyTo(modelView(cv::Range(0, 3), cv::Range(3, 4)));
	K /= K.at<float>(2, 2);
}

void phase3GenerateLookAtAndProjection(const cv::Mat &modelView, const cv::Mat &K, int iwidth, int iheight, cv::Mat &lookat, cv::Mat &projection) {
	// 在相机坐标系下选择相机点和其方向上的一个点
	// PA = (0, 0, 0), PB = (0, 0, 1)
	cv::Mat PA = cv::Mat::zeros(4, 1, CV_32F);
	PA.at<float>(3, 0) = 1;
	cv::Mat PB = cv::Mat::zeros(4, 1, CV_32F);
	PB.at<float>(2, 0) = 1;
	PB.at<float>(3, 0) = 1;
	// 相机坐标系下，相机头部的方向（向量）
	cv::Mat UpDir = cv::Mat::zeros(4, 1, CV_32F);
	UpDir.at<float>(1, 0) = 1;

	// 求取它们在世界坐标系下的表示
	PA = modelView.inv() * PA;
	PB = modelView.inv() * PB;
	UpDir = modelView.inv() * UpDir;

	// | from.x to.x upDir.x |
	// | from.y to.y upDir.y |
	// | from.z to.z upDir.z |
	lookat = cv::Mat::zeros(3, 3, CV_32F);
	PA(cv::Range(0, 3), cv::Range::all()).copyTo(lookat.col(0));
	PB(cv::Range(0, 3), cv::Range::all()).copyTo(lookat.col(1));
	UpDir(cv::Range(0, 3), cv::Range::all()).copyTo(lookat.col(2));

	/*glm::vec3 from(PA.at<float>(0, 0), PA.at<float>(1, 0), PA.at<float>(2, 0));
	glm::vec3 to(PB.at<float>(0, 0), PB.at<float>(1, 0), PB.at<float>(2, 0));
	glm::vec3 up(UpDir.at<float>(0, 0), UpDir.at<float>(1, 0), UpDir.at<float>(2, 0));
	glm::mat4 m = glm::lookAt(from, to, up);
	cv::Mat md(4, 4, CV_32F, &m[0][0]);
	md = md.t();*/

	projection = constructProjectionMatrix(K, 0.1, 10, iwidth, iheight);
	assert(verifyModelViewMatrix(modelView));
}
