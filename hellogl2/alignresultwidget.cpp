#include "alignresultwidget.h"

#include <opencv2/opencv.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "DLT.h"
#include "pointsmatchrelation.h"

AlignResultWidget::AlignResultWidget(PointsMatchRelation &relation, const QString &modelPath, int iwidth, int iheight, QWidget *parent):
    GLWidget(relation, modelPath, parent),
    m_iheight(iheight),
    m_iwidth(iwidth)
{
    setWindowTitle(tr("Aligned Result"));

    std::vector<glm::vec2> &points2d = relation.getPoints2d();
    std::vector<glm::vec3> &points3d = relation.getPoints3d();

    assert(points2d.size() >= 6);
    assert(points2d.size() == points3d.size());
    // 第一阶段, 用点对得到P
    cv::Mat P = phase1CalculateP(points2d.size(), points3d.size(), (float(*)[2])&points2d[0], (float(*)[3])&points3d[0]);

    // 第二阶段
    // 从P中分解出 K * [R t]
    // 由[R t]可以得到lookat的参数，由K可以构造GL_PROJECTION_MATRIX
    cv::Mat modelView, K;
    phase2ExtractParametersFromP(P, modelView, K);

    cv::Mat projMatrix;
    cv::Mat lookAtParams;
    phase3GenerateLookAtAndProjection(modelView, K, m_iwidth, m_iheight, lookAtParams, projMatrix);

    glm::vec3 eye = glm::vec3(lookAtParams.at<float>(0, 0), lookAtParams.at<float>(1, 0), lookAtParams.at<float>(2, 0));
    glm::vec3 center = glm::vec3(lookAtParams.at<float>(0, 1), lookAtParams.at<float>(1, 1), lookAtParams.at<float>(2, 1));
    glm::vec3 updir = glm::vec3(lookAtParams.at<float>(0, 2), lookAtParams.at<float>(1, 2), lookAtParams.at<float>(2, 2));
    // 在物体坐标系（世界坐标系）中摆放照相机和它的朝向
    m_customMV = glm::lookAt(eye, center, updir);
    // 使用生成的OpenGL投影矩阵
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m_customProj[j][i] = projMatrix.at<float>(i, j);
        }
    }
}

AlignResultWidget::~AlignResultWidget()
{

}

QSize AlignResultWidget::minimumSizeHint() const
{
    return QSize(m_iwidth, m_iheight);
}

QSize AlignResultWidget::sizeHint() const
{
    return QSize(m_iwidth, m_iheight);
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

