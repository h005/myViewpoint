#pragma once

#include "gloffscreenrenderframework.h"
#include <glm/glm.hpp>
#include "GModel.h"
#include "RenderObject/sphere.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class QString;
class OffscreenRender : public GLOffscreenRenderFramework
{
    Q_OBJECT

public:
    OffscreenRender(const QString &modelPath, QWidget *parent = 0);
    ~OffscreenRender();
    void renderToImageFile(glm::mat4 mvMatrix, glm::mat4 projMatrix, QString filePath, QSize dsize);

protected:
    void initializeGL() Q_DECL_OVERRIDE;

protected:
    GModel model;
};
