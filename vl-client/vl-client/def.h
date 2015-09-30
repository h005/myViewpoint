#pragma once

#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>

#define ORIGINAL_DIM		128
#define LEVELS_PER_OCTIVE	3
#define MIN_OCTIVE_INDEX	0

typedef enum {
	DETECT_HARRIS_AFFINE = 1,
	DETECT_HESSIAN_AFFINE,
	DETECT_VLFEAT_SIFT,
	DETECT_USING_OPENCV_SIFT,
	DETECT_USING_OPENCV_SURF,
	DETECT_USING_MSER_SIFT,
	DETECT_USING_MSER_SURF,
	DETECT_HARRIS_LAPLACE,
	DETECT_HESSIAN_LAPLACE
} detectTypes;

struct coordinate
{
	double x;
	double y;
};

struct bounds
{
	int left;
	int right;
	int top;
	int bottom;
};

struct singleFeature
{
	coordinate c;
	coordinate trackC;
	double scale;
	int octaveIndex;
	double oritation;
	int descriptor[ORIGINAL_DIM];
};

struct singleManifold
{
	coordinate trackC;
	std::vector<singleFeature*> feaList;
	std::vector<int> frameIndexList;
	std::vector<int> foundTypeList;
};