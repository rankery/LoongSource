// WorldEditorMgr.h : WorldEditor 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号


// CWorldEditorApp:
// 有关此类的实现，请参阅 WorldEditor.cpp
//

class CWorldEditorApp : public CWinApp
{
public:
	CWorldEditorApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int Run();

// 实现

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);

public:
	static	bool m_bMyActive;
};

extern CWorldEditorApp theApp;
