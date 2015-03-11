#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <map>
#include <QString>
#include "entity.h"

class EntityManager
{
public:
    EntityManager(const QString &basedir);
    ~EntityManager();
    bool load();

private:
    static glm::mat4 giveMVMatrix(float input[5][3]);

    std::map<QString, Entity> container;
    QString m_basedir;
};

#endif // ENTITYMANAGER_H
