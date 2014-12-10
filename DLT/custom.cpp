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

void drawCamera(const cv::Mat &lookAt) {
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(1.f, 0.f, 0.f);

	cv::Mat PA = lookAt.col(0);
	glTranslatef(PA.at<float>(0, 0), PA.at<float>(1, 0), PA.at<float>(2, 0));
	glutSolidSphere(0.1, 10, 10);

	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();
}

void transition(const cv::Mat &M1, const cv::Mat &M2, const cv::Mat &Md, cv::Mat &out) {
	out = cv::Mat::zeros(4, 4, CV_32F);
	out.at<float>(3, 3) = 1;

	cv::Mat R1 = M1(cv::Range(0, 3), cv::Range(0, 3));
	cv::Mat R2 = M2(cv::Range(0, 3), cv::Range(0, 3));
	cv::Mat Rd = Md(cv::Range(0, 3), cv::Range(0, 3));
	cv::Mat t1 = M1(cv::Range(0, 3), cv::Range(3, 4));
	cv::Mat t2 = M2(cv::Range(0, 3), cv::Range(3, 4));
	cv::Mat td = Md(cv::Range(0, 3), cv::Range(3, 4));

	cv::Mat R = R2 * R1.inv() * Rd;
	cv::Mat t = t2 + R2 * R1.inv() * (td - t1);
	R.copyTo(out(cv::Range(0, 3), cv::Range(0, 3)));
	t.copyTo(out(cv::Range(0, 3), cv::Range(3, 4)));
}

void doit(int index, cv::Mat &m2) {
	const char *dir = "D:\\DriverGenius2013\\NotreDame\\NotreDame\\images";
	char filepath[255];
	sprintf(filepath, "%s\\exp%d.txt", dir, index);
	FILE *fp;
	fp = fopen(filepath, "r");
	if (fp) {
		float input[5][3];
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 3; j++) {
				fscanf(fp, "%f", &input[i][j]);
			}
		}
		cv::Mat m_input = cv::Mat(5, 3, CV_32F, input);
		m2 = cv::Mat::zeros(3, 4, CV_32F);
		m_input(cv::Range(1, 4), cv::Range(0, 3)).copyTo(m2(cv::Range(0, 3), cv::Range(0, 3)));
		cv::Mat t = m_input.row(4).t();
		t.copyTo(m2.col(3));
		fclose(fp);
	}
}