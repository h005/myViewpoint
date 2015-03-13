#pragma once
#include <glm/glm.hpp>
#include "entity.h"

void recoveryCameraParameters(
        Entity &base,
        Entity &want,
        const glm::mat4 &baseAlignedMVMatrix,
        const glm::mat4 &baseAlignedProjMatrix,
        glm::mat4 &wantMVMatrix,
        glm::mat4 &wantProjMatrix);
void recoveryLookAtWithModelView(
        const glm::mat4 &mvMatrix,
        glm::vec3 &eye,
        glm::vec3 &center,
        glm::vec3 &up);
glm::vec3 projection(
        const glm::vec3 &vec,
        const glm::vec3 &N);
void testCustom();
