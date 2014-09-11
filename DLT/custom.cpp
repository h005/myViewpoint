#include <iostream>
#include <math.h>
#include "custom.h"

using namespace std;

#define NDIM 3
void normalize_3D(float original[][NDIM], float processed[][NDIM], size_t n, float param[NDIM + 1]) {
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
void normalize_2D(int original[][NDIM], float processed[][NDIM], size_t n, float param[NDIM + 1]) {
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