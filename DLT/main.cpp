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
#include "glm.h"
#include "matrix.h"
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
    { 12, 300, 80, 0.1, 10.0, 1.0, 0.05,
    "Specifies distance from viewer to near clipping plane.", "%.1f" },
    { 13, 360, 80, 0.1, 10.0, 10.0, 0.05,
    "Specifies distance from viewer to far clipping plane.", "%.1f" },
};

cell frustum[6] = {
    { 14, 120, 80, -10.0, 10.0, -1.0, 0.1,
        "Specifies coordinate for left vertical clipping plane.", "%.2f" },
    { 15, 180, 80, -10.0, 10.0, 1.0, 0.1,
    "Specifies coordinate for right vertical clipping plane.", "%.2f" },
    { 16, 240, 80, -10.0, 10.0, -1.0, 0.1,
    "Specifies coordinate for bottom vertical clipping plane.", "%.2f" },
    { 17, 300, 80, -10.0, 10.0, 1.0, 0.1,
    "Specifies coordinate for top vertical clipping plane.", "%.2f" },
    { 18, 360, 80, 0.1, 5.0, 1.0, 0.01,
    "Specifies distance to near clipping plane.", "%.2f" },
    { 19, 420, 80, 0.1, 5.0, 3.5, 0.01,
    "Specifies distance to far clipping plane.", "%.2f" },
};

cell ortho[6] = {
    { 14, 120, 80, -10.0, 10.0, -1.0, 0.1,
        "Specifies coordinate for left vertical clipping plane.", "%.2f" },
    { 15, 180, 80, -10.0, 10.0, 1.0, 0.1,
    "Specifies coordinate for right vertical clipping plane.", "%.2f" },
    { 16, 240, 80, -10.0, 10.0, -1.0, 0.1,
    "Specifies coordinate for bottom vertical clipping plane.", "%.2f" },
    { 17, 300, 80, -10.0, 10.0, 1.0, 0.1,
    "Specifies coordinate for top vertical clipping plane.", "%.2f" },
    { 18, 360, 80, -5.0, 5.0, 1.0, 0.01,
    "Specifies distance to near clipping plane.", "%.2f" },
    { 19, 420, 80, -5.0, 5.0, 3.5, 0.01,
    "Specifies distance to far clipping plane.", "%.2f" },
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

void redisplay_all(void);
GLdouble projection[16], modelview[16], inverse[16];
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
GLdouble rotation[16];            //旋转矩阵
bool flag_rotation = false;       //是否使用旋转矩阵
bool isRota = false;
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
drawaxes(void)
{
    glColor3ub(255, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, -0.25, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(0.75, 0.0, 0.25);
    glVertex3f(0.75, 0.0, -0.25);
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.75, 0.25);
    glVertex3f(0.0, 0.75, -0.25);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
    glVertex3f(-0.25, 0.75, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.25, 0.0, 0.75);
    glVertex3f(-0.25, 0.0, 0.75);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.25, 0.75);
    glVertex3f(0.0, -0.25, 0.75);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();
    
    glColor3ub(255, 255, 0);
    glRasterPos3f(1.1, 0.0, 0.0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'x');
    glRasterPos3f(0.0, 1.1, 0.0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'y');
    glRasterPos3f(0.0, 0.0, 1.1);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'z');
}

void
iidentity(GLdouble m[16])
{
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

GLboolean
invert(GLdouble src[16], GLdouble inverse[16])
{
    double t;
    int i, j, k, swap;
    GLdouble tmp[4][4];
    
    iidentity(inverse);
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp[i][j] = src[i*4+j];
        }
    }
    
    for (i = 0; i < 4; i++) {
        /* look for largest element in column. */
        swap = i;
        for (j = i + 1; j < 4; j++) {
            if (fabs(tmp[j][i]) > fabs(tmp[i][i])) {
                swap = j;
            }
        }
        
        if (swap != i) {
            /* swap rows. */
            for (k = 0; k < 4; k++) {
                t = tmp[i][k];
                tmp[i][k] = tmp[swap][k];
                tmp[swap][k] = t;
                
                t = inverse[i*4+k];
                inverse[i*4+k] = inverse[swap*4+k];
                inverse[swap*4+k] = t;
            }
        }
        
        if (tmp[i][i] == 0) {
        /* no non-zero pivot.  the matrix is singular, which
           shouldn't happen.  This means the user gave us a bad
            matrix. */
            return GL_FALSE;
        }
        
        t = tmp[i][i];
        for (k = 0; k < 4; k++) {
            tmp[i][k] /= t;
            inverse[i*4+k] /= t;
        }
        for (j = 0; j < 4; j++) {
            if (j != i) {
                t = tmp[j][i];
                for (k = 0; k < 4; k++) {
                    tmp[j][k] -= tmp[i][k]*t;
                    inverse[j*4+k] -= inverse[i*4+k]*t;
                }
            }
        }
    }
    return GL_TRUE;
}

float
normalize(float* v)
{
    float length;
    
    length = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
    
    return length;
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

void DLT()
{	
	assert((imClick == objClick)&&(imClick >= 6)); //必须保证图像与模型对应点数相同且>=6个
	double a1[12] = {0}, a2[12] = {0};
	double* A = new double[2*imClick*12];  
	double* B = new double[2*imClick];
	double* C = new double[2*imClick*11];  
	double* CT = new double[11*2*imClick];        //C的转置
	double CTC[11*11] = {0};
	double CTB[11*1] = {0};
	double L[12] = {0};
	a1[3] = 1;a2[7] = 1;L[11] = 1.0; //必须的初始化
	for(int i = 0; i < imClick; i++)
	{
		a1[0] = objCords[i][0]; a2[4] = objCords[i][0]; 
		a1[1] = objCords[i][1]; a2[5] = objCords[i][1];
		a1[2] = objCords[i][2]; a2[6] = objCords[i][2];
		
		a1[8]   = -1*objCords[i][0] * imCords[i][0]; a2[8]   = -1*objCords[i][0] * imCords[i][1];
		a1[9]   = -1*objCords[i][1] * imCords[i][0]; a2[9]   = -1*objCords[i][1] * imCords[i][1];
		a1[10] = -1*objCords[i][2] * imCords[i][0]; a2[10] = -1*objCords[i][2] * imCords[i][1];
		a1[11] = -1*imCords[i][0];                           a2[11] = -1*imCords[i][1];

		memcpy((A+(2*i)*12),a1,12*sizeof(double));
		memcpy((A+(2*i+1)*12),a2,12*sizeof(double));

		memcpy(C+(2*i)*11,a1,11*sizeof(double));
		memcpy(C+(2*i+1)*11,a2,11*sizeof(double));
		B[2*i] = a1[11];
		B[2*i+1] = a2[11];
	}
	//l = -1 * (CT*C)^-1 * CT * B;
	TranMatrix(C,CT,2*imClick,11);
	MultMatrix(CT,C,CTC,11,2*imClick,11);
	MultMatrix(CT,B,CTB,11,2*imClick,1);
	InverMatrix(CTC,11);
	MultMatrix(CTC,CTB,L,11,11,1);
	delete[] A;
	delete[] B;
	delete[] C;
	delete[] CT;

	double x = 0, u0 = 0, v0 = 0, ax = 0, ay = 0;
	double C1[3] = {0}, C2[3] = {0}; 
	double rota[16] = {0};
	rota[15] = 1.0;
    
    /*
    double homography[] = {
        4.40109366e-01, 3.92442352e-02, -3.84876052e+01,
        -7.10370840e-01, 5.32673161e-01, 3.85453016e+02,
        -1.13286420e-03, 1.93790472e-04, 1.00000000e+00
    };
     */
    // lugger
    double homography[] = {
        3.05626111e-01, 1.82391908e-02, 8.89773139e+00,
        -6.54682280e-01, 4.71682550e-01, 3.27711491e+02,
        -1.22366240e-03, 1.41183965e-04, 1.00000000e+00
    };
    double LTmp[12];
    memcpy(LTmp, L, sizeof(L));
    MultMatrix(homography, LTmp, L, 3, 3, 4);
    
	x = 1/sqrt(dot(L+8,L+8,3));
	u0 = x*x*dot(L,L+8,3);
	v0 = x*x*dot(L+4,L+8,3);
	cross(L,L+8,C1);
	cross(L+4,L+8,C2);
	ax = x*x*sqrt(dot(C1,C1,3));
	ay = x*x*sqrt(dot(C2,C2,3));

	SubMatrix(L,L+8,rota,u0,ax/x,1,3);
	SubMatrix(L+4,L+8,rota+4,v0,ay/x,1,3);
	SubMatrix(L+8,L+8,rota+8,0,1/x,1,3);

	TranMatrix(rota,rotation,4,4);
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
    case 'p':
        mode = PERSPECTIVE;
        break;
    case 'o':
        mode = ORTHO;
        break;
    case 'f':
        mode = FRUSTUM;
		break;
	case 'c':
        isRota = true;
        break;
    case 'r':
		isRota = false;
        perspective[0].value = 60.0;
        perspective[1].value = 1.0;
        perspective[2].value = 1.0;
        perspective[3].value = 10.0;
        ortho[0].value = -1.0;
        ortho[1].value = 1.0;
        ortho[2].value = -1.0;
        ortho[3].value = 1.0;
        ortho[4].value = 1.0;
        ortho[5].value = 3.5;
        frustum[0].value = -1.0;
        frustum[1].value = 1.0;
        frustum[2].value = -1.0;
        frustum[3].value = 1.0;
        frustum[4].value = 1.0;
        frustum[5].value = 3.5;
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
    /*glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width/height, 1.0, 256.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
    glRotatef(-45.0, 0.0, 1.0, 0.0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);*/
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
    //GLfloat light_pos[] = { 0.0, 0.0, 1.0, 0.0 };
    //double length;
    //float l[3];
    //
    //l[0] = lookat[3].value - lookat[0].value; 
    //l[1] = lookat[4].value - lookat[1].value; 
    //l[2] = lookat[5].value - lookat[2].value;
    //length = normalize(l);
    //
    //invert(modelview, inverse);
    //
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //if (world_draw) {
    //    glEnable(GL_LIGHTING);
    //    glPushMatrix();
    //    glMultMatrixd(inverse);
    //    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    //    glPopMatrix();
    //    drawmodel();
    //    glDisable(GL_LIGHTING);
    //}
    //
    //glPushMatrix();
    //
    //glMultMatrixd(inverse);
    //
    //glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    //
    ///* draw the axis and eye vector */
    //glPushMatrix();
    //glColor3ub(0, 0, 255);
    //glBegin(GL_LINE_STRIP);
    //glVertex3f(0.0, 0.0, 0.0);
    //glVertex3f(0.0, 0.0, -1.0*length);
    //glVertex3f(0.1, 0.0, -0.9*length);
    //glVertex3f(-0.1, 0.0, -0.9*length);
    //glVertex3f(0.0, 0.0, -1.0*length);
    //glVertex3f(0.0, 0.1, -0.9*length);
    //glVertex3f(0.0, -0.1, -0.9*length);
    //glVertex3f(0.0, 0.0, -1.0*length);
    //glEnd();
    //glColor3ub(255, 255, 0);
    //glRasterPos3f(0.0, 0.0, -1.1*length);
    //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'e');
    //glColor3ub(255, 0, 0);
    //glScalef(0.4, 0.4, 0.4);
    //drawaxes();
    //glPopMatrix();
    //
    //invert(projection, inverse);
    //glMultMatrixd(inverse);
    //
    ///* draw the viewing frustum */
    //glColor3f(0.2, 0.2, 0.2);
    //glBegin(GL_QUADS);
    //glVertex3i(1, 1, 1);
    //glVertex3i(-1, 1, 1);
    //glVertex3i(-1, -1, 1);
    //glVertex3i(1, -1, 1);
    //glEnd();
    //
    //glColor3ub(128, 196, 128);
    //glBegin(GL_LINES);
    //glVertex3i(1, 1, -1);
    //glVertex3i(1, 1, 1);
    //glVertex3i(-1, 1, -1);
    //glVertex3i(-1, 1, 1);
    //glVertex3i(-1, -1, -1);
    //glVertex3i(-1, -1, 1);
    //glVertex3i(1, -1, -1);
    //glVertex3i(1, -1, 1);
    //glEnd();
    //
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glColor4f(0.2, 0.2, 0.4, 0.5);
    //glBegin(GL_QUADS);
    //glVertex3i(1, 1, -1);
    //glVertex3i(-1, 1, -1);
    //glVertex3i(-1, -1, -1);
    //glVertex3i(1, -1, -1);
    //glEnd();
    //glDisable(GL_BLEND);
    //
    //glPopMatrix();
    //glutSwapBuffers();
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
            // 由于显示窗口有缩放和变形，imCords内存放的点使用的是图像坐标
            // 显示前先转化为窗口坐标
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
    /*switch (value) {
    case 'm':
        world_draw = !world_draw;
        break;
    }
    redisplay_all();*/
	    char* name = 0;
    char* txt_name = 0;
	isRota = value <= 0; //更新isRota
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
    

    //glutSetWindow(world);
    //texture();
    
    redisplay_all();
}

void
world_mouse(int button, int state, int x, int y)
{
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
    if (mode == PERSPECTIVE)
        gluPerspective(perspective[0].value, perspective[1].value, 
        perspective[2].value, perspective[3].value);
    else if (mode == ORTHO)
        glOrtho(ortho[0].value, ortho[1].value, ortho[2].value,
        ortho[3].value, ortho[4].value, ortho[5].value);
    else if (mode == FRUSTUM)
        glFrustum(frustum[0].value, frustum[1].value, frustum[2].value,
        frustum[3].value, frustum[4].value, frustum[5].value);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(lookat[0].value, lookat[1].value, lookat[2].value,
        lookat[3].value, lookat[4].value, lookat[5].value,
        lookat[6].value, lookat[7].value, lookat[8].value);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glClearColor(0.2, 0.2, 0.2, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void
screen_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	flag_rotation = imClick&&objClick;
	if(flag_rotation&&isRota)
	{
		GLdouble x[16] = {-1.0,0,0,0,
	                                 0,-1.0,0,0,
	                                 0,0,1.0,0,
	                                 0,0,0,1.0};
		glMultMatrixd(x);
		DLT();
		glMultMatrixd(rotation);
	}
	glRotatef(spin_y, 1.0, 0.0, 0.0);
    glRotatef(spin_x, 0.0, 1.0, 0.0);
	
	drawmodel();
	//画交互的点
	if(objClick)
	{
		//glPointSize(9.0);
		
		//glBegin(GL_QUADS);
		for(int i = 0; i < objClick; i++)
		{
			glPushMatrix();
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
			glColor3f(pointColor[i].red, pointColor[i].green, pointColor[i].blue);

		/*	glTranslatef((objCords[i][0]-pmodel->position[0])*model_scale,
							(objCords[i][1]-pmodel->position[1])*model_scale,
							(objCords[i][2]-pmodel->position[2])*model_scale);*/
			glTranslatef(objCords[i][0],objCords[i][1],objCords[i][2]);
			glutSolidSphere(0.04,10,10);
			glDisable(GL_COLOR_MATERIAL);
			glPopMatrix();
		}
		//glEnd();
	}
	//
    glutSwapBuffers();
}

void
screen_menu(int value)
{
    char* name = 0;
    char* txt_name = 0;
	isRota = value <= 0; //更新isRota
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
    
    if (mode == PERSPECTIVE) {
        drawstr(180, perspective[0].y-40, "fovy"); 
        drawstr(230, perspective[0].y-40, "aspect"); 
        drawstr(300, perspective[0].y-40, "zNear"); 
        drawstr(360, perspective[0].y-40, "zFar");
    } else {
        drawstr(120, perspective[0].y-40, "left"); 
        drawstr(180, perspective[0].y-40, "right"); 
        drawstr(230, perspective[0].y-40, "bottom");
        drawstr(310, perspective[0].y-40, "top"); 
        drawstr(360, perspective[0].y-40, "near");
        drawstr(420, perspective[0].y-40, "far");
    }
    
    if (mode == PERSPECTIVE) {
        drawstr(40, perspective[0].y, "gluPerspective(");
        drawstr(230, perspective[0].y, ","); 
        drawstr(290, perspective[0].y, ",");
        drawstr(350, perspective[0].y, ",");
        drawstr(410, perspective[0].y, ");");
    } else if (mode == FRUSTUM) {
        drawstr(20, frustum[0].y, "glFrustum(");
        drawstr(170, frustum[0].y, ",");
        drawstr(230, frustum[0].y, ","); 
        drawstr(290, frustum[0].y, ",");
        drawstr(350, frustum[0].y, ",");
        drawstr(410, frustum[0].y, ","); 
        drawstr(470, frustum[0].y, ");");
    } else {
        drawstr(35, ortho[0].y, "glOrtho(");
        drawstr(170, ortho[0].y, ",");
        drawstr(230, ortho[0].y, ","); 
        drawstr(290, ortho[0].y, ",");
        drawstr(350, ortho[0].y, ",");
        drawstr(410, ortho[0].y, ","); 
        drawstr(470, ortho[0].y, ");");
    }
    
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
    
    if (mode == PERSPECTIVE) {
        cell_draw(&perspective[0]);
        cell_draw(&perspective[1]);
        cell_draw(&perspective[2]);
        cell_draw(&perspective[3]); 
    } else if (mode == FRUSTUM) {
        cell_draw(&frustum[0]);
        cell_draw(&frustum[1]);
        cell_draw(&frustum[2]);
        cell_draw(&frustum[3]);
        cell_draw(&frustum[4]);
        cell_draw(&frustum[5]);
    } else if (mode == ORTHO) {
        cell_draw(&ortho[0]);
        cell_draw(&ortho[1]);
        cell_draw(&ortho[2]);
        cell_draw(&ortho[3]);
        cell_draw(&ortho[4]);
        cell_draw(&ortho[5]);
    }   
    
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
        if (mode == PERSPECTIVE) {
        /* mouse should only hit _one_ of the cells, so adding up all
            the hits just propagates a single hit. */
            selection += cell_hit(&perspective[0], x, y);
            selection += cell_hit(&perspective[1], x, y);
            selection += cell_hit(&perspective[2], x, y);
            selection += cell_hit(&perspective[3], x, y);
        } else if (mode == FRUSTUM) {
            selection += cell_hit(&frustum[0], x, y);
            selection += cell_hit(&frustum[1], x, y);
            selection += cell_hit(&frustum[2], x, y);
            selection += cell_hit(&frustum[3], x, y);
            selection += cell_hit(&frustum[4], x, y);
            selection += cell_hit(&frustum[5], x, y);
        } else if (mode == ORTHO) {
            selection += cell_hit(&ortho[0], x, y);
            selection += cell_hit(&ortho[1], x, y);
            selection += cell_hit(&ortho[2], x, y);
            selection += cell_hit(&ortho[3], x, y);
            selection += cell_hit(&ortho[4], x, y);
            selection += cell_hit(&ortho[5], x, y);
        }
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
    cell_update(&frustum[0], old_y-y);
    cell_update(&frustum[1], old_y-y);
    cell_update(&frustum[2], old_y-y);
    cell_update(&frustum[3], old_y-y);
    cell_update(&frustum[4], old_y-y);
    cell_update(&frustum[5], old_y-y);
    cell_update(&ortho[0], old_y-y);
    cell_update(&ortho[1], old_y-y);
    cell_update(&ortho[2], old_y-y);
    cell_update(&ortho[3], old_y-y);
    cell_update(&ortho[4], old_y-y);
    cell_update(&ortho[5], old_y-y);
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
    
    redisplay_all();
}

void
command_menu(int value)
{
    main_keyboard((unsigned char)value, 0, 0);
}

void
redisplay_all(void)
{
    glutSetWindow(command);
    glutPostRedisplay();
    glutSetWindow(world);
    world_reshape(sub_width, sub_height);
    glutPostRedisplay();
    glutSetWindow(screen);
    screen_reshape(sub_width, sub_height);
    glutPostRedisplay();
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
    //glutAddMenuEntry("Toggle model", 'm');
	glutAddMenuEntry("Images", 0);
    glutAddMenuEntry("", 0);
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
    glutAddMenuEntry("Models", 0);
    glutAddMenuEntry("", 0);
    glutAddMenuEntry("Soccerball", 's');
    glutAddMenuEntry("Al Capone", 'a');
    glutAddMenuEntry("F-16 Jet", 'j');
    glutAddMenuEntry("Dolphins", 'd');
    glutAddMenuEntry("Flowers", 'f');
    glutAddMenuEntry("Porsche", 'p');
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
    glutAddMenuEntry("", 0);
    glutAddMenuEntry("[o]  glOrtho", 'o');
    glutAddMenuEntry("[f]  glFrustum", 'f');
    glutAddMenuEntry("[p]  gluPerspective", 'p');
    glutAddMenuEntry("", 0);
    glutAddMenuEntry("[r]  Reset parameters", 'r');
    glutAddMenuEntry("", 0);
	glutAddMenuEntry("[c]  rotation", 'c');
	glutAddMenuEntry("", 0);
    glutAddMenuEntry("Quit", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    redisplay_all();
    
    glutMainLoop();
    
    return 0;
}
