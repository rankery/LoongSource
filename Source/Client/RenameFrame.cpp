#include "stdafx.h"
#include "RenameFrame.h"
#include "CombatSysUtil.h"
#include "FilterData.h"
#include "RoleMgr.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\msg_role_vcard.h"

RenameFrame::RenameFrame():m_Trunk(this)
{
	m_pWnd = NULL;
	m_pWndBackPic = NULL;
	m_pBtnCancle = NULL;
	m_pBtnRename = NULL;
	m_pEditNewName = NULL;
	m_pStcTips = NULL;
}

RenameFrame::~RenameFrame()
{

}

BOOL RenameFrame::Destroy()
{
	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);
	return GameFrame::Destroy();
}

BOOL RenameFrame::Init( GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;
	return TRUE;
}

BOOL RenameFrame::ReloadUI()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\common3.xml");
	m_pWnd = m_pGUI->CreateFromFile( "VFS_System", strPath.c_str() );
	m_pWndBackPic = (GUIWnd*)m_pWnd->GetChild(_T("comm_pic"));
	m_pStcTips = (GUIStaticEx*)m_pWndBackPic->GetChild(_T("comm_words"));
	m_pStcTips->SetText(g_StrTable[_T("InputNewName")]);
	m_pEditNewName = (GUIEditBox*)m_pWndBackPic->GetChild(_T("putinpic\\putin"));
	m_pBtnRename = (GUIButton*)m_pWndBackPic->GetChild(_T("surebutt"));
	m_pBtnRename->SetText(g_StrTable[_T("Common_OK")]);
	m_pBtnCancle = (GUIButton*)m_pWndBackPic->GetChild(_T("canclebutt"));
	m_pBtnCancle->SetText(g_StrTable[_T("Common_Cancel")]);
	ShowFrame(false);
	m_pGUI->RegisterEventHandler( m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&RenameFrame::EventHandler) );
	return true;
}

VOID RenameFrame::Update()
{

}

BOOL RenameFrame::EscCancle()
{
	if( m_pWnd->IsInvisible() )
		return false;
	ShowFrame(false);
	return true;
}

DWORD RenameFrame::EventHandler( tagGUIEvent *pEvent )
{
	GUIWnd *pWnd = m_pGUI->GetWnd( pEvent->dwWndID );
	if( !P_VALID(pWnd) )
		return false;
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnRename )
			{
				tstring strNewName = m_pEditNewName->GetText();
				FliterStr( strNewName );

				bool bError = CheckName( strNewName );
				LocalPlayer *pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();

				if( P_VALID(pLocalPlayer) && !bError )
				{
					tagNC_LPRename Rename;
					Rename.n64RenameItemID = m_n64SrcID;
					_tcscpy( Rename.szNewName, strNewName.c_str() );
					m_pSession->Send(&Rename);

					tagNC_GetRoleVCard msg;
					msg.dwRoleID = RoleMgr::Inst()->GetLocalPlayerID();
					m_pSession->Send(&msg);
					ShowFrame(false);
				}
			}
			else if( pWnd == m_pBtnCancle )
			{
				ShowFrame(false);
			}
		}
	}
	return false;
}

void RenameFrame::ShowFrame( bool bShow, INT64 n64SrcID )
{
	if( P_VALID(m_pWnd) )
	{
		m_pWnd->SetInvisible( !bShow );
		if( bShow )
		{	
			m_pWnd->FlipToTop();
			m_n64SrcID = n64SrcID;
		}
		else
		{
			m_pEditNewName->SetText(_T(""));
		}
	}
}

void RenameFrame::FliterStr( tstring &str )
{
	INT nFind = 0;
	while( ( nFind = str.find( _T('\r'), nFind ) ) != str.npos )
		str.replace( nFind, 1, _T('') );
	nFind = 0;
	while( ( nFind = str.find( _T('\n'), nFind ) ) != str.npos )
		str.replace( nFind, 1, _T('') );
}

bool RenameFrame::CheckName( tstring &strNewName )
{
	bool bError = false;
	if( _tcslen(strNewName.c_str()) != 0 && ( strNewName[0] >= '0' && strNewName[0] <= '9' ) )
	{
		bError = true;
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CreateRole_NameBad")]);
	}
	else if( _tcslen(strNewName.c_str()) > X_SHORT_NAME )
	{
		bError = true;
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Common_Error30")]);
	}
	else if( _tcslen(strNewName.c_str()) == 0 )
	{
		bError = true;
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Common_Error31")]);
	}
	else if( FilterData::Inst()->IsValidName(strNewName.c_str(), X_SHORT_NAME ) != E_Success )
	{
		bError = true;
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("CreateRole_NameBad")]);
	}
	return bError;
}