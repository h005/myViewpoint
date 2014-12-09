#pragma once
#include <gl/glut.h>

typedef struct {
	float mx, my, mz;
	int ix, iy;
	int ax, ay;
} RPair;

void printFloatv(int mode, char *title);
bool getPointInModels(int x, int y, GLint viewport[4], GLdouble modelview[16], GLdouble projection[16], int iwidth, int iheight, float &mx, float &my, float &mz);
void randomSample(int range, int need, int result[]);
void drawCamera(const cv::Mat &lookAt); 
void transition(const cv::Mat &M1, const cv::Mat &M2, const cv::Mat &Md, cv::Mat &out);