#include <iostream>
#include "example.h"

#pragma comment(lib, "example.lib")

int main()
{
	if (!exampleInitialize())
	{
		std::cout << "初始化失败！" << std::endl;
		return -1;
	}

	//调用dll中的函数

	mwArray a(3, 3, mxDOUBLE_CLASS);
	double in[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	// MATLAB's column-major array storage
	a.SetData(in, 3 * 3);
	std::cout << "Input" << std::endl;
	std::cout << a << std::endl;

	mwArray y(3, 3, mxDOUBLE_CLASS);
	// 该函数转置mat
	example3(1, y, a);
	std::cout << "Output" << std::endl;
	std::cout << y << std::endl;
	
	 
	return 0;
}