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
    virtual ~GLOffscreenRenderFramework();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    GLuint fboId = 0, depthRenderBuffer = 0, colorRenderBuffer = 0, secondRenderBuffer = 0;
    QSize m_windowSize;
    bool m_transparent;
    void createOrUpdateFrameBufferObject();
};
