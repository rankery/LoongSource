// ShowSum.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CShowSumApp:
// See ShowSum.cpp for the implementation of this class
//
class CShowSumDlg;

class CShowSumApp : public CWinApp
{
public:
	CShowSumApp();
	~CShowSumApp();
	CShowSumDlg *GetDlg(){ return m_pDlg; }
	

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	CShowSumDlg *m_pDlg;
};

extern CShowSumApp theApp;