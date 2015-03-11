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

EntityManager::EntityManager(const QString &basedir)
{
    m_basedir = basedir;
}

EntityManager::~EntityManager()
{
    
}

bool EntityManager::load()
{
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
        QString entityName = listIn.readLine();

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

        entity.f = input[0][0];
        entity.K1 = input[0][1];
        entity.K2 = input[0][2];
        entity.mvMatrix = giveMVMatrix(input);
        container[entityName] = entity;
    }
    listFile.close();
    bundleFile.close();
    return true;
}

glm::mat4 EntityManager::giveMVMatrix(float input[][3])
{
    // firstly, consider matrix as row major
    glm::mat4 matrix(1.f);
    // fectch rotation
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = input[i][j];
        }
    }
    // fetch translate
    matrix[0][3] = input[4][0];
    matrix[1][3] = input[4][1];
    matrix[2][3] = input[4][2];

    // secondly, return a column major matrix
    return glm::transpose(matrix);
}

