// NavMapEditor.h : main header file for the NavMapEditor DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CNavMapEditorApp
// See NavMapEditor.cpp for the implementation of this class
//

class CNavMapEditorApp : public CWinApp
{
public:
	CNavMapEditorApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
