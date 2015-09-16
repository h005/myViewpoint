#include "mainentrywindow.h"
#include "ui_mainentrywindow.h"

#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

#include "entitymanager.h"
#include "pointsmatchrelation.h"
#include "window.h"

MainEntryWindow::MainEntryWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainEntryWindow),
    manager(NULL)
{
    ui->setupUi(this);
}

MainEntryWindow::~MainEntryWindow()
{
    delete ui;
}

QSize MainEntryWindow::sizeHint() const
{
    return QSize(400, 400);
}

QSize MainEntryWindow::minimumSizeHint() const
{
    return QSize(400, 400);
}

void MainEntryWindow::on_labelFirstImageBtn_clicked()
{
    if (manager != NULL) {
//        PointsMatchRelation *relation = new PointsMatchRelation(manager->baseOneImageRelation());
//        if (!relation->loadFromFile()) {
//            std::cout << "read failed" << std::endl;
//            return;
//        }
//        std::cout << relation->getPoints2d().size() << std::endl;
//        Window *window = new Window(this, manager->baseOneImagePath(), manager->modelPath(), *relation);
//        window->resize(window->sizeHint());
//        window->show();
    }
}

void MainEntryWindow::on_pushButton_clicked()
{
    QString selfilter = tr("Config (*.ini)");
    QString fileName = QFileDialog::getOpenFileName(
            this,
            QString("打开配置文件"),
            QString(),
            tr("All files (*.*);;Config (*.ini)" ),
            &selfilter
    );

    if (!fileName.isEmpty()) {
        QFileInfo file(fileName);
        qDebug() << file.absolutePath();
        manager = new EntityManager(file.absolutePath());

        if (!manager->load()) {
            QMessageBox::about(this, "提示", fileName + " 打开失败");
            delete manager;
            manager = NULL;
            return;
        }

        ui->configFileLabel->setText(fileName);
    }
}
