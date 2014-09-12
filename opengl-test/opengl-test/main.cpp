#include "control.h"
#ifndef USING_ROBIN

#include <GL/glut.h>
#include <stdio.h>
#include <opencv2\opencv.hpp>

#include "custom.h"

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glLoadIdentity();             /* clear the matrix */
	/* viewing transformation  */
	gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// 等价于
	//glTranslatef(0, 0, -5);
	printFloatv(GL_MODELVIEW_MATRIX, "GL_MODELVIEW_MATRIX");
	{
		GLfloat v[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, v);
		cv::Mat modelView = cv::Mat(4, 4, CV_32F);
		for (int i = 0; i < modelView.rows; i++) {
			for (int j = 0; j < modelView.cols; j++) {
				modelView.at<float>(i, j) = v[j * 4 + i];
			}
		}
		assert(verifyModelViewMatrix(modelView));
	}
	glScalef(1.0, 2.0, 1.0);      /* modeling transformation */
	printFloatv(GL_MODELVIEW_MATRIX, "GL_MODELVIEW_MATRIX");
	{
		GLfloat v[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, v);
		cv::Mat modelView = cv::Mat(4, 4, CV_32F);
		for (int i = 0; i < modelView.rows; i++) {
			for (int j = 0; j < modelView.cols; j++) {
				modelView.at<float>(i, j) = v[j * 4 + i];
			}
		}
		verifyModelViewMatrix(modelView);

		// 在相机坐标系下选择相机点和其方向上的一个点
		// PA = (0, 0, 0), PB = (0, 0, 1)
		cv::Mat PA = cv::Mat::zeros(4, 1, CV_32F);
		PA.at<float>(3, 0) = 1;
		cv::Mat PB = cv::Mat::zeros(4, 1, CV_32F);
		PB.at<float>(2, 0) = -1;
		PB.at<float>(3, 0) = 1;
		// 相机坐标系下，相机头部的方向（向量）
		cv::Mat UpDir = cv::Mat::zeros(4, 1, CV_32F);
		UpDir.at<float>(1, 0) = 1;

		// 求取它们在世界坐标系下的表示
		PA = modelView.inv() * PA;
		PB = modelView.inv() * PB;
		UpDir = modelView.inv() * UpDir;
		PA /= PA.at<float>(3, 0);
		PB /= PB.at<float>(3, 0);

		glLoadIdentity();
		gluLookAt(
			PA.at<float>(0, 0), PA.at<float>(1, 0), PA.at<float>(2, 0),
			PB.at<float>(0, 0), PB.at<float>(1, 0), PB.at<float>(2, 0),
			UpDir.at<float>(0, 0), UpDir.at<float>(1, 0), UpDir.at<float>(2, 0)
			);
	}
	printFloatv(GL_MODELVIEW_MATRIX, "GL_MODELVIEW_MATRIX");
	glutWireCube(1.0);
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

#endif