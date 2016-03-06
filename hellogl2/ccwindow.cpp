#include "ccwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <glm/gtx/string_cast.hpp>
#include "DLT.h"
#include "alignresultwidget.h"
CCWindow::CCWindow()
{

}

CCWindow::CCWindow(QString modelPath, QString imgPath, QString relationPath)
{
    this->modelPath = modelPath;
    imgFile = new QFileInfo(imgPath);
    modelFile = new QFileInfo(modelPath);
    relationFile = new QFileInfo(relationPath);

    setAttribute(Qt::WA_DeleteOnClose);

    relation = new PointsMatchRelation(relationPath);
    if(!relation->ccaLoadFromFile()){
//        std::cout << "read failed"<<std::endl;
    }

    alignBtn = new QPushButton(tr("Align && See"),this);
    exportBtn = new QPushButton(tr("ExportBtn"),this);
    calibrateBtn = new QPushButton(tr("Calibrate"),this);
    pointsClear = new QPushButton(tr("Clear"),this);

    scrollArea = new QScrollArea();

    imgLabel = new ImgLabel(imgPath);
    imgLabel->setPoints(relation->getImgPoints());

    ccMW = new CCModelWidget(modelPath);
    ccMW->m_relation = relation;

    connect(alignBtn,SIGNAL(clicked()),this,SLOT(align()));
    connect(exportBtn,SIGNAL(clicked()),this,SLOT(exportInfo()));
    connect(calibrateBtn,SIGNAL(clicked()),this,SLOT(calibrate()));
    connect(pointsClear,SIGNAL(clicked()),this,SLOT(clearpoints()));

    QSizePolicy cellPolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    cellPolicy.setHorizontalStretch(1);
    scrollArea->setSizePolicy(cellPolicy);
    ccMW->setSizePolicy(cellPolicy);

    scrollArea->setWidget(imgLabel);
    // 中间一列放按钮
    QVBoxLayout *middleLayout = new QVBoxLayout;
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
//    assert(imgLabel->getPoints().size() >=6 );
    calibrate();

    std::cout << "cal_mv" << std::endl;
    std::cout << glm::to_string(cal_mv) << std::endl;
    std::cout << "cal_mv done" << std::endl;

    std::cout << "cal_proj" << std::endl;
    std::cout << glm::to_string(cal_proj) << std::endl;
    std::cout << "cal_proj done" << std::endl;
    int imgWidth,imgHeight;
    imgLabel->getImageSize(imgWidth,imgHeight);
    std::cout << "imgWidth " <<imgWidth << " imgHeight " << imgHeight << std::endl;
//    ccMW->fixMVP(cal_mv,cal_proj);
//    CCModelWidget *ccMWalign = new CCModelWidget(modelPath,cal_mv,cal_proj);
//    ccMWalign->m_relation = relation;
//    ccMWalign->show();
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
    int matchnum = imgLabel->getPoints().size();
//    int matchnum = 14;
//    float points2D[][2]={
//        1639.34,1203.67,
//        1594.72,1367.35,
//        1660.48,945.74,
//        427.45,1051.56,
//        481.468,1208.63,
//        807.927,729.145,
//        422.752,704.344,
//        988.771,939.126,
//        1204.84,762.213,
//        1641.69,791.974,
//        1660.48,633.249,
//        1134.39,768.827,
//        993.468,598.527,
//        984.073,125.658
//    };
//    float points3D[][3]={
//        52.0976,41.3629,-1.78954,
//        50.6093,49.6279,-3.31618,
//        52.1027,41.3751,-30.5137,
//        -1.84666,41.2625,-1.7354,
//        -0.734041,49.392,-3.53277,
//        15.3796,25.917,-3.74056,
//        -0.287441,26.1702,-3.46828,
//        25.3048,34.0983,-4.27587,
//        34.9188,26.1183,-4.02905,
//        50.0144,26.4673,-3.61542,
//        50.2464,26.3998,-28.4199,
//        24.7869,33.4816,-27.755,
//        17.2732,25.6229,-28.5115,
//        17.2796,0.154659,-28.0193
//    };

    float points2D[matchnum][2];
    float points3D[matchnum][3];
    getDLTpoints2D(points2D);
    getDLTpoints3D(points3D);

    std::cout << "points3D "<< std::endl;
    for(int i=0;i<matchnum;i++)
        std::cout << points3D[i][0] << " " << points3D[i][1] << " " << points3D[i][2]<< std::endl;
    std::cout << "points3D done " << std::endl;


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
