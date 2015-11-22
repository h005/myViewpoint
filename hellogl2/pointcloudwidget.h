#ifndef POINTCLOUDWIDGET_H
#define POINTCLOUDWIDGET_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include "dragablewidget.h"
#include "RenderObject/plycloudobject.h"
#include "RenderObject/sphere.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class PointCloudWidget: public DragableWidget
{
public:
    PointCloudWidget(const std::string &plyPath, QWidget *parent = NULL);
    ~PointCloudWidget();
    std::vector<glm::vec3> m_points;
    int addPoint(const QPoint &p);
    bool removeLastPoint();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    virtual glm::mat4 getModelMatrix() override;

protected:
    glm::mat4 m_camera;
    glm::mat4 m_proj;
    glm::mat4 m_scaleAndShift;
    PLYCloudObject m_renderObject;
    GLuint m_programID = 0;

    Sphere m_sphereObject;
    GLuint m_sphereProgramID = 0;
};

#endif // POINTCLOUDWIDGET_H
