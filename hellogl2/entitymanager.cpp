#include "entitymanager.h"

#include <iostream>
#include <QtGlobal>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStringList>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <QSettings>

EntityManager::EntityManager(const QString &basedir)
{
    m_basedir = basedir;
}

EntityManager::~EntityManager()
{
    
}

bool EntityManager::load()
{
    QSettings settings(QDir(m_basedir).filePath("config.ini"), QSettings::IniFormat);
    // 读取模型的信息
    m_modelPath = QDir(m_basedir).filePath(settings.value("model/path").toString());
    // 读取两幅配准图像的信息
    m_baseOneID = settings.value("align/base_one_id").toString();
    m_baseOneImagePath = QDir(m_basedir).filePath(m_baseOneID);
    m_baseOneImageRelation = QDir(m_basedir).filePath(settings.value("align/base_one_point").toString());
    m_baseTwoID = settings.value("align/base_two_id").toString();
    m_baseTwoImagePath = QDir(m_basedir).filePath(m_baseTwoID);
    m_baseTwoImageRelation = QDir(m_basedir).filePath(settings.value("align/base_two_point").toString());

    container.clear();

    QFile listFile(QDir(m_basedir).filePath("list.txt"));
    if (!listFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QFile bundleFile(QDir(m_basedir).filePath("bundle.out"));
    if (!bundleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream listIn(&listFile);
    QTextStream bundleIn(&bundleFile);
    // ignore bundle.out headers
    Q_ASSERT(bundleIn.readLine().startsWith("# Bundle file"));
    bundleIn.readLine();

    while (!listIn.atEnd()) {
        QString entityName = listIn.readLine().split(' ', QString::SkipEmptyParts)[0];

        Entity entity;
        entity.name = entityName;
        entity.path = entityName;

        float input[5][3];
        for (int i = 0; i < 5; i++) {
            QString line = bundleIn.readLine();
            QStringList tokens = line.split(' ', QString::SkipEmptyParts);
            Q_ASSERT(tokens.size() == 3);

            for (int j = 0; j < 3; j++) {
                input[i][j] = tokens[j].toFloat();
            }
        }

        // TODO:
        // read image's width and height
        entity.f = input[0][0];
        entity.K1 = input[0][1];
        entity.K2 = input[0][2];
        entity.mvMatrix = giveMVMatrix(input);
        // some image don't have estimated camera parameters
        if (entity.f > 1e-5) {
            container[entityName] = entity;
        }
    }
    listFile.close();
    bundleFile.close();
    return true;
}

bool EntityManager::getEntity(const QString &key, Entity &out)
{
    std::map<QString, Entity>::iterator it = container.find(key);
    if (it != container.end()) {
        out = it->second;
        return true;
    } else {
        return false;
    }
}

bool EntityManager::getImageList(std::vector<QString> &out)
{
    out.clear();
    std::map<QString, Entity>::iterator it;
    for (it = container.begin(); it != container.end(); it++)
        out.push_back(it->first);
    return true;
}

glm::mat4 EntityManager::giveMVMatrix(float input[][3])
{
    glm::mat4 matrix(1.f);
    // fetch rotation
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[j][i] = input[i + 1][j];
        }
    }
    // fetch translate
    matrix[3][0] = input[4][0];
    matrix[3][1] = input[4][1];
    matrix[3][2] = input[4][2];

    return matrix;
}
QString EntityManager::baseTwoID() const
{
    return m_baseTwoID;
}

void EntityManager::setBaseTwoID(const QString &baseTwoID)
{
    m_baseTwoID = baseTwoID;
}

QString EntityManager::baseOneID() const
{
    return m_baseOneID;
}

void EntityManager::setBaseOneID(const QString &baseOneID)
{
    m_baseOneID = baseOneID;
}

QString EntityManager::baseTwoImageRelation() const
{
    return m_baseTwoImageRelation;
}

void EntityManager::setBaseTwoImageRelation(const QString &baseTwoImageRelation)
{
    m_baseTwoImageRelation = baseTwoImageRelation;
}

QString EntityManager::baseOneImageRelation() const
{
    return m_baseOneImageRelation;
}

void EntityManager::setBaseOneImageRelation(const QString &baseOneImageRelation)
{
    m_baseOneImageRelation = baseOneImageRelation;
}

QString EntityManager::baseTwoImagePath() const
{
    return m_baseTwoImagePath;
}

void EntityManager::setBaseTwoImagePath(const QString &baseTwoImagePath)
{
    m_baseTwoImagePath = baseTwoImagePath;
}

QString EntityManager::baseOneImagePath() const
{
    return m_baseOneImagePath;
}

void EntityManager::setBaseOneImagePath(const QString &baseOneImagePath)
{
    m_baseOneImagePath = baseOneImagePath;
}

QString EntityManager::modelPath() const
{
    return m_modelPath;
}

void EntityManager::setModelPath(const QString &modelPath)
{
    m_modelPath = modelPath;
}


