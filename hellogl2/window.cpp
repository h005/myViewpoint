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
#include "custom.h"

extern EntityManager manager;

Window::Window(MainWindow *mw, const QString &imagePath, const QString &modelPath, PointsMatchRelation &relation)
    : mainWindow(mw), relation(relation),
      m_modelpath(modelPath), m_imagepath(imagePath)
{
    QImage img(imagePath);
    m_iwidth = img.width();
    m_iheight = img.height();

    right = new GLWidget(modelPath, this);
    right->m_relation = &relation;

    left = new ImageAndPoint(imagePath, relation, this);
    dockBtn = new QPushButton(tr("Undock"), this);
    alignBtn = new QPushButton(tr("Align && See"), this);
    confirmBtn = new QPushButton(tr("Confirm && Uplevel"), this);
    clearBtn = new QPushButton(tr("Clear"), this);

    connect(dockBtn, SIGNAL(clicked()), this, SLOT(dockUndock()));
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
    mainLayout->addWidget(dockBtn);
    mainLayout->addWidget(w);
    setLayout(mainLayout);

    setWindowTitle(tr("Hello GL"));
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

void Window::align()
{
    glm::mat4 mvMatrix, projMatrix;
    std::vector<glm::vec2> &points2d = relation.getPoints2d();
    std::vector<glm::vec3> &points3d = relation.getPoints3d();
    DLTwithPoints(points2d.size(), (float(*)[2])&points2d[0], (float(*)[3])&points3d[0], m_iwidth, m_iheight, mvMatrix, projMatrix);

//    AlignResultWidget *a = new AlignResultWidget(m_modelpath, m_iwidth * 1.f / m_iheight, mvMatrix, projMatrix, 0);
//    a->show();

    Entity base, second, want;
    Q_ASSERT(manager.getEntity(manager.baseOneID(), base));
    Q_ASSERT(manager.getEntity(manager.baseTwoID(), second));

    float scale;
    {
        QImage img(manager.baseTwoImagePath());
        int width = img.width();
        int height = img.height();

        PointsMatchRelation *rb = new PointsMatchRelation(manager.baseTwoImageRelation());
        if (!rb->loadFromFile()) {
            return;
        }

        // IMPORTANT!!
        // reference variable don't support re-assign, 'operator =' will replace content(s)
        // in the original vector
        // ref http://stackoverflow.com/a/4364586/4104893
        std::vector<glm::vec2> &points2d = rb->getPoints2d();
        std::vector<glm::vec3> &points3d = rb->getPoints3d();
        glm::mat4 secondMVMatrix, secondProjMatrix;
        DLTwithPoints(points2d.size(), (float(*)[2])&points2d[0], (float(*)[3])&points3d[0], width, height, secondMVMatrix, secondProjMatrix);
        scale = recoveryScale(base, second, mvMatrix, secondMVMatrix);
        std::cout << "scale: " << scale << std::endl;
        std::cout << glm::to_string(mvMatrix) << std::endl;
        std::cout << glm::to_string(secondMVMatrix) << std::endl;
//        scale = 1;
    }

//    zyns
//    Q_ASSERT(manager.getEntity(QString("./img0075.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0613.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0777.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0819.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0820.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0834.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0832.jpg"), want));

//    notre dame
//    Q_ASSERT(manager.getEntity(QString("images/alecea_2304877304.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/32219531@N00_102756761.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/8250661@N08_514024275.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/cfuga_1435599238.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/81596301@N00_248194737.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/58308412@N00_74499252.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/achtundsiebzig_196444302.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/7437937@N06_428202066.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/al_9_1355240900.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/_fxr_2223134257.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/2pworth_50915720.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("images/celesteh_102619571.jpg"), want));

//    kxm
//    Q_ASSERT(manager.getEntity(QString("./img0008.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0314.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0532.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0852.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0837.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0410.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0831.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0386.jpg"), want));

//    tiananmen
//    Q_ASSERT(manager.getEntity(QString("./61bOOOPIC53.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./53ad4c7b61138.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./OOOPIC_robbin_200910130202f5809b3c9d39.jpg"), want));

//    Q_ASSERT(manager.getEntity(QString("./img0001.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0006.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0052.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0050.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0063.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0062.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0926.jpg"), want));
    Q_ASSERT(manager.getEntity(QString("./img1188.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img1185.jpg"), want));
//        Q_ASSERT(manager.getEntity(QString("./img1260.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0058.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0108.jpg"), want));
//    Q_ASSERT(manager.getEntity(QString("./img0081.jpg"), want));


    glm::mat4 wantMVMatrix, wantProjMatrix;
    recoveryCameraParameters(scale, base, want, mvMatrix, projMatrix, wantMVMatrix, wantProjMatrix);
    wantProjMatrix = glm::perspective(glm::pi<float>() / 2, m_iwidth * 1.f / m_iheight, 0.1f, 100.f);
    CameraShowWidget *b = new CameraShowWidget(m_modelpath, m_iwidth * 1.f / m_iheight, wantMVMatrix, 0);
    b->show();

    AlignResultWidget *c = new AlignResultWidget(m_modelpath, m_iwidth * 1.f / m_iheight, wantMVMatrix, wantProjMatrix, 0);
    c->show();

    {
    glm::vec4 a(1.f, 0.f, 0.f, 0.f), b(0.f, 1.f, 0.f, 0.f), c(0.f, 0.f, 1.f, 0.f);
    glm::vec4 aa = wantMVMatrix * a, bb = wantMVMatrix * b, cc = wantMVMatrix * c;
    //glm::vec4 aa = base.mvMatrix * a, bb = base.mvMatrix * b, cc = base.mvMatrix * c;
    //glm::vec4 aa = mvMatrix * a, bb = mvMatrix * b, cc = mvMatrix * c;
    std::cout << glm::to_string(want.mvMatrix) << std::endl;
    std::cout << glm::dot(glm::vec3(aa), glm::vec3(bb)) << std::endl;
    std::cout << glm::dot(glm::vec3(cc), glm::vec3(bb)) << std::endl;
    std::cout << glm::dot(glm::vec3(aa), glm::vec3(cc)) << std::endl;
    }
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
