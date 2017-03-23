#include "cameraptshowwidget.h"

#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv.hpp>
#include "TransformationUtils.h"
#include "shader.hpp"
#include "colormap.h"


CameraPtShowWidget::CameraPtShowWidget(const QString &ptCloudPath,
                                       const float imgRatio,
                                       const std::vector<glm::mat4> &mvMatrixs,
                                       const std::vector<int> &clusterIndices,
                                       const std::vector<glm::mat4> &cMvMatrixs,
                                       QWidget *parent)
    :PointCloudWidget(ptCloudPath.toStdString(), parent)
{
    m_imgRatio = imgRatio;
    for( auto it = mvMatrixs.begin(); it != mvMatrixs.end(); it++)
        m_estimatedMVMatrixs.push_back(glm::inverse(*it));

    for( auto it = cMvMatrixs.begin(); it != cMvMatrixs.end(); it++)
        m_locationMVMatixs.push_back(glm::inverse(*it));


    m_clusterIndices = clusterIndices;
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

    std::string locationPath = "camera/location.obj";

    int ind = -1;
    cameraID.clear();
    std::map<int,int>::iterator it;
    // load cameraID
    for(int i=0;i<m_clusterIndices.size();i++)
    {
        it = cameraID.find(m_clusterIndices[i]);
        if(it == cameraID.end())
        {
            ind ++;
            ind = ind % num_cameras;
            cameraID.insert( std::pair<int,int>(m_clusterIndices[i],ind) );
        }
    }

    for(int i=0;i<num_cameras;i++)
    {
        GModel cameraModel;
        cameraModel.load(cameraPath[i].c_str());
        m_cameraModels.push_back(cameraModel);
        std::cout <<  "m_cameraModels " << i << std::endl;
    }
    m_locationModel.load(locationPath.c_str());
//    std::cout << "debug"

}

CameraPtShowWidget::~CameraPtShowWidget()
{
    makeCurrent();

//    m_renderObject.cleanup();
//    m_sphereObject.cleanup();

    if(_axisProgramID)
    {
        glDeleteProgram(_axisProgramID);
        _axisProgramID = 0;
    }

    doneCurrent();
}

QSize CameraPtShowWidget::minimumSizeHint() const
{
    return QSize((int)(720 * m_imgRatio), 720);
}

QSize CameraPtShowWidget::sizeHint() const
{
    return QSize((int)(720 * m_imgRatio), 720);
}

void CameraPtShowWidget::initializeGL()
{
    std::cout << "camera show widget initial ing..." << std::endl;
    PointCloudWidget::initializeGL();
    std::cout << "camera show widget initial ing... 1" << std::endl;
//    m_cameraModel.bindDataToGL();
    for(int i=0;i<15;i++)
        m_cameraModels[i].bindDataToGL();
    std::cout << "camera show widget initial ing... 2" << std::endl;
    m_locationModel.bindDataToGL();

    std::cout << "camera show widget initial ing... 3" << std::endl;
    _axisProgramID = LoadShaders("shader/axisShader.vert", "shader/axisShader.frag");

    std::cout << "camera show widget initial done" << std::endl;
}

void CameraPtShowWidget::paintGL()
{
    std::cout << "camera show widget paintGL 1" << std::endl;
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_DEPTH_TEST);

//    glEnable(GL_FLAT);
//    glShadeModel(GL_FLAT);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::mat4 modelViewMatrix = getModelViewMatrix();
////    modelViewMatrix = glm::scale(modelViewMatrix,glm::vec3(10.f));

//    glm::mat4  MVP = m_proj * modelViewMatrix;

//    glUseProgram(m_programID);
//    GLuint mvpID = glGetUniformLocation(m_programID,"MVP");
//    glUniformMatrix4fv(mvpID,1,GL_FALSE,glm::value_ptr(MVP));

//    m_renderObject.draw();

    PointCloudWidget::paintGL();

    glUseProgram(_axisProgramID);
    GLuint projMatrixID  = glGetUniformLocation(m_sphereProgramID, "projMatrix");
    GLuint mvMatrixID = glGetUniformLocation(m_sphereProgramID,"mvMatrix");

    float ScaleRate;
    // freeGodness
//    ScaleRate = 0.05f;
    // captiol
//    ScaleRate = 5.f;
    // notredame
//    ScaleRate = .2f;
    // Sacre
//    ScaleRate = 0.9f;
    // BrandenburgGate
//    ScaleRate = 0.6f;
    // BuckinghamPalace
//    ScaleRate = 0.6f;
    // kxm
    ScaleRate = .2f;


    for(int i=0;i<m_estimatedMVMatrixs.size();i++)
    {
        glm::mat4 axisMV = modelViewMatrix * (m_estimatedMVMatrixs[i]);
        axisMV = glm::scale(axisMV, glm::vec3(ScaleRate));
        glUniformMatrix4fv(projMatrixID,1,GL_FALSE,glm::value_ptr(m_proj));
        glUniformMatrix4fv(mvMatrixID,1,GL_FALSE,glm::value_ptr(axisMV));

        m_cameraModels[cameraID[m_clusterIndices[i]]].draw(axisMV,m_proj);
        std::cout << "render camera " << i << std::endl;
    }


    // draw location
//    for(int i=0;i<m_locationMVMatixs.size();i++)
//    {
//        glm::mat4 axisMV = modelViewMatrix * (m_locationMVMatixs[i]);
//        axisMV = glm::scale(axisMV, glm::vec3(ScaleRate/2.f));
//        glUniformMatrix4fv(projMatrixID,1,GL_FALSE,glm::value_ptr(m_proj));
//        glUniformMatrix4fv(mvMatrixID,1,GL_FALSE,glm::value_ptr(axisMV));
//        m_locationModel.draw(axisMV,m_proj);
//    }

}

glm::mat4 CameraPtShowWidget::getModelViewMatrix()
{
    // 缩放矩阵不能放在m_camera之前，否则是在相机坐标系下等比例缩放
    // 会得到一致的渲染结果（除了离相机更近以外）
//    return glm::scale(m_camera, glm::vec3(0.3)) * getModelMatrix();
    return m_camera * getModelMatrix();
}


