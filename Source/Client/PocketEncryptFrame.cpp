#include "StdAfx.h"
#include "PocketEncryptFrame.h"
#include "..\WorldDefine\msg_item.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "ItemMgr.h"
#include ".\SeleRoleMgr.h"

PocketEncryptFrame::PocketEncryptFrame(void):m_Trunk(this)
{
	m_pWnd			= NULL;
	m_pWndCaption	= NULL;
	m_pBtnSetup		= NULL;
	m_pBtnModify	= NULL;
	m_pBtnCancel	= NULL;
	m_pBtnExit		= NULL;

}

PocketEncryptFrame::~PocketEncryptFrame(void)
{
}

BOOL PocketEncryptFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	return bRet;
}

BOOL PocketEncryptFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if(P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
	}

	return bRet;
}

BOOL PocketEncryptFrame::ReloadUI()
{
	//创建
	tstring strPath = g_strLocalPath + _T("\\ui\\bagpasswordset.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&PocketEncryptFrame::EventHandler));

	m_pWndCaption = m_pWnd->GetChild(_T("bpset_pic"));

	m_pBtnSetup = (GUIButton*)m_pWndCaption->GetChild(_T("butt1"));
	m_pBtnModify = (GUIButton*)m_pWndCaption->GetChild(_T("butt100"));
	m_pBtnCancel = (GUIButton*)m_pWndCaption->GetChild(_T("butt1000"));
	m_pBtnExit   = (GUIButton*)m_pWndCaption->GetChild(_T("butt10"));


	return TRUE;
}

DWORD PocketEncryptFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if(pWnd == m_pBtnSetup)
			{
				if(RoleMgr::Inst()->GetLocalPlayer()->GetRoleStateEx(ERSE_BagPsdExist))
				{
					ItemMgr::Inst()->ShowItemErrorMsg(E_BagPsd_Exist);
					return 0;
				}

				// 如果没有安全码则弹出提示对话框
				if(!GT_VALID(SeleRoleMgr::Inst()->GetSafeCodeCrc()))
				{
					if(P_VALID(GetObj("NoSafeCodePrompt")))
						KillObj("NoSafeCodePrompt");

					CreateObj("NoSafeCodePrompt", "MsgBox");
					TObjRef<MsgBox>("NoSafeCodePrompt")->Init(_T(""), g_StrTable[_T("PocketEncrypt_NoSafeCodePrompt")], NULL, MBF_OK);
					return 0;
				}

				if(P_VALID(GetObj("SetPsdFirst")))
					KillObj("SetPsdFirst");

				CreateObj("SetPsdFirst", "MsgInputBox");
				TObjRef<MsgInputBox>("SetPsdFirst")->Init(
					g_StrTable[_T("PocketEncrypt_CaptionSetup")],g_StrTable[_T("PocketEncrypt_TextNewPassword")],
					_T("MsgBox_SetPsdFirst"),GT_INVALID, MBIT_Password);

				m_pMgr->AddToDestroyList(this);
			}
			else if(pWnd == m_pBtnModify)
			{
				if(!RoleMgr::Inst()->GetLocalPlayer()->GetRoleStateEx(ERSE_BagPsdExist))
				{
					ItemMgr::Inst()->ShowItemErrorMsg(E_BagPsd_NoExist);
					return 0;
				}
				
				if(P_VALID(GetObj("ModPsdFirst")))
					KillObj("ModPsdFirst");

				CreateObj("ModPsdFirst", "MsgInputBox");
				TObjRef<MsgInputBox>("ModPsdFirst")->Init(
					g_StrTable[_T("PocketEncrypt_CaptionModify")],g_StrTable[_T("PocketEncrypt_TextCurrentPassword")],
					_T("MsgBox_ModPsdFirst"),GT_INVALID, MBIT_Password);

				m_pMgr->AddToDestroyList(this);
			}
			else if(pWnd == m_pBtnCancel)
			{
				if(!RoleMgr::Inst()->GetLocalPlayer()->GetRoleStateEx(ERSE_BagPsdExist))
				{
					ItemMgr::Inst()->ShowItemErrorMsg(E_BagPsd_NoExist);
					return 0;
				}

				if(P_VALID(GetObj("DelPsdFirst")))
					KillObj("DelPsdFirst");

				CreateObj("DelPsdFirst", "MsgInputBox");
				TObjRef<MsgInputBox>("DelPsdFirst")->Init(
					g_StrTable[_T("PocketEncrypt_CaptionCancel")],g_StrTable[_T("PocketEncrypt_TextSafePassword")],
					_T("MsgBox_DelPsdFirst"),GT_INVALID, MBIT_Password);

				m_pMgr->AddToDestroyList(this);
			}
			else if(pWnd == m_pBtnExit)
			{
				m_pMgr->AddToDestroyList(this);
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

BOOL PocketEncryptFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	m_pMgr->AddToDestroyList(this);
	return TRUE;
}