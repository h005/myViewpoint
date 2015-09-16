#ifndef MAINENTRYWINDOW_H
#define MAINENTRYWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainEntryWindow *ui;

private:
    EntityManager *manager;
};

#endif // MAINENTRYWINDOW_H
