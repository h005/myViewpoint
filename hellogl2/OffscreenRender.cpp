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

OffscreenRender::~OffscreenRender()
{
    // 仅需负责子类生成对象的清理
    makeCurrent();
    model.cleanUp();
    doneCurrent();
}

void OffscreenRender::initializeGL()
{
    GLOffscreenRenderFramework::initializeGL();
    model.bindDataToGL();
}


void OffscreenRender::renderToImageFile(glm::mat4 mvMatrix, glm::mat4 projMatrix, QString filePath, QString depthFilePath, QSize dsize)
{
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    std::cout << "viewport " << viewport[0] << " " << viewport[1] << " " << viewport[2] << " " << viewport[3] << std::endl;

    // 默认开启背面剔除:GL_CULL_FACE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 绘制模型
    model.draw(mvMatrix, projMatrix);

    // 从fbo中读取

    if (!filePath.isNull())
    {
        std::vector<GLubyte> img((viewport[2] - viewport[0])*(viewport[3] - viewport[1])*4);
        glReadBuffer(GL_BACK_LEFT);
        glReadPixels(0,
                    0,
                    (viewport[2] - viewport[0]),
                    (viewport[3] - viewport[1]),
                    GL_BGRA,
                    GL_UNSIGNED_BYTE,
                    &img[0]);

        // 创建图片并写入路径
        // 由于OpenGL坐标原点位于左下角，保存前需要沿着x轴翻转图片
        qDebug()<<"save to" << filePath;
        cv::Mat image = cv::Mat((viewport[3] - viewport[1]), (viewport[2] - viewport[0]),CV_8UC4, &img[0]);
        cv::Mat flipped;
        cv::flip(image, flipped, 0);

        cv::Mat final;
        cv::resize(flipped, final, cv::Size(dsize.width(), dsize.height()));
        cv::imwrite(filePath.toUtf8().constData(), final);
    }

    if (!depthFilePath.isNull())
    {
        std::vector<GLfloat> img(BUFFER_WIDTH * BUFFER_HEIGHT);

        // 读到的深度位于[0, 1]之间
        glReadBuffer(GL_BACK);
        glReadPixels(0,0,BUFFER_WIDTH,BUFFER_HEIGHT,GL_DEPTH_COMPONENT,GL_FLOAT, &img[0]);

        GLfloat min_val = 2;
        for (auto it = img.begin(); it != img.end(); it++)
            min_val = std::min<GLfloat>(*it, min_val);

        // 创建图片并写入路径
        // 由于OpenGL坐标原点位于左下角，保存前需要沿着x轴翻转图片
        qDebug()<<"save to" << depthFilePath;
        cv::Mat image = cv::Mat(BUFFER_WIDTH, BUFFER_HEIGHT,CV_32FC1,&img[0]);

        // 深度是非线性变换，大部分值会集中在1附近，为了前景看得更明显需要作区间转换
        cv::Mat ucharMat;
        image.convertTo(ucharMat, CV_8UC1, 255.0 / (1 - min_val), -255.0 * min_val / (1 - min_val));

        cv::Mat flipped;
        cv::flip(ucharMat, flipped, 0);

        cv::Mat final;
        cv::resize(flipped, final, cv::Size(dsize.width(), dsize.height()));
        cv::imwrite(depthFilePath.toUtf8().constData(), final);
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    doneCurrent();
}
