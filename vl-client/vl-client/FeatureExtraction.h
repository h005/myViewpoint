#pragma

#include <vector>
#include <opencv2/core/core.hpp>
#include "def.h"

void generateFeats(cv::Mat &img, detectTypes detectTypeNum, std::vector<singleFeature *> &featsList);
void generateSIFTFeats(cv::Mat &img, std::vector<singleFeature *> &featsList);