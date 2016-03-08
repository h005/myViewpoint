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

CCWindow::CCWindow()
{

}

CCWindow::CCWindow(QString modelPath, QString imgPath, QString relationPath)
{
    ccSiftMatch = NULL;
    this->modelPath = modelPath;
    imgFile = new QFileInfo(imgPath);
    modelFile = new QFileInfo(modelPath);
    relationFile = new QFileInfo(relationPath);

    setAttribute(Qt::WA_DeleteOnClose);

    alignBtn = new QPushButton(tr("Align && See"),this);
    exportBtn = new QPushButton(tr("ExportBtn"),this);
    calibrateBtn = new QPushButton(tr("Calibrate"),this);
    pointsClear = new QPushButton(tr("Clear"),this);
    siftMatchBtn = new QPushButton(tr("SiftMatch"),this);

    calibrateBtn->setEnabled(false);
    alignBtn->setEnabled(false);

    scrollArea = new QScrollArea();

    imgLabel = new ImgLabel(imgPath);

    ccMW = new CCModelWidget(modelPath);
    ccMW->getScaleTranslateMatrix(cc_st);
    relation = new PointsMatchRelation(relationPath);
    if(!relation->ccaLoadFromFile()){
        std::cout << "relation file load failed" << std::endl;
    }
    imgLabel->setPoints(relation->getImgPoints());
    ccMW->m_relation = relation;

    connect(alignBtn,SIGNAL(clicked()),this,SLOT(align()));
    connect(exportBtn,SIGNAL(clicked()),this,SLOT(exportInfo()));
    connect(calibrateBtn,SIGNAL(clicked()),this,SLOT(calibrate()));
    connect(pointsClear,SIGNAL(clicked()),this,SLOT(clearpoints()));
    connect(siftMatchBtn,SIGNAL(clicked()),this,SLOT(siftMatch()));

    QSizePolicy cellPolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    cellPolicy.setHorizontalStretch(1);
    scrollArea->setSizePolicy(cellPolicy);
    ccMW->setSizePolicy(cellPolicy);

    scrollArea->setWidget(imgLabel);
    // 中间一列放按钮
    QVBoxLayout *middleLayout = new QVBoxLayout;
    middleLayout->addWidget(siftMatchBtn);
    middleLayout->addWidget(alignBtn);
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

}

CCWindow::~CCWindow()
{
    delete relation;
}

QSize CCWindow::sizeHint() const
{
    return QSize(1024,768);
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

void CCWindow::align()
{
    assert(imgLabel->getPoints().size() >=6 );
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
    std::ofstream fout(relationFile->filePath().toStdString().c_str());

    fout << imgFile->fileName().toStdString() << std::endl;

    std::vector<QPointF> imgPoints = imgLabel->getPoints();
    for(int i=0;i<imgPoints.size();i++)
        fout << imgPoints[i].x() << " " << imgPoints[i].y() << std::endl;

    fout << modelFile->fileName().toStdString() << std::endl;

    std::vector<glm::vec3> points = relation->getModelPoints();
    for(int i=0;i<points.size();i++)
        fout << points[i][0] << " " << points[i][1] << " " << points[i][2] << std::endl;
    fout.close();
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
    imgLabel->setPoints(ccSiftMatch->getImagePoints());
    ccMW->setPoints(ccSiftMatch->getModelPoints());

    calibrateBtn->setEnabled(true);
    alignBtn->setEnabled(true);
}
