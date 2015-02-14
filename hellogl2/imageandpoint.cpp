#include "imageandpoint.h"

#include <iostream>
#include <QLabel>
#include <QString>
#include <QImage>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPainter>
#include <glm/gtx/string_cast.hpp>

ImageAndPoint::ImageAndPoint(const QString &imagePath, QWidget *parent) :  QLabel(parent)
{
    image = new QImage(imagePath);
    if (image->width() == 0) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Waring");
        msgBox.setText("File: " + imagePath + " not exists");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
          // do something
        }else {
          // do something else
        }
    }

    setPixmap(QPixmap::fromImage(*image));
    setBackgroundRole(QPalette::Base);
    setScaledContents(true);
    setStyleSheet("border: 1px solid black");

    // 取消内边框
    setContentsMargins(0, 0, 0, 0);
}

int ImageAndPoint::addPoint(const QPoint &p) {
    // 将窗口坐标系（原点在左上角）转换为图像坐标系（原点在左下角）
    // 由于图片在控件中不是一对一显示，所以还存在尺度缩放变换
    float x = p.x() * 1.f / this->size().width() * image->size().width();
    float y = (this->size().height() - p.y()) * 1.f / this->size().height() * image->size().height();
    glm::vec2 point = glm::vec2(x, y);
    std::cout << glm::to_string(point) << std::endl;
    points.push_back(point);
    redisplay();
    return points.size();
}

bool ImageAndPoint::removePoint() {
    if (points.size() > 0) {
        points.pop_back();
        return true;
    }
    return false;
}

std::vector<glm::vec2> ImageAndPoint::getPoints() {
    return points;
}

void ImageAndPoint::redisplay()
{
    QImage tmp(*image);
    QPainter painter( &tmp );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( Qt::red );
    painter.setBrush(Qt::red);

    std::vector<glm::vec2>::iterator it;
    for (it = points.begin(); it != points.end(); it++) {
        // 准备将点绘制到tmp上，坐标转换
        // 长宽以tmp为准
        float iwidth = image->size().width();
        float iheight = image->size().height();
        float x = it->x / iwidth * tmp.size().width();
        float y = (1 - it->y / iheight) * tmp.size().height();
        QPoint p1;
        p1.setX(round(x));
        p1.setY(round(y));
        painter.drawEllipse(p1, 5, 5);
    }
    setPixmap(QPixmap::fromImage(tmp));
}

ImageAndPoint::~ImageAndPoint()
{
    delete image;
}

