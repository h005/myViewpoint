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

//  imglabel 中的mat在readin之后是RGB格式的
class ImgLabel : public QLabel
{
    Q_OBJECT
public:
    ImgLabel(QString path,QWidget *parent = 0);
    ~ImgLabel();

    void mousePressEvent(QMouseEvent *e);
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

    void getSift();
    CCSift* getCCSift();

    cv::Mat& getImage();

private:
    void readin();
    QImage mat2QImage(cv::Mat &mat);

private:
    QString path;
    QImage img;
    cv::Mat image;
    std::vector<QPointF> points;
    CCSift *cc_sift;
};

#endif // IMGLABEL_H