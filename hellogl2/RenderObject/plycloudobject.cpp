#include "plycloudobject.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

PLYCloudObject::PLYCloudObject(const std::string &path): BaseRenderObject()
{
    m_path = path;
}

bool PLYCloudObject::load()
{
    FILE *fp = fopen(m_path.c_str(), "r");
    if (fp == NULL)
        std::cout << "Error reading " << m_path << std::endl;

    char buf[2048];
    for (int i = 0; i < 13; i++)
        fgets(buf, sizeof(buf), fp);


    CloudPointInfo p;
    int index = 0;
    while (fscanf(fp, "%f %f %f %f %f %f %d %d %d", &p.x, &p.y,&p.z, &p.nx, &p.ny, &p.nz, &p.r, &p.g, &p.b) != EOF) {
        m_vertices.push_back(p.x);
        m_vertices.push_back(p.y);
        m_vertices.push_back(p.z);

        m_normals.push_back(p.nx);
        m_normals.push_back(p.ny);
        m_normals.push_back(p.nz);

        m_indices.push_back(index);
        index++;

        if (index % 10000 == 0)
            std::cout << "processd: " << index << std::endl;
    }

    fclose(fp);
    std::cout << "load finished" << std::endl;
    return true;
}

void PLYCloudObject::bindDataToGL(GLuint args[], void *others)
{
    GLuint vertexPositionID = args[0];
    GLuint vertexNormalID = args[1];

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), &m_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vertexPositionID, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray (vertexPositionID);

    glGenBuffers(1, &m_vboVertexNormal);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertexNormal);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(GLfloat), &m_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vertexNormalID, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray (vertexNormalID);

    glGenBuffers(1, &m_vboIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

    m_isInited = true;
}

void PLYCloudObject::draw()
{
    assert(m_isInited);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
    glDrawElements(GL_POINTS, m_indices.size(), GL_UNSIGNED_INT, 0);
}

glm::mat4 PLYCloudObject::recommendScaleAndShift()
{
    glm::vec3 scene_min = glm::vec3(1e10, 1e10, 1e10);
    glm::vec3 scene_max = glm::vec3(-1e10, -1e10, -1e10);
    for (int i = 0; i < m_vertices.size(); i += 3) {
        if (m_vertices[i] < scene_min.x)
            scene_min.x = m_vertices[i];
        if (m_vertices[i] > scene_max.x)
            scene_max.x = m_vertices[i];

        if (m_vertices[i+1] < scene_min.y)
            scene_min.y = m_vertices[i+1];
        if (m_vertices[i+1] > scene_max.y)
            scene_max.y = m_vertices[i+1];

        if (m_vertices[i+2] < scene_min.z)
            scene_min.z = m_vertices[i+2];
        if (m_vertices[i+2] > scene_max.z)
            scene_max.z = m_vertices[i+2];
    }
    float tmp = -1e10;
    tmp = std::max<float>(scene_max.x - scene_min.x, tmp);
    tmp = std::max<float>(scene_max.y - scene_min.y, tmp);
    tmp = std::max<float>(scene_max.z - scene_min.z, tmp);
    float scale = 2.f / tmp;

    glm::vec3 scene_center;
    scene_center.x = (scene_min.x + scene_max.x) / 2.f;
    scene_center.y = (scene_min.y + scene_max.y) / 2.f;
    scene_center.z = (scene_min.z + scene_max.z) / 2.f;

    // 缩放矩阵 * 移中矩阵，表示先移中后缩放
    glm::mat4 scaleAndShift = glm::scale(glm::mat4(1.f), glm::vec3(scale, scale, scale));
    scaleAndShift = glm::translate(scaleAndShift, glm::vec3(-scene_center.x, -scene_center.y, -scene_center.z));
    return scaleAndShift;
}

PLYCloudObject::~PLYCloudObject()
{
    // m_vao, m_vbo的清理在基类中会自动进行
}

