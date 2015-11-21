#include "pointcloudoffscreenrender.h"
#include <opencv2/opencv.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv2/opencv.hpp>
#include "shader.hpp"

PointCloudOffscreenRender::PointCloudOffscreenRender(const std::string &modelPath, QWidget *parent)
    : GLOffscreenRenderFramework(QSize(800, 800), parent),
      m_renderObject(modelPath)
{
    m_renderObject.load();
}

void PointCloudOffscreenRender::renderToImageFile(glm::mat4 mvMatrix, glm::mat4 projMatrix, QString filePath, QSize dsize)
{
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    int bwidth = viewport[2], bheight = viewport[3];

    // 渲染代码，和widget中相同
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FLAT);
    glShadeModel(GL_FLAT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glm::mat4 MVP = projMatrix * mvMatrix;
    glUseProgram(m_programID);
    GLuint mvpID = glGetUniformLocation(m_programID, "MVP");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(MVP));
    m_renderObject.draw();

    // 从fbo中读取
    // 图片的具体类型要看shader对应位置上的输出
    // 此处为vec4，所以大小也填4
    GLubyte *img = new GLubyte[bwidth * bheight * 4];
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0,
            0,
            bwidth,
            bheight,
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            img);

    // 创建图片并写入路径
    // 由于OpenGL坐标原点位于左下角，保存前需要沿着x轴翻转图片
    cv::Mat image = cv::Mat(bwidth, bheight, CV_8UC4, img);
    cv::Mat flipped;
    cv::flip(image, flipped, 0);

    cv::Mat final;
    cv::resize(flipped, final, cv::Size(dsize.width(), dsize.height()));
    cv::imwrite(filePath.toUtf8().constData(), final);

    delete img;
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    doneCurrent();
}

void PointCloudOffscreenRender::initializeGL()
{
    GLOffscreenRenderFramework::initializeGL();

    // link program for drawing sphere
    m_programID = LoadShaders( "shader/ptCloudShader.vert", "shader/ptCloudShader.frag" );
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(m_programID, "vertexPosition_modelspace");
    GLuint vertexColorID = glGetAttribLocation(m_programID, "vertexColor");

    GLuint args[] = {vertexPosition_modelspaceID, vertexColorID};
    m_renderObject.bindDataToGL(args);
}

