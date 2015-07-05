#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>

#include "common.hh"
#include "externalimporter.hh"
#include "mainwindow.hh"

int main(int argc, char *argv[])
{
    // Qt App
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

    MainWindow mainWindow;
    mainWindow.resize(mainWindow.sizeHint());
    int desktopArea = QApplication::desktop()->width() *
                     QApplication::desktop()->height();
    int widgetArea = mainWindow.width() * mainWindow.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f)
        mainWindow.show();
    else
        mainWindow.showMaximized();

    std::cout << "MainWindow: show complete" << std::endl;

    return app.exec();
}
