#include "StdAfx.h"
#include "CombatFrame_PK.h"
#include "CombatEvent.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\msg_pk.h"
#include "..\WorldDefine\pk_define.h"
#include "CombatSysUtil.h"
#include "PlayerStateFrame.h"
#include "MsgBox.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"

CombatFrame_PK::CombatFrame_PK(void):m_trunk(this)
{
}

CombatFrame_PK::~CombatFrame_PK(void)
{
}

BOOL CombatFrame_PK::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	//--注册游戏事件处理函数
	pMgr->RegisterEventHandle(_T("tagOpenSafeguardEvent"),	(FRAMEEVENTPROC)
		m_trunk.sfp1(&CombatFrame_PK::OnOpenSafeguardEvent));

	pMgr->RegisterEventHandle(_T("tagOpenPKEvent"),	(FRAMEEVENTPROC)
		m_trunk.sfp1(&CombatFrame_PK::OnOpenPKEvent));

	pMgr->RegisterEventHandle(_T("OnFreeModeMsgBoxEvent"),	(FRAMEEVENTPROC)
		m_trunk.sfp1(&CombatFrame_PK::OnFreeModeMsgBoxEvent));

	//--注册网络消息处理函数
	m_pCmdMgr->Register("NS_SafeGuard",		(NETMSGPROC)
		m_trunk.sfp2(&CombatFrame_PK::OnNS_SafeGuard),	_T("NS_SafeGuard"));

	m_pCmdMgr->Register("NS_PK",			(NETMSGPROC)
		m_trunk.sfp2(&CombatFrame_PK::OnNS_PK),			_T("NS_PK"));
	m_pCmdMgr->Register("NS_RoleBeAttacked",			(NETMSGPROC)m_trunk.sfp2(&CombatFrame_PK::OnNS_RoleBeAttacked),			_T("NS_RoleBeAttacked"));

	return GameFrame::Init(pMgr,this,0);
}

BOOL CombatFrame_PK::Destroy()
{
	//--注销游戏事件处理函数
	m_pMgr->UnRegisterEventHandler(_T("tagOpenSafeguardEvent"),	(FRAMEEVENTPROC)
		m_trunk.sfp1(&CombatFrame_PK::OnOpenSafeguardEvent));

	m_pMgr->UnRegisterEventHandler(_T("tagOpenPKEvent"),(FRAMEEVENTPROC)
		m_trunk.sfp1(&CombatFrame_PK::OnOpenPKEvent));

	m_pMgr->UnRegisterEventHandler(_T("OnFreeModeMsgBoxEvent"),	(FRAMEEVENTPROC)
		m_trunk.sfp1(&CombatFrame_PK::OnFreeModeMsgBoxEvent));

	//--注销网络消息处理函数
	m_pCmdMgr->UnRegister("NS_SafeGuard",	(NETMSGPROC)m_trunk.sfp2(&CombatFrame_PK::OnNS_SafeGuard));
	m_pCmdMgr->UnRegister("NS_PK",			(NETMSGPROC)m_trunk.sfp2(&CombatFrame_PK::OnNS_PK));
	m_pCmdMgr->UnRegister("NS_RoleBeAttacked",			(NETMSGPROC)m_trunk.sfp2(&CombatFrame_PK::OnNS_RoleBeAttacked));


	return 0;
}

VOID CombatFrame_PK::Update()
{

}

DWORD CombatFrame_PK::OnOpenSafeguardEvent( tagOpenSafeguardEvent* pEvent )
{
	//LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//if((pSelf->GetRoleState()&ERS_Safeguard)!=0
	//	&&pEvent->bOpen==false)
	//{
	//	TCHAR szWarnningText[256]={0};
	//	if( pSelf->GetRoleLevel()>=SAFE_GUARD_FREE_LEVEL )
	//		_stprintf( szWarnningText, g_StrTable[_T("FreeModeWarnning2")], SAFE_GUARD_FREE_LEVEL );
	//	else if(pSelf->GetRoleLevel()>=SAFE_GUARD_FORCE_LEVEL)
	//		_stprintf( szWarnningText, g_StrTable[_T("FreeModeWarnning1")], OPEN_SAFE_GUARD_TIME / 3600 );

	//	if(0!=szWarnningText[0])
	//	{
	//		if( P_VALID(GetObj("OpenPKMsgBox")) )
	//			KillObj("OpenPKMsgBox");

	//		CreateObj("OpenPKMsgBox", "MsgBox");
	//		TObjRef<MsgBox>("OpenPKMsgBox")->Init(_T(""), szWarnningText, 
	//			_T("OnFreeModeMsgBoxEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
	//		return 0;
	//	}
	//}

	tagNC_SafeGuard cmd;
	cmd.bOpen=pEvent->bOpen;
	m_pSession->Send(&cmd);

	return 0;
}

DWORD CombatFrame_PK::OnOpenPKEvent( tagOpenPKEvent* pEvent )
{
	tagNC_PK cmd;
	cmd.bOnlyPK = pEvent->bPK;
	m_pSession->Send(&cmd);
	return 0;
}

DWORD CombatFrame_PK::OnFreeModeMsgBoxEvent(tagMsgBoxEvent* pEvent)
{
	//进入自由模式
	if(pEvent->eResult==MBF_OK)
	{
		tagNC_SafeGuard cmd;
		cmd.bOpen=false;
		m_pSession->Send(&cmd);
	}
	return 0;
}

DWORD CombatFrame_PK::OnNS_SafeGuard( tagNS_SafeGuard* pNetCmd, DWORD dwParam )
{
	if(pNetCmd->dwErrorCode==E_Success)
	{
		if(pNetCmd->bOpen)
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("BeSafeGuardMode")]);
		else
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("BeFreeMode")]);
	}
	else
	{
		CombatSysUtil::Inst()->ShowPKErrorMsg(pNetCmd->dwErrorCode);
	}

	return 0;
}

DWORD CombatFrame_PK::OnNS_PK( tagNS_PK* pNetCmd, DWORD dwParam )
{
	if(pNetCmd->dwErrorCode==E_Success)
	{
		//显示行凶倒计时
		tagPKStateStartCountDownEvent event(_T("tagPKStateStartCountDownEvent"),this);
		event.dwTimeCount=120000;
		m_pMgr->SendEvent(&event);

		if( pNetCmd->bReset == false )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("BePKMode")]);
			if( pNetCmd->bOnlyPK )
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PK_Mode0")]);
			else
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PK_Mode1")]);
		}
	}
	else
	{
		CombatSysUtil::Inst()->ShowPKErrorMsg(pNetCmd->dwErrorCode);
	}

	return 0;
}


DWORD CombatFrame_PK::OnNS_RoleBeAttacked( tagNS_RoleBeAttacked* pNetCmd, DWORD dwParam )
{
	CombatSysUtil::Inst()->ShowScreenCenterMsgEx(g_StrTable[_T("LP_BeAttacked")]);
	return 0;
}
