﻿#include "pointcloudwidget.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <qdebug.h>
#include "shader.hpp"

PointCloudWidget::PointCloudWidget(const std::string &plyPath, QWidget *parent)
    : DragableWidget(parent),
      m_renderObject(plyPath)
{
    m_renderObject.load();
    m_scaleAndShift = m_renderObject.recommendScaleAndShift();
}

PointCloudWidget::~PointCloudWidget()
{
    // 仅清理改子类生成的对象
    makeCurrent();

    m_sphereObject.cleanup();
    m_renderObject.cleanup();
    if (m_programID) {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
    if (m_sphereProgramID) {
        glDeleteProgram(m_sphereProgramID);
        m_sphereProgramID = 0;
    }

    doneCurrent();
}

void PointCloudWidget::initializeGL()
{
    // http://stackoverflow.com/a/8303331
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    assert(err == GLEW_OK);

    initializeOpenGLFunctions();
    glClearColor( 0.368, 0.368, 0.733, 1);

    // Our camera never changes in this example.
    // Equal to:
    // m_camera = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -3.f));
    m_camera = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    // link program for drawing sphere
    m_programID = LoadShaders( "shader/ptCloudShader.vert", "shader/ptCloudShader.frag" );
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(m_programID, "vertexPosition_modelspace");
    GLuint vertexColorID = glGetAttribLocation(m_programID, "vertexColor");
    GLuint args[] = {vertexPosition_modelspaceID, vertexColorID};
    m_renderObject.bindDataToGL(args);
}

void PointCloudWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE

    // 粗糙渲染，看得更清楚
    glEnable(GL_FLAT);
    glShadeModel(GL_FLAT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // 显示三角形网格，这样看得更清楚一些
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    // 计算MVP矩阵
    glm::mat4 MVP = m_proj * m_camera * getModelMatrix();

    glUseProgram(m_programID);
    GLuint mvpID = glGetUniformLocation(m_programID, "MVP");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(MVP));
    m_renderObject.draw();
}

void PointCloudWidget::resizeGL(int width, int height)
{
    m_proj = glm::perspective(glm::pi<float>() / 3, GLfloat(width) / height, 0.01f, 100.0f);
}

glm::mat4 PointCloudWidget::getModelMatrix()
{
    return DragableWidget::getModelMatrix() * m_scaleAndShift;
}

int PointCloudWidget::addPoint(const QPoint &p) {
    return 0;
//    static int dx[] = {0, 0, 1, 0, -1};
//    static int dy[] = {0, 1, 0, -1, 0};

//    makeCurrent();

//    std::vector<glm::vec3> &points = m_points;
//    GLfloat x = p.x();
//    GLfloat y = p.y();

//    GLint viewport[4];
//    GLdouble object_x,object_y,object_z;
//    GLfloat realy, winZ = 0;

//    glGetIntegerv(GL_VIEWPORT, viewport);
//    realy=(GLfloat)viewport[3] - (GLfloat)y;
//    glReadBuffer(GL_BACK);
//    for (int i = 0; i < sizeof(dx) / sizeof(int); i++) {
//        int xx = x + dx[i];
//        int yy = int(realy) + dy[i];
//        glReadPixels(xx,yy,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);
//        if (winZ < 1 - 1e-5) {
//            glm::mat4 modelViewMatrix = m_camera * getModelMatrix();
//            glm::dmat4 mvDouble, projDouble;
//            for (int i = 0; i < 4; i++) {
//                for (int j = 0; j < 4; j++) {
//                    mvDouble[i][j] = modelViewMatrix[i][j];
//                    projDouble[i][j] = m_proj[i][j];
//                }
//            }
//            gluUnProject((GLdouble)xx,(GLdouble)yy,winZ, glm::value_ptr(mvDouble), glm::value_ptr(projDouble),viewport,&object_x,&object_y,&object_z);
//            points.push_back(glm::vec3(object_x, object_y, object_z));
//        }

//    }

//    doneCurrent();
//    update();
//    return points.size();
}

bool PointCloudWidget::removeLastPoint() {
    return true;
//    std::vector<glm::vec3> &points = m_points;
//    if (points.size() > 0) {
//        points.pop_back();
//        update();
//        return true;
//    } else
//        return false;
}

