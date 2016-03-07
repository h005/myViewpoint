#include "ccmodelwidget.h"
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
#include <QDebug>

#include "shader.hpp"

CCModelWidget::CCModelWidget(const QString &modelPath,
                             QWidget *parent) : DragableWidget(parent),
                                m_relation(NULL),
                                m_sphereProgramID(0)
{
    model.load(modelPath.toLocal8Bit().data());
    std::pair<GLfloat,glm::mat4> scaleAndShift = model.recommandScaleAndShift();
    m_scaleBeforeRender = scaleAndShift.first;
    m_shiftBeforeRender = scaleAndShift.second;
    f_fixMVP = false;

    std::cout << "ccmodelwidget transform matrix" << std::endl;
    glm::mat4 tmp = model.getInnerTransformation();
    std::cout << glm::to_string(tmp) << std::endl;
    std::cout << "ccmodelwidget transform matrix done" << std::endl;
}

CCModelWidget::CCModelWidget(const QString &modelPath,
                             glm::mat4 mv,
                             glm::mat4 proj,
                             QWidget *parent) : DragableWidget(parent),
                                m_relation(NULL),
                                m_sphereProgramID(0),
                                fix_mv(mv),
                                m_proj(proj)
{
    model.load(modelPath.toLocal8Bit().data());
    std::pair<GLfloat,glm::mat4> scaleAndShift = model.recommandScaleAndShift();
    m_scaleBeforeRender = scaleAndShift.first;
    m_shiftBeforeRender = scaleAndShift.second;
    f_fixMVP = true;
}

CCModelWidget::~CCModelWidget()
{
    cleanup();
}

QSize CCModelWidget::minimumSizeHint() const
{
    return QSize(400,400);
}

QSize CCModelWidget::sizeHint() const
{
    return QSize(400,400);
}

int CCModelWidget::addPoint(const QPoint &p)
{
    makeCurrent();
    std::vector<glm::vec3> &points = m_relation->getModelPoints();
    GLfloat x = p.x();
    GLfloat y = p.y();

    GLint viewport[4];
    GLdouble object_x, object_y, object_z;
    GLfloat realy, winZ = 0;

    glGetIntegerv(GL_VIEWPORT,viewport);
    realy = (GLfloat)viewport[3] - (GLfloat)y;
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

bool CCModelWidget::removeLastPoint()
{
    std::vector<glm::vec3> &points = m_relation->getModelPoints();
    if(points.size() > 0)
    {
        points.pop_back();
        update();
        return true;
    }
    else
        return false;
}

void CCModelWidget::fixMVP(glm::mat4 &mv, glm::mat4 &proj)
{
    this->fix_mv = mv;
    m_proj = proj;
    f_fixMVP  = !f_fixMVP;
    update();
}

void CCModelWidget::clearRelations()
{
    m_relation->clearPoints();
    update();
}

void CCModelWidget::getScaleTranslateMatrix(glm::mat4 &cc_st)
{
    cc_st =  glm::scale(glm::mat4(1.f), glm::vec3(m_scaleBeforeRender)) * m_shiftBeforeRender;
    return;
}

void CCModelWidget::getCCMVPmatrix(glm::mat4 &ccMV, glm::mat4 &ccProj)
{
    ccMV = getModelMatrix();
    ccProj = m_proj;
}

void CCModelWidget::getSift()
{
    render2Image();
    cc_sift = new CCSift(renderImage);
//    cc_sift->showSift("renderImage");
}

CCSift *CCModelWidget::getCCSift()
{
    return cc_sift;
}

void CCModelWidget::siftMatch(CCSift *cc_sift)
{
    this->cc_sift->match(cc_sift);
}

void CCModelWidget::cleanup()
{
    // 仅清理该子类生成的对象
    makeCurrent();

    if (m_sphereProgramID) {
        glDeleteProgram(m_sphereProgramID);
        m_sphereProgramID = 0;
    }
    sphere.cleanup();
    model.cleanUp();

    doneCurrent();
}

void CCModelWidget::render2Image()
{
    makeCurrent();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    GLfloat *img = new GLfloat[(viewport[2] - viewport[0]) * (viewport[3] - viewport[1])];
    glReadBuffer(GL_BACK);
    glReadPixels(0,
                 0,
                 viewport[2],
                 viewport[3],
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 img);

    // 从这样构造出来的mat是BGRA的
    renderImage = cv::Mat(viewport[3],viewport[2],CV_8UC4,img);
    cv::flip(renderImage,renderImage,0);
    // 这里要转一下，不然sift特征无法画在图像上
    cv::cvtColor(renderImage,renderImage,CV_BGRA2BGR);
//    cv::namedWindow("render");
//    cv::imshow("render",renderImage);
//    cv::waitKey(0);

    doneCurrent();
}

void CCModelWidget::initializeGL()
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

void CCModelWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE
    // 计算modelView矩阵
    glm::mat4 modelViewMatrix;
    if(!f_fixMVP)
        modelViewMatrix = m_camera * getModelMatrix();
    else
        modelViewMatrix = fix_mv;
    // 绘制模型
    model.draw(modelViewMatrix, m_proj);
    // 绘制模型上被选择的点
    std::vector<glm::vec3> &points = m_relation->getModelPoints();
    if (points.size() > 0) {
        glUseProgram(m_sphereProgramID);
        GLuint projMatrixID = glGetUniformLocation(m_sphereProgramID, "projMatrix");
        GLuint mvMatrixID = glGetUniformLocation(m_sphereProgramID, "mvMatrix");
        glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_proj));

        std::vector<glm::vec3>::iterator it;
        for (it = points.begin(); it != points.end(); it++) {
            // multiple point's position ball draw at (*it)
glm::mat4 pointMV = glm::translate(modelViewMatrix, *it);
            pointMV = glm::scale(pointMV, glm::vec3(0.015 / m_scaleBeforeRender));
            glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, glm::value_ptr(pointMV));
            sphere.draw();
        }
    }
}

void CCModelWidget::resizeGL(int w, int h)
{
    m_proj = glm::perspective(glm::pi<float>() / 3, GLfloat(w) / h, 0.01f, 100.0f);
}

glm::mat4 CCModelWidget::getModelViewMatrix()
{
    return m_camera * getModelMatrix();
}

glm::mat4 CCModelWidget::getModelMatrix()
{
    return DragableWidget::getModelMatrix() * glm::scale(glm::mat4(1.f), glm::vec3(m_scaleBeforeRender)) * m_shiftBeforeRender;
}
