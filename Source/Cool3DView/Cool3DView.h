// Cool3DView.h : Cool3DView 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号


// CCool3DViewApp:
// 有关此类的实现，请参阅 Cool3DView.cpp
//

class CCool3DViewApp : public CWinApp
{
public:
	CCool3DViewApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CCool3DViewApp theApp;
