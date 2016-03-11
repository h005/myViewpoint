#include "ccwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <glm/gtx/string_cast.hpp>
#include "DLT.h"
#include "alignresultwidget.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opencv.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "lmdlt.h"

CCWindow::CCWindow()
{

}

CCWindow::CCWindow(QString modelPath, QString imgPath, QString relationPath)
{
    ccSiftMatch = NULL;
    this->modelPath = modelPath;
    imgFile = new QFileInfo(imgPath);
    modelFile = new QFileInfo(modelPath);
    relationFile = new QFileInfo(relationPath + "/" + imgFile->baseName() + ".txt");

    std::cout << "relation file info "<<relationFile->absoluteFilePath().toStdString() << std::endl;

    setAttribute(Qt::WA_DeleteOnClose);

    alignBtn = new QPushButton(tr("Align && See"),this);
    alignDLTBtn = new QPushButton(tr("Aligen DLT"),this);
    exportBtn = new QPushButton(tr("ExportBtn"),this);
    calibrateBtn = new QPushButton(tr("Calibrate"),this);
    pointsClear = new QPushButton(tr("Clear"),this);
    siftMatchBtn = new QPushButton(tr("SiftMatch"),this);
    ccSMW = new CCSiftMatchWindow();
//    calibrateBtn->setEnabled(false);
//    alignBtn->setEnabled(false);

    scrollArea = new QScrollArea();

    imgLabel = new ImgLabel(imgPath);

    ccMW = new CCModelWidget(modelPath);
    ccMW->getScaleTranslateMatrix(cc_st);
    relation = new PointsMatchRelation(relationFile->absoluteFilePath());
    if(!relation->ccaLoadFromFile()){
        std::cout << "relation file load failed" << std::endl;
    }
    imgLabel->setPoints(relation->getImgPoints());
    ccMW->m_relation = relation;

    connect(alignBtn,SIGNAL(clicked()),this,SLOT(align()));
    connect(alignDLTBtn,SIGNAL(clicked()),this,SLOT(alignDLT()));
    connect(exportBtn,SIGNAL(clicked()),this,SLOT(exportInfo()));
    connect(calibrateBtn,SIGNAL(clicked()),this,SLOT(calibrate()));
    connect(pointsClear,SIGNAL(clicked()),this,SLOT(clearpoints()));
    connect(siftMatchBtn,SIGNAL(clicked()),this,SLOT(siftMatch()));
//    connect(ccSMW,SIGNAL(relationDone()),this,SLOT(this->showPoints()));

    QSizePolicy cellPolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    cellPolicy.setHorizontalStretch(1);
    scrollArea->setSizePolicy(cellPolicy);
    ccMW->setSizePolicy(cellPolicy);

    scrollArea->setWidget(imgLabel);
    // 中间一列放按钮
    QVBoxLayout *middleLayout = new QVBoxLayout;
    middleLayout->addWidget(siftMatchBtn);
    middleLayout->addWidget(alignBtn);
    middleLayout->addWidget(alignDLTBtn);
    middleLayout->addWidget(exportBtn);
    middleLayout->addWidget(calibrateBtn);
    middleLayout->addWidget(pointsClear);

    // 左边放入imgLable，中间放按钮，右边放ccMW
    QHBoxLayout *container = new QHBoxLayout;
    container->addWidget(scrollArea);
    container->addLayout(middleLayout);
    container->addWidget(ccMW);

    QWidget *w = new QWidget;
    w->setLayout(container);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(w);
    setLayout(mainLayout);

    setCentralWidget(w);
    setWindowTitle(tr("Camera Calibration"));
    this->resize(1600,1024);
}

CCWindow::~CCWindow()
{
    delete relation;
}

QSize CCWindow::sizeHint() const
{
    return QSize(1600,1024);
}

void CCWindow::keyPressEvent(QKeyEvent *event)
{
    QSize rsize = ccMW->size();
    QSize lsize = imgLabel->size();
    if(event->key() == Qt::Key_Escape)
        close();
    else if(event->key() == Qt::Key_0)
    {   // 增加一个点
        QPoint p;

        // 检测鼠标是否在右侧(模型)
        p = ccMW->mapFromGlobal(QCursor::pos());
        if ((p.x() >= 0 && p.x() < rsize.width())
                && (p.y() >= 0 && p.y() < rsize.height())) {
            std::cout << ccMW->addPoint(p) << std::endl;
        }
    }
    else if(event->key() == Qt::Key_Minus)
    {
        // 删除一个点
        QPoint p;

        // 检测鼠标是否在右侧(模型)
        p = ccMW->mapFromGlobal(QCursor::pos());
        if ((p.x() >= 0 && p.x() < rsize.width())
                && (p.y() >= 0 && p.y() < rsize.height())) {
            std::cout << ccMW->removeLastPoint() << std::endl;
        }

        // 检测鼠标是否在左侧(图片)
        p = imgLabel->mapFromGlobal(QCursor::pos());
        if ((p.x() >= 0 && p.x() < lsize.width())
                      && (p.y() >= 0 && p.y() < lsize.height())) {
            std::cout << imgLabel->removeLastPoint() << std::endl;
        }
    }
}

void CCWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void CCWindow::getDLTpoints2D(float points2D[][2])
{
    std::vector<QPointF> imgPoints = imgLabel->getPoints();
    int width,height;
    imgLabel->getImageSize(width,height);
    if(imgPoints.size() == 0)
        return ;
    for(int i=0;i<imgPoints.size();i++)
    {
        points2D[i][0] = imgPoints[i].x();
        points2D[i][1] = height - imgPoints[i].y();
    }
}

void CCWindow::getDLTpoints3D(float points3D[][3])
{
    std::vector<glm::vec3> points = relation->getModelPoints();
    if(points.size() == 0)
        return ;
    for(int i=0;i<points.size();i++)
    {
        points3D[i][0] = points[i][0];
        points3D[i][1] = points[i][1];
        points3D[i][2] = points[i][2];
    }
}

void CCWindow::calibrateLevmar(glm::mat3 &R, glm::vec3 &t, float &c)
{
    int num = imgLabel->numPoints();
    int width,height;
    imgLabel->getImageSize(width,height);
    std::vector<QPointF> pts = imgLabel->getPoints();
    // convert to std::vector<glm::vec2>
    glm::vec2 vecPts[num];
    for(int i=0;i<num;i++)
    {
        vecPts[i].x = pts[i].x();
        vecPts[i].y = height - pts[i].y();
    }
    glm::vec2 imgSize(width,height);
    LMDLT::CCalibrate(num,
                      vecPts,
                      imgSize,
                      &relation->getModelPoints()[0],
                      R,
                      t,
                      c);

    cal_mv = glm::mat4(R);
    cal_mv[3] = glm::vec4(t,1.f);

    // set K
    float Kval[9] = {c,0,width/2.f,0,c,height/2.f,0,0,1.f};
    cv::Mat K(3,3,CV_32FC1,Kval);
    cv::Mat proj = constructProjectionMatrix(K,0.1,1000000000,width,height);
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            cal_proj[j][i] = proj.at<float>(i,j);

    std::cout << "calibrateLevmar"<< std::endl;
    std::cout << "mv"<<std::endl;
    std::cout << glm::to_string(cal_mv)<<std::endl;
    std::cout << "proj"<<std::endl;
    std::cout << glm::to_string(cal_proj)<<std::endl;
}

void CCWindow::align()
{
    // method 1 DLT directly
//        assert(imgLabel->getPoints().size() >=6 );
//        calibrate();

    std::cout << "image keypoints size "<<imgLabel->getPoints().size()<<std::endl;
    std::cout << "model keypoints size "<< relation->getModelPoints().size()<<std::endl;

    glm::mat3 R;
    glm::vec3 t;
    float f;
    calibrateLevmar(R,t,f);

    int imgWidth,imgHeight;
    imgLabel->getImageSize(imgWidth,imgHeight);

    AlignResultWidget *a =
            new AlignResultWidget(modelPath,
                                  imgWidth * 1.f / imgHeight,
                                  cal_mv,
                                  cal_proj);
    a->show();
}

void CCWindow::alignDLT()
{
    assert(imgLabel->getPoints().size() >= 6);
    calibrate();

    int imgWidth,imgHeight;
    imgLabel->getImageSize(imgWidth,imgHeight);

    AlignResultWidget *a =
            new AlignResultWidget(modelPath,
                                  imgWidth * 1.f / imgHeight,
                                  cal_mv,
                                  cal_proj);
    a->show();
}

void CCWindow::exportInfo()
{
    std::cout << "realtion file info " << relationFile->absoluteFilePath().toStdString() << std::endl;
    std::ofstream fout(relationFile->absoluteFilePath().toStdString().c_str());

//    fout << imgFile->fileName().toStdString() << std::endl;

    std::vector<QPointF> imgPoints = imgLabel->getPoints();
//    fout << modelFile->fileName().toStdString() << std::endl;

    int width,height;
    imgLabel->getImageSize(width,height);
    std::vector<glm::vec3> points = relation->getModelPoints();
    for(int i=0;i<imgPoints.size();i++)
    {
        fout << imgPoints[i].x() << " " << height - imgPoints[i].y() << " "
                << points[i][0] << " " << points[i][1] << " " << points[i][2] << std::endl;;
    }
    fout.close();
    alignBtn->setEnabled(true);
}

void CCWindow::calibrate()
{
    // DLT 检测相机参数的代码，2D图像中是的坐标系是x,y坐标系，即原点在图像的左下角
    int matchnum = imgLabel->getPoints().size();

    float points2D[matchnum][2];
    float points3D[matchnum][3];
    getDLTpoints2D(points2D);
    getDLTpoints3D(points3D);

    int imgWidth, imgHeight;
    imgLabel->getImageSize(imgWidth,imgHeight);
    DLTwithPoints(matchnum,
                  (float(*)[2])&points2D[0],
                  (float(*)[3])&points3D[0],
                  imgWidth,
                  imgHeight,
                  cal_mv,
                  cal_proj);
}

void CCWindow::clearpoints()
{
    imgLabel->clearPoints();
    ccMW->clearRelations();
}

void CCWindow::siftMatch()
{

    ccSiftMatch = new CCSiftMatch(imgLabel->getImage(),
                                  ccMW->getRenderImage());
    ccSiftMatch->match();

    ccSMW = new CCSiftMatchWindow(ccSiftMatch,ccMW,imgLabel);

    ccSMW->show();

    showPoints();
}

void CCWindow::showPoints()
{
    std::cout<<"show Points called"<<std::endl;
    std::vector<int> index;
    ccMW->setPoints(ccSiftMatch->getModelPoints(),index);
    imgLabel->setPoints(ccSiftMatch->getImagePoints(),index);
}

