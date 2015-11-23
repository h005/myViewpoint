#ifndef MAINENTRYWINDOW_H
#define MAINENTRYWINDOW_H

#include <QMainWindow>
#include <Qstring>
#include <glm/glm.hpp>

namespace Ui {
class MainEntryWindow;
}

class EntityManager;
class OffscreenRender;
class PointCloudOffscreenRender;

class MainEntryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainEntryWindow(QWidget *parent = 0);
    ~MainEntryWindow();

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

private slots:
    void on_labelFirstImageBtn_clicked();

    void on_pushButton_clicked();

    void on_labelSecondImageBtn_clicked();

    void on_executePreviewTargetBtn_clicked();

    void on_saveLabeledResultBtn_clicked();

    void on_saveLabeledImages_clicked();

    void on_openOffscreenRenderBtn_clicked();

    void on_printMvMatrixBtn_clicked();

    void on_printMvPMatrixBtn_clicked();

    void on_saveLabeledResultBtn_2_clicked();

    void on_openPtCloudModelBtn_clicked();

    void on_seeLabeledResultInPtCloudBtn_clicked();

    void on_savePtCloudLabeledResultBtn_clicked();

    void on_openPtCloudLabeledWindowBtn_clicked();

private:
    Ui::MainEntryWindow *ui;
    EntityManager *manager;
    OffscreenRender *offscreenRender = NULL;
    PointCloudOffscreenRender *ptCloudOffscreenRender = NULL;
    glm::mat4 getModel2PtCloudTrans();
    QSize GetImageParamter(QString handler);
};

#endif // MAINENTRYWINDOW_H
