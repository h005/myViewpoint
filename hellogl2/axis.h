#ifndef AXIS_H
#define AXIS_H

#include <GL/glew.h>

class Axis
{
public:
    Axis();
    ~Axis();
    void init(GLuint vertexPositionID);
    void cleanup();
    void draw();

private:
    bool isInited;
};

#endif // AXIS_H
