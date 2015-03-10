#pragma once
#include <opencv2/opencv.hpp>

cv::Mat phase1CalculateP(int imClick, int objClick, float imCords[][2], float objCords[][3]);
void phase2ExtractParametersFromP(cv::Mat &P, cv::Mat &modelView, cv::Mat &K);
void phase3GenerateLookAtAndProjection(const cv::Mat &modelView, const cv::Mat &K, int iwidth, int iheight, cv::Mat &lookat, cv::Mat &projection);
