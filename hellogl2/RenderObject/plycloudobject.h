#pragma once

#include "baserenderobject.h"
#include <string>
#include <vector>

class PLYCloudObject: public BaseRenderObject
{
    struct CloudPointInfo {
        float x, y, z;
        float nx, ny, nz;
        int r, g, b;
    };

public:
    PLYCloudObject(const std::string &path);
    bool load() override;
    void bindDataToGL(GLuint args[], void *others=0) override;
    void draw() override;
    ~PLYCloudObject();

private:
    std::string m_path;
    std::vector<GLfloat> m_vertices;
    std::vector<GLfloat> m_normals;
    std::vector<GLuint> m_indices;
};
