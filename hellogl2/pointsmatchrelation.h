#ifndef POINTSMATCHRELATION_H
#define POINTSMATCHRELATION_H

#include <glm/glm.hpp>
#include <vector>
#include <QString>

class PointsMatchRelation
{
public:
    PointsMatchRelation(QString filepath, QString modelPath);
    ~PointsMatchRelation();

    bool loadFromFile();
    bool saveToFile();
    bool isPointsEqual();

    std::vector<glm::vec3>& getPtCloudPoints();
    void setPtCloudPoints(const std::vector<glm::vec3> &value);

    std::vector<glm::vec3>& getModelPoints();
    void setModelPoints(const std::vector<glm::vec3> &value);

private:
    QString filepath;
    QString modelPath;
    std::vector<glm::vec3> ptCloudPoints;
    std::vector<glm::vec3> modelPoints;
};

#endif // POINTSMATCHRELATION_H
