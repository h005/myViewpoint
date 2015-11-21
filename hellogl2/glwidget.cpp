/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include <iostream>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "pointsmatchrelation.h"
#include "shader.hpp"

GLWidget::GLWidget(const QString &modelPath, QWidget *parent)
    : DragableWidget(parent),
      m_relation(NULL),
      m_sphereProgramID(0)
{
    model.load(modelPath.toLocal8Bit().data());
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(400, 400);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::cleanup()
{
    makeCurrent();
    if (m_sphereProgramID) {
        glDeleteProgram(m_sphereProgramID);
    }
    doneCurrent();
}

void GLWidget::initializeGL()
{
    // http://stackoverflow.com/a/8303331
    glewExperimental = GL_TRUE;
    
    GLenum err = glewInit();
    assert(err == GLEW_OK);
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    // Our camera never changes in this example.
    // Equal to:
    // m_camera = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -3.f));
    m_camera = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    // load data for model rendering
    model.bindDataToGL();

    // link program for drawing sphere
    m_sphereProgramID = LoadShaders("shader/sphereShader.vert", "shader/sphereShader.frag");
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(m_sphereProgramID, "vertexPosition_modelspace");

    GLuint args[] = {vertexPosition_modelspaceID};
    sphere.bindDataToGL(args);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE

    // 计算modelView矩阵
    glm::mat4 modelViewMatrix = m_camera * getModelMatrix();

    // 绘制模型
    model.drawNormalizedModel(modelViewMatrix, m_proj);

    // 绘制模型上被选择的点
    std::vector<glm::vec3> &points = m_relation->getPoints3d();
    if (points.size() > 0) {
        glUseProgram(m_sphereProgramID);
        GLuint projMatrixID = glGetUniformLocation(m_sphereProgramID, "projMatrix");
        GLuint mvMatrixID = glGetUniformLocation(m_sphereProgramID, "mvMatrix");
        glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_proj));

        std::vector<glm::vec3>::iterator it;
        for (it = points.begin(); it != points.end(); it++) {
            // multiple point's position
            glm::mat4 pointMV = glm::translate(modelViewMatrix, *it);
            pointMV = glm::scale(pointMV, glm::vec3(0.005, 0.005, 0.005));
            glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, glm::value_ptr(pointMV));
            sphere.draw();
        }
    }

}

void GLWidget::resizeGL(int w, int h)
{
    m_proj = glm::perspective(glm::pi<float>() / 3, GLfloat(w) / h, 0.01f, 100.0f);
}

glm::mat4 GLWidget::getModelViewMatrix()
{
    return m_camera * DragableWidget::getModelMatrix();
}


int GLWidget::addPoint(const QPoint &p) {
    makeCurrent();

    std::vector<glm::vec3> &points = m_relation->getPoints3d();
    GLfloat x = p.x();
    GLfloat y = p.y();

    GLint viewport[4];
    GLdouble object_x,object_y,object_z;
    GLfloat realy, winZ = 0;

    glGetIntegerv(GL_VIEWPORT, viewport);
    realy=(GLfloat)viewport[3] - (GLfloat)y;
    glReadBuffer(GL_BACK);
    glReadPixels(x,int(realy),1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);
    if (winZ < 1 - 1e-5) {
        glm::mat4 modelViewMatrix = getModelViewMatrix();
        glm::dmat4 mvDouble, projDouble;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mvDouble[i][j] = modelViewMatrix[i][j];
                projDouble[i][j] = m_proj[i][j];
            }
        }
        gluUnProject((GLdouble)x,(GLdouble)realy,winZ, glm::value_ptr(mvDouble), glm::value_ptr(projDouble),viewport,&object_x,&object_y,&object_z);
        points.push_back(glm::vec3(object_x, object_y, object_z));
    }

    doneCurrent();
    update();
    return points.size();
}

bool GLWidget::removeLastPoint() {
    std::vector<glm::vec3> &points = m_relation->getPoints3d();
    if (points.size() > 0) {
        points.pop_back();
        update();
        return true;
    } else
        return false;
}
