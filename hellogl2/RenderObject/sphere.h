#ifndef SPHERE_H
#define SPHERE_H

#include "baserenderobject.h"

class Sphere: public BaseRenderObject
{
public:
    Sphere();
    ~Sphere();
    void bindDataToGL(GLuint args[], void *others=0) override;
    void draw() override;

private:
    int lats, longs;
    GLuint m_vao, m_vboVertex, m_vboIndex;
    int numsToDraw;
};

#endif // SPHERE_H
