#include "camerashowwidget.h"

#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "TransformationUtils.h"
#include "shader.hpp"
#include "colormap.h"

CameraShowWidget::CameraShowWidget(const QString &modelPath,
                                   const float imgRatio,
                                   const std::vector<glm::mat4> &mvMatrixs,
                                   const std::vector<int> &clusterIndices,
                                   QWidget *parent)
    :GLWidget(modelPath, parent)
{
    m_imgRatio = imgRatio;
    for (auto it = mvMatrixs.begin(); it != mvMatrixs.end(); it++)
        m_estimatedMVMatrixs.push_back(glm::inverse(*it));

    m_clusterIndices = clusterIndices;
    m_cameraModel.load("camera/camera2.obj");
    int num_cameras = 15;
    std::string* cameraPath = new std::string[num_cameras]{
                            "camera/camera1.obj",
                            "camera/camera2.obj",
                            "camera/camera3.obj",
                            "camera/camera4.obj",
                            "camera/camera5.obj",
                            "camera/camera6.obj",
                            "camera/camera7.obj",
                            "camera/camera8.obj",
                            "camera/camera9.obj",
                            "camera/camera10.obj",
                            "camera/camera11.obj",
                            "camera/camera12.obj",
                            "camera/camera13.obj",
                            "camera/camera14.obj",
                            "camera/camera15.obj",
                            };

    int ind = -1;
    cameraID.clear();
    std::map<int,int>::iterator it;
    // load cameraID
    for(int i=0;i<m_clusterIndices.size();i++)
    {
        it = cameraID.find(m_clusterIndices[i]);
        if(it == cameraID.end())
        {
            ind++;
            ind = ind % num_cameras;
            cameraID.insert( std::pair<int,int>(m_clusterIndices[i],ind) );
        }
    }

    for(int i=0; i<num_cameras; i++)
    {
        GModel cameraModel;
        cameraModel.load(cameraPath[i].c_str());
        m_cameraModels.push_back(cameraModel);
        std::cout << "m_cameraModels " << i << std::endl;
    }
    std::cout << "debug " << std::endl;
    //recoveryLookAtWithModelView(mvMatrix, m_eye, m_center, m_up);
}

CameraShowWidget::~CameraShowWidget()
{
    makeCurrent();

    // axis无需清理
    model.cleanUp();

    if(_axisProgramID)
    {
        glDeleteProgram(_axisProgramID);
        _axisProgramID = 0;
    }

    doneCurrent();
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

    m_cameraModel.bindDataToGL();
    for(int i=0;i<8;i++)
        m_cameraModels[i].bindDataToGL();

//    GLuint args[] = {0};
//    m_axis.bindDataToGL(args);

    _axisProgramID = LoadShaders("shader/axisShader.vert", "shader/axisShader.frag");
}

void CameraShowWidget::paintGL()
{
//    int num_cameras = 8;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 默认开启背面剔除:GL_CULL_FACE

    // 计算modelView矩阵
    glm::mat4 modelViewMatrix = getModelViewMatrix();

    // 绘制模型
    model.draw(modelViewMatrix, m_proj);

    // 绘制坐标系
    glUseProgram(_axisProgramID);
    GLuint projMatrixID = glGetUniformLocation(m_sphereProgramID, "projMatrix");
    GLuint mvMatrixID = glGetUniformLocation(m_sphereProgramID, "mvMatrix");

//    int lastClusterId = m_clusterIndices[0];
    for (int i = 0; i < m_estimatedMVMatrixs.size(); i++) {
            glm::mat4 axisMV = modelViewMatrix * (m_estimatedMVMatrixs[i]);
            axisMV = glm::scale(axisMV, glm::vec3(500.f));
//            axisMV = glm::scale(axisMV, glm::vec3(100.f));
            glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_proj));
            glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, glm::value_ptr(axisMV));

            double color[3];
            ColorMap::jet(color, m_clusterIndices[i], 0, 50);
            glColor3ub(color[0] * 255, color[1]  * 255, color[2] * 255);

            m_cameraModels[cameraID[m_clusterIndices[i]]].draw(axisMV,m_proj);
//            std::cout << "render camera "<< indCluster << std::endl;
//            m_cameraModel.draw(axisMV,m_proj);
//            m_axis.draw();
     }
//    std::cout << "rendering ....................." << std::endl;
//    for (auto it = m_estimatedMVMatrixs.begin(); it != m_estimatedMVMatrixs.end(); it++) {
//        glm::mat4 axisMV = modelViewMatrix * (*it);
//        axisMV = glm::scale(axisMV, glm::vec3(0.00005 / m_scaleBeforeRender));
//        m_cameraModel.draw(axisMV, m_proj);
//    }

}

glm::mat4 CameraShowWidget::getModelViewMatrix()
{
    // 缩放矩阵不能放在m_camera之前，否则是在相机坐标系下等比例缩放
    // 会得到一致的渲染结果（除了离相机更近以外）
    return glm::scale(m_camera, glm::vec3(0.3)) * getModelMatrix();
}
