#include "clustermanager.h"
#include <fstream>
#include <glm/gtx/transform.hpp>
#include <QDebug>
#include <QString>

using namespace std;

ClusterManager::ClusterManager(std::string path)
{
    qDebug() << QString::fromStdString(path) << endl;
    ifstream in(path);
    assert(in.is_open());

    int clusterNums, entityNums;
    in >> clusterNums >> entityNums;

    qDebug() << clusterNums << " " << entityNums << endl;

    _vps.resize(clusterNums);
    _vpCenter.resize(clusterNums);

    for (int i = 0; i < entityNums; i++) {
        string name;
        int clusterIndex;

        in >> name;

        qDebug() << QString::fromStdString(name) << endl;

        glm::vec3 eye, forward, up;
        in >> eye.x >> eye.y >> eye.z;
        in >> forward.x >> forward.y >> forward.z;
        in >> up.x >> up.y >> up.z;

        in >> clusterIndex;
        glm::vec3 center = eye + forward;
        _vps[clusterIndex - 1].push_back(glm::lookAt(eye, center, up));
    }

    std::cout << in.good() << std::endl;

    assert(in.good());

    for (int i = 0; i < clusterNums; i++) {
        glm::vec3 eye, forward, up;
        in >> eye.x >> eye.y >> eye.z;
        in >> forward.x >> forward.y >> forward.z;
        in >> up.x >> up.y >> up.z;

        glm::vec3 center = eye + forward;
        _vpCenter[i] = glm::lookAt(eye, center, up);
    }

    in.close();
    qDebug() << QString("hello") << endl;
}

ClusterManager::~ClusterManager()
{

}

ClusterManager::VP_ARRAY &ClusterManager::getCluster(int index)
{
    assert(index <= _vps.size());
    return _vps[index-1];
}

glm::mat4 ClusterManager::getCenter(int index)
{
    assert(index <= _vpCenter.size());
    return _vpCenter[index-1];
}

int ClusterManager::getClusterNums()
{
    return _vps.size();
}
