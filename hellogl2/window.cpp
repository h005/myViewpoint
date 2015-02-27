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
#include "mainwindow.h"
#include <iostream>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>

#include "imageandpoint.h"
#include "pointsmatchrelation.h"

Window::Window(MainWindow *mw, const QString &imagePath, const QString &modelPath, PointsMatchRelation &relation)
    : mainWindow(mw), relation(relation)
{
    right = new GLWidget(relation, this);
    left = new ImageAndPoint(this, imagePath, relation);

    // 将左右窗口加入布局管理器
    QHBoxLayout *container = new QHBoxLayout;

    QSizePolicy cellPolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cellPolicy.setHorizontalStretch(1);
    left->setSizePolicy(cellPolicy);
    right->setSizePolicy(cellPolicy);
    container->addWidget(left);
    container->addWidget(right);

    // 布局用，类似于div
    QWidget *w = new QWidget;
    w->setLayout(container);

    dockBtn = new QPushButton(tr("Undock"), this);
    connect(dockBtn, SIGNAL(clicked()), this, SLOT(dockUndock()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(dockBtn);
    mainLayout->addWidget(w);
    setLayout(mainLayout);

    setWindowTitle(tr("Hello GL"));
}

Window::~Window()
{
}

QSlider *Window::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
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
            std::cout << p.x() << std::endl;
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
            std::cout << p.x() << std::endl;
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
    event->accept();
}

void Window::dockUndock()
{
    if (parent()) {
        setParent(0);
        setAttribute(Qt::WA_DeleteOnClose);
        move(QApplication::desktop()->width() / 2 - width() / 2,
             QApplication::desktop()->height() / 2 - height() / 2);
        dockBtn->setText(tr("Dock"));
        show();
    } else {
        if (!mainWindow->centralWidget()) {
            if (mainWindow->isVisible()) {
                setAttribute(Qt::WA_DeleteOnClose, false);
                dockBtn->setText(tr("Undock"));
                mainWindow->setCentralWidget(this);
            } else {
                QMessageBox::information(0, tr("Cannot dock"), tr("Main window already closed"));
            }
        } else {
            QMessageBox::information(0, tr("Cannot dock"), tr("Main window already occupied"));
        }
    }
}
