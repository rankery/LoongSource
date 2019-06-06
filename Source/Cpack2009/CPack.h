// CPack.h : main header file for the CPACK application
//

#if !defined(AFX_CPACK_H__F3DBFD93_E448_4D49_B4C2_F5F381FA0BAE__INCLUDED_)
#define AFX_CPACK_H__F3DBFD93_E448_4D49_B4C2_F5F381FA0BAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCPackApp:
// See CPack.cpp for the implementation of this class
//

class CCPackApp : public CWinApp
{
public:
	CCPackApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCPackApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCPackApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPACK_H__F3DBFD93_E448_4D49_B4C2_F5F381FA0BAE__INCLUDED_)
