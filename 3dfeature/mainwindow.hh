#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QWidget>

class QPushButton;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSize sizeHint() const Q_DECL_OVERRIDE;

private:
    QPushButton *dockBtn;

signals:

private slots:
    void startMyProcess();

};

#endif // MAINWINDOW_H
