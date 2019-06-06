// stdafx.cpp : 只包括标准包含文件的源文件
// DummyClient.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
//引用任何所需的附加头文件，而不是在此文件中引用

#ifdef _DEBUG
	#pragma comment(lib,"..\\VSOut\\Cool3D\\Debug\\Cool3D.lib")
	#pragma comment(lib,"..\\VSOut\\WorldBase\\Debug\\WorldBase.lib")
#else
	#pragma comment(lib,"..\\VSOut\\Cool3D\\Release\\Cool3D.lib")
	#pragma comment(lib,"..\\VSOut\\WorldBase\\Release\\WorldBase.lib")
#endif


HWND g_hWnd;