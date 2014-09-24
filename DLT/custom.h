#pragma once

typedef struct {
	float mx, my, mz;
	int ix, iy;
} RPair;

void printFloatv(int mode, char *title);
void normalize_3D(float original[][3], float processed[][3], size_t n, float param[3 + 1]);
void normalize_2D(int original[][2], float processed[][2], size_t n, float param[2 + 1]);
bool verifyModelViewMatrix(cv::Mat &modelView);
bool getPointInModels(int x, int y, GLint viewport[4], GLdouble modelview[16], GLdouble projection[16], int iwidth, int iheight, float &mx, float &my, float &mz);
void randomSample(int range, int need, int result[]);