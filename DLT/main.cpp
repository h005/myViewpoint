/*
    projection.c
    Nate Robins, 1997

    Tool for teaching about OpenGL projections.
    
*/

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

#include "glm.h"
#include "matrix.h"
#include "custom.h"

using namespace std;

typedef struct _cell {
    int id;
    int x, y;
    float min, max;
    float value;
    float step;
    char* info;
    char* format;
} cell;

typedef struct _color{
	float red;
	float green;
	float blue;
} color;

color pointColor[12] = {
	{1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 0.0, 1.0},
	{1.0, 1.0, 0.0},
	{1.0, 0.0, 1.0},
	{0.0, 1.0, 1.0},
	{0.5, 0.0, 0.0},
	{1.0, 0.6, 0.0},
	{0.8, 0.6, 1.0},
	{0.0, 0.6, 0.2},
	{0.0, 0.6, 1.0},
	{0.6, 0.0, 1.0},
};

cell lookat[9] = {
    { 1, 180, 120, -5.0, 5.0, 0.0, 0.1,
        "Specifies the X position of the eye point.", "%.2f" },
    { 2, 240, 120, -5.0, 5.0, 0.0, 0.1,
    "Specifies the Y position of the eye point.", "%.2f" },
    { 3, 300, 120, -5.0, 5.0, 2.0, 0.1,
    "Specifies the Z position of the eye point.", "%.2f" },
    { 4, 180, 160, -5.0, 5.0, 0.0, 0.1,
    "Specifies the X position of the reference point.", "%.2f" },
    { 5, 240, 160, -5.0, 5.0, 0.0, 0.1,
    "Specifies the Y position of the reference point.", "%.2f" },
    { 6, 300, 160, -5.0, 5.0, 0.0, 0.1,
    "Specifies the Z position of the reference point.", "%.2f" },
    { 7, 180, 200, -2.0, 2.0, 0.0, 0.1,
    "Specifies the X direction of the up vector.", "%.2f" },
    { 8, 240, 200, -2.0, 2.0, 1.0, 0.1,
    "Specifies the Y direction of the up vector.", "%.2f" },
    { 9, 300, 200, -2.0, 2.0, 0.0, 0.1,
    "Specifies the Z direction of the up vector.", "%.2f" },
};

cell perspective[4] = {
    { 10, 180, 80, 1.0, 179.0, 60.0, 1.0,
        "Specifies field of view angle (in degrees) in y direction.", "%.1f" },
    { 11, 240, 80, -3.0, 3.0, 1.0, 0.01,
    "Specifies field of view in x direction (width/height).", "%.2f" },
    { 12, 300, 80, 0.1, 10.0, 0.1, 0.05,
    "Specifies distance from viewer to near clipping plane.", "%.1f" },
    { 13, 360, 80, 0.1, 10.0, 10.0, 0.05,
    "Specifies distance from viewer to far clipping plane.", "%.1f" },
};

cell pcolor[4] = {
    { 31, 120, 310, 0.0, 1.0, 0.6, 0.01,
        "Specifies red component of polygon color.", "%.2f" },
    { 32, 180, 310, 0.0, 1.0, 0.6, 0.01,
    "Specifies green component of polygon color.", "%.2f" },
    { 33, 240, 310, 0.0, 1.0, 0.6, 0.01,
    "Specifies blue component of polygon color.", "%.2f" },
    { 34, 300, 310, 0.0, 1.0, 1.0, 0.01,
    "Specifies alpha component of polygon color.", "%.2f" },
};

cell bcolor[4] = {
    { 39, 240, 30, 0.0, 1.0, 1.0, 0.01,
        "Specifies red component of texture border color.", "%.2f" },
    { 40, 300, 30, 0.0, 1.0, 0.0, 0.01,
    "Specifies green component of texture border color.", "%.2f" },
    { 41, 360, 30, 0.0, 1.0, 0.0, 0.01,
    "Specifies blue component of texture border color.", "%.2f" },
    { 42, 420, 30, 0.0, 1.0, 1.0, 0.01,
    "Specifies alpha component of texture border color.", "%.2f" },
};

cell ecolor[4] = {
    { 35, 240, 60, 0.0, 1.0, 0.0, 0.01,
        "Specifies red component of texture environment color.", "%.2f" },
    { 36, 300, 60, 0.0, 1.0, 1.0, 0.01,
    "Specifies green component of texture environment color.", "%.2f" },
    { 37, 360, 60, 0.0, 1.0, 0.0, 0.01,
    "Specifies blue component of texture environment color.", "%.2f" },
    { 38, 420, 60, 0.0, 1.0, 1.0, 0.01,
    "Specifies alpha component of texture environment color.", "%.2f" },
};

enum {
    PERSPECTIVE, FRUSTUM, ORTHO
} mode = PERSPECTIVE;

GLboolean world_draw = GL_TRUE;
GLMmodel* pmodel = NULL;
GLint selection = 0;
GLfloat spin_x = 0.0;
GLfloat spin_y = 0.0;
GLfloat model_scale = 0.0;

int iheight, iwidth;
unsigned char* image = NULL;
int twidth, theight;

const GLfloat GL_PI = 3.1415926536f;

GLenum minfilter = GL_NEAREST;
GLenum magfilter = GL_NEAREST;
GLenum env = GL_MODULATE;
GLenum wraps = GL_REPEAT;
GLenum wrapt = GL_REPEAT;

void redisplay_command();
void redisplay_screen();
void redisplay_world();
void redisplay_all(void);

GLuint window, world, screen, command;
GLuint Width = 1000,Height = 1000;
GLuint sub_width = Width/2, sub_height = Height/2;

//DLT
int imCords[10][2] = {0};          //交互对应的图像点，默认10个点
float objCords[10][3] = {0};     //对应的模型点
int imClick = 0;                        //记录点的个数
int objClick = 0;
ofstream outImfile("im.txt");   //存图像坐标
ofstream outObjfile("obj.txt");//存模型坐标
char str[80];
//
GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_10;

void
setfont(char* name, int size)
{
    font_style = GLUT_BITMAP_HELVETICA_10;
    if (strcmp(name, "helvetica") == 0) {
        if (size == 12) 
            font_style = GLUT_BITMAP_HELVETICA_12;
        else if (size == 18)
            font_style = GLUT_BITMAP_HELVETICA_18;
    } else if (strcmp(name, "times roman") == 0) {
        font_style = GLUT_BITMAP_TIMES_ROMAN_10;
        if (size == 24)
            font_style = GLUT_BITMAP_TIMES_ROMAN_24;
    } else if (strcmp(name, "8x13") == 0) {
        font_style = GLUT_BITMAP_8_BY_13;
    } else if (strcmp(name, "9x15") == 0) {
        font_style = GLUT_BITMAP_9_BY_15;
    }
}

void 
drawstr(GLuint x, GLuint y, char* format, ...)
{
    va_list args;
    char buffer[255], *s;
    
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    
    glRasterPos2i(x, y);
    for (s = buffer; *s; s++)
        glutBitmapCharacter(font_style, *s);
}

void
cell_draw(cell* cell)
{
    glColor3ub(0, 255, 128);
    if (selection == cell->id) {
        glColor3ub(255, 255, 0);
        drawstr(10, 240, cell->info);
        glColor3ub(255, 0, 0);
    }
    
    drawstr(cell->x, cell->y, cell->format, cell->value);
}

int
cell_hit(cell* cell, int x, int y)
{
    if (x > cell->x && x < cell->x + 60 &&
        y > cell->y-30 && y < cell->y+10)
        return cell->id;
    return 0;
}

void
cell_update(cell* cell, int update)
{
    if (selection != cell->id)
        return;
    
    cell->value += update * cell->step;
    
    if (cell->value < cell->min)
        cell->value = cell->min;
    else if (cell->value > cell->max) 
        cell->value = cell->max;
    
}

void
cell_vector(float* dst, cell* cell, int num)
{
    while (--num >= 0)
        dst[num] = cell[num].value;
}

void
drawmodel(void)
{
    if (!pmodel) {
        pmodel = glmReadOBJ("data/f-16.obj");
        if (!pmodel) exit(0);
        model_scale = glmUnitize(pmodel);
        glmFacetNormals(pmodel);
        glmVertexNormals(pmodel, 90.0);
    }
    
    glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
}

void
texenv(void)
{
    GLfloat env_color[4], border_color[4];
    
    cell_vector(env_color, ecolor, 4);
    cell_vector(border_color, bcolor, 4);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, env);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env_color);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
}

void
texture(void)
{
    texenv();
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, iwidth, iheight, GL_RGB, GL_UNSIGNED_BYTE, image); 
}

bool loadTXT(char* filename, int Cords[10][2], int &count_click)
{
	FILE* fp;
	count_click = 0;
    fp = fopen(filename, "rb");
    if (!fp) {
        perror(filename);
        return false;
    }
    
    // feof很不好用，很容易出现最后一行读两次的情况
    // http://stackoverflow.com/questions/5431941/while-feof-file-is-always-wrong
    // 这样处理之后，*_im_norm.txt文件的最后一行可以为空
	while(fscanf(fp,"%d %d",&Cords[count_click][0],&Cords[count_click][1]) != EOF) {
        count_click++;
    }
	for(int i = 0; i < count_click; i++)
		printf("%d %d\n",Cords[i][0],Cords[i][1]);
	fclose(fp);
    return true;
}
bool loadTXT(char* filename, float Cords[10][3], int &count_click)
{
	FILE* fp;
	count_click = 0;
    fp = fopen(filename, "rb");
    if (!fp) {
        perror(filename);
        return false;
    }
	while(fscanf(fp,"%f %f %f",&Cords[count_click][0],&Cords[count_click][1],&Cords[count_click][2]) != EOF) {
        count_click++;
    }
	for(int i = 0; i < count_click; i++)
		printf("%f %f %f\n",Cords[i][0],Cords[i][1],Cords[i][2]);
	fclose(fp);
    return true;
}

void SVDDLT() {
	// DLT using SVD
	// Reference: http://www.maths.lth.se/matematiklth/personal/calle/datorseende13/pres/forelas3.pdf
	assert((imClick == objClick) && (imClick >= 6)); //必须保证图像与模型对应点数相同且>=6个
	float *cords3d = new float[imClick * 3];
	float *cords2d = new float[imClick * 2];
	float param2d[3], param3d[4];

	// 将选定的点进行正规化，提高SVD分解的数值精度
	// 正规化使用的参数需要记录下来，以后还原使用
	normalize_2D(imCords, (float (*)[2])cords2d, imClick, param2d);
	normalize_3D(objCords,(float (*)[3])cords3d, imClick, param3d);

	printf("%f %f %f\n", param2d[0], param2d[1], param2d[2]);
	printf("%f %f %f %f\n", param3d[0], param3d[1], param3d[2], param3d[3]);
	for (int i = 0; i < imClick; i++) {
		printf("%f %f %f %f %f\n", cords3d[i * 3 + 0], cords3d[i * 3 + 1], cords3d[i * 3 + 2], cords2d[i * 2 + 0], cords2d[i * 2 + 1]);
	}

	cv::Mat M = cv::Mat::zeros(3 * imClick, 12 + imClick, CV_32F);
	for (int i = 0; i < imClick; i++) {
		for (int j = 0; j < 12; j++) {
			if (j % 4 == 3) {
				// 3D齐次坐标中的最后一维
				M.at<float>(3 * i + j / 4, j) = 1;
			} else {
				// 3D齐次坐标的前三维
				M.at<float>(3 * i + j / 4, j) = cords3d[i * 3 + j % 4];
			}
			
		}
		M.at<float>(3 * i + 0 ,12 + i) = -cords2d[i * 2 + 0];
		M.at<float>(3 * i + 1, 12 + i) = -cords2d[i * 2 + 1];
		M.at<float>(3 * i + 2, 12 + i) = -1;
	}

	// 执行SVD分解
	cv::SVD thissvd(M, cv::SVD::FULL_UV);
	cv::Mat U = thissvd.u;
	cv::Mat S = thissvd.w;
	cv::Mat VT = thissvd.vt;

	// g(v) = 0, 求出使得((Mv)T * (Mv))最小的v
	// 但是v和-v用于计算二范数是一样的，所以选择λ全部大于零的那个v
	cv::Mat v = VT.row(S.rows - 1);
	if (cv::countNonZero(v(cv::Range(0, 1), cv::Range(12, v.cols)) > 0) < (imClick / 2)) {
		v = -v;
	}

	std::cout << "SV: " << S.t() << std::endl;
	std::cout << "v: " << v << std::endl;
	cv::Mat z = M * v.t();
	std::cout << "LSR: " << cv::norm(z, cv::NORM_L2) << std::endl;

	// 教程中使用的公式是
	// λx = PX，但是里面的x和X都是正规化后的，我们需要找到真实的P'
	// λ(NL * x) = P * (NR * X) => λx = NL(-1) * P * NR * X
	// 所以 P' = NL(-1) * P * NR

	// 将用于正规化的矩阵构造出来
	cv::Mat NR = cv::Mat::eye(4, 4, CV_32F);
	NR.at<float>(0, 3) = -param3d[0];
	NR.at<float>(1, 3) = -param3d[1];
	NR.at<float>(2, 3) = -param3d[2];
	NR *= param3d[3];
	NR.at<float>(3, 3) = 1;

	cv::Mat NL = cv::Mat::eye(3, 3, CV_32F);
	NL.at<float>(0, 2) = -param2d[0];
	NL.at<float>(1, 2) = -param2d[1];
	NL *= param2d[2];
	NL.at<float>(2, 2) = 1;

	// λ(NL * x) = P * (NR * X)
	cv::Mat P = v(cv::Range(0, 1), cv::Range(0, 12)).reshape(0, 3);
	// 求出P'
	// 因为 λx = NL(-1) * P * NR * X
	// 所以 P' = NL(-1) * P * NR
	P = NL.inv() * P * NR;

	// 求解旋转矩阵
	cv::Mat A = P(cv::Range::all(), cv::Range(0, 3));
	cv::Mat A1 = A.row(0).t();
	cv::Mat A2 = A.row(1).t();
	cv::Mat A3 = A.row(2).t();
	
	cv::Mat t3 = A3;
	float f = cv::norm(t3, cv::NORM_L2);
	cv::Mat R3 = t3 / f;

	float e = A2.dot(R3);
	cv::Mat t2 = A2 - e * R3;
	float d = cv::norm(t2, cv::NORM_L2);
	cv::Mat R2 = t2 / d;

	float c = A1.dot(R3);
	float b = A1.dot(R2);
	cv::Mat t1 = A1 - b * R2 - c * R3;
	float a = cv::norm(t1, cv::NORM_L2);
	cv::Mat R1 = t1 / a;

	cv::Mat modelView = cv::Mat::zeros(4, 4, CV_32F);
	modelView(cv::Range(0, 1), cv::Range(0, 3)) = R1.t();
	modelView(cv::Range(1, 2), cv::Range(0, 3)) = R2.t();
	modelView(cv::Range(2, 3), cv::Range(0, 3)) = R3.t();
	modelView.at<float>(3, 3) = 1;
    
    cv::Mat K = cv::Mat::zeros(3, 3, CV_32F);
    K.at<float>(0, 0) = a;
    K.at<float>(0, 1) = b;
    K.at<float>(0, 2) = c;
    K.at<float>(1, 1) = d;
    K.at<float>(1, 2) = e;
    K.at<float>(2, 2) = f;
    
    cv::Mat A4 = P(cv::Range::all(), cv::Range(3, 4));
    cv::Mat T = K.inv() * A4;
    T.copyTo(modelView(cv::Range(0, 3), cv::Range(3, 4)));

	if (false) {
		printf("Apply transforamtion to objCords:\n");
		for (int i = 0; i < imClick; i++) {
			cv::Mat objHomogeneous = cv::Mat(4, 1, CV_32F);
			objHomogeneous.at<float>(0, 0) = objCords[i][0];
			objHomogeneous.at<float>(1, 0) = objCords[i][1];
			objHomogeneous.at<float>(2, 0) = objCords[i][2];
			objHomogeneous.at<float>(3, 0) = 1;

			cv::Mat imHomogeneous = cv::Mat(3, 1, CV_32F);
			imHomogeneous.at<float>(0, 0) = imCords[i][0];
			imHomogeneous.at<float>(1, 0) = imCords[i][1];
			imHomogeneous.at<float>(2, 0) = 1;

			cv::Mat a = K * modelView(cv::Range(0, 3), cv::Range::all()) * objHomogeneous;
			cv::Mat b = imHomogeneous * v.at<float>(0, 12 + i);
			cv::Mat c;
			cv::hconcat(a, b, c);
			cout << c << endl;
		}
	}
	
	// 清理空间
	delete cords2d;
	delete cords3d;

	// 在相机坐标系下选择相机点和其方向上的一个点
	// PA = (0, 0, 0), PB = (0, 0, 1)
	cv::Mat PA = cv::Mat::zeros(4, 1, CV_32F);
	PA.at<float>(3, 0) = 1;
	cv::Mat PB = cv::Mat::zeros(4, 1, CV_32F);
	PB.at<float>(2, 0) = 1;
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

	// 在物体坐标系（世界坐标系）中摆放照相机和它的朝向
	for (int i = 0; i < 9; i++) {
		switch (i / 3) {
		case 0:
			lookat[i].value = PA.at<float>(i % 3, 0);
			break;
		case 1:
			lookat[i].value = PB.at<float>(i % 3, 0);
			break;
		case 2:
			lookat[i].value = UpDir.at<float>(i % 3, 0);
			break;
		}
	}

	cout << K << endl;
	assert(verifyModelViewMatrix(modelView));
}

void
main_reshape(int width,  int height) 
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
#define GAP  25             /* gap between subwindows */
    sub_width = (width - GAP * 3) / 2;
    sub_height = sub_width;
    
    glutSetWindow(world);
    glutPositionWindow(GAP, GAP);
    glutReshapeWindow(sub_width, sub_height);
    glutSetWindow(screen);
    glutPositionWindow(GAP+sub_width+GAP, GAP);
    glutReshapeWindow(sub_width, sub_height);
    glutSetWindow(command);
    glutPositionWindow(GAP, GAP+sub_height+GAP);
    glutReshapeWindow(sub_width+GAP+sub_width, height - sub_height - GAP * 3);
}

void
main_display(void)
{
    glClearColor(0.8, 0.8, 0.8, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3ub(0, 0, 0);
    setfont("helvetica", 12);
    drawstr(GAP, GAP-5, "Image view");//World-space
    drawstr(GAP+sub_width+GAP, GAP-5, "Model view");//Screen-space
    drawstr(GAP, GAP+sub_height+GAP-5, "Command manipulation window");
    glutSwapBuffers();
}

void
main_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
	case 'c':
		SVDDLT();
        break;
    case 'r':
        perspective[0].value = 60.0;
        perspective[1].value = 1.0;
        perspective[2].value = 0.1;
        perspective[3].value = 10.0;
		lookat[0].value = 0.0;
		lookat[1].value = 0.0;
		lookat[2].value = 2.0;
		lookat[3].value = 0.0;
		lookat[4].value = 0.0;
		lookat[5].value = 0.0;
		lookat[6].value = 0.0;
		lookat[7].value = 1.0;
		lookat[8].value = 0.0;
        break;
    case 27:
        exit(0);
    }

	redisplay_all();
}

void
world_reshape(int width, int height)
{
	twidth = width;
    theight = height;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
}

int oldx, oldy;

void
world_display(void)
{
	texture();
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, twidth, 0, theight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3ub(255, 255, 255);
    
    glEnable(GL_TEXTURE_2D);
//    glEnable(GL_LIGHTING);
    
	glBegin(GL_POLYGON);
    glTexCoord2f(0, 1);
    glVertex2i(0, 0);
    glTexCoord2f(0, 0);
    glVertex2i(0,theight);
    glTexCoord2f(1, 0);
    glVertex2i(twidth, theight);
    glTexCoord2f(1, 1);
    glVertex2i(twidth,0);
    glEnd();

	glDisable(GL_TEXTURE_2D);
	//    glDisable(GL_LIGHTING);
	//画点
	if(imClick)
	{
		double n=50;//分段数
        float R=5;//半径
		//glPointSize(9.0);
		//glLineWidth(5.0);
        
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        GLint wWidth = viewport[2], wHeight = viewport[3];
//        printf("[ww] %d %d %d %d\n", wWidth, wHeight, iwidth, iheight);

		//glBegin(GL_POINTS);
		for(int i = 0; i < imClick; i++)
		{
            // 由于显示窗口有缩放和变形，imCords内存放的点单位是图像像素
			// 这里使用的坐标系统，坐标原点在窗口的左下角
            // 显示前要将坐标对应到屏幕上的像素
            GLint iX = imCords[i][0], iY = imCords[i][1];
            GLint wX = iX * 1.0 * wWidth / iwidth, wY = iY * 1.0 * wHeight / iheight;
            
			glColor3f(pointColor[i].red, pointColor[i].green, pointColor[i].blue);
			glBegin(GL_TRIANGLE_FAN);
            for(int j = 0; j < n; j++)
				glVertex2f(wX + R*cos(2*GL_PI/n*j), wY + R*sin(2*GL_PI/n*j));
            glEnd();
			//glVertex2i(imCords[i][0],imCords[i][1]);
		}
		//glEnd();
	}
	//
    
    glutSwapBuffers();
}

void
world_menu(int value)
{
	char* name = 0;
    char* txt_name = 0;
    switch (value) {
    case 'b':
        name = "data/Bigben.ppm";
		txt_name = "data/Bigben_im_norm.txt";
        break;
    case 'h':
        name = "data/House.ppm";
		txt_name = "data/House_im_norm.txt";
        break;
    case 'c':
        name = "data/Capitol.ppm";
		txt_name = "data/Capitol_im_norm.txt";
        break;
    case 'o':
        name = "data/Oslo.ppm";
		txt_name = "data/Oslo_im_norm.txt";
        break;
	case 'm':
        name = "data/JinMao.ppm";
		txt_name = "data/JinMao_im_norm.txt";
        break;
	case 'l':
        name = "data/Lugger.ppm";
		txt_name = "data/Lugger_im_norm.txt";
        break;
	case 'e':
        name = "data/Eiffel.ppm";
		txt_name = "data/Eiffel_im_norm.txt";
        break;
	case 't':
        name = "data/triumph.ppm";
		txt_name = "data/triumph_im_norm.txt";
        break;
    }
    
    if (name) {
		//加载点
		if(!loadTXT(txt_name,imCords,imClick))
			imClick = 0;
		//
        free(image);
        image = glmReadPPM(name, &iwidth, &iheight);
        if (!image)
            image = glmReadPPM("data/opengl.ppm", &iwidth, &iheight);
    }

    redisplay_all();
}

void
world_mouse(int button, int state, int x, int y)
{
	// 传入的坐标，坐标原点在窗口的左上角
	oldx = x;
    oldy = y;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
        GLint width = viewport[2];
        GLint height = viewport[3];
        GLint realy = height - oldy;
        printf("%d %d\n", viewport[2], viewport[3]);

		// 这里使用的坐标系统，坐标原点在窗口的左下角
		imCords[imClick][0] = oldx * 1.0 * iwidth / width;
		imCords[imClick][1] = realy * 1.0 * iheight / height;
		printf("Coordinates at cursor are (%d, %d)\n",oldx,realy);
		sprintf(str,"%d %d",oldx,realy);//把格式化的数据写入某个字符串
	    outImfile<<str<<endl;
		imClick++;
	}
	redisplay_all();
}
void
screen_reshape(int width, int height)
{
    perspective[1].value = width * 1.0 / height;
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(perspective[0].value, perspective[1].value,
		perspective[2].value, perspective[3].value);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(lookat[0].value, lookat[1].value, lookat[2].value,
        lookat[3].value, lookat[4].value, lookat[5].value,
        lookat[6].value, lookat[7].value, lookat[8].value);

	printFloatv(GL_PROJECTION_MATRIX, "GP");

    glClearColor(0.2, 0.2, 0.2, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void
screen_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 绘制模型
	drawmodel();
	// 绘制模型上的点
	if(objClick)
	{
		for(int i = 0; i < objClick; i++)
		{
			glPushMatrix();
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
			glColor3f(pointColor[i].red, pointColor[i].green, pointColor[i].blue);

			glTranslatef(objCords[i][0],objCords[i][1],objCords[i][2]);
			glutSolidSphere(0.04,10,10);
			glDisable(GL_COLOR_MATERIAL);
			glPopMatrix();
		}
	}
    glutSwapBuffers();
}

void
screen_menu(int value)
{
    char* name = 0;
    char* txt_name = 0;
    switch (value) {
    case 'a':
        name = "data/al.obj";
		txt_name = "data/al_obj.txt";
        break;
    case 's':
        name = "data/soccerball.obj";
		txt_name = "data/soccerball_obj.txt";
        break;
    case 'd':
        name = "data/dolphins.obj";
		txt_name = "data/dolphins_obj.txt";
        break;
    case 'f':
        name = "data/flowers.obj";
		txt_name = "data/flowers_obj.txt";
        break;
    case 'j':
        name = "data/f-16.obj";
		txt_name = "data/f-16_obj.txt";
        break;
    case 'p':
        name = "data/porsche.obj";
		txt_name = "data/porsche_obj.txt";
        break;
    case 'r':
        name = "data/rose+vase.obj";
		txt_name = "data/rose+vase_obj.txt";
		break;
	case 'b':
        name = "data/BigBen.obj";
		txt_name = "data/BigBen_obj.txt";
		break;
	case 'g':
        name = "data/Guggenheim.obj";
		txt_name = "data/Guggenheim_obj.txt";
		break;
	case 'm':
        name = "data/JinMaoTower.obj";
		txt_name = "data/JinMao_obj.txt";
		break;
	case 'c':
        name = "data/MissStateCapitol.obj";
		txt_name = "data/Capitol_obj.txt";
		break;
	case 'o':
        name = "data/Oslo_GovtBuilding.obj";
		txt_name = "data/Oslo_obj.txt";
		break;
	case 'h':
        name = "data/House.obj";
		txt_name = "data/House_obj.txt";
        break;
	case 'l':
        name = "data/Lugger.obj";
		txt_name = "data/Lugger_obj.txt";
        break;
	case 'e':
        name = "data/EiffelTower.obj";
		txt_name = "data/Eiffel_obj.txt";
        break;
	case 't':
        name = "data/triumph.obj";
		txt_name = "data/triumph_obj.txt";
        break;
    }
    
    if (name) {
		//加载球
		if(!loadTXT(txt_name,objCords,objClick))
			objClick = 0;
		//
        pmodel = glmReadOBJ(name);
        if (!pmodel) exit(0);
        glmUnitize(pmodel);
        glmFacetNormals(pmodel);
        glmVertexNormals(pmodel, 90.0);
    }
    
    redisplay_all();
}

void
screen_mouse(int button, int state, int x, int y)
{
    oldx = x;
    oldy = y;

    if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLdouble object_x,object_y,object_z;
		GLfloat realy, winZ = 0;

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	    glGetDoublev(GL_PROJECTION_MATRIX, projection);
	       
		realy=(GLfloat)viewport[3] - (GLfloat)oldy;
		//printf("Coordinates at cursor are (%d, %d)\n",oldx,(int)realy);
	
		/*gluUnProject((GLdouble)oldx,(GLdouble)realy,0.0,modelview,projection,viewport,&object_x,&object_y,&object_z);
		object_x /= model_scale; object_y /= model_scale; object_z /= model_scale;
		object_x += pmodel->position[0]; object_y += pmodel->position[1]; object_z += pmodel->position[2]; 
		printf("World Coordinates of Object are (%f,%f,%f)\n",object_x,object_y,object_z);*/

		glReadBuffer(GL_BACK);
		glReadPixels(oldx,int(realy),1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);
		gluUnProject((GLdouble)oldx,(GLdouble)realy,winZ,modelview,projection,viewport,&object_x,&object_y,&object_z);
		//object_x /= model_scale; object_y /= model_scale; object_z /= model_scale;
		//object_x += pmodel->position[0]; object_y += pmodel->position[1]; object_z += pmodel->position[2]; 
		printf("World Coordinates of Object are (%f,%f,%f)\n",object_x,object_y,object_z);
		
		objCords[objClick][0] = object_x; objCords[objClick][1] = object_y; objCords[objClick][2] = object_z;
		sprintf(str,"%f %f %f",(float)object_x,(float)object_y,(float)object_z);//把格式化的数据写入某个字符串
		outObjfile <<str<<endl;

		/*gluUnProject((GLdouble)oldx,(GLdouble)realy,1.0,modelview,projection,viewport,&object_x,&object_y,&object_z); 
		object_x /= model_scale; object_y /= model_scale; object_z /= model_scale;
		object_x += pmodel->position[0]; object_y += pmodel->position[1]; object_z += pmodel->position[2]; 
		printf("World Coordinates of Object are (%f,%f,%f)\n",object_x,object_y,object_z);*/
		objClick++;
	}

    redisplay_all();
}

void
screen_motion(int x, int y)
{
    spin_x = x - oldx;
    spin_y = y - oldy;
    
    redisplay_all();
}

void
command_reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void
command_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3ub(255,255,255);
    
    setfont("helvetica", 18);
    
	drawstr(180, perspective[0].y - 40, "fovy");
	drawstr(230, perspective[0].y - 40, "aspect");
	drawstr(300, perspective[0].y - 40, "zNear");
	drawstr(360, perspective[0].y - 40, "zFar");
	drawstr(40, perspective[0].y, "gluPerspective(");
	drawstr(230, perspective[0].y, ",");
	drawstr(290, perspective[0].y, ",");
	drawstr(350, perspective[0].y, ",");
	drawstr(410, perspective[0].y, ");");

    drawstr(78, lookat[0].y, "gluLookAt(");
    drawstr(230, lookat[0].y, ","); 
    drawstr(290, lookat[0].y, ",");
    drawstr(350, lookat[0].y, ",");
    drawstr(380, lookat[0].y, "<- eye");
    drawstr(230, lookat[3].y, ","); 
    drawstr(290, lookat[3].y, ",");
    drawstr(350, lookat[3].y, ",");
    drawstr(380, lookat[3].y, "<- center");
    drawstr(230, lookat[6].y, ","); 
    drawstr(290, lookat[6].y, ",");
    drawstr(350, lookat[6].y, ");");
    drawstr(380, lookat[6].y, "<- up");
    
	cell_draw(&perspective[0]);
	cell_draw(&perspective[1]);
	cell_draw(&perspective[2]);
	cell_draw(&perspective[3]);

    cell_draw(&lookat[0]);
    cell_draw(&lookat[1]);
    cell_draw(&lookat[2]);
    cell_draw(&lookat[3]);
    cell_draw(&lookat[4]);
    cell_draw(&lookat[5]);
    cell_draw(&lookat[6]);
    cell_draw(&lookat[7]);
    cell_draw(&lookat[8]);
    
    if (!selection) {
        glColor3ub(255, 255, 0);
        drawstr(10, 240,
            "Click on the arguments and move the mouse to modify values.");
    }   
    
    glutSwapBuffers();
}

int old_y;

void
command_mouse(int button, int state, int x, int y)
{
    selection = 0;
    
    if (state == GLUT_DOWN) {
		/* mouse should only hit _one_ of the cells, so adding up all
		the hits just propagates a single hit. */
		selection += cell_hit(&perspective[0], x, y);
		selection += cell_hit(&perspective[1], x, y);
		selection += cell_hit(&perspective[2], x, y);
		selection += cell_hit(&perspective[3], x, y);

        selection += cell_hit(&lookat[0], x, y);
        selection += cell_hit(&lookat[1], x, y);
        selection += cell_hit(&lookat[2], x, y);
        selection += cell_hit(&lookat[3], x, y);
        selection += cell_hit(&lookat[4], x, y);
        selection += cell_hit(&lookat[5], x, y);
        selection += cell_hit(&lookat[6], x, y);
        selection += cell_hit(&lookat[7], x, y);
        selection += cell_hit(&lookat[8], x, y);
    }
    
    old_y = y;
    
    redisplay_all();
}

void
command_motion(int x, int y)
{
    cell_update(&perspective[0], old_y-y);
    cell_update(&perspective[1], old_y-y);
    cell_update(&perspective[2], old_y-y);
    cell_update(&perspective[3], old_y-y);
    
    cell_update(&lookat[0], old_y-y);
    cell_update(&lookat[1], old_y-y);
    cell_update(&lookat[2], old_y-y);
    cell_update(&lookat[3], old_y-y);
    cell_update(&lookat[4], old_y-y);
    cell_update(&lookat[5], old_y-y);
    cell_update(&lookat[6], old_y-y);
    cell_update(&lookat[7], old_y-y);
    cell_update(&lookat[8], old_y-y);
    
    old_y = y;
    
	// 不要用redisplay_all()，会降低滑动性能
	redisplay_screen();
	redisplay_command();
}

void
command_menu(int value)
{
    main_keyboard((unsigned char)value, 0, 0);
}

void
redisplay_world() {
	glutSetWindow(world);
	world_reshape(sub_width, sub_height);
	glutPostRedisplay();
}

void redisplay_screen() {
	glutSetWindow(screen);
	screen_reshape(sub_width, sub_height);
	glutPostRedisplay();
}

void redisplay_command() {
	glutSetWindow(command);
	glutPostRedisplay();
}

void
redisplay_all(void)
{
	redisplay_command();
	redisplay_screen();
	redisplay_world();
}

int
main(int argc, char** argv)
{
    
	image = glmReadPPM("./data/opengl.ppm", &iwidth, &iheight);
    if (!image)
        exit(0);

    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(Width+GAP*3, Height+GAP*3);
    glutInitWindowPosition(50, 50);
    glutInit(&argc, argv);
    
    window = glutCreateWindow("Projection");
    glutReshapeFunc(main_reshape);
    glutDisplayFunc(main_display);
    glutKeyboardFunc(main_keyboard);
    
    world = glutCreateSubWindow(window, GAP, GAP, sub_width, sub_height);
    glutReshapeFunc(world_reshape);
    glutDisplayFunc(world_display);
    glutKeyboardFunc(main_keyboard);
	glutMouseFunc(world_mouse);
    glutCreateMenu(world_menu);
    glutAddMenuEntry("Bigben", 'b');
    glutAddMenuEntry("House", 'h');
    glutAddMenuEntry("Capitol", 'c');
    glutAddMenuEntry("Oslo", 'o');
	glutAddMenuEntry("JinMao", 'm');
	glutAddMenuEntry("Lugger", 'l');
	glutAddMenuEntry("Eiffel", 'e');
	glutAddMenuEntry("triumph", 't');
	
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    screen = glutCreateSubWindow(window, GAP+sub_width+GAP, GAP, sub_width, sub_height);
    glutReshapeFunc(screen_reshape);
    glutDisplayFunc(screen_display);
    glutKeyboardFunc(main_keyboard);
	glutMotionFunc(screen_motion);
    glutMouseFunc(screen_mouse);
    glutCreateMenu(screen_menu);
    glutAddMenuEntry("BigBen", 'b');
	glutAddMenuEntry("Guggenheim", 'g');
	glutAddMenuEntry("JinMaoTower", 'm');
	glutAddMenuEntry("StateCapitol", 'c');
	glutAddMenuEntry("OsloBuilding", 'o');
	glutAddMenuEntry("House", 'h');
	glutAddMenuEntry("Lugger", 'l');
	glutAddMenuEntry("EiffelTower", 'e');
	glutAddMenuEntry("triumph", 't');
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    command = glutCreateSubWindow(window, GAP+sub_height+GAP, GAP+sub_height+GAP, sub_width, sub_height);
    glutReshapeFunc(command_reshape);
    glutDisplayFunc(command_display);
    glutMotionFunc(command_motion);
    glutMouseFunc(command_mouse);
    glutKeyboardFunc(main_keyboard);
    glutCreateMenu(command_menu);
    glutAddMenuEntry("Projection", 0);
    glutAddMenuEntry("[r]  Reset parameters", 'r');
    glutAddMenuEntry("", 0);
	glutAddMenuEntry("[c]  Update Lookat() using DLT", 'c');
	glutAddMenuEntry("", 0);
    glutAddMenuEntry("Quit", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    redisplay_all();
    
    glutMainLoop();
    
    return 0;
}
