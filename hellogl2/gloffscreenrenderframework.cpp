#include "gloffscreenrenderframework.h"
#include <iostream>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <assert.h>

using std::cout;
using std::endl;

GLOffscreenRenderFramework::GLOffscreenRenderFramework(QSize windowSize, QWidget *parent)
    : QOpenGLWidget(parent),
      m_windowSize(windowSize)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent)
        setAttribute(Qt::WA_TranslucentBackground);
}

GLOffscreenRenderFramework::~GLOffscreenRenderFramework()
{
    makeCurrent();

    // 删除fbo相关结构
    if (depthRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &depthRenderBuffer);
        depthRenderBuffer = 0;
    }
    if (colorRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &colorRenderBuffer);
        colorRenderBuffer = 0;
    }
    if (fboId != 0) {
        glDeleteFramebuffers(1, &fboId);
        fboId = 0;
    }

    doneCurrent();
}

QSize GLOffscreenRenderFramework::minimumSizeHint() const
{
    return QSize(m_windowSize.width(), m_windowSize.height());
}

QSize GLOffscreenRenderFramework::sizeHint() const
{
    return QSize(m_windowSize.width(), m_windowSize.height());
}

void GLOffscreenRenderFramework::resizeGL(int width, int height)
{
    m_windowSize = QSize(width, height);

    //创建或更新fbo
    // 需要根据窗口的尺寸调整fbo中帧缓冲的尺寸
    createOrUpdateFrameBufferObject();
}

void GLOffscreenRenderFramework::initializeGL()
{
    // http://stackoverflow.com/a/8303331
    glewExperimental = GL_TRUE;

    GLenum err = glewInit();
    assert(err == GLEW_OK);
    initializeOpenGLFunctions();
    glClearColor( 0.368, 0.368, 0.733, 1);
}

void GLOffscreenRenderFramework::createOrUpdateFrameBufferObject()
{
    if (fboId == 0)
        glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    if (depthRenderBuffer == 0)
        glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_windowSize.width(), m_windowSize.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

    if (colorRenderBuffer == 0)
        glGenRenderbuffers(1, &colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_windowSize.width(), m_windowSize.height()); // 类型跟shader中的输出对应
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    qDebug()<<"FramebufferName fbo...init"<<endl;
}


