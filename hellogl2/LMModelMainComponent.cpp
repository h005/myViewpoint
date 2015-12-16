#include "LMModelMainComponent.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <levmar.h>
#include <opencv2/opencv.hpp>
#include <glm/gtx/matrix_cross_product.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

static glm::vec3 ballAxisToVector(double alpha, double beta)
{
    return glm::vec3(sin(alpha) * cos(beta), sin(alpha) * sin(beta), cos(alpha));
}

static void calcProjection(double parameter[], double newP[], int LM_m, int LM_n, void *adata)
{
    double alpha = parameter[0], beta = parameter[1];
    glm::vec3 w = ballAxisToVector(alpha, beta);
//    std::cout << glm::to_string(w) << std::endl;
    glm::vec3 *p = (glm::vec3 *)adata;
    int pnum = LM_n;
    for (int i = 0; i < pnum; i++)
        newP[i] = glm::dot(w, p[i]);
}

// not thread-safe !!!!
static glm::mat3 rot;
static void calcProjection2(double parameter[], double newP[], int LM_m, int LM_n, void *adata)
{
    float alpha = parameter[0];
    glm::vec3 w = rot * glm::vec3(1 * glm::cos(alpha), 1 * glm::sin(alpha), 0);
//    std::cout << glm::to_string(w) << std::endl;
    glm::vec3 *p = (glm::vec3 *)adata;
    int pnum = LM_n;
    for (int i = 0; i < pnum; i++)
        newP[i] = glm::dot(w, p[i]);
}


glm::vec3 LMModelMainComponent::PCAWithModelPoints(int num, glm::vec3 p[])
{
    // 求点阵的中心
    glm::vec3 center = glm::vec3(0.f);
    for (int i = 0; i < num; i++)
        center += p[i];
    center /= num;

    void (*minfn)(double *p, double *hx, int m, int n, void *adata);
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    opts[0] = LM_INIT_MU;
    opts[1] = 1E-12;
    opts[2] = 1E-12;
    opts[3] = 1E-15;
    opts[4] = LM_DIFF_DELTA;

    // 球坐标系下，一共两个可优化参数
    int LM_m = 2;
    int LM_n = num;
    std::vector<double> para(LM_m);
    std::vector<double> ptx(num);
    std::vector<glm::vec3> inputPoints3d(num);

    for (int i = 0; i < num; i++) {
        // 方程左边数据 * para = 方程右边
        inputPoints3d[i] = p[i] - center;

        // 方程的右边
        ptx[i] = 0;
    }

    // !!! 注意初始值要设置得有意义，起码计算结果中不能出现IND
    para[0] = glm::pi<float>() / 4;
    para[1] = glm::pi<float>() / 4;

    minfn = calcProjection;
    int iter = dlevmar_dif(minfn, &para[0], &ptx[0], LM_m, LM_n, 1000,
                           opts, info, NULL, NULL, &inputPoints3d[0]);
    printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n", info[5], info[6], info[1], info[0]);
    std::cout << "Itered: " << iter << std::endl;
    return ballAxisToVector(para[0], para[1]);
}

std::pair<glm::vec3, glm::vec3> LMModelMainComponent::PCAWithModelPoints2(int num, glm::vec3 p[], glm::mat3 rotation)
{
    glm::vec3 center = glm::vec3(0.f);
    for (int i = 0; i < num; i++)
        center += p[i];
    center /= num;

    void (*minfn)(double *p, double *hx, int m, int n, void *adata);
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    opts[0] = LM_INIT_MU;
    opts[1] = 1E-12;
    opts[2] = 1E-12;
    opts[3] = 1E-15;
    opts[4] = LM_DIFF_DELTA;

    // 平面极坐标系下，只有一个参数
    int LM_m = 1;
    int LM_n = num;
    std::vector<double> para(LM_m);
    std::vector<double> ptx(num);
    std::vector<glm::vec3> inputPoints3d(num);

    for (int i = 0; i < num; i++) {
        // 方程左边数据 * para = 方程右边
        inputPoints3d[i] = p[i] - center;

        // 方程的右边
        ptx[i] = 0;
    }

    // !!! 注意初始值要设置得有意义，起码计算结果中不能出现IND
    para[0] = glm::pi<float>() / 4;

    rot = rotation;
    minfn = calcProjection2;
    int iter = dlevmar_dif(minfn, &para[0], &ptx[0], LM_m, LM_n, 1000,
                           opts, info, NULL, NULL, &inputPoints3d[0]);
    printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n", info[5], info[6], info[1], info[0]);
    std::cout << "Itered: " << iter << std::endl;

    float alpha = para[0];
    float beta = alpha + glm::pi<float>() / 2;
    glm::vec3 second = rot * glm::vec3(1 * glm::cos(alpha), 1 * glm::sin(alpha), 0);
    glm::vec3 third = rot * glm::vec3(1 * glm::cos(beta), 1 * glm::sin(beta), 0);
    return std::make_pair(second, third);
}

std::vector<glm::vec3> LMModelMainComponent::getModelMainComponent(int pnum, glm::vec3 p[])
{
    glm::vec3 firstN = LMModelMainComponent::PCAWithModelPoints(pnum, p);

    // 求出从z轴到firstN的变换
    auto quat = glm::rotation(glm::vec3(0.f, 0.f, 1.f), firstN);
    glm::mat3 r = glm::toMat3(quat);
    auto pair = LMModelMainComponent::PCAWithModelPoints2(pnum, p, r);
    glm::vec3 secondN = pair.first;
    glm::vec3 thirdN = pair.second;

    std::vector<glm::vec3> result;
    result.push_back(firstN);
    result.push_back(secondN);
    result.push_back(thirdN);
    return result;
}
