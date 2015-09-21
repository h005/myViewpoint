#include "OffscreenRender.h"
#include <iostream>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv2/opencv.hpp>
#include "shader.hpp"

#define BUFFER_WIDTH 800
#define BUFFER_HEIGHT 800

using std::cout;
using std::endl;

OffscreenRender::OffscreenRender(const QString &modelPath, QWidget *parent)
    : GLOffscreenRenderFramework(QSize(800, 800), parent)
{
    model.load(modelPath.toLocal8Bit().data());
}

void OffscreenRender::initializeGL()
{
    GLOffscreenRenderFramework::initializeGL();
    model.bindDataToGL();
}


void OffscreenRender::renderToImageFile(glm::mat4 mvMatrix, glm::mat4 projMatrix, QString filePath)
{
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    qDebug() << viewport[0] << viewport[1] << viewport[2] << viewport[3];

    // 默认开启背面剔除:GL_CULL_FACE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 绘制模型
    model.drawNormalizedModel(mvMatrix, projMatrix);

    // 从fbo中读取
    GLubyte *img = new GLubyte[BUFFER_WIDTH * BUFFER_HEIGHT * 4];
    glReadBuffer(GL_BACK_LEFT);
    glReadPixels(0,
            0,
            BUFFER_WIDTH,
            BUFFER_HEIGHT,
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            img);

    // 创建图片并写入路径
    // 由于OpenGL坐标原点位于左下角，保存前需要沿着x轴翻转图片
    qDebug()<<"save to" << filePath;
    cv::Mat image = cv::Mat(BUFFER_WIDTH, BUFFER_HEIGHT,CV_8UC4,img);
    cv::Mat flipped;
    cv::flip(image, flipped, 0);
    cv::imwrite(filePath.toUtf8().constData(), flipped);
    qDebug() << "...ok";

    delete img;
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    doneCurrent();
}
