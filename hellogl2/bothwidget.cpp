#include "bothwidget.h"

BothWidget::BothWidget(const std::string ptCloudPath, const std::string modelPath, glm::mat4 modelMV, QWidget *parent)
    : PointCloudWidget(ptCloudPath, parent),
      m_modelMV(modelMV)
{
    m_gModel.load(modelPath.c_str());
}

BothWidget::~BothWidget()
{
    makeCurrent();
    m_gModel.cleanUp();
    doneCurrent();
}

void BothWidget::initializeGL()
{
    PointCloudWidget::initializeGL();
    m_gModel.bindDataToGL();
}

void BothWidget::paintGL()
{
    PointCloudWidget::paintGL();

    glm::mat4 gModelVP = m_camera * getModelMatrix() * m_modelMV;
    m_gModel.drawNormalizedModel(gModelVP, m_proj);
}

