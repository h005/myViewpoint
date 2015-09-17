#ifndef MAINENTRYWINDOW_H
#define MAINENTRYWINDOW_H

#include <QMainWindow>
#include <Qstring>
#include <glm/glm.hpp>

namespace Ui {
class MainEntryWindow;
}

class EntityManager;

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

private:
    Ui::MainEntryWindow *ui;
    EntityManager *manager;
    void RecoveryMvMatrixYouWant(QString handler, glm::mat4 &wantMVMatrix);
};

#endif // MAINENTRYWINDOW_H
