/////////////////////////////////////////////////////////////////////////////
// Name:        servermergetoolapp.cpp
// Purpose:     
// Author:      songg
// Modified by: 
// Created:     15/07/2009 09:30:27
// RCS-ID:      
// Copyright:   songg
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "StdAfx.h"

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "ServerMergeToolapp.h"
#include "Loong_LoongDBCtrl.h"
#include "Loong_LoginDBCtrl.h"
////@begin XPM images
////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( ServerMergeToolApp )
////@end implement app


/*
 * ServerMergeToolApp type definition
 */

IMPLEMENT_CLASS( ServerMergeToolApp, wxApp )


/*
 * ServerMergeToolApp event table definition
 */

BEGIN_EVENT_TABLE( ServerMergeToolApp, wxApp )

////@begin ServerMergeToolApp event table entries
////@end ServerMergeToolApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for ServerMergeToolApp
 */
 //------------------------------------------------------------------------------
 // CONSTANT
 //------------------------------------------------------------------------------


ServerMergeToolApp::ServerMergeToolApp()
{
    Init();
}


/*
 * Member initialisation
 */ 

void ServerMergeToolApp::Init()
{
	//先建个log文件夹
	TCHAR tszPath[MAX_PATH];
	tstring tstrPath;
	GetModuleFileName(NULL,tszPath,MAX_PATH);

	//把文件路径处理一下
	tstrPath=tszPath;
	tstrPath=tstrPath.substr(0,tstrPath.rfind('\\'));
	tstrPath=tstrPath.substr(0,tstrPath.rfind('\\'));
	tstrPath += _T('/');
	tstrPath += _T(LOG_FOLDER); //一个自适应类型的路径

	_tmkdir(tstrPath.c_str());


	BOOL bRet = vEngine::InitEngine();
	ASSERT(bRet);

	new CLoongLoginDBCtrl;
	new	CLoongLoongDBCtrl;
	//程序初始化
	m_pLog		=	"Log";
	m_pUtil		=	"Util";

	m_pLog->Create();
}

/*
 * Initialisation for ServerMergeToolApp
 */

bool ServerMergeToolApp::OnInit()
{    
	if(!wxApp::OnInit())
		return false;

	MainPage* mainWindow = new MainPage(NULL);
	mainWindow->ShowModal();

	mainWindow->Destroy();

    return true;
}


/*
 * Cleanup for ServerMergeToolApp
 */

int ServerMergeToolApp::OnExit()
{  
	vEngine::DestroyEngine();

	return wxApp::OnExit();
}

