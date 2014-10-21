#include <iostream>
#include <math.h>
#include <gl/glut.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <time.h>
#include "custom.h"

using namespace std;

void printFloatv(int mode, char *title) {
	GLfloat v[16];
	printf("%s\n", title);
	glGetFloatv(mode, v);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", v[j * 4 + i]);
		}
		printf("\n");
	}
	printf("\n");
}

bool getPointInModels(int x, int y, GLint viewport[4], GLdouble modelview[16], GLdouble projection[16],int iwidth, int iheight, float &mx, float &my, float &mz) {
	GLdouble oldx = x * 1.0 * viewport[2] / iwidth;
	GLdouble oldy = y * 1.0 * viewport[3] / iheight;
	GLdouble object_x, object_y, object_z;
	GLfloat winZ = 0;

	glReadBuffer(GL_BACK);
	glReadPixels(oldx, oldy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	if (abs(winZ - 1) < 1e-5) {
		return false;
	}

	gluUnProject((GLdouble)oldx, oldy, winZ, modelview, projection, viewport, &object_x, &object_y, &object_z);
	mx = object_x;
	my = object_y;
	mz = object_z;
	return true;
}

void randomSample(int range, int need, int result[]) {
	static bool inited = false;
	if (!inited) {
		srand(time(NULL));
		inited = true;
	}
	for (int i = 0; i < need; i++) {
		result[i] = rand() % range;
	}
}