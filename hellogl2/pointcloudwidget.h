#ifndef POINTCLOUDWIDGET_H
#define POINTCLOUDWIDGET_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include "dragablewidget.h"
#include "RenderObject/plycloudobject.h"
#include "RenderObject/sphere.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class PointsMatchRelation;
class PointCloudWidget: public DragableWidget
{
public:
    PointCloudWidget(const std::string &plyPath, QWidget *parent = NULL);
    ~PointCloudWidget();
    int addPoint(const QPoint &p);
    bool removeLastPoint();
    PointsMatchRelation *m_relation = NULL;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    virtual glm::mat4 getModelMatrix() override;

protected:
    glm::mat4 m_camera;
    glm::mat4 m_proj;
    GLfloat m_scaleBeforeRender;
    glm::mat4 m_shiftBeforeRender;
    PLYCloudObject m_renderObject;
    GLuint m_programID = 0;

    Sphere m_sphereObject;
    GLuint m_sphereProgramID = 0;
};

#endif // POINTCLOUDWIDGET_H
