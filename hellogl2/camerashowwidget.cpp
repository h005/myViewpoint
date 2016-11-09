#include "camerashowwidget.h"

#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv.hpp>
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

CameraShowWidget::CameraShowWidget(const QString &modelPath,
                                   const float imgRatio,
                                   const std::vector<glm::mat4> &mvMatrixs,
                                   const std::vector<int> &clusterIndices,
                                   const std::vector<glm::mat4> &cMvMatrixs,
                                   QWidget *parent)
    :GLWidget(modelPath, parent)
{
//    this->meshModel = meshModel;
    m_imgRatio = imgRatio;
    for (auto it = mvMatrixs.begin(); it != mvMatrixs.end(); it++)
        m_estimatedMVMatrixs.push_back(glm::inverse(*it));

    for(auto it = cMvMatrixs.begin(); it != cMvMatrixs.end(); it++)
        m_locationMVMatrixs.push_back(glm::inverse(*it));

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
    m_locationModel.load(locationPath.c_str());
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
    for(int i=0;i<15;i++)
        m_cameraModels[i].bindDataToGL();
    m_locationModel.bindDataToGL();

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

//    modelViewMatrix = glm::scale(modelViewMatrix,glm::vec3(10.f));
    // 绘制模型

//    float aaa[16] = {-4.37114e-08,1,0,0,
//                     0.242536,1.06016e-08,0.970142,-0.291043,
//                     0.970143,4.24063e-08,-0.242536,-1.16417,
//                     0,0,0,1};
//    modelViewMatrix = glm::make_mat4(aaa);
//    modelViewMatrix = glm::transpose(modelViewMatrix);
//    float bbb[16] = {1.29904,0,0,0,
//                     0,1.73205,0,0,
//                     0,0,-1.10526,-1.05263,
//                     0,0,-1,0};
//    m_proj = glm::make_mat4(bbb);
//    m_proj = glm::transpose(m_proj);
    model.draw(modelViewMatrix, m_proj);

    // 绘制坐标系
    glUseProgram(_axisProgramID);
    GLuint projMatrixID = glGetUniformLocation(m_sphereProgramID, "projMatrix");
    GLuint mvMatrixID = glGetUniformLocation(m_sphereProgramID, "mvMatrix");

    float ScaleRate = 0.0f;
    // BrandenburgGate done
//    ScaleRate = 0.1f;
    // BritishMuseum done but not good
//    ScaleRate = 0.02f;
    // capitol done
//    ScaleRate = 0.1f;
    // Sacre done
//    ScaleRate = 0.1f;
    // mont done
//    ScaleRate = 0.1f;
    // potalaPalace done
//    ScaleRate  = 1000.f;
    // BuckinghamPalace done
//    ScaleRate = 0.05f;
    // HelsinkiCathedral done
//    ScaleRate = 0.05f;
    // notredame done
//    ScaleRate =  300.f;
    // tajMahal done
//    ScaleRate = 0.1f;
    // freeGodness done
//    ScaleRate  = 1000.f;
    // bigBen
//    ScaleRate = 10.f;
    // TengwangPavilion
//    ScaleRate = 10.f;
    // cctv3
//    ScaleRate = 1000.f;
    // kxm
//    ScaleRate = 500.f;
    //    int lastClusterId = m_clusterIndices[0];
    // house8
//    ScaleRate = 0.0f;
    for (int i = 0; i < m_estimatedMVMatrixs.size(); i++) {
            glm::mat4 axisMV = modelViewMatrix * (m_estimatedMVMatrixs[i]);            
            axisMV = glm::scale(axisMV, glm::vec3(ScaleRate));
            glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_proj));
            glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, glm::value_ptr(axisMV));

            double color[3];
            ColorMap::jet(color, m_clusterIndices[i], 0, 50);
            glColor3ub(color[0] * 255, color[1]  * 255, color[2] * 255);

            m_cameraModels[cameraID[m_clusterIndices[i]]].draw(axisMV,m_proj);
            std::cout << "render camera "<< i << std::endl;
//            m_cameraModel.draw(axisMV,m_proj);
//            m_axis.draw();
     }

    // draw location
    for(int i=0;i<m_locationMVMatrixs.size();i++)
    {
        glm::mat4 axisMV = modelViewMatrix * (m_locationMVMatrixs[i]);
        axisMV = glm::scale(axisMV, glm::vec3(ScaleRate/2.f));
        glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_proj));
        glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, glm::value_ptr(axisMV));
        m_locationModel.draw(axisMV,m_proj);
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

void CameraShowWidget::storeRenderResult(QString imageFile,QString ourImgFile)
{
    makeCurrent();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLubyte *img =
                new GLubyte[(viewport[2] - viewport[0])
    *(viewport[3] - viewport[1])*4];
    glReadBuffer(GL_BACK_LEFT);
    std::cout << "read pixel done 1" << std::endl;
    std::cout << "viewport " << viewport[0] << " " << viewport[1] << " " << viewport[2] << " " << viewport[3] << std::endl;
//    glReadPixels(0,0,viewport[2],viewport[3],GL_RGBA,GL_UNSIGNED_BYTE,img);

    glReadPixels(0,
                0,
                viewport[2],
                viewport[3],
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                img);

    std::cout << "read pixel done 2" << std::endl;
    cv::Mat originalImage = cv::imread(imageFile.toStdString().c_str());
    cv::Mat rgbaImgFliped = cv::Mat(viewport[3],viewport[2],CV_8UC4,img);
    cv::Mat rgbImg;
    cv::flip(rgbaImgFliped,rgbImg,0);
    cv::resize(rgbImg,rgbImg,cv::Size(originalImage.cols,originalImage.rows));
    cv::cvtColor(rgbImg,rgbImg,CV_RGBA2BGR);
    rgbImg.convertTo(rgbImg,CV_8UC3);
    IplImage *saveImage = new IplImage(rgbImg);
    cvSaveImage(ourImgFile.toStdString().c_str(),saveImage);

    std::cout << "debug ...." << std::endl;

    delete []img;
    rgbaImgFliped.release();
    originalImage.release();

}
