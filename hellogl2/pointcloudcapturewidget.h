#ifndef POINTCLOUDCAPTUREWIDGET_H
#define POINTCLOUDCAPTUREWIDGET_H

#include "pointcloudwidget.h"

class PointCloudCaptureWidget : public PointCloudWidget
{
public:
    PointCloudCaptureWidget(const std::string path, glm::mat4 mvMatrix, glm::mat4 projMatrix, QWidget *parent=NULL);
    ~PointCloudCaptureWidget();

protected:
    virtual glm::mat4 getModelMatrix() override;
    void paintGL() Q_DECL_OVERRIDE;

private:
    glm::mat4 m_mvMatrix;
    glm::mat4 m_inProj;
};

#endif // POINTCLOUDCAPTUREWIDGET_H
