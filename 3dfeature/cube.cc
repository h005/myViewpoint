#include <iostream>


#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"


#include "common.hh"
#include "gausscurvature.hh"
#include "meancurvature.hh"
#include "externalimporter.hh"

int main()
{
    MyMesh mesh;
    // read mesh from stdin
//    if ( ! OpenMesh::IO::read_mesh(mesh, "94.off") )
//    {
//        std::cerr << "Error: Cannot read mesh from " << std::endl;
//        return 1;
//    }
    if (!ExternalImporter<MyMesh>::read_mesh(mesh, "94.off") )
    {
        std::cerr << "Error: Cannot read mesh from " << std::endl;
        return 1;
    }

    GaussCurvature<MyMesh> a(mesh);
    MeanCurvature<MyMesh> b(mesh);
    std::cout << "finished" << std::endl;

    a.assignVertexColor();
    OpenMesh::IO::Options opt(OpenMesh::IO::Options::VertexColor);
    OpenMesh::IO::write_mesh(mesh, "temp.off", opt);
    return 0;
}
