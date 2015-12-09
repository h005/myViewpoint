#include "lmpca.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <levmar.h>
#include <opencv2/opencv.hpp>
#include <glm/gtx/matrix_cross_product.hpp>
#include <glm/gtx/string_cast.hpp>

static glm::vec3 ballAxisToVector(double alpha, double beta)
{
    return glm::vec3(sin(alpha) * cos(beta), sin(alpha) * sin(beta), cos(alpha));
}

static void calcProjection(double parameter[], double newP[], int LM_m, int LM_n, void *adata)
{
    double alpha = parameter[0], beta = parameter[1];
    glm::vec3 w = ballAxisToVector(alpha, beta);
    std::cout << glm::to_string(w) << std::endl;
    glm::vec3 *p = (glm::vec3 *)adata;
    int pnum = LM_n;
    for (int i = 0; i < pnum; i++)
        newP[i] = glm::dot(w, p[i]);
}

glm::vec3 LMPCA::PCAWithModelPoints(int num, glm::vec3 p[])
{
    // 求点阵的中心
    glm::vec3 center = glm::vec3(0.f);
    for (int i = 0; i < num; i++)
        center += p[i];
    center /= num;

    std::cout << glm::to_string(center) << std::endl;

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

    std::cout << para[0] << " " << para[1] << std::endl;
    return ballAxisToVector(para[0], para[1]);
}
