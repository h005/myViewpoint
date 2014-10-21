#pragma once
#include <opencv2/opencv.hpp>

cv::Mat phase1CalculateP(int imClick, int objClick, int imCords[][2], float objCords[][3]);
void phase2ExtractParametersFromP(cv::Mat &P, int iwidth, int iheight, cv::Mat &lookat, cv::Mat &projection);