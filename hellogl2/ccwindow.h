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
#include "ccsiftmatch.h"
#include "ccsiftmatchwindow.h"
#include <set>

class PointsMatchRelation;
class EntityManager;
class CCWindow : public QMainWindow
{
    Q_OBJECT

public:
    CCWindow();
    CCWindow(QString modelPath, QString imgFilePath, QString imgDir, QString outputPath, QString relationPath);
    ~CCWindow();

    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void getDLTpoints2D(float points2D[][2]);
    void getDLTpoints3D(float points3D[][3]);
    void calibrateLevmar(glm::mat3 &R,glm::vec3 &t,float &c);

    void fillImgsVec();

private slots:
    void align();
    void alignDLT();
    void exportInfo();
    void exportMVP();
    void calibrate();
    void clearpoints();
    void siftMatch();
    void showPoints();
    void confirm();

private:
    QPushButton *alignBtn;
    QPushButton *exportBtn;
    QPushButton *calibrateBtn;
    QPushButton *siftMatchBtn;
    QPushButton *pointsClear;
    QPushButton *alignDLTBtn;
    QPushButton *confirmBtn;
    QScrollArea *scrollArea;
    ImgLabel *imgLabel;
    CCModelWidget *ccMW;
    QFileInfo *imgsFile;
    QFileInfo *modelFile;
    QFileInfo *relationFile; // export path
    QFileInfo *outputFile;
    QString imgDir;
    CCSiftMatchWindow *ccSMW;
    std::vector<QString> imgsVec;
    // calibraeted done
    std::map<QString,glm::mat4> imgsMV;
    std::map<QString,glm::mat4> imgsProj;
    int tcase;
private:
    PointsMatchRelation *relation;

    QString modelPath;
    glm::mat4 cal_mv;
    glm::mat4 cal_proj;
    glm::mat4 cc_st; // scale and translate matrix
    CCSiftMatch *ccSiftMatch;

};

#endif // CCWINDOW_H
