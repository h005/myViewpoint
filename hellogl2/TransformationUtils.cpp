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

#include "TransformationUtils.h"
#include "entity.h"

using namespace std;

void recoveryCameraParameters(
        float c,
        Entity &base,
        Entity &want,
        const glm::mat4 &baseAlignedMVMatrix,
        glm::mat4 &wantMVMatrix) {
    glm::mat3 R1 = glm::mat3(base.mvMatrix);
    glm::mat3 R2 = glm::mat3(want.mvMatrix);
    glm::mat3 Rd = glm::mat3(baseAlignedMVMatrix);
    glm::vec3 t1 = glm::vec3(base.mvMatrix[3]);
    glm::vec3 t2 = glm::vec3(want.mvMatrix[3]);
    glm::vec3 td = glm::vec3(baseAlignedMVMatrix[3]);

    // DLT得到的相机坐标系和SfM得到的相机坐标系不是同一个系
    // 假设它们的坐标之间存在一个缩放系数c
    //
    // 可以看到，缩放系数并不影响旋转矩阵，所以也不影响相机的朝向
    // 相机的中心受c影响较大，尝试通过两个DLT来找到一个较好的c
    glm::mat3 R = R2 * glm::inverse(R1) * Rd;
    glm::vec3 t = (1/c) * t2 + R2 * glm::inverse(R1) * (td - t1 * (1/c));
    wantMVMatrix = glm::mat4(R);
    wantMVMatrix[3] = glm::vec4(t, 1.f);
}

static float scale(const glm::vec3 &A, const glm::vec3 &B) {
    // 最小化目标
    // 由于向量自身的点积大于等于0
    // 所以要找到一个c，使得((1/c) * (1/c) * A'A + (2/c) * A'B + B'B)尽量小
    float a = glm::dot(A, A);
    float b = 2 * glm::dot(A, B);
    float c = glm::dot(B, B);
    // t = -b/2a, c = 1/t
    return -2 * a / b;
}

float recoveryScale(
        Entity &base,
        Entity &want,
        const glm::mat4 &baseAlignedMVMatrix,
        const glm::mat4 &wantAlignedMVMatrix
        ) {
    glm::mat3 R1 = glm::mat3(base.mvMatrix);
    glm::mat3 R2 = glm::mat3(want.mvMatrix);
    glm::mat3 Rd = glm::mat3(baseAlignedMVMatrix);
    glm::vec3 t1 = glm::vec3(base.mvMatrix[3]);
    glm::vec3 t2 = glm::vec3(want.mvMatrix[3]);
    glm::vec3 td = glm::vec3(baseAlignedMVMatrix[3]);

    glm::vec3 tWanted = glm::vec3(wantAlignedMVMatrix[3]);

    glm::vec3 A = t2 - R2 * glm::inverse(R1) * t1;
    glm::vec3 B = R2 * glm::inverse(R1) * td - tWanted;

    return scale(A, B);
}

void recoveryLookAtWithModelView(
        const glm::mat4 &viewMatrix,
        glm::vec3 &eye,
        glm::vec3 &center,
        glm::vec3 &up) {
    glm::vec3 t = glm::vec3(viewMatrix[3]);
    glm::mat3 R = glm::mat3(viewMatrix);
    // OC是世界中心指向相机中心的向量（世界坐标系下），也是相机中心在世界坐标系下的表示
    // 从公式角度看，有:
    // | R t |   |OC|   |0|
    // |     | * |  | = | |
    // | 0 1 |   |1 |   |1|
    // 所以R * OC + t = 0，可以求出C在世界坐标系下的表示
    //
    // 另外，由于旋转矩阵R是正交矩阵，所以R^(-1) = R^T
    // 对正交矩阵用转置代替求逆能节省cpu资源
    eye = -glm::transpose(R) * t;
    center = normalize(glm::transpose(R) * glm::vec3(0.f, 0.f, -1.f)) + eye;
    up = glm::normalize(glm::transpose(R) * glm::vec3(0.f, 1.f, 0.f));
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

    glm::mat4 mm(1.f);
    glm::vec3 m1,m2,m3;
    recoveryLookAtWithModelView(mm, m1, m2, m3);
    std::cout << glm::to_string(m1) << std::endl;
    std::cout << glm::to_string(m2) << std::endl;
    std::cout << glm::to_string(m3) << std::endl;
}
