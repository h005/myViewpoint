#pragma once
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>

cv::Mat phase1CalculateP(int imClick, int objClick, float imCords[][2], float objCords[][3]);
void phase2ExtractParametersFromP(cv::Mat &P, cv::Mat &modelView, cv::Mat &K);
void phase3GenerateLookAtAndProjection(const cv::Mat &modelView, const cv::Mat &K, int iwidth, int iheight, cv::Mat &lookat, cv::Mat &projection);
void DLTwithPoints(int matchnum,
        float points2d[][2],
        float points3d[][3],
        int imgWidth,
        int imgHeight,
        glm::mat4 &mvMatrix,
        glm::mat4 &projMatrix);
