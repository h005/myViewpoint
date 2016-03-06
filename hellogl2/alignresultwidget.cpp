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

    float tmp[16] = {0.036455, 0.000000, 0.000000, 0.000000, 0.000000, 0.036455, 0.000000, 0.000000, 0.000000, 0.000000, 0.036455, 0.000000, -0.916903, -0.997222, 0.590807, 1.000000};
    glm::mat4 transform = glm::make_mat4(tmp);

    // 计算modelView矩阵
//    glm::mat4 modelViewMatrix = m_customMV * glm::rotate(glm::mat4(1.f), m_angle, m_rotateN) * m_baseRotate * transform;
    glm::mat4 modelViewMatrix = m_customMV * transform;

    std::cout << "custom MV"<<std::endl;
    std::cout << glm::to_string(m_customMV) << std::endl;
    std::cout << "custom MV done" << std::endl;

    // 绘制模型
    model.draw(modelViewMatrix, m_customProj);
}

