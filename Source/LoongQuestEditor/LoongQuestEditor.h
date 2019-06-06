// LoongQuestEditor.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLoongQuestEditorApp:
// See LoongQuestEditor.cpp for the implementation of this class
//

class CLoongQuestEditorApp : public CWinApp
{
public:
	CLoongQuestEditorApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CLoongQuestEditorApp theApp;