#include "mainentrywindow.h"
#include "ui_mainentrywindow.h"

#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <opencv2/opencv.hpp>
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
#include "TransformationUtils.h"
#include "OffscreenRender.h"
#include "pointcloudwidget.h"
#include "pointcloudcapturewidget.h"
#include "pointcloudoffscreenrender.h"
#include "lmdlt.h"
#include "LMModelMainComponent.h"
#include "externalimporter.h"

#define P_NEAR 0.5f
#define P_FAR 100000.f

//#define USE_DEFAULT_PROJECTION

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
    if (offscreenRender != NULL)
        delete offscreenRender;
}

QSize MainEntryWindow::sizeHint() const
{
    return QSize(400, 700);
}

QSize MainEntryWindow::minimumSizeHint() const
{
    return QSize(400, 700);
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

        AlignWindow *window = new AlignWindow(manager->modelPath(), manager->ptCloudPath(),manager->registrationFile());
        window->resize(window->sizeHint());
        window->show();

        ui->labelFirstImageBtn->setEnabled(true);
    }
}

QString target="./img0006.jpg";

void MainEntryWindow::on_executePreviewTargetBtn_clicked()
{
    // 获得模型到点云的变换
    glm::mat4 model2ptCloud = getModel2PtCloudTrans();

    Entity want;
    Q_ASSERT(manager->getEntity(target, want));
    QSize imgSize = GetImageParamter(target);
    glm::mat4 wantMVMatrix = want.mvMatrix * model2ptCloud;
    wantMVMatrix = normalizedModelView(wantMVMatrix);

    std::vector<glm::mat4> mvMatrixs;
    mvMatrixs.push_back(wantMVMatrix);
    CameraShowWidget *w = new CameraShowWidget(manager->modelPath(), imgSize.width() / 1.0 / imgSize.height(), mvMatrixs);
    w->show();
}


void MainEntryWindow::on_showAllViewpoints_clicked()
{
    glm::mat4 model2ptCloud = getModel2PtCloudTrans();

    std::vector<QString> list;
    manager->getImageList(list);
    std::vector<glm::mat4> mvMatrixs;
    for (auto it = list.begin(); it != list.end(); it++) {
        Entity want;
        Q_ASSERT(manager->getEntity(*it, want));
        glm::mat4 wantMVMatrix = want.mvMatrix * model2ptCloud;
        wantMVMatrix = normalizedModelView(wantMVMatrix);
        mvMatrixs.push_back(wantMVMatrix);
    }

    CameraShowWidget *w = new CameraShowWidget(manager->modelPath(), 1.f, mvMatrixs);
    w->show();
}

void MainEntryWindow::on_printMvMatrixBtn_clicked()
{
    // 输出MV
    QString selfilter = tr("Model View Matrix File (*.matrix)");
    QString fileName = QFileDialog::getSaveFileName(
            this,
            QString("打开配置文件"),
            QString(),
            tr("All files (*.*);;Model View Matrix File (*.matrix)" ),
            &selfilter
    );

    // 获得模型到点云的变换
    glm::mat4 model2ptCloud = getModel2PtCloudTrans();
    if (!fileName.isEmpty()) {
        qDebug() << fileName;
        std::ofstream matrixFile;
        matrixFile.open(fileName.toUtf8().constData());

        std::vector<QString> list;
        manager->getImageList(list);
        std::vector<QString>::iterator it;
        for (it = list.begin(); it != list.end(); it++) {

            qDebug() << "********************* " << it - list.begin() << " ****************";

            Entity want;
            Q_ASSERT(manager->getEntity(*it, want));
            glm::mat4 wantMVMatrix = want.mvMatrix * model2ptCloud;
            wantMVMatrix = normalizedModelView(wantMVMatrix);

            matrixFile << it->toUtf8().constData() << std::endl;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++)
                    matrixFile << wantMVMatrix[j][i] << " ";
                matrixFile << std::endl;
            }
        }

        qDebug() << "output finished";
        matrixFile.close();
    }
}

void MainEntryWindow::on_printMvPMatrixBtn_clicked()
{
    // 输出MV和P
    QString selfilter = tr("Model View Matrix File (*.matrix)");
    QString fileName = QFileDialog::getSaveFileName(
            this,
            QString("打开配置文件"),
            QString(),
            tr("All files (*.*);;Model View Matrix File (*.matrix)" ),
            &selfilter
    );

    // 获得模型到点云的变换
    glm::mat4 model2ptCloud = getModel2PtCloudTrans();
    if (!fileName.isEmpty()) {
        qDebug() << fileName;
        std::ofstream matrixFile;
        matrixFile.open(fileName.toUtf8().constData());

        std::vector<QString> list;
        manager->getImageList(list);
        std::vector<QString>::iterator it;
        for (it = list.begin(); it != list.end(); it++) {

            qDebug() << "********************* " << it - list.begin() << " ****************";

            Entity want;
            Q_ASSERT(manager->getEntity(*it, want));
            glm::mat4 wantMVMatrix = want.mvMatrix * model2ptCloud;
            wantMVMatrix = normalizedModelView(wantMVMatrix);

            matrixFile << it->toUtf8().constData() << std::endl;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++)
                    matrixFile << wantMVMatrix[j][i] << " ";
                matrixFile << std::endl;
            }

            QSize imgSize = GetImageParamter(*it);
            glm::mat4 wantProjMatrix = projectionMatrixWithFocalLength(want.f, imgSize.width(), imgSize.height(), P_NEAR, P_FAR);
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++)
                    matrixFile << wantProjMatrix[j][i] << " ";
                matrixFile << std::endl;
            }
        }
        qDebug() << "output finished";
        matrixFile.close();
    }
}

QSize MainEntryWindow::GetImageParamter(QString handler)
{
    // 先拼接，在去除路径中多余的信息
    QFileInfo fileInfo(ui->configFileLabel->text());
    QString finalPath = QDir::cleanPath(QDir(fileInfo.absoluteDir()).filePath(handler));
    qDebug() << finalPath;
    cv::Mat img = cv::imread(finalPath.toUtf8().constData());

    return QSize(img.size().width, img.size().height);
}

void MainEntryWindow::on_saveLabeledImages_clicked()
{
    if (offscreenRender != NULL && offscreenRender->isVisible()) {
            QString outputDir = QFileDialog::getExistingDirectory(
                    this,
                    QString("选定输出目录"),
                    QString()
            );
            qDebug() << "Selected Dir:" << outputDir;

            // 获得模型到点云的变换
            glm::mat4 model2ptCloud = getModel2PtCloudTrans();
            if (!outputDir.isEmpty()) {
                std::vector<QString> list;
                manager->getImageList(list);
                std::vector<QString>::iterator it;
                for (it = list.begin(); it != list.end(); it++) {
                    qDebug() << "********************* " << it - list.begin() << " ****************";

                    Entity want;
                    Q_ASSERT(manager->getEntity(*it, want));
                    QSize imgSize = GetImageParamter(*it);
                    glm::mat4 wantMVMatrix = want.mvMatrix * model2ptCloud;
                    wantMVMatrix = normalizedModelView(wantMVMatrix);
                    glm::mat4 wantProjMatrix = projectionMatrixWithFocalLength(want.f, imgSize.width(), imgSize.height(), P_NEAR, P_FAR);

                    // 将图片生成到选定目录中，文件名与之前一致，但后缀改为.m.png，方便对照
                    QFileInfo file(*it);
                    QString basename = file.baseName();
                    QString finalPath = QDir(outputDir).filePath(basename + ".m.png");
                    QString depthImgPath = QDir(outputDir).filePath(basename + ".d.bmp");

                    //更改窗口尺寸不可以即时生效，所以我们先渲染出图片再对图片进行缩放
                    offscreenRender->renderToImageFile(wantMVMatrix, wantProjMatrix, finalPath, depthImgPath, imgSize);
                }
                qDebug() << "finished" << endl;
            }
        } else {
            std::cout << "Please Open A Render Window First" << std::endl;
        }
}

void MainEntryWindow::on_openOffscreenRenderBtn_clicked()
{
    if (manager != NULL) {
        if (offscreenRender == NULL) {
            offscreenRender = new OffscreenRender(manager->modelPath(), NULL);
            offscreenRender->resize(offscreenRender->sizeHint());
        }
        offscreenRender->show();
        this->activateWindow();
    } else
        std::cout << "Please load config file first" << std::endl;
}


// 点云模型部分
void MainEntryWindow::on_openPtCloudModelBtn_clicked()
{
    PointCloudWidget *w = new PointCloudWidget(manager->ptCloudPath().toStdString());
    w->show();
}

void MainEntryWindow::on_seeLabeledResultInPtCloudBtn_clicked()
{
    Entity want;
    Q_ASSERT(manager->getEntity(target, want));

    // 获取图片宽高
    QSize imgSize = GetImageParamter(target);
    glm::mat4 wantProjMatrix = projectionMatrixWithFocalLength(want.f, imgSize.width(), imgSize.height(), P_NEAR, P_FAR);

    PointCloudCaptureWidget *w = new PointCloudCaptureWidget(manager->ptCloudPath().toStdString(), want.mvMatrix, wantProjMatrix);
    w->resize(imgSize);
    w->show();
    return;
}

void MainEntryWindow::on_savePtCloudLabeledResultBtn_clicked()
{
    // 使用相机的内外参数渲染到图片
    if (ptCloudOffscreenRender != NULL && ptCloudOffscreenRender->isVisible()) {
        QString outputDir = QFileDialog::getExistingDirectory(
                this,
                QString("选定输出目录"),
                QString()
        );
        qDebug() << "Selected Dir:" << outputDir;

        if (!outputDir.isEmpty()) {
            std::vector<QString> list;
            manager->getImageList(list);
            for (auto it = list.begin(); it != list.end(); it++) {
                qDebug() << "********************* " << it - list.begin() << " ****************";

                Entity want;
                Q_ASSERT(manager->getEntity(*it, want));
                QSize imgSize = GetImageParamter(*it);
                glm::mat4 projMatrix = projectionMatrixWithFocalLength(want.f, imgSize.width(), imgSize.height(), P_NEAR, P_FAR);

                // 将图片生成到选定目录中，文件名与之前一致，但后缀改为png，方便对照
                QFileInfo file(*it);
                QString basename = file.baseName();
                QString finalPath = QDir(outputDir).filePath(basename + ".png");

                //渲染过程中无法更改窗口大小，只能先渲染出图片再对图片进行缩放
                ptCloudOffscreenRender->renderToImageFile(want.mvMatrix, projMatrix, finalPath, imgSize);
            }
            qDebug() << "render finished" << endl;
        }
    } else {
        std::cout << "Please Open A Render Window First" << std::endl;
    }
}

void MainEntryWindow::on_openPtCloudLabeledWindowBtn_clicked()
{

    if (manager != NULL) {
        if (ptCloudOffscreenRender == NULL) {
            ptCloudOffscreenRender = new PointCloudOffscreenRender(manager->ptCloudPath().toStdString());
            ptCloudOffscreenRender->resize(QSize(800, 800));
        }
        ptCloudOffscreenRender->show();
        this->activateWindow();
    } else
        std::cout << "Please load config file first" << std::endl;
}

glm::mat4 MainEntryWindow::getModel2PtCloudTrans()
{
    // 从文件中读取
    PointsMatchRelation relation(manager->registrationFile());
    if (!relation.loadFromFile()) {
        std::cout << "read failed" << std::endl;
    }

    // 获得模型到点云的变换
    Q_ASSERT(relation.isPointsEqual());
    glm::mat3 R;
    glm::vec3 t;
    float c;
    LMDLT::ModelRegistration(relation.getModelPoints().size(), &relation.getPtCloudPoints()[0], &relation.getModelPoints()[0], R, t, c);

    // 整理为mat4
    glm::mat4 model2ptCloud = glm::mat4(c*R);
    model2ptCloud[3] = glm::vec4(t, 1.f);

    return model2ptCloud;
}

void MainEntryWindow::on_pushButton_2_clicked()
{
    std::vector<glm::vec3> vertices;
    ExternalImporter<float>::read_vertices(vertices, manager->modelPath().toStdString().c_str());
    std::vector<glm::vec3> dirs = LMModelMainComponent::getModelMainComponent(vertices.size(), &vertices[0]);
    auto firstN = dirs[0];
    auto secondN = dirs[1];
    auto thirdN = dirs[2];

    std::cout << glm::to_string(firstN) << std::endl;
    std::cout << glm::to_string(secondN) << std::endl;
    std::cout << glm::to_string(thirdN) << std::endl;

    std::cout << glm::dot(firstN, secondN) << std::endl;

    glm::mat3 R = glm::mat3(firstN, secondN, thirdN);
    std::cout << glm::to_string(R) << std::endl;

    glm::mat4 mv = glm::mat4(R);
    mv[3][3] = 1.f;

    std::vector<glm::mat4> mvMatrixs;
    mvMatrixs.push_back(mv);
    CameraShowWidget *w = new CameraShowWidget(manager->modelPath(), 1.0, mvMatrixs);
    w->show();
//    std::vector<glm::vec3> p;
//    for (int i = -100; i <= 100; i++)
//        for (int j = -50; j <= 50; j++)
//            p.push_back(glm::vec3((float)i, 0, (float)j));

//    std::cout << glm::to_string(LMPCA::PCAWithModelPoints(p.size(), &p[0])) << std::endl;
}
