﻿#pragma

#include <vector>

#include "def.h"

void generateFeats(const char* fileName, detectTypes detectTypeNum, std::vector<singleFeature *> &featsList);
void generateSIFTFeats(const char* fileName, std::vector<singleFeature *> &featsList);