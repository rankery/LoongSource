#include "stdafx.h"
#include "QuestTargetWnd.h"
#include "..\WorldDefine\msg_map.h"
#include "InstanceMgr.h"

QuestTargetWnd::QuestTargetWnd()
: m_trunk(this) 
, m_pGUI(NULL)
, m_pFrameMgr(NULL)
, m_pWnd(NULL)
, m_pTargetDesc(NULL)
, m_pBtnOut(NULL)
{
}

QuestTargetWnd::~QuestTargetWnd()
{
	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}
}

BOOL QuestTargetWnd::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	m_pTargetDesc->SetText(_T(""));

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), 
		m_trunk.sfp1(&QuestTargetWnd::EventHandler));

	m_pFrameMgr->RegisterEventHandle(_T("OnInstanceRequestToLeaveBox"), 
		(FRAMEEVENTPROC)(m_trunk.sfp1(&QuestTargetWnd::OnInstanceRequestToLeaveBox)));
	
	return bRet;
}

BOOL QuestTargetWnd::Destroy()
{
	m_pGUI->UnRegisterEventHandler(m_pWnd->GetFullName().c_str(), 
		m_trunk.sfp1(&QuestTargetWnd::EventHandler));
	return TRUE;
}

BOOL QuestTargetWnd::ReloadUI()
{
	tstring strPath = g_strLocalPath + _T("\\ui\\quest_target.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());

	m_pTargetDesc = (GUIStatic*)(m_pWnd->GetChild(_T("qt_pic")));
	m_pBtnOut = (GUIButton*)(m_pTargetDesc->GetChild(_T("outbutt")));
	return TRUE;
}

BOOL QuestTargetWnd::EventHandler(tagGUIEvent *pEvent)
{
	GUIWnd *pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if (!P_VALID(pWnd))
		return FALSE;

	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if (pWnd == m_pBtnOut)
			{
				if (P_VALID(GetObj("InstanceRequestToLeaveBox")))
					KillObj("InstanceRequestToLeaveBox");
				CreateObj("InstanceRequestToLeaveBox", "MsgBox");
				TObjRef<MsgBox>("InstanceRequestToLeaveBox")->Init(_T(""), g_StrTable[_T("Instance_Request_Leave")], 
					_T("OnInstanceRequestToLeaveBox"), 
					(MsgBoxFlag)(MBF_OK | MBF_Cancel), TRUE);
			}
		} break;
	default: break;
	}

	return TRUE;
}

VOID QuestTargetWnd::OnInstanceRequestToLeaveBox(tagMsgBoxEvent *pEvent)
{
	switch (pEvent->eResult)
	{
	case MBF_OK:
		{
			InstanceMgr::Inst()->Send_RequestLeaveInstance();
		} break;
	case MBF_Cancel:
	case MBF_Null:
		break;
	default:
		break;
	}
}
