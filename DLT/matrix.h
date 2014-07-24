#ifndef _MATRIX_PRO
#define _MATRIX_PRO

//n*n的单位矩阵
void I​d​e​n​t​i​t​yMatrix(double* A, int n);
//矩阵相乘
void MultMatrix(double *A, double *B, double *Result, int m, int p, int n);
//矩阵求逆
void InverMatrix(double *A, int n);
//矩阵转置
void TranMatrix(double *A, double *AT, int m, int n);
//矩阵相加
void AddMatrix(double *A, double *B, double *Add, int m, int n);
//向量的点乘
double dot(double *A, double *B, int n);
//向量的叉乘
void cross(double *A, double *B, double *C);
//矩阵相减
void SubMatrix(double *A, double *B, double *Sub, double scale1, double scale2,  int m, int n);
#endif