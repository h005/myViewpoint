#include "alignresultwidget.h"

#include <opencv2/opencv.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "DLT.h"
#include "pointsmatchrelation.h"

AlignResultWidget::AlignResultWidget(const QString &modelPath, float imgRatio, const glm::mat4 &mvMatrix, const glm::mat4 &projMatrix, QWidget *parent):
    GLWidget(modelPath, parent),
    m_customMV(mvMatrix),
    m_customProj(projMatrix),
    m_imgRatio(imgRatio)
{
    setWindowTitle(tr("Aligned Result"));
}

AlignResultWidget::~AlignResultWidget()
{

}

QSize AlignResultWidget::minimumSizeHint() const
{
    return QSize((int)(720 * m_imgRatio), 720);
}

QSize AlignResultWidget::sizeHint() const
{
    return QSize((int)(720 * m_imgRatio), 720);
}

void AlignResultWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE

    // 绘制模型
    model.draw(m_customMV, m_customProj);
}

