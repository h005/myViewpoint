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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
//#include <glm/ext.hpp>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

#include "glm.h"
#include "custom.h"
#include "DLT.h"
#include "GModel.h"

using namespace std;

GModel model;

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

GLfloat customProjection[16];
GLboolean usingCustomProjection = false;



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
char *wTxtName, *sTxtName;

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

	fclose(fp);
    return true;
}

bool saveTXT(char* filename, int Cords[10][2], int &count_click)
{
	FILE* fp;
	fp = fopen(filename, "w");
	if (!fp) {
		perror(filename);
		return false;
	}

	for (int i = 0; i < count_click; i++)
		fprintf(fp, "%d %d\n", Cords[i][0], Cords[i][1]);
	fclose(fp);
	return true;
}
bool saveTXT(char* filename, float Cords[10][3], int &count_click)
{
	FILE* fp;
	fp = fopen(filename, "w");
	if (!fp) {
		perror(filename);
		return false;
	}

	for (int i = 0; i < count_click; i++)
		fprintf(fp, "%f %f %f\n", Cords[i][0], Cords[i][1], Cords[i][2]);
	fclose(fp);
	return true;
}




void SVDDLT(int imClick, int objClick, int imCords[][2], float objCords[][3]) {
	// DLT using SVD
	// Reference: http://www.maths.lth.se/matematiklth/personal/calle/datorseende13/pres/forelas3.pdf
	assert((imClick == objClick) && (imClick >= 6)); //必须保证图像与模型对应点数相同且>=6个

	// 第一阶段, 用点对得到P
	cv::Mat P = phase1CalculateP(imClick, objClick, imCords, objCords);
	
	// 第二阶段
	// 从P中分解出 K * [R t]
	// 由[R t]可以得到lookat的参数，由K可以构造GL_PROJECTION_MATRIX
	cv::Mat lookatParam, projMatrix;
	phase2ExtractParametersFromP(P, iwidth, iheight, lookatParam, projMatrix);

	// 在物体坐标系（世界坐标系）中摆放照相机和它的朝向
	for (int i = 0; i < 9; i++) {
		lookat[i].value = lookatParam.at<float>(i % 3, i / 3);
	}
	// 使用生成的OpenGL投影矩阵
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			customProjection[j * 4 + i] = projMatrix.at<float>(i, j);
		}
	}
	usingCustomProjection = true;

	glm::mat4 rotate = glm::rotate(glm::mat4(1.f), 90.f, glm::vec3(0.f, 0.f, -1.f));
	glm::vec4 point = glm::vec4(glm::vec3(1.f, 0.f, 0.f), 1.f);
	cout << glm::to_string(rotate * point) << endl; 
	cout << glm::to_string(rotate) << endl;
}

double validator(const cv::Mat &P, int count, RPair candidate[]) {
	int pass = 0;
	cv::Mat objPoint = cv::Mat::ones(4, 1, CV_32F);
	cv::Mat imPoint = cv::Mat::zeros(2, 1, CV_32F);
	for (int i = 0; i < count; i++) {
		objPoint.at<float>(0, 0) = candidate[i].mx;
		objPoint.at<float>(1, 0) = candidate[i].my;
		objPoint.at<float>(2, 0) = candidate[i].mz;
		imPoint.at<float>(0, 0) = candidate[i].ix;
		imPoint.at<float>(1, 0) = candidate[i].iy;

		cv::Mat t = P * objPoint;
		t = t / t.at<float>(2, 0);
		double norm = cv::norm(t(cv::Range(0, 2), cv::Range::all()) - imPoint, cv::NORM_L2);
		if (norm < 5) {
			pass++;
		}
	}

	return pass;
}

void RansacDLT() {
	// 切换到screen界面中，保存原来的窗口以便还原
	int context = glutGetWindow();
	glutSetWindow(screen);

	GLint gViewport[4];
	GLdouble gModelview[16];
	GLdouble gProjection[16];
	glGetIntegerv(GL_VIEWPORT, gViewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, gModelview);
	glGetDoublev(GL_PROJECTION_MATRIX, gProjection);


	static RPair candidate[100000];
	int rPairCount = 0;

	printf("mm\n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", gModelview[j * 4 + i]);
		}
		printf("\n");
	}

	FILE *fp = fopen("pFile.dat", "r");
	int a, b, c, d;
	int i = 0;
	while (fscanf(fp, "%d %d %d %d", &a, &b, &c, &d) != EOF) {
		float mx, my, mz;
		bool r = getPointInModels(a, b, gViewport, gModelview, gProjection, iwidth, iheight, mx, my, mz);
		if (r) {
			candidate[rPairCount].mx = mx;
			candidate[rPairCount].my = my;
			candidate[rPairCount].mz = mz;
			candidate[rPairCount].ix = c;
			candidate[rPairCount].iy = d;
			candidate[rPairCount].ax = a;
			candidate[rPairCount].ay = b;
			rPairCount++;
		}
	}
	fclose(fp);

	{
#define NEED 6
		int max = 0;
		cv::Mat store;
		int slist[100];
		{
			float objCords[10][3];
			int imCords[10][2];


			for (int i = 0; i < 10000; i++) {
				int list[NEED];
				randomSample(rPairCount, NEED, list);
				for (int j = 0; j < NEED; j++) {
					objCords[j][0] = candidate[list[j]].mx;
					objCords[j][1] = candidate[list[j]].my;
					objCords[j][2] = candidate[list[j]].mz;

					imCords[j][0] = candidate[list[j]].ix;
					imCords[j][1] = candidate[list[j]].iy;
				}

				cv::Mat P = phase1CalculateP(NEED, NEED, imCords, objCords);
				int pass = validator(P, rPairCount, candidate);
				if (max < pass) {
					max = pass;
					P.copyTo(store);
					for (int k = 0; k < NEED; k++) {
						slist[k] = list[k];
					}
				}
			}
		}

		// 在图上显示选定的6个点，借用了全局数据结构
		cout << "pass: " << max << endl;
		imClick = NEED;
		objClick = NEED;
		for (int i = 0; i < NEED; i++) {
			imCords[i][0] = candidate[slist[i]].ax;
			imCords[i][1] = candidate[slist[i]].ay;

			objCords[i][0] = candidate[slist[i]].mx;
			objCords[i][1] = candidate[slist[i]].my;
			objCords[i][2] = candidate[slist[i]].mz;
		}

		// 将这些点画到B图上面去
		cv::Mat tImg = cv::imread("1t.png");
		cout << tImg.size().width << " " << tImg.size().height << endl;
		for (int i = 0; i < NEED; i++) {
			cv::Point2i p;
			p.x = candidate[slist[i]].ix;
			p.y = tImg.size().height - candidate[slist[i]].iy;
			cv::Scalar pColor(pointColor[i].blue * 255, pointColor[i].green * 255, pointColor[i].red * 255);
			cv::circle(tImg, p, 4, pColor, -1);
		}
		cv::imwrite("amd.png", tImg);
		//phase2ExtractParametersFromP(store);
#undef NEED
	}

	// 恢复原来的窗口
	glutSetWindow(context);
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
		imClick = 0;
		objClick = 0;
		break;
	case 's':
		saveTXT(wTxtName, imCords, imClick);
		saveTXT(sTxtName, objCords, objClick);
		break;
	case '-':
		imClick = (imClick > 0) ? imClick - 1 : imClick;
		break;
	case '=':
		objClick = (objClick > 0) ? objClick - 1 : objClick;
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
		spin_x = 0;
		spin_y = 0;
		usingCustomProjection = false;
        break;
	case 'u':
		SVDDLT(imClick, objClick, imCords, objCords);
		spin_x = 0;
		spin_y = 0;
		break;
	case 'v':
		RansacDLT();
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
		}
	}
    
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
	case 'n':
		name = "data/luggg.ppm";
		txt_name = "data/luggg_im_norm.txt";
		break;
	case 'e':
        name = "data/Eiffel.ppm";
		txt_name = "data/Eiffel_im_norm.txt";
        break;
	case 't':
        name = "data/triumph.ppm";
		txt_name = "data/triumph_im_norm.txt";
        break;
	case 'a':
		name = "data/notre_dame.ppm";
		txt_name = "data/notre_dame_im_norm.txt";
    }
    
    if (name) {
		wTxtName = txt_name;
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
	int oldx = x;
    int oldy = y;
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

void initGL(int glutWindow) {
	int context = glutGetWindow();
	glutSetWindow(glutWindow);

	glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glClearDepth(1.0f);				// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // Uses default lighting parameters
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_NORMALIZE);

	static GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	static GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static GLfloat LightPosition[] = { 0.0f, 0.0f, 15.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);


	// 抗锯齿选项
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// XXX docs say all polygons are emitted CCW, but tests show that some aren't.
	if (getenv("MODEL_IS_BROKEN"))
		glFrontFace(GL_CW);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	glutSetWindow(context);
}
void
screen_reshape(int width, int height)
{
    perspective[1].value = width * 1.0 / height;
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	if (usingCustomProjection) {
		glMultMatrixf(customProjection);
	}
	else {
		gluPerspective(perspective[0].value, perspective[1].value,
			perspective[2].value, perspective[3].value);
	}
	

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(lookat[0].value, lookat[1].value, lookat[2].value,
        lookat[3].value, lookat[4].value, lookat[5].value,
        lookat[6].value, lookat[7].value, lookat[8].value);
}


void loadModel(const char *modelPath, char *pointsName) {
	// 由于要绑定纹理，所以需要切换到screen的上下文中
	int context = glutGetWindow();
	glutSetWindow(screen);

	assert(model.load(modelPath));
	model.bindTextureToGL();

	if (!loadTXT(pointsName, objCords, objClick))
		objClick = 0;

	glutSetWindow(context);
}

void
screen_display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glRotatef(spin_x, 0, 1, 0);
	glRotatef(spin_y, 1, 0, 0);

	if (!model.hasModel()) {
		loadModel("data/EiffelTower.obj", "data/Eiffel_obj.txt");
	}
	
	// 使用下面的方法，你无需考虑模型原生的点坐标
	// 只需要考虑规范化后的坐标
	model.drawNormalizedModel();

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

			if (usingCustomProjection) {
				glutSolidSphere(0.01, 10, 10);
			} else {
				glutSolidSphere(0.02, 10, 10);
			}
			
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
	case 'n':
		name = "data/Lugger.obj";
		txt_name = "data/luggg_obj.txt";
		break;
	case 'e':
        name = "data/EiffelTower.obj";
		txt_name = "data/Eiffel_obj.txt";
        break;
	case 't':
        name = "data/triumph.obj";
		txt_name = "data/triumph_obj.txt";
        break;
	case 'a':
		name = "F:/no2/models/model.dae";
		txt_name = "data/notre_dame.txt";
		break;
	case 's':
		name = "F:/tools/assimp-3.1.1-win-binaries/test/models/OBJ/spider.obj";
		txt_name = "data/spider.txt";
		break;
    }
    
    if (name) {
		sTxtName = txt_name;
		// 加载模型和模型上选取的点
		loadModel(name, txt_name);
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
    
    redisplay_screen();
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

    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
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
    glutAddMenuEntry("StateCapitol", 'c');
    glutAddMenuEntry("Oslo", 'o');
	glutAddMenuEntry("JinMao", 'm');
	glutAddMenuEntry("Lugger", 'l');
	glutAddMenuEntry("Lugger2", 'n');
	glutAddMenuEntry("Eiffel", 'e');
	glutAddMenuEntry("triumph", 't');
	glutAddMenuEntry("notre dame", 'a');
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    screen = glutCreateSubWindow(window, GAP+sub_width+GAP, GAP, sub_width, sub_height);
	initGL(screen);
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
	glutAddMenuEntry("Lugger2", 'n');
	glutAddMenuEntry("EiffelTower", 'e');
	glutAddMenuEntry("triumph", 't');
	glutAddMenuEntry("notre dame", 'a');
	glutAddMenuEntry("Spider", 's');
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
	glutAddMenuEntry("[c]  Clear all points", 'c');
	glutAddMenuEntry("[s]  Save all points", 's');
    glutAddMenuEntry("", 0);
	glutAddMenuEntry("[u]  Update Lookat() using DLT", 'u');
	glutAddMenuEntry("[v]  cuuu", 'v');
	glutAddMenuEntry("", 0);
    glutAddMenuEntry("Quit", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    redisplay_all();
    
    glutMainLoop();
    
    return 0;
}
