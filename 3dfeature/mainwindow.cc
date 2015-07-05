#include "mainwindow.hh"
#include <QVBoxLayout>
#include <QPushButton>
#include "glwidget.hh"
#include "externalimporter.hh"
#include "gausscurvature.hh"
#include "meancurvature.hh"

/**
 * @brief MainWindow::startMyProcess 主要的代码写在这里
 */
void MainWindow::startMyProcess()
{
    MyMesh mesh;
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

    GLWidget *window = new GLWidget(mesh, 0);
    window->show();
}


// 下面的代码不用看
MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    dockBtn = new QPushButton(tr("Start"), this);
    connect(dockBtn, SIGNAL(clicked()), this, SLOT(startMyProcess()));

    // 最中间的一列按钮
    QVBoxLayout *middleLayout = new QVBoxLayout;
    middleLayout->addWidget(dockBtn);
    setLayout(middleLayout);
}

MainWindow::~MainWindow()
{
}

QSize MainWindow::sizeHint() const
{
    return QSize(100, 100);
}

