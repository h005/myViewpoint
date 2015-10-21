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
    // 开启normal支持(相当于分配存储空间)
    mesh.request_vertex_normals();
    // 验证是否开启成功/存在normal的存储空间
    if (!mesh.has_vertex_normals())
    {
        std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
        return;
    }

    // 需要设置参数，这样才会从文件中读取vertex normals
    OpenMesh::IO::Options inopt;
    inopt += OpenMesh::IO::Options::VertexNormal;
    if ( ! OpenMesh::IO::read_mesh(mesh, "../models/1.off", inopt) )
    {
        std::cerr << "Error: Cannot read mesh from " << std::endl;
        return;
    }

    if ( !inopt.check( OpenMesh::IO::Options::VertexNormal ) ) {
        // If the file did not provide vertex normals,
        // we will calculate vertex normals later
        mesh.release_vertex_normals();
    }
#else
    if (!ExternalImporter<MyMesh>::read_mesh(mesh, "dragon.off") )
    {
        std::cerr << "Error: Cannot read mesh from " << std::endl;
        return;
    }
#endif

    // If the file did not provide vertex normals, then calculate them
    if (!mesh.has_vertex_normals())
    {
        std::cout << "we need calculate vertex normal first" << std::endl;
        // allocate memory for normals storage
        // we need face normals to update the vertex normals
        mesh.request_face_normals();
        mesh.request_vertex_normals();

        // let the mesh update the normals
        mesh.update_normals();
        // dispose the face normals, as we don't need them anymore
        mesh.release_face_normals();
    }

    // 创建两个对象，分别用于提取高斯曲率和平均曲率
    GaussCurvature<MyMesh> a(mesh);
    MeanCurvature<MyMesh> b(mesh);

    // 将每个点的平均曲率以颜色的形式附加到mesh对象上，并进行输出
    // 之后就可以使用meshlab软件打开模型文件，直观地查看每个点的平均曲率
    // 暂时没什么用，可以删除
    b.assignVertexColor();
    OpenMesh::IO::Options opt(OpenMesh::IO::Options::VertexColor);
    OpenMesh::IO::write_mesh(mesh, "temp.off", opt);

    // TODO:
    //
    // 虽然每个顶点的平均曲率已经有了，但是对于一个视点，它的平均曲率是什么呢？
    // 其实就是以当前视点下所有的可见顶点作为集合，将这些顶点的平均曲率相加，最后再除以投影面积
    // 这里就涉及两个问题：
    // 1.收集到所有可见顶点
    // 2.获得该视点下的投影面积
    // 其中第2点很好实现，用当前视点渲染一下，获取它的深度缓冲（是一个二维数组），统计“深度值有效”的像素数目，即可作为投影面积

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

