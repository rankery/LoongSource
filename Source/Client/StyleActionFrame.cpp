#include "stdafx.h"
#include "StyleActionFrame.h"
#include "PersonalityActData.h"
#include "CombatEvent.h"
#include "CombatSys.h"
#include "CombatActionChecker.h"
#include "..\WorldDefine\msg_motion.h"
#include "CombatSysUtil.h"
#include "PlayerNameTab.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"

StyleActionFrame::StyleActionFrame() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pBn_Close( NULL ),
m_dwActionTargetID( GT_INVALID ),
m_bWaitRemotePesponses( false ),
m_bWaitLocalPesponses( false ),
m_dwInviteMsgBoxShowTime( 0 ),
m_dwWaitRemoteResponseTime(0),
m_bWaitForInviteRoleName( false ),
m_bMovingTo( false ),
m_bReadyToStart( false )
{

}

StyleActionFrame::~StyleActionFrame()
{
}

BOOL StyleActionFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("OpenClose_StyleAction"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnOpenClose_StyleActionEvent));
	m_pMgr->RegisterEventHandle( _T("tagTryToPlayPersonalityActEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnTryToPlayPersonalityActEvent));
	m_pMgr->RegisterEventHandle( _T("OnStyleActionInviteMsgBox"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnStyleActionInviteMsgBox));
	m_pMgr->RegisterEventHandle( _T("tagRoleGetNameEvent"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnRoleGetNameEvent));
	m_pMgr->RegisterEventHandle( _T("tagMouseMoveFinishEvent"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnMouseMoveFinishEvent));

	m_pCmdMgr->Register("NS_DuetMotionInvite",		(NETMSGPROC)m_Trunk.sfp2(&StyleActionFrame::OnNS_DuetMotionInvite),	_T("NS_DuetMotionInvite"));
	m_pCmdMgr->Register("NS_DuetMotionOnInvite",	(NETMSGPROC)m_Trunk.sfp2(&StyleActionFrame::OnNS_DuetMotionOnInvite),	_T("NS_DuetMotionOnInvite"));
	m_pCmdMgr->Register("NS_RoleStyleAction",		(NETMSGPROC)m_Trunk.sfp2(&StyleActionFrame::OnNS_RoleStyleAction),		_T("NS_RoleStyleAction"));

	return TRUE;
}

BOOL StyleActionFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("OpenClose_StyleAction"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnOpenClose_StyleActionEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagTryToPlayPersonalityActEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnTryToPlayPersonalityActEvent));
	m_pMgr->UnRegisterEventHandler( _T("OnStyleActionInviteMsgBox"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnStyleActionInviteMsgBox));
	m_pMgr->UnRegisterEventHandler( _T("tagRoleGetNameEvent"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnRoleGetNameEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagMouseMoveFinishEvent"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StyleActionFrame::OnMouseMoveFinishEvent));

	m_pCmdMgr->UnRegister("NS_DuetMotionInvite",	(NETMSGPROC)m_Trunk.sfp2(&StyleActionFrame::OnNS_DuetMotionInvite) );
	m_pCmdMgr->UnRegister("NS_DuetMotionOnInvite",	(NETMSGPROC)m_Trunk.sfp2(&StyleActionFrame::OnNS_DuetMotionOnInvite) );
	m_pCmdMgr->UnRegister("NS_RoleStyleAction",		(NETMSGPROC)m_Trunk.sfp2(&StyleActionFrame::OnNS_RoleStyleAction));

	if( P_VALID(GetObj("StyleActionInviteMsgBox")) )
		KillObj("StyleActionInviteMsgBox");

	GameFrame::Destroy();
	SafeDestroyUI();
	return TRUE;
}

BOOL StyleActionFrame::ReloadUI()
{
	SafeDestroyUI();

	// 创建
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\action.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );
	if( !P_VALID( m_pWnd ) )
		return false;

	m_pGUI->RegisterEventHandler( m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&StyleActionFrame::EventHandler) );

	m_pBn_Close = (GUIButton*)m_pWnd->GetChild( _T("actionback\\closebutt") );

	// 隐藏
	m_pWnd->SetInvisible( TRUE );

	return TRUE;
}

VOID StyleActionFrame::SafeDestroyUI()
{
	if( P_VALID( m_pWnd ) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}
}

BOOL StyleActionFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pBn_Close )
			m_pWnd->SetInvisible( TRUE );
		else
		{			
			int nID	= _tcstol( pWnd->GetName().c_str(), NULL, 10 );
			if( nID > 0 )
			{
				const tagPersonAct* pActData = PersonalityActData::Inst()->FindPersonAct( (DWORD)nID );
				if( P_VALID( pActData ) )
				{
					tagTryToPlayPersonalityActEvent actEvent( _T("tagTryToPlayPersonalityActEvent"), this, pActData );
					m_pMgr->SendEvent( &actEvent );
				}
			}
		}
		break;
	}

	return FALSE;
}

VOID StyleActionFrame::Update()
{
	//等待用户按MsgBox超时
	if( m_bWaitLocalPesponses && timeGetTime() - m_dwInviteMsgBoxShowTime > 30000 )
	{
		tagMsgBoxEvent evt( _T("OnStyleActionInviteMsgBox"), NULL );
		evt.eResult = MBF_Cancel;
		m_pMgr->SendEvent(&evt);
		if( P_VALID(GetObj("StyleActionInviteMsgBox")) )
			KillObj("StyleActionInviteMsgBox");
	}
	//等待服务器回应
	if( m_bWaitRemotePesponses )
	{
		Role* pTarget=RoleMgr::Inst()->FindRole(m_dwActionTargetID);
		if( !P_VALID( pTarget ) )
		{
			m_bWaitRemotePesponses = false;
			CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_RoleNotFround );
		}
		if(timeGetTime() - m_dwWaitRemoteResponseTime > 60*1000 ) //timeout
		{
			m_bWaitRemotePesponses = false;
			CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_DstRoleRefuse );
		}
	}
	if( m_bReadyToStart )
	{
		// 再次检查
		if( CombatActionChecker::Inst()->IfCanPlayStyleAction( EMT1_Interactive==m_sPersonActData.eMotionType, m_sPersonActData.dwFriendVal, m_dwActionTargetID, true ) )
		{
			// 发送开始播放动作消息
			tagNC_DuetMotionStart cmd;
			cmd.dwActionID = m_sPersonActData.dwID;
			cmd.dwDstRoleID = m_dwActionTargetID;
			m_pSession->Send( &cmd );
		}
		m_bReadyToStart = false;
	}
	GameFrame::Update();
}

BOOL StyleActionFrame::EscCancel()
{
	if( !P_VALID( m_pWnd ) || m_pWnd->IsInvisible() )
		return FALSE;
	m_pWnd->SetInvisible( TRUE );
	return TRUE;
}

DWORD StyleActionFrame::OnOpenClose_StyleActionEvent(tagGameEvent* )
{
	m_pWnd->SetInvisible( !m_pWnd->IsInvisible() );
	return 0;
}

DWORD StyleActionFrame::OnTryToPlayPersonalityActEvent(tagTryToPlayPersonalityActEvent* pEvent)
{
	ASSERT( P_VALID(pEvent->pData) );

	const bool bInteractive = EMT1_Interactive==pEvent->pData->eMotionType;

	// 正在等待对方或自己回应，不处理新的交互动作
	if( bInteractive )
	{
		if( m_bWaitLocalPesponses )
			return 0;
		if( m_bWaitRemotePesponses )
		{
			CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_CannotStartWhenWait );
			return 0;
		}
	}

	m_dwActionTargetID = GetCurTargetID();
	m_sPersonActData = *pEvent->pData;

	if( !CombatActionChecker::Inst()->IfCanPlayStyleAction( bInteractive, m_sPersonActData.dwFriendVal, m_dwActionTargetID, true ) )
		return 0;

	if( bInteractive )
	{
		// 交互动作，需要请求
		tagNC_DuetMotionInvite cmdRequest;
		cmdRequest.dwActionID = pEvent->pData->dwID;
		cmdRequest.dwDstRoleID = m_dwActionTargetID;
		m_pSession->Send( &cmdRequest );
		m_dwWaitRemoteResponseTime = timeGetTime();
		m_bWaitRemotePesponses = true;		
		CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_WaitRemotePesponses );
	}
	else
	{
		// 发游戏事件给服务器
		tagNC_RoleStyleAction cmd;
		cmd.dwActionID = m_sPersonActData.dwID;
		TObjRef<NetSession>()->Send(&cmd);
	}
	return 0;
}

DWORD StyleActionFrame::OnNS_DuetMotionInvite( tagNS_DuetMotionInvite* pCmd, DWORD )
{
	if( !m_bWaitRemotePesponses )
		return 0;

	m_bWaitRemotePesponses = false;
	if( E_Motion_Success != pCmd->dwErrCode )
	{
		CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( pCmd->dwErrCode );
		return 0;
	}

	// 再次检查
	if( !CombatActionChecker::Inst()->IfCanPlayStyleAction( EMT1_Interactive==m_sPersonActData.eMotionType, m_sPersonActData.dwFriendVal, m_dwActionTargetID, true ) )
		return 0;

	Role* pTarget=RoleMgr::Inst()->FindRole(m_dwActionTargetID);
	ASSERT( P_VALID(pTarget) );

	MoveToTarget( pTarget->GetPos() );
	return 0;
}

DWORD StyleActionFrame::OnNS_DuetMotionOnInvite( tagNS_DuetMotionOnInvite* pCmd, DWORD )
{
	// 正在等待对方或自己回应，拒绝新的邀请
	if( m_bWaitRemotePesponses || m_bWaitLocalPesponses )
	{
		tagNC_DuetMotionOnInvite cmd;
		cmd.dwSrcRoleID = m_dwActionTargetID;
		cmd.dwActionID = m_sPersonActData.dwID;
		cmd.bAccpet = FALSE;
		m_pSession->Send( &cmd );
		return 0;
	}

	const tagPersonAct* pActData = PersonalityActData::Inst()->FindPersonAct( pCmd->dwActionID );
	if( !P_VALID( pActData ) )
	{
		CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_NotValid );
		return 0;
	}

	m_dwActionTargetID = pCmd->dwSrcRoleID;
	m_sPersonActData = *pActData;

	tstring strInviteName = PlayerNameTab::Inst()->FindNameByID( pCmd->dwSrcRoleID );
	if( !strInviteName.empty() )
		ShowInviteMsgBox( strInviteName.c_str() );
	else
		m_bWaitForInviteRoleName = TRUE;


	return 0;
}

DWORD StyleActionFrame::OnStyleActionInviteMsgBox(tagMsgBoxEvent* pEvent)
{
	if( MBF_OK == pEvent->eResult || MBF_Cancel == pEvent->eResult )
	{
		tagNC_DuetMotionOnInvite cmd;
		cmd.dwSrcRoleID = m_dwActionTargetID;
		cmd.dwActionID = m_sPersonActData.dwID;
		cmd.bAccpet = MBF_OK == pEvent->eResult;
		m_pSession->Send( &cmd );
		m_bWaitLocalPesponses = false;
		m_dwInviteMsgBoxShowTime = 0;
	}
	return 0;
}

VOID StyleActionFrame::ShowInviteMsgBox( const TCHAR* szInviteName )
{
	ASSERT( P_VALID( szInviteName ) && szInviteName[0] != _T('\0') );

	if( P_VALID(GetObj("StyleActionInviteMsgBox")) )
		KillObj("StyleActionInviteMsgBox");

	CreateObj("StyleActionInviteMsgBox", "MsgBox");
	MsgBox* pMsgBox = TObjRef<MsgBox>("StyleActionInviteMsgBox");

	tstring strActName = m_sPersonActData.strCommand1;
	if( strActName.length() > 0 )
		strActName.erase( 0, 1 );
	tstring strText = szInviteName;
	strText += g_StrTable[_T("Motion_InviteText")];
	strText += strActName;
	pMsgBox->Init( g_StrTable[_T("Motion_InviteCaption")], strText.c_str(), 
		_T("OnStyleActionInviteMsgBox"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), FALSE);

	m_dwInviteMsgBoxShowTime = timeGetTime();
	m_bWaitLocalPesponses = true;
}

DWORD StyleActionFrame::OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent)
{
	if( m_bWaitForInviteRoleName && pEvent->dwRoleID == m_dwActionTargetID )
	{
		ShowInviteMsgBox( pEvent->strRoleName.c_str() );
		m_bWaitForInviteRoleName = false;
	}
	return 0;
}

DWORD StyleActionFrame::OnNS_RoleStyleAction(tagNS_RoleStyleAction* pNetCmd, DWORD)
{
	if( pNetCmd->dwRoleID != RoleMgr::Inst()->GetLocalPlayerID() && 
		pNetCmd->dwDstRoleID != RoleMgr::Inst()->GetLocalPlayerID() )
		return 0;

	// 发游戏事件给战斗系统
	tagPlayPersonalityActEvent actEvent( _T("tagPlayPersonalityActEvent"), this, &m_sPersonActData);
	actEvent.dwTargetID = m_dwActionTargetID;
	actEvent.bDestRole = RoleMgr::Inst()->GetLocalPlayerID() == pNetCmd->dwDstRoleID;
	m_pMgr->SendEvent( &actEvent );

	return 0;
}

void StyleActionFrame::MoveToTarget(const Vector3& vTargetPos)
{
	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	ASSERT( P_VALID(pLP) );
	tagMouseMoveEvent event;
	event.start=pLP->GetPos();
	event.end=vTargetPos;
	event.bUserCtrl=false;
	event.validDist=0.0f;
	pLP->OnGameEvent(&event);
	m_bMovingTo = true;
}

DWORD StyleActionFrame::OnMouseMoveFinishEvent( tagMouseMoveFinishEvent* pEvent )
{
	if( m_bMovingTo )
	{
		m_bMovingTo = false;

		if( pEvent->bSuccessed )
		{
			// 检查是否处与同一位置
			Role* pTarget=RoleMgr::Inst()->FindRole(m_dwActionTargetID);
			Role* pLP = RoleMgr::Inst()->GetLocalPlayer();
			if( !P_VALID(pTarget) || !P_VALID(pLP) )
				return 0;
			const float fDistSq = Vec3DistSq( pLP->GetPos(), pTarget->GetPos() );
			if( fDistSq > FLOAT_EPSILON )
			{
				CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_CanotActive );
				return 0;
			}
			m_bReadyToStart = true;
		}
	}
	return 0;
}