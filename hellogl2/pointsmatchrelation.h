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

    std::vector<glm::vec2>& getPoints2d();
    void setPoints2d(const std::vector<glm::vec2> &value);

    std::vector<glm::vec3>& getPoints3d();
    void setPoints3d(const std::vector<glm::vec3> &value);

private:
    QString filepath;
    QString modelPath;
    glm::mat4 RepresentationToModel;
    std::vector<glm::vec2> points2d;
    std::vector<glm::vec3> points3d;

    glm::mat4 getModelTranslateAndScaleTransform();
};

#endif // POINTSMATCHRELATION_H
