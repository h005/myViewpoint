#include "imglabel.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPixmap>
#include "opencv2/xfeatures2d.hpp"

ImgLabel::ImgLabel(QString path, QWidget *parent) :
    QLabel(parent)
{
    scaleRatio = 1.0;
    ccSiftMatch = NULL;
    flagMatch = false;
    flagDrawRect = false;
    cc_sift = NULL;
    this->path = path;
    readin();
    QPixmap pixmap = QPixmap::fromImage(img);
    this->setPixmap(pixmap);
}

ImgLabel::ImgLabel(CCSiftMatch *ccSiftMatch, CCModelWidget *ccMW, QWidget *parent) :
    QLabel(parent)
{
    this->ccMW = ccMW;
    this->ccSiftMatch = ccSiftMatch;
    flagMatch = true;
    flagDrawRect = true;
    cc_sift = NULL;

    cv::Mat tmp = ccSiftMatch->getMatchImg();
    tmp.copyTo(image);
    this->img = mat2QImage(image);

    tmp = ccSiftMatch->getRawMatchImg();
    tmp.copyTo(rawImage);
    this->rawImg = mat2QImage(rawImage);
    // show match image with correspond lines
    showMatchResult();
    //    setPoints(ccSiftMatch->getImagePoints())
    // 设置点之前需要先检测3D模型中的这个点是否存在
    // 如果存在则得到其索引
    std::vector<int> index;
    std::vector<cv::Point2f> imgPoints = ccSiftMatch->getImagePoints();
    std::cout << "imagelabel points initial " << std::endl;
    for(int i=0;i<imgPoints.size();i++)
    {
        index.push_back(i);
        std::cout << imgPoints[i].x  << " "<<imgPoints[i].y << std::endl;
    }
    ccMW->setPoints(ccSiftMatch->getModelPoints(),index);
    setPoints(ccSiftMatch->getImagePoints(),index);

}


ImgLabel::~ImgLabel()
{
    image.release();
    delete cc_sift;
}

void ImgLabel::mousePressEvent(QMouseEvent *e)
{
    if(!flagMatch)
        points.push_back(QPointF((float)e->x(),(float)e->y()));
    else
    {
        flagDrawRect = true;
        from.setX(e->x());
        from.setY(e->y());
    }
    update();
}

void ImgLabel::mouseMoveEvent(QMouseEvent *e)
{
    if(flagMatch)
    {
        to.setX(e->x());
        to.setY(e->y());
        update();
    }
}

void ImgLabel::mouseReleaseEvent(QMouseEvent *e)
{
    if(flagMatch)
    {
        to.setX(e->x());
        to.setY(e->y());
        std::cout << "image label points size " << points.size() << std::endl;
        clearRegionPoints();
        flagDrawRect = false;
        ccSiftMatch->setKeyPointsCCsift1(points);
        update();
    }
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

    if(flagMatch && flagDrawRect)
    {
        float minx = from.x() < to.x() ? from.x() : to.x();
        float miny = from.y() < to.y() ? from.y() : to.y();
        float maxx = from.x() > to.x()  ? from.x() : to.x();
        float maxy = from.y() > to.y() ? from.y() : to.y();
        painter->drawRect(minx,miny,maxx-minx,maxy-miny);
    }

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

double ImgLabel::getScaleRatio()
{
    return scaleRatio;
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

void ImgLabel::updateImg()
{
    cv::Mat tmp = ccSiftMatch->getMatchImg();
    tmp.copyTo(image);
    this->img = mat2QImage(image);

    tmp = ccSiftMatch->getRawMatchImg();
    tmp.copyTo(rawImage);
    this->rawImg = mat2QImage(rawImage);

    showMatchResult();

    std::vector<int> index;
//    ccMW->setPoints(ccSiftMatch->getModelPoints(),index);
//    setPoints(ccSiftMatch->getImagePoints(),index);
    std::vector<cv::Point2f> imgPoints = ccSiftMatch->getImagePoints();
    std::cout << "imagelabel points " << std::endl;
    for(int i=0;i<imgPoints.size();i++)
    {
        index.push_back(i);
        std::cout << imgPoints[i].x  << " "<<imgPoints[i].y << std::endl;
    }
    setPoints(ccSiftMatch->getImagePoints(),index);
}

void ImgLabel::readin()
{
    image = cv::imread(path.toStdString().c_str());
    iniSize = QSize(image.cols,image.rows);

    imageResize();

    img = mat2QImage(image);
}

void ImgLabel::imageResize()
{
    if(image.rows <= HEIGHT && image.cols <= WIDTH)
        return;
    else
    {
        double ratioWidth = (double)WIDTH / image.cols;
        double ratioHeight = (double)HEIGHT / image.rows;
        std::cout << "rate " << ratioWidth << " " << ratioHeight << std::endl;
        scaleRatio = ratioWidth < ratioHeight ? ratioWidth : ratioHeight;
        cv::Size dsize(image.cols * scaleRatio,image.rows * scaleRatio);
        std::cout << "dsize " << dsize.width << " " << dsize.height << std::endl;
        cv::resize(image,image,dsize);
    }
}

QImage ImgLabel::mat2QImage(cv::Mat &mat)
{
    cv::cvtColor(mat,mat,CV_BGR2RGB);
    QImage img((uchar*)mat.data,
               mat.cols,mat.rows,
               mat.cols * mat.channels(),
               QImage::Format_RGB888);
    img.bits();
    return img;
}

void ImgLabel::clearRegionPoints()
{
    std::vector<cv::Point2f> modelPoints = ccSiftMatch->getModelPoints();
    float minx = from.x() > to.x() ? to.x() : from.x();
    float miny = from.y() > to.y() ? to.y() : from.y();
    float maxx = from.x() > to.x() ? from.x() : to.x();
    float maxy = from.y() > to.y() ? from.y() : to.y();
    // delete points in model
    std::vector<cv::Point2f>::iterator itModel = modelPoints.begin();
    std::vector<QPointF>::iterator it = points.begin();
    int width = ccSiftMatch->getImWidth1();
    for(;itModel != modelPoints.end(); )
    {
        if((itModel->x + width > minx && itModel->x + width < maxx)
                && (itModel->y > miny && itModel->y < maxy))
        {
            std::cout << "region points clear " << it->x() << " " << it->y() << std::endl;
            modelPoints.erase(itModel);
            points.erase(it);
        }
        else
        {
            it++;
            itModel++;
        }
    }

    it = points.begin();
    for(;it != points.end();)
    {
        if((it->x() > minx && it->x() < maxx)
       &&(it->y() > miny && it->y() < maxy))
            points.erase(it);
        else
            it++;
    }
}

void ImgLabel::showMatchResult()
{
    QPixmap pixmap = QPixmap::fromImage(img);
    this->setPixmap(pixmap);
}

void ImgLabel::showRawMatch()
{
    QPixmap pixmap = QPixmap::fromImage(rawImg);
    this->setPixmap(pixmap);
}


