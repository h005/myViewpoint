#include "pointcloudcapturewidget.h"

PointCloudCaptureWidget::PointCloudCaptureWidget(const std::string path, glm::mat4 mvMatrix, glm::mat4 projMatrix, QWidget *parent)
    : PointCloudWidget(path, parent),
      m_mvMatrix(mvMatrix),
      m_inProj(projMatrix)
{

}

PointCloudCaptureWidget::~PointCloudCaptureWidget()
{

}

glm::mat4 PointCloudCaptureWidget::getModelMatrix()
{
    return m_mvMatrix;
}

void PointCloudCaptureWidget::paintGL()
{
    // 渲染之前，强行设置投影
    m_camera = glm::mat4(1.f);
    m_proj = m_inProj;
    PointCloudWidget::paintGL();
}
