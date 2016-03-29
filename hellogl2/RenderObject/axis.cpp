#include "axis.h"

#include <vector>
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

Axis::Axis()
{
}

Axis::~Axis()
{

}

void Axis::bindDataToGL(GLuint args[], void *others)
{
    m_isInited = true;
}

void Axis::draw()
{
    if (!m_isInited) {
        std::cout << "please call init() before draw()" << std::endl;
    }

    glColor3ub(250, 250, 100);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, -0.25, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(0.75, 0.0, 0.25);
    glVertex3f(0.75, 0.0, -0.25);
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.75, 0.25);
    glVertex3f(0.0, 0.75, -0.25);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
    glVertex3f(-0.25, 0.75, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, -1.0);
    glVertex3f(0.25, 0.0, -0.75);
    glVertex3f(-0.25, 0.0, -0.75);
    glVertex3f(0.0, 0.0, -1.0);
    glVertex3f(0.0, 0.25, -0.75);
    glVertex3f(0.0, -0.25, -0.75);
    glVertex3f(0.0, 0.0, -1.0);
    glEnd();
}

