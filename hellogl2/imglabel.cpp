#include "imglabel.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPixmap>
#include "opencv2/xfeatures2d.hpp"

ImgLabel::ImgLabel(QString path, QWidget *parent) :
    QLabel(parent)
{
    cc_sift = NULL;
    this->path = path;
    readin();
    QPixmap pixmap = QPixmap::fromImage(img);
    this->setPixmap(pixmap);
}

ImgLabel::~ImgLabel()
{
    image.release();
    delete cc_sift;
}

void ImgLabel::mousePressEvent(QMouseEvent *e)
{
    std::cout << (float)e->x()  << " " << (float)e->y() << std::endl;
    points.push_back(QPointF((float)e->x(),(float)e->y()));
    update();
}

void ImgLabel::mouseReleaseEvent(QMouseEvent *e)
{

}

void ImgLabel::keyPressEvent(QKeyEvent *e)
{

}

void ImgLabel::paintEvent(QPaintEvent *e)
{
    // 这是例行的，不调用会出错
    QLabel::paintEvent(e);
    QPainter *painter = new QPainter(this);
    QPen pen = QPen(Qt::red,5);
    painter->setPen(pen);
    for(int i=0;i<points.size();i++)
        painter->drawPoint(points[i]);
//        painter->drawPoint(points[i].x(),image.rows - points[i].y());
}

// this function failed
void ImgLabel::printPoints(std::ofstream &fout)
{
//    for(int i=0;i<points.size();i++)
//    {
//        QPointF tmp = points[i];
//        fout << tmp.x() << " " << tmp.y() << std::endl;
    //    }
}

std::vector<QPointF> &ImgLabel::getPoints()
{
    return points;
}



int ImgLabel::numPoints()
{
    return points.size();
}

bool ImgLabel::removeLastPoint()
{
    if(points.size() > 0)
    {
        points.pop_back();
        update();
        return true;
    }
    return false;
}

void ImgLabel::getImageSize(int &width, int &height)
{
    width = image.cols;
    height = image.rows;
}

void ImgLabel::clearPoints()
{
    points.clear();
    update();
}

void ImgLabel::setPoints(std::vector<glm::vec2> points)
{
    this->points.clear();
    for(int i=0;i<points.size();i++)
        this->points.push_back(QPointF(points[i][0],image.rows - points[i][1]));
    update();
}

void ImgLabel::setPoints(std::vector<cv::Point2f> points, std::vector<int> index)
{
    this->points.clear();
    for(int i=0;i<index.size();i++)
            this->points.push_back(QPointF(points[index[i]].x,points[index[i]].y));
    update();
}

void ImgLabel::getSift()
{
    cc_sift = new CCSift(image);
    cc_sift->showSift("imgLabel");
}

CCSift *ImgLabel::getCCSift()
{
    return cc_sift;
}

cv::Mat &ImgLabel::getImage()
{
    return image;
}

void ImgLabel::readin()
{
    image = cv::imread(path.toStdString().c_str());
    cv::cvtColor(image,image,CV_BGR2RGB);
    img = mat2QImage(image);
}

QImage ImgLabel::mat2QImage(cv::Mat &mat)
{
    QImage img((uchar*)mat.data,
               mat.cols,mat.rows,
               mat.cols * mat.channels(),
               QImage::Format_RGB888);
    img.bits();
    return img;
}
