#include "StdAfx.h"
#include "CombatFrame_DeadMenu.h"
#include "CombatEvent.h"
#include "CombatSysUtil.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "ItemMgr.h"
#include "..\WorldDefine\specfunc_define.h"

CombatFrame_DeadMenu::CombatFrame_DeadMenu(void)
: m_Trunk(this)
, m_pWnd(0)
, m_pBtnLocalRise(0)
, m_pBtnAcceptRise(0)
, m_pBtnBackRise(0)
, m_pStcHint(0)
{
}

CombatFrame_DeadMenu::~CombatFrame_DeadMenu(void)
{
	
}

BOOL CombatFrame_DeadMenu::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	//--注册网络消息处理函数
	m_pCmdMgr->Register("NS_RoleRevive", (NETMSGPROC)m_Trunk.sfp2(&CombatFrame_DeadMenu::NetRecvNS_RoleRevive),
		_T("NS_RoleRevive"));
	m_pCmdMgr->Register("NS_RoleReviveNotify", (NETMSGPROC)m_Trunk.sfp2(&CombatFrame_DeadMenu::NetRecvNS_RoleReviveNotify), 
		_T("NS_RoleReviveNotify"));

	return TRUE;
}

BOOL CombatFrame_DeadMenu::Destroy()
{
	GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);

	//--注册网络消息处理函数
	m_pCmdMgr->UnRegister("NS_RoleRevive", (NETMSGPROC)m_Trunk.sfp2(&CombatFrame_DeadMenu::NetRecvNS_RoleRevive));
	m_pCmdMgr->UnRegister("NS_RoleReviveNotify", (NETMSGPROC)m_Trunk.sfp2(&CombatFrame_DeadMenu::NetRecvNS_RoleReviveNotify));

	return 0;
}

VOID CombatFrame_DeadMenu::Update()
{
}

DWORD CombatFrame_DeadMenu::NetRecvNS_RoleRevive( tagNS_RoleRevive* pMsg, DWORD dwParam )
{
	DWORD dwLocalID = RoleMgr::Inst()->GetLocalPlayerID();

	if(pMsg->dwRoleID == dwLocalID)
	{
		switch(pMsg->dwErrorCode)
		{
		case E_Success:
			m_pWnd->SetInvisible(TRUE);
			m_pFrameMgr->AddToDestroyList(this);
			break;
		case E_Revive_NotDead:
			m_pStcHint->SetText(g_StrTable[_T("Revive_NotDead")]);
			break;
		case E_Revive_CanNotTransmit:
			m_pStcHint->SetText(g_StrTable[_T("Revive_CanNotTransmit")]);
			break;
		case E_Revive_ItemLimit:
			m_pStcHint->SetText(g_StrTable[_T("Revive_ItemLimit")]);
			break;
		case E_Revive_CanNotAccept:
			m_pStcHint->SetText(g_StrTable[_T("Revive_CanNotAccept")]);
			break;
		}
		m_pWnd->SetRefresh(TRUE);
	}
	return 0;
}

DWORD CombatFrame_DeadMenu::NetRecvNS_RoleReviveNotify( tagNS_RoleReviveNotify* pMsg, DWORD dwParam )
{
	if(P_VALID(m_pBtnAcceptRise))
	{
		m_pBtnAcceptRise->SetEnable(true);
		m_pWnd->SetRefresh(TRUE);
	}
	return 0;
}

BOOL CombatFrame_DeadMenu::ReloadUI()
{
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\resurrection .xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&CombatFrame_DeadMenu::EventHandler));

	m_pBtnLocalRise = (GUIButton*) m_pWnd->GetChild(_T("pic\\butt1"));
	m_pBtnAcceptRise = (GUIButton*) m_pWnd->GetChild(_T("pic\\butt2"));
	if(P_VALID(m_pBtnAcceptRise))
		m_pBtnAcceptRise->SetEnable(false);
	m_pBtnBackRise	= (GUIButton*) m_pWnd->GetChild(_T("pic\\butt3"));
	m_pStcHint = (GUIStatic*) m_pWnd->GetChild(_T("pic\\word"));
	if( P_VALID(m_pStcHint) )
		m_pStcHint->SetText(g_StrTable[_T("Revive_Hint")]);

	return TRUE;
}

BOOL CombatFrame_DeadMenu::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnLocalRise )
			{
				INT64 n64ID = ItemMgr::Inst()->GetSpecfuncItemID(EISF_RevieveItem);
				if(n64ID == GT_INVALID)
				{
					//提示
					if( P_VALID(m_pStcHint) )
					{
						m_pStcHint->SetText(g_StrTable[_T("Revive_ItemLimit")]);
						m_pWnd->SetRefresh(TRUE);
					}
					return FALSE;
				}
						
				tagNC_RoleRevive msg;
				msg.eType = ERRT_Locus;
				msg.n64ItemID = n64ID;
				m_pSession->Send(&msg);

			}
			else if( pWnd == m_pBtnAcceptRise )
			{
				tagNC_RoleRevive msg;
				msg.eType = ERRT_Accept;
				m_pSession->Send(&msg);
			}
			else if( pWnd == m_pBtnBackRise)
			{				
				tagNC_RoleRevive msg;
				msg.eType = ERRT_ReturnCity;
				m_pSession->Send(&msg);
			}		
		}
		break;
	}

	return TRUE;
}