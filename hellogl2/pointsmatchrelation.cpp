#include "pointsmatchrelation.h"

#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GModel.h"

PointsMatchRelation::PointsMatchRelation(QString filepath)
    :filepath(filepath)
{
    ptCloudPoints.clear();
    modelPoints.clear();
}

PointsMatchRelation::~PointsMatchRelation()
{

}

bool PointsMatchRelation::loadFromFile()
{
    ptCloudPoints.clear();
    modelPoints.clear();

    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList tokens = line.split(' ', QString::SkipEmptyParts);
        if (tokens.size() != 6)
            return false;

        glm::vec3 modelPoint = glm::vec3(tokens[3].toFloat(), tokens[4].toFloat(), tokens[5].toFloat());
        glm::vec3 ptCloudPoint = glm::vec3(tokens[0].toFloat(), tokens[1].toFloat(), tokens[2].toFloat());
        modelPoints.push_back(modelPoint);
        ptCloudPoints.push_back(ptCloudPoint);
    }
    file.close();
    return true;
}

bool PointsMatchRelation::saveToFile()
{
    std::cout << filepath.toStdString() << std::endl;
    if (ptCloudPoints.size() == modelPoints.size()) {
        QFile file(filepath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        for (uint32_t i = 0; i < ptCloudPoints.size(); i++) {
            glm::vec3 ptCloudPoint = ptCloudPoints[i];
            glm::vec3 modelPoint = modelPoints[i];
            out << ptCloudPoint.x << " " << ptCloudPoint.y << " " << ptCloudPoint.z << " " << modelPoint.x << " " << modelPoint.y << " " << modelPoint.z << "\n";
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
    return ptCloudPoints.size() == modelPoints.size();
}

bool PointsMatchRelation::clearPoints()
{
    imgPoints.clear();
    ptCloudPoints.clear();
    modelPoints.clear();
}

bool PointsMatchRelation::ccaLoadFromFile()
{
    imgPoints.clear();
    modelPoints.clear();

    QFile file(filepath);
    if( !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList tokens = line.split(' ', QString::SkipEmptyParts);
        if (tokens.size() != 5)
            return false;

        glm::vec2 imgPoint = glm::vec2(tokens[0].toFloat(), tokens[1].toFloat());
//        glm::vec4 modelPoint = cc_st * glm::vec4(tokens[2].toFloat(), tokens[3].toFloat(), tokens[4].toFloat(), 1.f);
        glm::vec4 modelPoint = glm::vec4(tokens[2].toFloat(), tokens[3].toFloat(), tokens[4].toFloat(), 1.f);
        modelPoints.push_back(glm::vec3(modelPoint));
        imgPoints.push_back(imgPoint);
    }
    file.close();
    return true;
}

std::vector<glm::vec3>& PointsMatchRelation::getPtCloudPoints()
{
    return ptCloudPoints;
}

void PointsMatchRelation::setPtCloudPoints(const std::vector<glm::vec3> &value)
{
    ptCloudPoints = value;
}
std::vector<glm::vec3>& PointsMatchRelation::getModelPoints()
{
    return modelPoints;
}

void PointsMatchRelation::setModelPoints(const std::vector<glm::vec3> &value)
{
    modelPoints = value;
}

std::vector<glm::vec2> &PointsMatchRelation::getImgPoints()
{
    return imgPoints;
}


