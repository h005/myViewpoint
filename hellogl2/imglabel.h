#ifndef IMGLABEL_H
#define IMGLABEL_H

#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QString>
#include <QImage>
#include <QPointF>
#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "ccsift.h"
#include "ccsiftmatch.h"
#include "ccmodelwidget.h"

#define WIDTH 1500
#define HEIGHT 1500

//  imglabel 中的mat在readin之后是RGB格式的
class ImgLabel : public QLabel
{
    Q_OBJECT
public:
    ImgLabel(QString path,QWidget *parent = 0);
    ImgLabel(CCSiftMatch *ccSiftMatch,CCModelWidget *ccMW,QWidget *parent = 0);
    ~ImgLabel();

    void setImage(QString path);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *e);

    void printPoints(std::ofstream &fout);
    std::vector<QPointF>& getPoints();
    int numPoints();
    bool removeLastPoint();

    void getImageSize(int &width,int &height);
    void clearPoints();
    void setPoints(std::vector<glm::vec2> points);
    void setPoints(std::vector<cv::Point2f> points,std::vector<int> index);
    double getScaleRatio();

    void getSift();
    CCSift* getCCSift();

    cv::Mat& getImage();

    void updateImg();

private:
    void readin();
    void imageResize();
    QImage mat2QImage(cv::Mat &mat);
    void clearRegionPoints();
    void showMatchResult();
    void showRawMatch();

private:
    double scaleRatio;
    QSize iniSize;
    QPointF from;
    QPointF to;
    bool flagMatch;
    bool flagDrawRect;
    QString path;
    QImage img;
    QImage rawImg;
    cv::Mat image;
    cv::Mat rawImage;
    std::vector<QPointF> points;
    CCSift *cc_sift;
    CCSiftMatch *ccSiftMatch;
    CCModelWidget *ccMW;

};

#endif // IMGLABEL_H
