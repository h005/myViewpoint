#pragma once
#include <glm/glm.hpp>
#include "entity.h"

///
/// \brief 用基准图片标定第三张图片
/// \param c 坐标系之间的缩放比率
/// \param base 第一张图片参数
/// \param want 想要标定图片的参数
/// \param baseAlignedMVMatrix 第一张图片的mvMatrix
/// \param wantMVMatrix 输出欲标定图片的mvMatrix
///
void recoveryCameraParameters(float c,
        Entity &base,
        Entity &want,
        const glm::mat4 &baseAlignedMVMatrix,
        glm::mat4 &wantMVMatrix);

// 使用两个DLT配准的数据，算出模型之间的相互关系
float recoveryScale(
        Entity &base,
        Entity &want,
        const glm::mat4 &baseAlignedMVMatrix,
        const glm::mat4 &wantAlignedMVMatrix);

// 从viewMatrix中恢复出等价的lookAt参数
void recoveryLookAtWithModelView(
        const glm::mat4 &viewMatrix,
        glm::vec3 &eye,
        glm::vec3 &center,
        glm::vec3 &up);

glm::mat4 projectionMatrixWithFocalLength(
        float f,
        float width,
        float height,
        float near,
        float far
        );

glm::vec3 projection(
        const glm::vec3 &vec,
        const glm::vec3 &N);
void testCustom();
