// Dxt.h : main header file for the DXT application
//

#if !defined(AFX_DXT_H__CE354613_7405_46FB_86AB_31AC0C132278__INCLUDED_)
#define AFX_DXT_H__CE354613_7405_46FB_86AB_31AC0C132278__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDxtApp:
// See Dxt.cpp for the implementation of this class
//

class CDxtApp : public CWinApp
{
public:
	CDxtApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDxtApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDxtApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DXT_H__CE354613_7405_46FB_86AB_31AC0C132278__INCLUDED_)
