#ifndef POINTSMATCHRELATION_H
#define POINTSMATCHRELATION_H

#include <glm/glm.hpp>
#include <vector>
#include <QString>

class PointsMatchRelation
{
public:
    PointsMatchRelation(QString filepath);
    ~PointsMatchRelation();

    bool loadFromFile();
    bool saveToFile();
    bool isPointsEqual();
    bool clearPoints();

    bool ccaLoadFromFile();

    std::vector<glm::vec3>& getPtCloudPoints();
    void setPtCloudPoints(const std::vector<glm::vec3> &value);

    std::vector<glm::vec3>& getModelPoints();
    void setModelPoints(const std::vector<glm::vec3> &value);

    std::vector<glm::vec2>& getImgPoints();

private:
    QString filepath;
    std::vector<glm::vec2> imgPoints;
    std::vector<glm::vec3> ptCloudPoints;
    std::vector<glm::vec3> modelPoints;
};

#endif // POINTSMATCHRELATION_H
