#ifndef CCMODELWINDOW_H
#define CCMODELWINDOW_H

#include <GL/glew.h>
#include "pointsmatchrelation.h"
#include "dragablewidget.h"
#include "GModel.h"
#include "RenderObject/sphere.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class QString;
class CCModelWidget : public DragableWidget
{
    Q_OBJECT

public:
    CCModelWidget(const QString &modelPath, QWidget *parent = 0);
    CCModelWidget(const QString &modelPath, glm::mat4 mv, glm::mat4 proj, QWidget *parent = 0);
    ~CCModelWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    int addPoint(const QPoint &p);
    bool removeLastPoint();

    void fixMVP(glm::mat4 &mv, glm::mat4 &proj);
    void clearRelations();
    PointsMatchRelation *m_relation;

private:
    void cleanup();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    glm::mat4 getModelViewMatrix();
    virtual glm::mat4 getModelMatrix() override;

protected:
    glm::mat4 fix_mv;
    glm::mat4 m_camera;
    glm::mat4 m_proj;
    GLfloat m_scaleBeforeRender;
    glm::mat4 m_shiftBeforeRender;
    GModel model;
    GLuint m_sphereProgramID;
    Sphere sphere;

private:
    bool m_transparent;
    bool f_fixMVP;
};

#endif // CCMODELWINDOW_H
