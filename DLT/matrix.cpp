#include "matrix.h"
#include <stdio.h>
#include <math.h>

void I​d​e​n​t​i​t​yMatrix(double* A, int n)
{
	int i;
	int j;
	for(i = 0; i < n; ++i)
		for(j = 0; j < n; ++j)
		{
			if(i == j)
				A[i*n + j] = 1.0;
			else
				A[i*n + j] = 0.0;
		}
}

void MultMatrix(double *A, double *B, double *Result, int m, int p, int n)
{
	int i;
	int j;
	int k;
	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			for(k=0; k<p; k++)
				Result[i*n+j] += A[i*p+k]*B[k*n+j];
}

void TranMatrix(double *A, double *AT, int m, int n)
{
	int i;
	int j;

	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
		{
			AT[j*m+i] = A[i*n+j];
		}
}

void InverMatrix(double *A, int n)
{
    int *is=new int[n];
    int *js=new int[n];

    int i,j,k,l,u,v;
    double temp,max_v;

    if(is==NULL||js==NULL)
    {
        printf("out of memory!\n");
        return ;
    }

    for(k=0;k<n;k++)
    {
		max_v=0.0;
		
		for(i=k;i<n;i++)
			for(j=k;j<n;j++)
			{
				temp=fabs(A[i*n+j]);
			
				if(temp>max_v)
				{
				    max_v=temp; is[k]=i; js[k]=j;
				}
			}
	
		if(max_v==0.0)
		{
			delete []is;
			delete []js;
			
			printf("inverts is not available!\n");
			return ;
		}
	
		if(is[k]!=k)
			for(j=0;j<n;j++)
			{
				u=k*n+j; v=is[k]*n+j;
				temp=A[u]; A[u]=A[v]; A[v]=temp;
			}
	
		if(js[k]!=k)
			for(i=0;i<n;i++)
			{
				u=i*n+k; v=i*n+js[k];
				temp=A[u]; A[u]=A[v]; A[v]=temp;
			}
		
		l=k*n+k;
		A[l]=1.0/A[l];
	
		for(j=0;j<n;j++)
			if(j!=k)
			{
				u=k*n+j;
				A[u]*=A[l];
			}
	
		for(i=0;i<n;i++)
			if(i!=k)
				for(j=0;j<n;j++)
					if(j!=k)
					{
						u=i*n+j;
						A[u]-=A[i*n+k]*A[k*n+j];
					}
	
		for(i=0;i<n;i++)
			if(i!=k)
			{
				u=i*n+k;
				A[u]*=-A[l];
			}

    }
    
	for(k=n-1;k>=0;k--)
    {
		if(js[k]!=k)
			for(j=0;j<n;j++)
			{
				u=k*n+j; v=js[k]*n+j;
				temp=A[u]; A[u]=A[v]; A[v]=temp;
			}
	
		if(is[k]!=k)
			for(i=0;i<n;i++)
			{
				u=i*n+k; v=i*n+is[k];
				temp=A[u]; A[u]=A[v]; A[v]=temp;
			}
    }
    
	delete []is;
    delete []js;
}

void AddMatrix(double *A, double *B, double *Add, int m, int n)
{
	int i;
	int j;

	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			Add[i*n+j] = A[i*n+j] + B[i*n+j];
}

double dot(double *A, double *B, int n)
{
	double sum = 0.0;
	for(int i = 0; i < n; i ++)
		sum  += A[i]*B[i];
	return sum;
}

void cross(double *A, double *B,double *C)
{
	C[0] = A[1]*B[2] - A[2]*B[1];
	C[1] = A[2]*B[0] - A[0]*B[2];
	C[2] = A[0]*B[1] - A[1]*B[0];
}

void SubMatrix(double *A, double *B, double *Sub, double scale1,double scale2, int m, int n)
{
	int i;
	int j;

	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			Sub[i*n+j] = (A[i*n+j] - scale1*B[i*n+j])/scale2;
}