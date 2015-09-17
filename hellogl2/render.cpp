#include "render.h"
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

using std::cout;
using std::endl;

OffscreenRender::OffscreenRender(const QString &modelPath, QWidget *parent)
    : QOpenGLWidget(parent)
{
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent)
        setAttribute(Qt::WA_TranslucentBackground);
    model.load(modelPath.toLocal8Bit().data());
}

OffscreenRender::~OffscreenRender()
{
    cleanup();
}

QSize OffscreenRender::minimumSizeHint() const
{
    return QSize(400, 400);
}

QSize OffscreenRender::sizeHint() const
{
    return QSize(400, 400);
}

void OffscreenRender::cleanup()
{
    makeCurrent();

    // Something to clean
    if (depthRenderBuffer != 0)
        glDeleteRenderbuffers(1, &depthRenderBuffer);
    if (colorRenderBuffer != 0)
        glDeleteRenderbuffers(1, &colorRenderBuffer);
    if (fboId != 0)
        glDeleteFramebuffers(1, &fboId);

    doneCurrent();
}

void OffscreenRender::initializeGL()
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
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OffscreenRender::cleanup);
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    // load data for model rendering
    model.bindDataToGL();
}

void OffscreenRender::createFrameBufferObject()
{
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 400, 400);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

    glGenRenderbuffers(1, &colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 400, 400);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    qDebug()<<"FramebufferName fbo...init"<<endl;
}

void OffscreenRender::renderToImageFile(glm::mat4 mvMatrix, glm::mat4 projMatrix, QString filePath)
{
    makeCurrent();

    // 默认开启背面剔除:GL_CULL_FACE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 绘制模型
    model.drawNormalizedModel(mvMatrix, projMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    GLfloat *img0 = new GLfloat[(viewport[2]-viewport[0])*(viewport[3]-viewport[1])];
    glReadBuffer(GL_BACK_LEFT);
    glReadPixels(0,0,viewport[2],viewport[3],GL_DEPTH_COMPONENT,GL_FLOAT,img0);

    cv::Mat image0 = cv::Mat(viewport[3],viewport[2],CV_32FC1,img0);
    cv::namedWindow("test0");
    imshow("test0",image0);


    GLubyte *img =
            new GLubyte[(viewport[2]-viewport[0])
            *(viewport[3]-viewport[1])*4];
    glReadBuffer(GL_BACK_LEFT);
    glReadPixels(0,
            0,
            viewport[2],
            viewport[3],
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            img);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    qDebug()<<"show fbo info...ok"<<endl;

    qDebug()<<"save to " << filePath;
    cv::Mat image = cv::Mat(viewport[3],viewport[2],CV_8UC4,img);
    cv::imwrite(filePath.toUtf8().constData(), image);
    qDebug() << "...ok";

    doneCurrent();
}

void OffscreenRender::paintGL()
{
    // 这个窗口纯粹是为了使用fbo而开的，所以paintGL中不需要做什么
}
