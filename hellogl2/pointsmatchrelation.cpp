#include "pointsmatchrelation.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>

PointsMatchRelation::PointsMatchRelation(const QString &filepath)
    : filepath(filepath)
{

}

PointsMatchRelation::~PointsMatchRelation()
{

}

bool PointsMatchRelation::loadFromFile()
{
    points2d.clear();
    points3d.clear();

    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList tokens = line.split(' ', QString::SkipEmptyParts);
        if (tokens.size() != 5)
            return false;

        glm::vec3 p3d = glm::vec3(tokens[2].toFloat(), tokens[3].toFloat(), tokens[4].toFloat());
        glm::vec2 p2d = glm::vec2(tokens[0].toFloat(), tokens[1].toFloat());
        points3d.push_back(p3d);
        points2d.push_back(p2d);
    }
    file.close();
    return true;
}

bool PointsMatchRelation::saveToFile()
{
    if (points2d.size() == points3d.size()) {
        QFile file(filepath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        for (uint32_t i = 0; i < points2d.size(); i++) {
            glm::vec2 p2d = points2d[i];
            glm::vec3 p3d = points3d[i];
            out << p2d.x << " " << p2d.y << " " << p3d.x << " " << p3d.y << " " << p3d.z << "\n";
        }
        file.close();

        return true;
    } else {
        return false;
    }
}

std::vector<glm::vec2>& PointsMatchRelation::getPoints2d()
{
    return points2d;
}

void PointsMatchRelation::setPoints2d(const std::vector<glm::vec2> &value)
{
    points2d = value;
}
std::vector<glm::vec3>& PointsMatchRelation::getPoints3d()
{
    return points3d;
}

void PointsMatchRelation::setPoints3d(const std::vector<glm::vec3> &value)
{
    points3d = value;
}


