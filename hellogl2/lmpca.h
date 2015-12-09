#ifndef LMPCA_H
#define LMPCA_H

#include <glm/glm.hpp>

class LMPCA
{
public:
    static glm::vec3 PCAWithModelPoints(int num, glm::vec3 p[]);
};

#endif // LMPCA_H
