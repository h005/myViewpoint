#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
void convertLineEquationToPoints(double a, double b, double c, cv::Size imgSize, cv::Point &pa, cv::Point &pb);
void combineImage(const cv::Mat &qImg, const cv::Mat &tImg, cv::Mat &output);
void explore_point_homograhy(const cv::Mat &qImg, const cv::Mat &tImg, char *qPointsFile, const cv::Mat &H, char *fileName);
void explore_match(const cv::Mat &qImg, const cv::Mat &tImg, const vector<cv::KeyPoint> &qMatchPoints, const vector<cv::KeyPoint> &tMatchPoints, char *fileName);