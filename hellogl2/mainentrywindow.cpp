#include "mainentrywindow.h"
#include "ui_mainentrywindow.h"

#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

#include "entitymanager.h"
#include "pointsmatchrelation.h"
#include "alignwindow.h"

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


void MainEntryWindow::on_labelFirstImageBtn_clicked()
{
    if (manager != NULL) {
        ui->labelFirstImageBtn->setEnabled(false);

        AlignWindow *window = new AlignWindow(manager->baseOneImagePath(), manager->modelPath(), manager->baseOneImageRelation(), *manager);
        window->resize(window->sizeHint());
        window->show();

        ui->labelFirstImageBtn->setEnabled(true);
    }
}

void MainEntryWindow::on_labelSecondImageBtn_clicked()
{
    if (manager != NULL) {
        ui->labelSecondImageBtn->setEnabled(false);

        AlignWindow *window = new AlignWindow(manager->baseTwoImagePath(), manager->modelPath(), manager->baseTwoImageRelation(), *manager);
        window->resize(window->sizeHint());
        window->show();

        ui->labelSecondImageBtn->setEnabled(true);
    }
}
