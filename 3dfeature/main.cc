#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>
#include <QWidget>
#include "common.hh"
#include "mainwindow.hh"

int main(int argc, char *argv[])
{
    // 这里只是为了启动Qt环境，下面的代码不用改
    QApplication app(argc, argv);

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    if (QCoreApplication::arguments().contains(QStringLiteral("--multisample")))
        fmt.setSamples(4);
    if (QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"))) {
        fmt.setVersion(3, 2);
        fmt.setProfile(QSurfaceFormat::CoreProfile);
    }
    QSurfaceFormat::setDefaultFormat(fmt);

    // 这个窗口带有一个按钮，点击这个按钮可以*异步*调用到自己的窗口函数
    MainWindow *mw = new MainWindow();
    mw->show();

    return app.exec();
}
