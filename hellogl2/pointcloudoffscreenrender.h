#ifndef POINTCLOUDOFFSCREENRENDER_H
#define POINTCLOUDOFFSCREENRENDER_H

#include "gloffscreenrenderframework.h"
#include "RenderObject/plycloudobject.h"

class PointCloudOffscreenRender: public GLOffscreenRenderFramework
{
    Q_OBJECT

public:
    PointCloudOffscreenRender(const std::string &modelPath, QWidget *parent = 0);
    ~PointCloudOffscreenRender();
    void renderToImageFile(glm::mat4 mvMatrix, glm::mat4 projMatrix, QString filePath, QSize dsize);

protected:
    void initializeGL() Q_DECL_OVERRIDE;

private:
    PLYCloudObject m_renderObject;
    GLuint m_programID = 0;
};

#endif // POINTCLOUDOFFSCREENRENDER_H
