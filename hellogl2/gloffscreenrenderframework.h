#pragma once

#include <GL/glew.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class QString;
class GLOffscreenRenderFramework : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLOffscreenRenderFramework(QSize windowSize, QWidget *parent = 0);
    ~GLOffscreenRenderFramework();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    virtual void cleanup();

signals:

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    GLuint fboId = 0, depthRenderBuffer = 0, colorRenderBuffer = 0;
    QSize m_windowSize;
    bool m_transparent;
    void createFrameBufferObject();
};