#ifndef ENTITY_H
#define ENTITY_H

#include <QString>
#include <glm/glm.hpp>

class Entity
{
public:
    Entity();
    ~Entity();

    QString name;
    QString path;
    glm::mat4 mvMatrix;
    float f, K1, K2;
};

#endif // ENTITY_H
