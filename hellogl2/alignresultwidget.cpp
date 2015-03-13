#include "alignresultwidget.h"

#include <opencv2/opencv.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "DLT.h"
#include "pointsmatchrelation.h"

AlignResultWidget::AlignResultWidget(PointsMatchRelation *relation, const QString &modelPath, int iwidth, int iheight, QWidget *parent):
    GLWidget(modelPath, parent),
    m_iheight(iheight),
    m_iwidth(iwidth)
{
    setWindowTitle(tr("Aligned Result"));

    std::vector<glm::vec2> &points2d = relation->getPoints2d();
    std::vector<glm::vec3> &points3d = relation->getPoints3d();

    assert(points2d.size() >= 6);
    assert(points2d.size() == points3d.size());
    // 由点的匹配信息求出视图变换矩阵和投影矩阵
    DLTwithPoints(points2d.size(), (float(*)[2])&points2d[0], (float(*)[3])&points3d[0], m_iwidth, m_iheight, m_customMV, m_customProj);
}

AlignResultWidget::~AlignResultWidget()
{

}

QSize AlignResultWidget::minimumSizeHint() const
{
    float scale = m_iwidth * 1.f / m_iheight;
    return QSize((int)(720 * scale), 720);
}

QSize AlignResultWidget::sizeHint() const
{
    float scale = m_iwidth * 1.f / m_iheight;
    return QSize((int)(720 * scale), 720);
}

void AlignResultWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE

    // 计算modelView矩阵
    glm::mat4 modelViewMatrix = m_customMV * glm::rotate(glm::mat4(1.f), m_angle, m_rotateN) * m_baseRotate;

    // 绘制模型
    model.drawNormalizedModel(modelViewMatrix, m_customProj);
}

