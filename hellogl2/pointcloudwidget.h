#ifndef POINTCLOUDWIDGET_H
#define POINTCLOUDWIDGET_H

#include <GL/glew.h>
#include <string>
#include "dragablewidget.h"
#include "RenderObject/plycloudobject.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class PointCloudWidget: public DragableWidget
{
public:
    PointCloudWidget(const std::string &plyPath, QWidget *parent = NULL);
    ~PointCloudWidget();

public slots:
    virtual void cleanup();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

private:
    PLYCloudObject m_renderObject;
    GLuint m_programID;
};

#endif // POINTCLOUDWIDGET_H
