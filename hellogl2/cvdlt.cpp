#include "cvdlt.h"
#include <stdio.h>
#include <iostream>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "DLT.h"

void CVDLT::DLTwithPoints(int matchnum, float points2d[][2], float points3d[][3], int imgWidth, int imgHeight, const cv::Mat &initialCameraMatrix, glm::mat4 &mvMatrix, glm::mat4 &projMatrix)
{
    std::vector<cv::Point3f> point3ds;
    std::vector<cv::Point2f> point2ds;
    for (int i = 0; i < matchnum; i++) {
        cv::Point3f p3d(points3d[i][0], points3d[i][1], points3d[i][2]);
        cv::Point2f p2d(points2d[i][0], points2d[i][1]);
        point3ds.push_back(p3d);
        point2ds.push_back(p2d);
    }

    std::vector<std::vector<cv::Point3f>> input3ds;
    std::vector<std::vector<cv::Point2f>> input2ds;
    cv::Size imageSize(imgWidth, imgHeight);
    input3ds.push_back(point3ds);
    input2ds.push_back(point2ds);

    cv::Mat cameraMatrix = initialCameraMatrix, distCoeffs;
    float f = (initialCameraMatrix.at<float>(0, 0) + initialCameraMatrix.at<float>(1, 1)) / 2;
    cameraMatrix.at<float>(0, 1) = 0;
    cameraMatrix.at<float>(0, 0) = f;
    cameraMatrix.at<float>(1, 1) = f;
    std::vector<cv::Mat> rvecs,tvecs;
    cv::calibrateCamera(input3ds, input2ds, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_USE_INTRINSIC_GUESS | CV_CALIB_FIX_ASPECT_RATIO);

    cv::Mat R;
    cv::Rodrigues(rvecs[0], R);

    cv::Mat modelView = cv::Mat::zeros(4, 4, CV_32F);
    R.copyTo(modelView(cv::Range(0,3), cv::Range(0,3)));
    tvecs[0].copyTo(modelView(cv::Range(0,3), cv::Range(3,4)));
    modelView.at<float>(3,3) = 1;

    std::cout << modelView << std::endl;

    cv::Mat proj;
    cv::Mat lookAtParams;
    phase3GenerateLookAtAndProjection(modelView, cameraMatrix, imgWidth, imgHeight, lookAtParams, proj);
    glm::vec3 eye = glm::vec3(lookAtParams.at<float>(0, 0), lookAtParams.at<float>(1, 0), lookAtParams.at<float>(2, 0));
    glm::vec3 center = glm::vec3(lookAtParams.at<float>(0, 1), lookAtParams.at<float>(1, 1), lookAtParams.at<float>(2, 1));
    glm::vec3 updir = glm::vec3(lookAtParams.at<float>(0, 2), lookAtParams.at<float>(1, 2), lookAtParams.at<float>(2, 2));
    // 在物体坐标系（世界坐标系）中摆放照相机和它的朝向
    mvMatrix = glm::lookAt(eye, center, updir);
    // 使用生成的OpenGL投影矩阵
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            projMatrix[j][i] = proj.at<float>(i, j);
        }
    }
    std::cout << "aaa" << std::endl;
}

