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

#include "mainwindow.h"
#include "window.h"
#include <iostream>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include "pointsmatchrelation.h"
#include "entitymanager.h"
#include "custom.h"

EntityManager manager(QString("D:\\testcase"));

MainWindow::MainWindow()
{
    QMenuBar *menuBar = new QMenuBar;
    QMenu *menuWindow = menuBar->addMenu(tr("&Window"));
    QAction *addNew = new QAction(menuWindow);
    addNew->setText(tr("Add new"));
    menuWindow->addAction(addNew);
    connect(addNew, SIGNAL(triggered()), this, SLOT(onAddNew()));
    setMenuBar(menuBar);

    onAddNew();
}

void MainWindow::onAddNew()
{
    std::cout << manager.load() << std::endl;

    if (!centralWidget()) {
        PointsMatchRelation *relation = new PointsMatchRelation(manager.baseOneImageRelation());
        if (!relation->loadFromFile()) {
            std::cout << "read failed" << std::endl;
        }
        std::cout << relation->getPoints2d().size() << std::endl;
        setCentralWidget(new Window(this, manager.baseOneImagePath(), manager.modelPath(), *relation));
        //setCentralWidget(new Window(this, QString("D:\\alex1961_2466374890.rd.jpg"), QString("D:\\no\\models\\untitled.dae"), *relation));
//        setCentralWidget(new Window(this, QString("C:\\Users\\mzd\\Desktop\\NotreDame\\NotreDame\\images\\85428086@N00_231122289.jpg"), QString("D:\\no2\\models\\model.dae"), *relation));
    }
    else
        QMessageBox::information(0, tr("Cannot add new window"), tr("Already occupied. Undock first."));

}
