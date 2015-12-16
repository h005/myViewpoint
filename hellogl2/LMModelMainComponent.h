#pragma once

#include <vector>
#include <utility>
#include <glm/glm.hpp>

class LMModelMainComponent
{
private:
    static glm::vec3 PCAWithModelPoints(int num, glm::vec3 p[]);
    static std::pair<glm::vec3, glm::vec3> PCAWithModelPoints2(int num, glm::vec3 p[], glm::mat3 rotation);

public:
    static std::vector<glm::vec3> getModelMainComponent(int pnum, glm::vec3 p[]);
};
