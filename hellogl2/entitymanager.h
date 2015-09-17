#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <map>
#include <QString>
#include <vector>
#include "entity.h"

class EntityManager
{
public:
    EntityManager(const QString &basedir);
    ~EntityManager();
    bool load();
    bool getEntity(const QString &key, Entity &out);
    bool getImageList(std::vector<QString> &out);

    QString modelPath() const;
    void setModelPath(const QString &modelPath);

    QString baseOneImagePath() const;
    void setBaseOneImagePath(const QString &baseOneImagePath);

    QString baseTwoImagePath() const;
    void setBaseTwoImagePath(const QString &baseTwoImagePath);

    QString baseOneImageRelation() const;
    void setBaseOneImageRelation(const QString &baseOneImageRelation);

    QString baseTwoImageRelation() const;
    void setBaseTwoImageRelation(const QString &baseTwoImageRelation);

    QString baseOneID() const;
    void setBaseOneID(const QString &baseOneID);

    QString baseTwoID() const;
    void setBaseTwoID(const QString &baseTwoID);

private:
    static glm::mat4 giveMVMatrix(float input[5][3]);

    std::map<QString, Entity> container;
    QString m_basedir;
    QString m_modelPath;
    QString m_baseOneImagePath;
    QString m_baseTwoImagePath;
    QString m_baseOneImageRelation;
    QString m_baseTwoImageRelation;
    QString m_baseOneID;
    QString m_baseTwoID;
};

#endif // ENTITYMANAGER_H
