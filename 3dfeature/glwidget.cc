#include "glwidget.hh"
#include <iostream>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "shader.hh"
#include "trackball.hh"

GLWidget::GLWidget(MyMesh &in_mesh, QWidget *parent)
    : QOpenGLWidget(parent),
      m_mesh(in_mesh),
      m_angle(0),
      m_scale(1),
      m_rotateN(1.f),
      m_baseRotate(1.f),
      m_programID(0),
      m_helper(in_mesh)
{
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
    return QSize(800, 800);
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
    if (m_programID) {
        glDeleteProgram(m_programID);
    }
    m_helper.cleanup();
    doneCurrent();
}

void GLWidget::initializeGL()
{
    // http://stackoverflow.com/a/8303331
    glewExperimental = GL_TRUE;    
    GLenum err = glewInit();
    assert(err == GLEW_OK);

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    // Our camera never changes in this example.
    // Equal to:
    // m_camera = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -3.f));
    m_camera = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    // link program for drawing sphere
    m_programID = LoadShaders( "shader/sphereShader.vert", "shader/sphereShader.frag" );
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(m_programID, "vertexPosition_modelspace");
    m_helper.init(vertexPosition_modelspaceID);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE

    // 显示三角形网格，这样看得更清楚一些
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    // 计算modelView矩阵
    glm::mat4 modelViewMatrix = getModelViewMatrix();

    glUseProgram(m_programID);
    GLuint projMatrixID = glGetUniformLocation(m_programID, "projMatrix");
    GLuint mvMatrixID = glGetUniformLocation(m_programID, "mvMatrix");
    glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_proj));
    glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    m_helper.draw();
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj = glm::perspective(glm::pi<float>() / 3, GLfloat(w) / h, 0.01f, 100.0f);
}

/**
 * @brief GLWidget::getModelViewMatrix 由于这个窗口支持鼠标拖拽和缩放，所以m_camera并不是最终的变换矩阵
 * @return 渲染时使用的ModelView矩阵
 */
glm::mat4 GLWidget::getModelViewMatrix()
{
    return (m_camera
            * glm::scale(glm::mat4(1.f), glm::vec3(m_scale, m_scale, m_scale))
            * glm::rotate(glm::mat4(1.f), m_angle, m_rotateN)
            * m_baseRotate);
}

// 下面这些函数用户支持鼠标拖拽、滚轮缩放

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

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_scale += event->delta() / (120.f * 50);
    if (m_scale < 1)
        m_scale = 1;
    if (m_scale > 5)
        m_scale = 5;
    update();
}
