﻿#include "camerashowwidget.h"

#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "TransformationUtils.h"
#include "shader.hpp"
#include "colormap.h"

CameraShowWidget::CameraShowWidget(const QString &modelPath, const float imgRatio, const std::vector<glm::mat4> &mvMatrixs, const std::vector<int> &clusterIndices, QWidget *parent)
    :GLWidget(modelPath, parent)
{
    m_imgRatio = imgRatio;
    for (auto it = mvMatrixs.begin(); it != mvMatrixs.end(); it++)
        m_estimatedMVMatrixs.push_back(glm::inverse(*it));

    m_clusterIndices = clusterIndices;
    m_cameraModel.load("camera/camera_adjust.obj");
}

CameraShowWidget::~CameraShowWidget()
{
    makeCurrent();

    // axis不占有显存资源，无需清理
    model.cleanUp();

    if (_axisProgramID) {
        glDeleteProgram(_axisProgramID);
        _axisProgramID = 0;
    }

    doneCurrent();
}

QSize CameraShowWidget::minimumSizeHint() const
{
    return QSize((int)(720 * m_imgRatio), 720);
}

QSize CameraShowWidget::sizeHint() const
{
    return QSize((int)(720 * m_imgRatio), 720);
}

void CameraShowWidget::initializeGL()
{
    GLWidget::initializeGL();

    m_cameraModel.bindDataToGL();

    GLuint args[] = {0};
    m_axis.bindDataToGL(args);

    _axisProgramID = LoadShaders("shader/axisShader.vert", "shader/axisShader.frag");
}

void CameraShowWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE

    // 计算modelView矩阵
    glm::mat4 modelViewMatrix = getModelViewMatrix();

    // 绘制模型
    model.draw(modelViewMatrix, m_proj);

    // 绘制坐标系
    glUseProgram(_axisProgramID);
    GLuint projMatrixID = glGetUniformLocation(m_sphereProgramID, "projMatrix");
    GLuint mvMatrixID = glGetUniformLocation(m_sphereProgramID, "mvMatrix");
    for (int i = 0; i < m_estimatedMVMatrixs.size(); i++) {
        glm::mat4 axisMV = modelViewMatrix * (m_estimatedMVMatrixs[i]);
        //axisMV = glm::scale(axisMV, glm::vec3(500.f));
        axisMV = glm::scale(axisMV, glm::vec3(100.f));
        glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_proj));
        glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, glm::value_ptr(axisMV));

        if (m_clusterIndices[i] != 2)
            continue;
        double color[3];
        ColorMap::jet(color, m_clusterIndices[i], 0, 8);
        glColor3ub(color[0] * 255, color[1]  * 255, color[2] * 255);
        m_axis.draw();
    }

//    for (auto it = m_estimatedMVMatrixs.begin(); it != m_estimatedMVMatrixs.end(); it++) {
//        glm::mat4 axisMV = modelViewMatrix * (*it);
//        axisMV = glm::scale(axisMV, glm::vec3(0.5 / m_scaleBeforeRender));
//        m_cameraModel.draw(axisMV, m_proj);
//    }

}

glm::mat4 CameraShowWidget::getModelViewMatrix()
{
    // 缩放矩阵不能放在m_camera之前，否则是在相机坐标系下等比例缩放
    // 会得到一致的渲染结果（除了离相机更近以外）
    return glm::scale(m_camera, glm::vec3(0.3)) * getModelMatrix();
}
