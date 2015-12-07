#ifndef AXIS_H
#define AXIS_H

#include "baserenderobject.h"

class Axis: public BaseRenderObject
{
public:
    Axis();
    ~Axis();
    void bindDataToGL(GLuint args[], void *others=0) override;
    void draw() override;
};

#endif // AXIS_H
