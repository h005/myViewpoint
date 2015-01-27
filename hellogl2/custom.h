#pragma once
#include <QOpenGLShaderProgram>

typedef struct {
	float mx, my, mz;
	int ix, iy;
	int ax, ay;
} RPair;

void printFloatv(int mode, char *title);
bool getPointInModels(int x, int y, GLint viewport[4], GLdouble modelview[16], GLdouble projection[16], int iwidth, int iheight, float &mx, float &my, float &mz);
void randomSample(int range, int need, int result[]);
void drawCamera(const cv::Mat &lookAt); 
void getCameraPosByDLTandSfM(const char *dataDir, const cv::Mat &Md, int srcLabel, int dstLabel, cv::Point3f &dstPos);
