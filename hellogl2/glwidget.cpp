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
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "trackball.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_angle(0),
      m_rotateN(1.f),
      m_baseRotate(1.f)
{
    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent)
        setAttribute(Qt::WA_TranslucentBackground);
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
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
    doneCurrent();
}

void GLWidget::initializeGL()
{
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
    // m_camera = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -2.f));
    m_camera = glm::lookAt(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    model.load("D:\\no4\\models\\model.dae");
    model.bindDataToGL();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // 计算modelView矩阵
    glm::mat4 worldTransform = glm::rotate(glm::mat4(1.f), m_angle, m_rotateN);
    worldTransform *= m_baseRotate;
    glm::mat4 modelViewMatrix = m_camera * worldTransform;
    model.drawNormalizedModel(modelViewMatrix, m_proj);
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj = glm::perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // 注意新增加的旋转量是左乘，与paintGL中一致
    glm::mat4 leftRotationMatrix = glm::rotate(glm::mat4(1.f), m_angle, m_rotateN);
    m_baseRotate = leftRotationMatrix * m_baseRotate;
    // 重点是将m_angle清零，因为旋转已经被融合进m_baseRotate了
    m_angle = 0.f;
    m_rotateN = glm::vec3(1.f);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float width = this->width();
    float height = this->height();

    glm::vec2 a, b;
    a.x = (m_lastPos.x() - width / 2.f) / (width / 2.f);
    a.y = (height / 2.f - m_lastPos.y()) / (height / 2.f);
    b.x = (event->pos().x() - width / 2.f) / (width / 2.f);
    b.y = (height / 2.f - event->pos().y()) / (height / 2.f);

    computeRotation(a, b, m_rotateN, m_angle);
    update();
}
