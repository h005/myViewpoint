#include "imageandpoint.h"

#include <iostream>
#include <QLabel>
#include <QString>
#include <QImage>
#include <QVBoxLayout>
#include <QMessageBox>

ImageAndPoint::ImageAndPoint(const QString &imagePath, QWidget *parent) : QWidget(parent)
{
    label = new QLabel(this);
    label->setBackgroundRole(QPalette::Base);
    label->setScaledContents(true);

    image = new QImage(imagePath);
    if (image->width() == 0) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("title");
        msgBox.setText("File: " + imagePath + " not exists");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
          // do something
        }else {
          // do something else
        }
    }
    label->setPixmap(QPixmap::fromImage(*image));
    label->setStyleSheet("border: 1px solid black");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    setLayout(mainLayout);

    // 取消内边框
    setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

int ImageAndPoint::addPoint(const QPoint &p) {
    points.push_back(glm::vec2(p.x(), p.y()));
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

ImageAndPoint::~ImageAndPoint()
{
    delete image;
}

