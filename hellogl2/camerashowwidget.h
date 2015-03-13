#ifndef CAMERASHOWWIDGET_H
#define CAMERASHOWWIDGET_H


#include <glm/glm.hpp>
#include "glwidget.h"
#include "axis.h"

class CameraShowWidget: public GLWidget
{
public:
    CameraShowWidget(const QString &modelPath, const float imgRatio, const glm::mat4 &mvMatrix, QWidget *parent = 0);
    ~CameraShowWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:
    glm::vec3 m_eye, m_center, m_up;
    float m_imgRatio;
    glm::mat4 m_estimatedMVMatrix;
    Axis m_axis;

};

#endif // CAMERASHOWWIDGET_H
