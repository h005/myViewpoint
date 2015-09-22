/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include "window.h"
#include "alignwindow.h"
#include <iostream>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "imageandpoint.h"
#include "pointsmatchrelation.h"
#include "alignresultwidget.h"
#include "camerashowwidget.h"
#include "DLT.h"
#include "entitymanager.h"
#include "entity.h"
#include "TransformationUtils.h"

Window::Window(AlignWindow *mw, const QString &imagePath, const QString &modelPath, PointsMatchRelation &relation)
    : mainWindow(mw),
      relation(relation),
      m_modelpath(modelPath),
      m_imagepath(imagePath)
{
    QImage img(imagePath);
    m_iwidth = img.width();
    m_iheight = img.height();

    right = new GLWidget(modelPath, this);
    right->m_relation = &relation;

    left = new ImageAndPoint(imagePath, relation, this);
    alignBtn = new QPushButton(tr("Align && See"), this);
    confirmBtn = new QPushButton(tr("Confirm && Uplevel"), this);
    clearBtn = new QPushButton(tr("Clear"), this);

    connect(alignBtn, SIGNAL(clicked()), this, SLOT(align()));
    connect(confirmBtn, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(clearBtn, SIGNAL(clicked()), this, SLOT(clearPressed()));


    QSizePolicy cellPolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    cellPolicy.setHorizontalStretch(1);
    left->setSizePolicy(cellPolicy);
    right->setSizePolicy(cellPolicy);

    // 最中间的一列按钮
    QVBoxLayout *middleLayout = new QVBoxLayout;
    middleLayout->addWidget(alignBtn);
    middleLayout->addWidget(confirmBtn);
    middleLayout->addWidget(clearBtn);
    QWidget *middle = new QWidget;
    middle->setLayout(middleLayout);

    // 左中右窗口加入布局管理器
    QHBoxLayout *container = new QHBoxLayout;
    container->addWidget(left);
    container->addWidget(middle);
    container->addWidget(right);
    QWidget *w = new QWidget;
    w->setLayout(container);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(w);
    setLayout(mainLayout);

    setWindowTitle(tr("Hello GL"));
    qDebug() << "finished";
}

Window::~Window()
{
}

QSize Window::sizeHint() const
{
    return QSize(1024, 768);
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else if (e->key() == Qt::Key_0) {
        // 增加一个点
        QSize rsize = right->size();
        QSize lsize = left->size();
        QPoint p;

        // 检测鼠标是否在右侧(模型)
        p = right->mapFromGlobal(QCursor::pos());
        if ((p.x() >= 0 && p.x() < rsize.width())
                && (p.y() >= 0 && p.y() < rsize.height())) {
            std::cout << right->addPoint(p) << std::endl;
        }

        // 检测鼠标是否在左侧(图片)
        p = left->mapFromGlobal(QCursor::pos());
        if ((p.x() >= 0 && p.x() < lsize.width())
                      && (p.y() >= 0 && p.y() < lsize.height())) {
            std::cout << left->addPoint(p) << std::endl;
        }
    } else if (e->key() == Qt::Key_Minus) {
        // 删除一个点
        QSize rsize = right->size();
        QSize lsize = left->size();
        QPoint p;

        // 检测鼠标是否在右侧(模型)
        p = right->mapFromGlobal(QCursor::pos());
        if ((p.x() >= 0 && p.x() < rsize.width())
                && (p.y() >= 0 && p.y() < rsize.height())) {
            std::cout << right->removeLastPoint() << std::endl;
        }

        // 检测鼠标是否在左侧(图片)
        p = left->mapFromGlobal(QCursor::pos());
        if ((p.x() >= 0 && p.x() < lsize.width())
                      && (p.y() >= 0 && p.y() < lsize.height())) {
            std::cout << left->removePoint() << std::endl;
        }
    } else
        QWidget::keyPressEvent(e);
}

void Window::closeEvent(QCloseEvent *event)
{
    assert(relation.isPointsEqual());
    event->accept();
}

void Window::align()
{
    glm::mat4 mvMatrix, projMatrix;
    std::vector<glm::vec2> &points2d = relation.getPoints2d();
    std::vector<glm::vec3> &points3d = relation.getPoints3d();
    DLTwithPoints(points2d.size(), (float(*)[2])&points2d[0], (float(*)[3])&points3d[0], m_iwidth, m_iheight, mvMatrix, projMatrix);

    // [GUI]把DLT标定结果显示出来
    AlignResultWidget *a = new AlignResultWidget(m_modelpath, m_iwidth * 1.f / m_iheight, mvMatrix, projMatrix, 0);
    a->show();
}

void Window::confirm()
{
    if (relation.saveToFile()) {
        std::cout << "saved" << std::endl;
    } else {
        std::cout << "failed" << std::endl;
    }
}

void Window::clearPressed()
{
    relation.getPoints2d().clear();
    relation.getPoints3d().clear();
    left->redisplay();
    right->update();
}
