#include "mainentrywindow.h"
#include "ui_mainentrywindow.h"

#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "entitymanager.h"
#include "pointsmatchrelation.h"
#include "alignwindow.h"
#include "camerashowwidget.h"
#include "alignresultwidget.h"
#include "imageandpoint.h"
#include "pointsmatchrelation.h"
#include "camerashowwidget.h"
#include "DLT.h"
#include "entity.h"
#include "custom.h"

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

        AlignWindow *window = new AlignWindow(manager->baseOneImagePath(), manager->modelPath(), manager->baseOneImageRelation());
        window->resize(window->sizeHint());
        window->show();

        ui->labelFirstImageBtn->setEnabled(true);
    }
}

void MainEntryWindow::on_labelSecondImageBtn_clicked()
{
    if (manager != NULL) {
        ui->labelSecondImageBtn->setEnabled(false);

        AlignWindow *window = new AlignWindow(manager->baseTwoImagePath(), manager->modelPath(), manager->baseTwoImageRelation());
        window->resize(window->sizeHint());
        window->show();

        ui->labelSecondImageBtn->setEnabled(true);
    }
}

void MainEntryWindow::on_executePreviewTargetBtn_clicked()
{
    // 根据目前标定的结果，恢复新图片的外参矩阵
    glm::mat4 wantMVMatrix, wantProjMatrix;
    RecoveryMvMatrixYouWant(QString("./img0063.jpg"), wantMVMatrix);
    wantProjMatrix = glm::perspective(glm::pi<float>() / 2, 1.f, 0.1f, 100.f);

    // [GUI]把目标图像的相机位置展现出来
    CameraShowWidget *b = new CameraShowWidget(manager->modelPath(), 1.f, wantMVMatrix, 0);
    b->show();

    // [GUI]用目标图像的相机参数，渲染模型
    AlignResultWidget *c = new AlignResultWidget(manager->modelPath(), 1.f, wantMVMatrix, wantProjMatrix, 0);
    c->show();
}

void MainEntryWindow::RecoveryMvMatrixYouWant(QString handler, glm::mat4 &wantMVMatrix)
{
    // 这个传递算法依赖两张基准图像，base和second，这两张图像的相机参数是直接通过DLT得到的
    // want是要求的图像，它的参数借助base和second的参数和SfM结果传递得到
    Entity base, second, want;
    Q_ASSERT(manager->getEntity(manager->baseOneID(), base));
    Q_ASSERT(manager->getEntity(manager->baseTwoID(), second));

    // 下面的算法在我04-05的周报中描述了
    float scale;
    glm::mat4 firstMVMatrix, firstProjMatrix;
    glm::mat4 secondMVMatrix, secondProjMatrix;
    {
        // 对第一张图片执行 优化+DLT 过程，恢复出相机的内外参矩阵
        QImage img(manager->baseOneImagePath());
        int width = img.width();
        int height = img.height();

        PointsMatchRelation *rb = new PointsMatchRelation(manager->baseOneImageRelation());
        if (!rb->loadFromFile() || rb->getPoints2d().size() < 6) {
            delete rb;
            QMessageBox::information(this, tr("错误"), tr("第一张图片标定未完成"));
            return;
        }

        // IMPORTANT!!
        // reference variable don't support re-assign, 'operator =' will replace content(s)
        // in the original vector
        // ref http://stackoverflow.com/a/4364586/4104893
        std::vector<glm::vec2> &points2d = rb->getPoints2d();
        std::vector<glm::vec3> &points3d = rb->getPoints3d();
        DLTwithPoints(points2d.size(), (float(*)[2])&points2d[0], (float(*)[3])&points3d[0], width, height, firstMVMatrix, firstProjMatrix);
        delete rb;
    }

    {
        // 对第二张图片执行 优化+DLT 过程，恢复出相机的内外参矩阵
        QImage img(manager->baseTwoImagePath());
        int width = img.width();
        int height = img.height();

        PointsMatchRelation *rb = new PointsMatchRelation(manager->baseTwoImageRelation());
        if (!rb->loadFromFile() || rb->getPoints2d().size() < 6) {
            delete rb;
            QMessageBox::information(this, tr("错误"), tr("第二张图片标定未完成"));
            return;
        }

        // IMPORTANT!!
        // reference variable don't support re-assign, 'operator =' will replace content(s)
        // in the original vector
        // ref http://stackoverflow.com/a/4364586/4104893
        std::vector<glm::vec2> &points2d = rb->getPoints2d();
        std::vector<glm::vec3> &points3d = rb->getPoints3d();
        DLTwithPoints(points2d.size(), (float(*)[2])&points2d[0], (float(*)[3])&points3d[0], width, height, secondMVMatrix, secondProjMatrix);
        delete rb;
    }

    scale = recoveryScale(base, second, firstMVMatrix, secondMVMatrix);
    std::cout << "scale: " << scale << std::endl;
    std::cout << glm::to_string(firstMVMatrix) << std::endl;
    std::cout << glm::to_string(secondMVMatrix) << std::endl;


    Q_ASSERT(manager->getEntity(handler, want));


    // 在得到两个坐标系的缩放系数后，用第一张图片的DLT结果恢复出你想要的图片的结果
    recoveryCameraParameters(scale, base, want, firstMVMatrix, wantMVMatrix);
}
