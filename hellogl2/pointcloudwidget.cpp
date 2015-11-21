#include "pointcloudwidget.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

}

void PointCloudWidget::cleanup()
{
    makeCurrent();
    if (m_programID)
        glDeleteProgram(m_programID);
    doneCurrent();
}

void PointCloudWidget::initializeGL()
{
    // http://stackoverflow.com/a/8303331
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    assert(err == GLEW_OK);

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &PointCloudWidget::cleanup);
    initializeOpenGLFunctions();
    glClearColor( 0.368, 0.368, 0.733, 1);

    // Our camera never changes in this example.
    // Equal to:
    // m_camera = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -3.f));
    m_camera = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    // link program for drawing sphere
    m_programID = LoadShaders( "lightShader.vert", "lightShader.frag" );
    GLuint vertexNormal_modelspaceID = glGetAttribLocation(m_programID, "vertexNormal_modelspace");
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(m_programID, "vertexPosition_modelspace");

    GLuint args[] = {vertexPosition_modelspaceID, vertexNormal_modelspaceID};
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

    // 计算modelView矩阵
    glm::mat4 M = getModelMatrix();
    glm::mat4 V = m_camera;
    glm::mat4 MV = V * M;
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(MV));
    glm::mat4 MVP = m_proj * MV;
    glm::vec3 lightPos = glm::vec3(4,4,4);

    glUseProgram(m_programID);
    GLuint mvpID = glGetUniformLocation(m_programID, "MVP");
    GLuint mID = glGetUniformLocation(m_programID, "M");
    GLuint vID = glGetUniformLocation(m_programID, "V");
    GLuint nID = glGetUniformLocation(m_programID, "normalMatrix");
    GLuint LightID = glGetUniformLocation(m_programID, "LightPosition_worldspace");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(MVP));
    glUniformMatrix4fv(mID, 1, GL_FALSE, glm::value_ptr(M));
    glUniformMatrix4fv(vID, 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(nID, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
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

