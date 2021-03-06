﻿#ifndef CLUSTERMANAGER_H
#define CLUSTERMANAGER_H

#include <glm/glm.hpp>
#include <vector>

using std::vector;

class ClusterManager
{
    typedef vector<glm::mat4> VP_ARRAY;

public:
    ClusterManager(std::string path);
    ~ClusterManager();

    int         getClusterNums();
    VP_ARRAY    &getCluster(int index);
    glm::mat4   getCenter(int index);


private:
    vector<VP_ARRAY>  _vps;
    VP_ARRAY          _vpCenter;
};

#endif // CLUSTERMANAGER_H
