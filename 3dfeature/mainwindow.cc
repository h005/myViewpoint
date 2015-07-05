#include "mainwindow.hh"
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include "glwidget.hh"
#include "externalimporter.hh"
#include "gausscurvature.hh"
#include "meancurvature.hh"

MyMesh mesh;

MainWindow::MainWindow()
{
    QMenuBar *menuBar = new QMenuBar;
    QMenu *menuWindow = menuBar->addMenu(tr("&Window"));
    QAction *addNew = new QAction(menuWindow);
    addNew->setText(tr("Add new"));
    menuWindow->addAction(addNew);
    connect(addNew, SIGNAL(triggered()), this, SLOT(onAddNew()));
    setMenuBar(menuBar);

    onAddNew();
}

void MainWindow::onAddNew()
{
    // 从文件中读入mesh。ExternalImporter是我实现的模版类，支持的格式更多些
#ifdef BUILDIN_READER
    if ( ! OpenMesh::IO::read_mesh(mesh, "1.off") )
    {
        std::cerr << "Error: Cannot read mesh from " << std::endl;
        return 1;
    }
#else
    if (!ExternalImporter<MyMesh>::read_mesh(mesh, "1.off") )
    {
        std::cerr << "Error: Cannot read mesh from " << std::endl;
        return;
    }
#endif

    GaussCurvature<MyMesh> a(mesh);
    MeanCurvature<MyMesh> b(mesh);
    a.assignVertexColor();

    OpenMesh::IO::Options opt(OpenMesh::IO::Options::VertexColor);
    OpenMesh::IO::write_mesh(mesh, "temp.off", opt);

    if (!centralWidget()) {
        setCentralWidget(new GLWidget(mesh, this));
    }
    else
        QMessageBox::information(0, tr("Cannot add new window"), tr("Already occupied. Undock first."));

    std::cout << "finished" << std::endl;

}

