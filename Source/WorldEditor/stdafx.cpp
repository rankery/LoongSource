// stdafx.cpp : 只包括标准包含文件的源文件
// WorldEditor.pch 将是预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#ifdef _DEBUG
	#pragma comment(lib,"..\\VSOut\\Cool3D\\Debug\\Cool3D.lib")
	#pragma comment(lib,"..\\VSOut\\WorldBase\\Debug\\WorldBase.lib")
#else
	#pragma comment(lib,"..\\VSOut\\Cool3D\\Release\\Cool3D.lib")
	#pragma comment(lib,"..\\VSOut\\WorldBase\\Release\\WorldBase.lib")
#endif