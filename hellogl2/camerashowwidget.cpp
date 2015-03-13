#include "camerashowwidget.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "custom.h"

CameraShowWidget::CameraShowWidget(const QString &modelPath, const float imgRatio, const glm::mat4 &mvMatrix, QWidget *parent)
    :GLWidget(modelPath, parent)
{
    m_imgRatio = imgRatio;
    m_estimatedMVMatrix = mvMatrix;
    recoveryLookAtWithModelView(mvMatrix, m_eye, m_center, m_up);
}

CameraShowWidget::~CameraShowWidget()
{

}

QSize CameraShowWidget::minimumSizeHint() const
{
    return QSize((int)(720 * m_imgRatio), 720);
}

QSize CameraShowWidget::sizeHint() const
{
    return QSize((int)(720 * m_imgRatio), 720);
}

void CameraShowWidget::initializeGL()
{
    GLWidget::initializeGL();
    m_axis.init(NULL);
}

void CameraShowWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE

    // 计算modelView矩阵
    glm::mat4 modelViewMatrix = glm::scale(getModelViewMatrix(), glm::vec3(0.5, 0.5, 0.5));

    // 绘制模型
    model.drawNormalizedModel(modelViewMatrix, m_proj);

    // 绘制坐标系
    glUseProgram(m_sphereProgramID);
    GLuint projMatrixID = glGetUniformLocation(m_sphereProgramID, "projMatrix");
    GLuint mvMatrixID = glGetUniformLocation(m_sphereProgramID, "mvMatrix");
    glm::mat4 axisMV = modelViewMatrix * glm::inverse(m_estimatedMVMatrix);
    axisMV = glm::scale(axisMV, glm::vec3(0.3, 0.3, 0.3));
    glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_proj));
    glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, glm::value_ptr(axisMV));
    m_axis.draw();
}
