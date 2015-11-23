#ifndef BOTHWIDGET_H
#define BOTHWIDGET_H

#include "pointcloudwidget.h"
#include "GModel.h"

class BothWidget : public PointCloudWidget
{
public:
    BothWidget(const std::string ptCloudPath, const std::string modelPath, glm::mat4 modelMV, QWidget *parent = NULL);
    ~BothWidget();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:
    GModel m_gModel;
    glm::mat4 m_modelMV;
};

#endif // BOTHWIDGET_H
