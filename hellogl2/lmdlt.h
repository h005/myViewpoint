#ifndef LMDLT_H
#define LMDLT_H

#include <glm/glm.hpp>

class LMDLT
{
public:
    LMDLT();
    ~LMDLT();

    static void calcProjectedPoint(double p[], double parameter[], double newP[], bool radialDistoration = 1);
    static glm::mat3 RodtoR(const glm::vec3 &w);
};

#endif // LMDLT_H
