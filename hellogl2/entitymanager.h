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

    QString ptCloudPath() const;
    void setPtCloudPath(const QString &path);

    QString registrationFile() const;
    void setRegistrationFile(const QString &registrationFile);

private:
    static glm::mat4 giveMVMatrix(float input[5][3]);

    std::map<QString, Entity> container;
    QString m_basedir;
    QString m_modelPath;
    QString m_ptCloudPath;
    QString m_registrationFile;
};

#endif // ENTITYMANAGER_H
