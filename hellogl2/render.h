#pragma once

#include <GL/glew.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <glm/glm.hpp>
#include "GModel.h"
#include "sphere.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class QString;
class OffscreenRender : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    OffscreenRender(const QString &modelPath, QWidget *parent = 0);
    ~OffscreenRender();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    void renderToImageFile(glm::mat4 mvMatrix, glm::mat4 projMatrix, QString filePath);

public slots:
    virtual void cleanup();

signals:

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void createFrameBufferObject();
    void printFramebufferInfo();

protected:
    GModel model;
    GLuint fboId = 0, depthRenderBuffer = 0, colorRenderBuffer = 0;
    bool fboUsed;

private:
    bool m_transparent;
};
