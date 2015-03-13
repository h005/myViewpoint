#include <iostream>
#include <gl/glew.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <time.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>

#include "custom.h"
#include "entity.h"

using namespace std;

void recoveryCameraParameters(
        Entity &base,
        Entity &want,
        const glm::mat4 &baseAlignedMVMatrix,
        const glm::mat4 &baseAlignedProjMatrix,
        glm::mat4 &wantMVMatrix,
        glm::mat4 &wantProjMatrix) {
    glm::mat3 R1 = glm::mat3(base.mvMatrix);
    glm::mat3 R2 = glm::mat3(want.mvMatrix);
    glm::mat3 Rd = glm::mat3(baseAlignedMVMatrix);
    glm::vec3 t1 = glm::vec3(base.mvMatrix[3]);
    glm::vec3 t2 = glm::vec3(want.mvMatrix[3]);
    glm::vec3 td = glm::vec3(baseAlignedMVMatrix[3]);

    glm::mat3 R = R2 * glm::inverse(R1) * Rd;
    glm::vec3 t = t2 + R2 * glm::inverse(R1) * (td - t1);
    wantMVMatrix = glm::mat4(R);
    wantMVMatrix[3] = glm::vec4(t, 1.f);
}

void recoveryLookAtWithModelView(
        const glm::mat4 &mvMatrix,
        glm::vec3 &eye,
        glm::vec3 &center,
        glm::vec3 &up) {
    glm::vec3 t = glm::vec3(mvMatrix[3]);
    glm::mat3 R = glm::mat3(mvMatrix);
    // OC是世界中心指向相机中心的向量（世界坐标系下），也是相机中心在世界坐标系下的表示
    // 从公式角度看，有:
    // | R t |   |OC|   |0|
    // |     | * |  | = | |
    // | 0 1 |   |1 |   |1|
    // 所以R * OC + t = 0，可以求出C在世界坐标系下的表示
    eye = -glm::inverse(R) * t;
    center = normalize(glm::inverse(R) * glm::vec3(0.f, 0.f, -1.f)) + eye;
    up = glm::normalize(glm::inverse(R) * glm::vec3(0.f, 1.f, 0.f));
}

glm::vec3 projection(
        const glm::vec3 &vec,
        const glm::vec3 &N) {
    glm::vec3 NN = glm::normalize(N);
    return vec - glm::dot(vec, NN) * NN;
}

void testCustom() {
    // The following code is only for test, and can be deleted
    glm::vec3 a_eye = glm::vec3(1.f, 2.f, 3.f);
    glm::vec3 a_center = glm::vec3(3.f, 3.f, 3.f);
    glm::vec3 a_up = glm::vec3(4.f, 5.f, 6.f);
    glm::mat4 mv = glm::lookAt(a_eye, a_center, a_up);
    glm::vec3 eye, center, up;
    recoveryLookAtWithModelView(mv, eye, center, up);
    std::cout << glm::to_string(eye - a_eye) <<std::endl;
    std::cout << glm::to_string(glm::normalize(center - eye) - glm::normalize(a_center - a_eye)) << std::endl;
    std::cout << glm::to_string(
        glm::normalize(projection(a_up, a_center - a_eye))
        - glm::normalize(projection(up, center - eye)))
              << std::endl;
}
