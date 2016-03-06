#ifndef CCWINDOW_H
#define CCWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <iostream>
#include <fstream>
#include <QFileInfo>
#include "imglabel.h"
#include "ccmodelwidget.h"

class PointsMatchRelation;
class EntityManager;
class CCWindow : public QMainWindow
{
    Q_OBJECT

public:
    CCWindow();
    CCWindow(QString modelPath,QString imgPath,QString relationPath);
    ~CCWindow();

    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void getDLTpoints2D(float points2D[][2]);
    void getDLTpoints3D(float points3D[][3]);

private slots:
    void align();
    void exportInfo();
    void calibrate();
    void clearpoints();

private:
    QPushButton *alignBtn;
    QPushButton *exportBtn;
    QPushButton *calibrateBtn;
    QPushButton *pointsClear;
    QScrollArea *scrollArea;
    ImgLabel *imgLabel;
    CCModelWidget *ccMW;
    QFileInfo *imgFile;
    QFileInfo *modelFile;
    QFileInfo *relationFile; // export path

private:
    PointsMatchRelation *relation;

    QString modelPath;
    glm::mat4 cal_mv;
    glm::mat4 cal_proj;

};

#endif // CCWINDOW_H
