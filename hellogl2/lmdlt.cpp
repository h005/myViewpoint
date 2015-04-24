#include "lmdlt.h"
#include "math.h"
#include <glm/gtx/matrix_cross_product.hpp>

LMDLT::LMDLT()
{

}

LMDLT::~LMDLT()
{

}

// 根据相机参数计算投影后的点
void LMDLT::calcProjectedPoint(double p[], double parameter[], double newP[], bool radialDistoration)
{
    // 内参矩阵K
    glm::vec3 point(p[0], p[1], p[2]);
    double alpha = parameter[0], beta = parameter[1];
    double gamma = parameter[2];
    double u0 = parameter[3], v0 = parameter[4];

    //Radial distoration
    double k1 = parameter[5], k2 = parameter[6];

    // 外参矩阵
    // 绕轴旋转
    glm::mat3 R = LMDLT::RodtoR(glm::vec3(parameter[7], parameter[8], parameter[9]));
    glm::vec3 t = glm::vec3(parameter[10], paramater[11], parameter[12]);

    glm::vec3 final = R * point + t;
    double x = final[0] / final[2];
    double y = final[1] / final[2];

    double u = u0 + aplha*x + gamma*y;
    double v = v0 + beta*y;
    if (radialDistoration) {
        double value = pow(x, 2.0) + pow(y, 2.0);
        newP[0] = u + (u - u0) * (k1 * value + k2 * pow(value, 2.0));
        newP[1] = v + (v - v0) * (k1 * value + k2 * pow(value, 2.0));
    } else {
        newP[0] = u;
        newP[1] = v;
    }
}

glm::mat3 LMDLT::RodtoR(const glm::vec3 &w)
{
    // 将向量表示的 绕轴旋转 转化成 3x3的旋转矩阵
    // ref http://en.wikipedia.org/wiki/Rodrigues'_rotation_formula#Matrix_notation
    float wLength = glm::length(w);
    glm::mat3 K = matrixCross3(w) / wLength;
    glm::mat3 R = glm::mat3(1.f) + sin(wLength) * K + (1-cos(wLength)) * K * K;
    return R;
}

