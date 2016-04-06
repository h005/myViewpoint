#ifndef CAMERASHOWWIDGET_H
#define CAMERASHOWWIDGET_H

#include <vector>
#include <glm/glm.hpp>
#include "glwidget.h"
#include "RenderObject/axis.h"
#include "GModel.h"

class CameraShowWidget: public GLWidget
{
public:
    CameraShowWidget(const QString &modelPath, const float imgRatio, const std::vector<glm::mat4> &mvMatrixs, const std::vector<int> &clusterIndices, QWidget *parent = 0);
    ~CameraShowWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    glm::mat4 getModelViewMatrix();

private:
    glm::vec3 m_eye, m_center, m_up;
    float m_imgRatio;
    std::vector<glm::mat4> m_estimatedMVMatrixs;
    std::vector<int> m_clusterIndices;
    Axis m_axis;
    GModel m_cameraModel;
    GLuint _axisProgramID = 0;
};

#endif // CAMERASHOWWIDGET_H
