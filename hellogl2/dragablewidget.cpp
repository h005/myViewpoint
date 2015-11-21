#include "dragablewidget.h"
#include <iostream>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "trackball.h"

DragableWidget::DragableWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_angle(0),
      m_scale(1),
      m_rotateN(1.f),
      m_baseRotate(1.f)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

DragableWidget::~DragableWidget()
{
}

void DragableWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void DragableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // 注意新增加的旋转量是左乘，与paintGL中一致
    glm::mat4 leftRotationMatrix = glm::rotate(glm::mat4(1.f), m_angle, m_rotateN);
    m_baseRotate = leftRotationMatrix * m_baseRotate;
    // 重点是将m_angle清零，因为旋转已经被融合进m_baseRotate了
    m_angle = 0.f;
    m_rotateN = glm::vec3(1.f);
}

void DragableWidget::mouseMoveEvent(QMouseEvent *event)
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

void DragableWidget::wheelEvent(QWheelEvent *event)
{
    m_scale += event->delta() / (120.f * 50);
    if (m_scale < 1)
        m_scale = 1;
    if (m_scale > 5)
        m_scale = 5;
    update();
}

glm::mat4 DragableWidget::getModelViewMatrix()
{
    return (m_camera
            * glm::scale(glm::mat4(1.f), glm::vec3(m_scale, m_scale, m_scale))
            * glm::rotate(glm::mat4(1.f), m_angle, m_rotateN)
            * m_baseRotate);
}

glm::mat4 DragableWidget::getModelMatrix()
{
    return (glm::scale(glm::mat4(1.f), glm::vec3(m_scale, m_scale, m_scale))
            * glm::rotate(glm::mat4(1.f), m_angle, m_rotateN)
            * m_baseRotate);
}

