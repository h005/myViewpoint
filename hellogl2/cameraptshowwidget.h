#ifndef CAMERAPTSHOWWIDGET_H
#define CAMERAPTSHOWWIDGET_H

#include <vector>
#include <glm/glm.hpp>
#include "GModel.h"
#include "pointcloudwidget.h"
#include <map>

class CameraPtShowWidget: public PointCloudWidget
{
public:
    CameraPtShowWidget(const QString &ptCloudPath,
                       const float imgRatio,
                       const std::vector<glm::mat4> &mvMatrixs,
                       const std::vector<int> &clusterIndices,
                       const std::vector<glm::mat4> &cMvMatrixs,
                       QWidget *parent = 0);

    ~CameraPtShowWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    glm::mat4 getModelViewMatrix();

private:
    glm::vec3 m_eye, m_center, m_up;
    std::vector<glm::mat4> m_estimatedMVMatrixs;
    std::vector<glm::mat4> m_locationMVMatixs;
    std::vector<int> m_clusterIndices;

    GLuint _axisProgramID = 0;
    GLuint m_programID = 0;
    float m_imgRatio;
    GModel m_cameraModel;
    std::vector<GModel> m_cameraModels;
    GModel m_locationModel;
    std::map<int,int> cameraID;

};

#endif // CAMERAPTSHOWWIDGET_H
