#include "lmdlt.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <levmar.h>
#include <opencv2/opencv.hpp>
#include <glm/gtx/matrix_cross_product.hpp>
#include <glm/gtx/string_cast.hpp>

static glm::mat3 RodtoR(const glm::vec3 &w)
{
    // 将向量表示的 绕轴旋转 转化成 3x3的旋转矩阵
    // ref http://en.wikipedia.org/wiki/Rodrigues'_rotation_formula#Matrix_notation
    float wLength = glm::length(w);
    glm::mat3 K = matrixCross3(w) / wLength;
    glm::mat3 R = glm::mat3(1.f) + sin(wLength) * K + (1-cos(wLength)) * K * K;
    return R;
}

// 根据相机参数计算投影后的点
static void calcProjectedPoint(double parameter[], double newP[], int LM_m, int LM_n, void *adata)
{
    std::cout << "aa" << std::endl;
    bool radialDistoration = 1;
    // 内参矩阵K
    double alpha = parameter[0], beta = parameter[1];
    double gamma = parameter[2];
    double u0 = parameter[3], v0 = parameter[4];

    //Radial distoration
    double k1 = parameter[5], k2 = parameter[6];

    // 外参矩阵
    // 绕轴旋转，是一个1x3的向量，向量方向是旋转轴，向量的长度是旋转的弧度角
    glm::mat3 R = RodtoR(glm::vec3(parameter[7], parameter[8], parameter[9]));
    // 平移向量
    glm::vec3 t = glm::vec3(parameter[10], parameter[11], parameter[12]);
    std::cout << glm::to_string(R) << std::endl;
    double *p = (double *)adata;
    int matchnum = LM_n / 2;
    for (int i = 0; i < matchnum; i++) {
        glm::vec3 point(p[3*i + 0], p[3*i + 1], p[3*i + 2]);
        glm::vec3 final = R * point + t;
        double x = final[0] / final[2];
        double y = final[1] / final[2];

        // 和内参矩阵相乘
        double u = u0 + alpha*x + gamma*y;
        double v = v0 + beta*y;

        if (radialDistoration) {
            // 考虑径向畸变
            double value = pow(x, 2.0) + pow(y, 2.0);
            newP[i*2 + 0] = u + (u - u0) * (k1 * value + k2 * pow(value, 2.0));
            newP[i*2 + 1] = v + (v - v0) * (k1 * value + k2 * pow(value, 2.0));
        } else {
            newP[i*2 + 0] = u;
            newP[i*2 + 1] = v;
        }
    }
}

void LMDLT::DLTwithPoints(int matchnum, float points2d[][2], float points3d[][3], int imgWidth, int imgHeight, const cv::Mat &initialCameraMatrix, const cv::Mat &initialExtrinsicMatrix, cv::Mat &modelView, cv::Mat &K)
{
    void (*minfn)(double *p, double *hx, int m, int n, void *adata);
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    opts[0] = LM_INIT_MU;
    opts[1] = 1E-12;
    opts[2] = 1E-12;
    opts[3] = 1E-15;
    opts[4] = LM_DIFF_DELTA;

    int LM_m = 5 + 2 + 6;
    int LM_n = 4*matchnum;
    double *para = (double *)malloc(LM_m*sizeof(double));
    double *ptx = (double *)malloc(matchnum*4*sizeof(double));
    double *inputPoints3d = (double *)malloc(matchnum*6*sizeof(double));

    for (int j = 0; j < 2*matchnum; j++) {
        int i = j % matchnum;
        inputPoints3d[i*3 + 0] = points3d[i][0];
        inputPoints3d[i*3 + 1] = points3d[i][1];
        inputPoints3d[i*3 + 2] = points3d[i][2];
        ptx[i*2+0] = points2d[i][0];
        ptx[i*2+1] = points2d[i][1];
    }

    memset(para, 0, LM_m*sizeof(double));
    cv::Mat w(3, 1, CV_32F);
    cv::Rodrigues(initialExtrinsicMatrix(cv::Range(0,3), cv::Range(0,3)), w);
    std::cout << "w :" << w << std::endl;
    cv::Mat zz(3, 3, CV_32F);
    cv::Rodrigues(w, zz);
    std::cout << zz << std::endl;
    para[0] = initialCameraMatrix.at<float>(0, 0);
    para[1] = initialCameraMatrix.at<float>(1, 1);
    para[2] = 0;
    para[3] = imgWidth / 2;
    para[4] = imgHeight / 2;
    para[5] = 0;
    para[6] = 0;
    para[7] = w.at<float>(0,0);
    para[8] = w.at<float>(1,0);
    para[9] = w.at<float>(2,0);
    para[10] = initialExtrinsicMatrix.at<float>(0,3);
    para[11] = initialExtrinsicMatrix.at<float>(1,3);
    para[12] = initialExtrinsicMatrix.at<float>(2,3);

    minfn = calcProjectedPoint;
    int iter = dlevmar_dif(calcProjectedPoint, para, ptx, LM_m, LM_n, 1000,
                           opts, info, NULL, NULL, inputPoints3d);
    printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n", info[5], info[6], info[1], info[0]);

    K = cv::Mat::zeros(3, 3, CV_32F);
    K.at<float>(0, 0) = para[0];
    K.at<float>(1, 1) = para[1];
    K.at<float>(0, 1) = para[2];
    K.at<float>(0, 2) = para[3];
    K.at<float>(1, 2) = para[4];
    K.at<float>(2, 2) = 1;

    glm::mat3 Rmatrix = RodtoR(glm::vec3(para[7], para[8], para[9]));
    modelView = cv::Mat::zeros(4, 4, CV_32F);
    cv::Mat R;
    cv::Rodrigues(cv::Mat(1, 3, CV_64F, para+7), R);
    R.copyTo(modelView(cv::Range(0,3), cv::Range(0,3)));
    (cv::Mat(3, 1, CV_64F, para+10)).copyTo(modelView(cv::Range(0,3), cv::Range(3,4)));
    modelView.at<float>(3, 3) = 1;

    std::cout << "[LMDLT]" << std::endl;
    std::cout << modelView << std::endl;
    std::cout << K << std::endl;

    free(ptx);
    free(inputPoints3d);
    free(para);
}

