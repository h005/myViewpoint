#include "pointsmatchrelation.h"

#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <glm/gtx/string_cast.hpp>
#include "GModel.h"

PointsMatchRelation::PointsMatchRelation(QString filepath, QString modelPath)
    : filepath(filepath),
      modelPath(modelPath)
{
    points2d.clear();
    points3d.clear();
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

        glm::vec4 p3d = glm::vec4(tokens[2].toFloat(), tokens[3].toFloat(), tokens[4].toFloat(), 1.f);
        glm::vec2 p2d = glm::vec2(tokens[0].toFloat(), tokens[1].toFloat());
        points3d.push_back(glm::vec3(p3d));
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

            // 由于模型显示前要进行移中和缩放，所以得到的3d点和模型的尺度有关
            // 将3d点当前的坐标变换到模型坐标系下，获得尺度无关的坐标
            glm::vec3 tmp = points3d[i];
            glm::vec4 p3d = glm::vec4(tmp, 1.f);
            out << p2d.x << " " << p2d.y << " " << p3d.x << " " << p3d.y << " " << p3d.z << "\n";
        }
        file.close();

        return true;
    } else {
        return false;
    }
}

/**
 * @brief PointsMatchRelation::isPointsEqual
 * @return 其中保存的三维点和二维点数目是否一致
 */
bool PointsMatchRelation::isPointsEqual()
{
    return points2d.size() == points3d.size();
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


