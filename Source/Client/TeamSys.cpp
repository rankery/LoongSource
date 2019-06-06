#include "StdAfx.h"
#include "TeamSys.h"
#include "TeamEvent.h"
#include "..\WorldDefine\msg_group.h"
#include "..\WorldDefine\msg_common_errorcode.h"
#include "Role.h"
#include "RoleMgr.h"
#include "MapMgr.h"
#include "PlayerNameTab.h"
#include "TeammatesStateFrame.h"
#include "CombatSysUtil.h"
#include "ChatFrame.h"

#include "AudioSys.h"

TeamSys::TeamSys(void) :
m_trunk( this ),
m_dwTeamID( GT_INVALID ),
m_dwLeaderRoleID( GT_INVALID ),
m_ePickMode( EPUM_Order ),
m_eRefuseApplyMode( ERAM_Accept ),
m_dwInviteRoleID( GT_INVALID ),
m_dwApplyTeamRPId( GT_INVALID ),
m_bWaitForInviteRoleName( FALSE ),
m_dwKickingMemberID( GT_INVALID ),
m_dwInviteMsgBoxShowTime( 0 ),
m_dwWantToJoinTeamTime(0)
{
}

TeamSys::~TeamSys(void)
{
}

TeamSys g_ts;
TeamSys* TeamSys::Inst()
{
	return &g_ts;
}

BOOL TeamSys::Init(void)
{
	m_pCmdMgr		= TObjRef<NetCmdMgr>();
	m_pSession		= TObjRef<NetSession>();
	m_pUtil			= TObjRef<Util>();
	m_pGameFrameMgr = TObjRef<GameFrameMgr>();

	m_pCmdMgr->Register("NS_InviteToLeader",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_InviteToLeader),		_T("NS_InviteToLeader"));
	m_pCmdMgr->Register("NS_InviteJoinTeam",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_InviteJoinTeam),		_T("NS_InviteJoinTeam"));
	m_pCmdMgr->Register("NS_InviteReply",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_InviteReply),			_T("NS_InviteReply"));
	m_pCmdMgr->Register("NS_KickMember",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_KickMember),			_T("NS_KickMemberr"));
	m_pCmdMgr->Register("NS_LeaveTeam",					(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_LeaveTeam),				_T("NS_LeaveTeam"));
	m_pCmdMgr->Register("NS_SetPickMode",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_SetPickMode),			_T("NS_SetPickMode"));
	m_pCmdMgr->Register("NS_ChangeLeader",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_ChangeLeader),			_T("NS_ChangeLeader"));
	m_pCmdMgr->Register("NS_RoleStateToTeam",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RoleStateToTeam),		_T("NS_RoleStateToTeam"));
	m_pCmdMgr->Register("NS_RolePosToTeam",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RolePosToTeam),			_T("NS_RolePosToTeam"));
	m_pCmdMgr->Register("NS_RoleLevelChange",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RoleLevelChange),		_T("NS_RoleLevelChange"));
	m_pCmdMgr->Register("NS_RemoteAttChangeSingle",		(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RemoteAttChangeSingle),	_T("NS_RemoteAttChangeSingle"));
	m_pCmdMgr->Register("NS_RemoteAttChangeMutiple",	(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RemoteAttChangeMutiple),_T("NS_RemoteAttChangeMutiple"));
	m_pCmdMgr->Register("NS_GetTeamID",					(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_GetTeamID),				_T("NS_GetTeamID"));
	m_pCmdMgr->Register("NS_ApplyToJoinTeam",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RPWantToJoinTeam),		_T("NS_ApplyToJoinTeam"));
	m_pCmdMgr->Register("NS_ApplyReplyRefuse",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_ApplyReplyRefuse),	_T("NS_ApplyReplyRefuse") );

	m_pGameFrameMgr->RegisterEventHandle( _T("tagRoleGetNameEvent"),    (FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnRoleGetNameEvent));
	m_pGameFrameMgr->RegisterEventHandle( _T("OnInviteMsgBox"),			(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnInviteMsgBox));
	m_pGameFrameMgr->RegisterEventHandle( _T("OnLeaveTeamMsgBox"),		(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnLeaveTeamMsgBox));
	m_pGameFrameMgr->RegisterEventHandle( _T("OnKickMemberMsgBox"),		(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnKickMemberMsgBox));
	m_pGameFrameMgr->RegisterEventHandle( _T("tagInviteJoinTeamEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnInviteJoinTeamEvent));
	m_pGameFrameMgr->RegisterEventHandle( _T("tagApplyJoinTeamEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnApplyJoinTeamEvent));
	m_pGameFrameMgr->RegisterEventHandle( _T("OnApplyToJoinTeamMsgBox"),	(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnApplyToJoinTeamMsgBox ));
	return TRUE;
}

VOID TeamSys::Destroy(void)
{
	m_pCmdMgr->UnRegister("NS_InviteToLeader",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_InviteToLeader));
	m_pCmdMgr->UnRegister("NS_InviteJoinTeam",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_InviteJoinTeam));
	m_pCmdMgr->UnRegister("NS_InviteReply",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_InviteReply));
	m_pCmdMgr->UnRegister("NS_KickMember",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_KickMember));
	m_pCmdMgr->UnRegister("NS_LeaveTeam",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_LeaveTeam));
	m_pCmdMgr->UnRegister("NS_SetPickMode",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_SetPickMode));
	m_pCmdMgr->UnRegister("NS_ChangeLeader",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_ChangeLeader));
	m_pCmdMgr->UnRegister("NS_RoleStateToTeam",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RoleStateToTeam));
	m_pCmdMgr->UnRegister("NS_RolePosToTeam",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RolePosToTeam));
	m_pCmdMgr->UnRegister("NS_RoleLevelChange",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RoleLevelChange));
	m_pCmdMgr->UnRegister("NS_RemoteAttChangeSingle",	(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RemoteAttChangeSingle));
	m_pCmdMgr->UnRegister("NS_RemoteAttChangeMutiple",	(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RemoteAttChangeMutiple));
	m_pCmdMgr->UnRegister("NS_GetTeamID",				(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_GetTeamID));
	m_pCmdMgr->UnRegister("NS_ApplyToJoinTeam",			(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_RPWantToJoinTeam));
	m_pCmdMgr->UnRegister("NS_ApplyReplyRefuse",		(NETMSGPROC)m_trunk.sfp2(&TeamSys::OnNS_ApplyReplyRefuse));

	m_pGameFrameMgr->UnRegisterEventHandler( _T("tagRoleGetNameEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnRoleGetNameEvent));
	m_pGameFrameMgr->UnRegisterEventHandler( _T("OnInviteMsgBox"),			(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnInviteMsgBox));
	m_pGameFrameMgr->UnRegisterEventHandler( _T("OnLeaveTeamMsgBox"),		(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnLeaveTeamMsgBox));
	m_pGameFrameMgr->UnRegisterEventHandler( _T("OnKickMemberMsgBox"),		(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnKickMemberMsgBox));
	m_pGameFrameMgr->UnRegisterEventHandler( _T("tagInviteJoinTeamEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnInviteJoinTeamEvent));
	m_pGameFrameMgr->UnRegisterEventHandler( _T("OnApplyToJoinTeamMsgBox"),	(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnApplyToJoinTeamMsgBox ));
	m_pGameFrameMgr->UnRegisterEventHandler( _T("tagApplyJoinTeamEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&TeamSys::OnApplyJoinTeamEvent));
	m_vecTeammates.clear();
	OnBreakTeam( false );

	if( P_VALID(GetObj("TeamInviteMsgBox")) )
		KillObj("TeamInviteMsgBox");
	if( P_VALID(GetObj("LeaveTeamMsgBox")) )
		KillObj("LeaveTeamMsgBox");
	if( P_VALID(GetObj("KickTeamMemberMsgBox")) )
		KillObj("KickTeamMemberMsgBox");
	if( P_VALID( GetObj( "ApplyToJoinTeam_MsgBox" ) ) )
		KillObj( "ApplyToJoinTeam_MsgBox" );
}

VOID TeamSys::Updata()
{
	// 15s没有响应邀请提示，拒绝邀请
	if( 0 != m_dwInviteMsgBoxShowTime && timeGetTime() - m_dwInviteMsgBoxShowTime > 15000 )
	{
		tagMsgBoxEvent evt( _T("OnInviteMsgBox"), NULL );
		evt.eResult = MBF_Cancel;
		m_pGameFrameMgr->SendEvent(&evt);
		if( P_VALID(GetObj("TeamInviteMsgBox")) )
			KillObj("TeamInviteMsgBox");
	}

	// 如果在九宫格内则使用角色位置更新队友坐标
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		tagTeammateData& data = m_vecTeammates[i];
		Role* pRole = RoleMgr::Inst()->FindRole( data.dwRoleID );
		if( P_VALID(pRole) && pRole->IsAttributeInited() )
		{
			data.dwMapID = MapMgr::Inst()->GetCurMapID();
			data.fX = pRole->GetPos().x;
			data.fZ = pRole->GetPos().z;
		}
	}
	
	//其他玩家申请入队，如果队长15秒内没做处理，关闭窗口，发送拒绝信息
	if( m_dwWantToJoinTeamTime != 0 && timeGetTime() - m_dwWantToJoinTeamTime > 15000 )
	{
		tagMsgBoxEvent evt( _T("OnApplyToJoinTeamMsgBox"), NULL );
		evt.eResult = MBF_Cancel;
		m_pGameFrameMgr->SendEvent( &evt );
		if( P_VALID( GetObj("ApplyToJoinTeam_MsgBox") ) )
			KillObj("ApplyToJoinTeam_MsgBox");
		
		m_dwWantToJoinTeamTime = 0;
		m_dwApplyTeamRPId = GT_INVALID;
	}

	//没有正在处理的玩家，并且有等待处理的玩家
	if( m_dwApplyTeamRPId == GT_INVALID && m_mapApplyRP.Size() > 0 ) 
	{

		DWORD dwRPId = GT_INVALID;
		INT nLevel = GT_INVALID;
		
		tagNS_ApplyToJoinTeam req;
		m_mapApplyRP.ResetIterator();
		if( !m_mapApplyRP.PeekNext( dwRPId, req) ) //nLevel ) )
			return;

		m_dwApplyTeamRPId = dwRPId;
		if( m_dwApplyTeamRPId != GT_INVALID )
		{
			if( P_VALID( GetObj("ApplyToJoinTeam_MsgBox") ) )
				KillObj("ApplyToJoinTeam_MsgBox");
			CreateObj( "ApplyToJoinTeam_MsgBox", "MsgBox" );

			TCHAR msg[50] = {0};
			if( nLevel == GT_INVALID )
			{
				_stprintf( msg, g_StrTable[_T("Team_RPWantToJoinTeam_1")], req.szName );//PlayerNameTab::Inst()->FindNameByNameID(m_dwApplyTeamRPId) );
			}
			else
			{
				_stprintf( msg, g_StrTable[_T("Team_RPWantToJoinTeam_2")], req.szName,/*PlayerNameTab::Inst()->FindNameByNameID(m_dwApplyTeamRPId)*/ req.nLevel );
			}

			TObjRef<MsgBox>("ApplyToJoinTeam_MsgBox")->Init( _T(""), msg, 
				_T("OnApplyToJoinTeamMsgBox"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE );
			
			m_dwWantToJoinTeamTime = timeGetTime();		//开始计时
		}
	}
}

DWORD TeamSys::OnNS_InviteToLeader( tagNS_InviteToLeader* pCmd, DWORD )
{
	if( m_dwLeaderRoleID != RoleMgr::Inst()->GetLocalPlayerID() && m_vecTeammates.size() > 0 )
		return 0;

	if( E_Success != pCmd->dwErrorCode )
	{
		HandleNSErrorCode( pCmd->dwErrorCode );
		return 0;
	}
	return 0;
}

DWORD TeamSys::OnNS_InviteJoinTeam( tagNS_InviteJoinTeam* pCmd, DWORD )
{
	if( RoleMgr::Inst()->GetLocalPlayerID() != pCmd->dwDestRoleID )
		return 0;

	tstring strInviteName = PlayerNameTab::Inst()->FindNameByID( pCmd->dwLeaderID );
	m_dwInviteRoleID = pCmd->dwLeaderID;
	if( !strInviteName.empty() )
		ShowInviteMsgBox( strInviteName.c_str() );
	else
		m_bWaitForInviteRoleName = TRUE;

	return 0;
}

DWORD TeamSys::OnNS_RPWantToJoinTeam( tagNS_ApplyToJoinTeam* pCmd, DWORD )
{
	DWORD dwLPId = RoleMgr::Inst()->GetLocalPlayerID();

	//队伍满了 或者 队长设置了拒绝申请模式
	if( m_eRefuseApplyMode == ERAM_Refuse || TeamSys::Inst()->IsTeamFull() )
	{
		tagNC_ApplyReply e;
		e.dwApplicantID = pCmd->dwApplicantID;
		e.bTeamLeaderAgree = FALSE;
		m_pSession->Send( &e );
		return 0;
	}

	if( TeamSys::Inst()->IsTeammate( pCmd->dwApplicantID ) )
		return 0;

	tagNS_ApplyToJoinTeam reqJoinTeam;
	memcpy( &reqJoinTeam, pCmd, sizeof( tagNS_ApplyToJoinTeam) );

	if( !m_mapApplyRP.IsExist( pCmd->dwApplicantID ) )
		m_mapApplyRP.Add( pCmd->dwApplicantID, reqJoinTeam );//pCmd->nLevel );
	
	return 0;
}

DWORD TeamSys::OnNS_ApplyReplyRefuse( tagNS_ApplyReplyRefuse* pCmd, DWORD )
{
	if( P_VALID( GetObj( "LeaderRefuseYouMsgBox" ) ) )
		KillObj( "LeaderRefuseYouMsgBox" );

	CreateObj( "LeaderRefuseYouMsgBox","MsgBox" );
	
	TCHAR szTmp[100] ={0};
	switch( pCmd->dwErrCode )
	{
	case E_Team_Member_Full:
		{
			HandleNSErrorCode( E_Team_Member_Full );
			TObjRef<MsgBox>("LeaderRefuseYouMsgBox")->Init( _T(""), 
				g_StrTable[_T("Team_ErrorCode_4")], _T("tagMsgBoxEvent"), MBF_OK, TRUE );
		}
		break;
	case E_Team_Peer_Not_Online:
		{
			TObjRef<MsgBox>("LeaderRefuseYouMsgBox")->Init( _T(""), 
				g_StrTable[_T("Team_YourTargetOrLeaderOffLine")], _T("tagMsgBoxEvent"), MBF_OK, TRUE );
		}
		break;
	case E_Team_Refuse:
		{
			_stprintf( szTmp, g_StrTable[_T("LeaderRefuseYouJoin")], pCmd->szName );
			TObjRef<MsgBox>("LeaderRefuseYouMsgBox")->Init( _T(""), szTmp, _T("tagMsgBoxEvent"), MBF_OK, TRUE );
		}
		break;
	}
	
	return 0;
}

DWORD TeamSys::OnNS_InviteReply( tagNS_InviteReply* pCmd, DWORD )
{
	if( E_Success != pCmd->dwErrorCode )
	{
		HandleNSErrorCode( pCmd->dwErrorCode );
		return 0;
	}

	if( !pCmd->bAgree )
	{
		if( RoleMgr::Inst()->GetLocalPlayerID() != pCmd->dwTeamMemID[1] )
		{
			tstring strMsg = PlayerNameTab::Inst()->FindNameByID( pCmd->dwTeamMemID[1] );
			strMsg += g_StrTable[_T("Team_InviteDisAgree")];
			CombatSysUtil::Inst()->ShowScreenCenterMsg( strMsg.c_str() );
			AddChatMsg( strMsg.c_str() );
		}
		return 0;
	}
	const DWORD dwLocalPlayerID = RoleMgr::Inst()->GetLocalPlayerID();
	for( int i = 0; i < MAX_TEAM_NUM; i++ )
	{
		if( !P_VALID( pCmd->dwTeamMemID[i] ) )
			break;
		if( pCmd->dwTeamMemID[i] == dwLocalPlayerID )
			continue;
		if( NULL != GetTeammateData( pCmd->dwTeamMemID[i] ) )
			continue;

		tagTeammateData data;
		data.dwRoleID = pCmd->dwTeamMemID[i];
		data.strName  = PlayerNameTab::Inst()->FindNameByID( data.dwRoleID );
		m_vecTeammates.push_back( data );
		tagAddTeammateEvent evt;
		evt.dwRoleID = data.dwRoleID;
		m_pGameFrameMgr->SendEvent( &evt );
	}

	// 伪造NS_ChangeLeader
	if( GT_INVALID == m_dwLeaderRoleID )
	{
		tagNS_ChangeLeader ccmd;
		ccmd.dwLeaderID = m_dwLeaderRoleID;
		ccmd.dwNewLeaderID = pCmd->dwTeamMemID[0];
		ccmd.dwErrorCode = E_Success;
		OnNS_ChangeLeader( &ccmd, 0 );
	}

	// 播放进入队伍音效
	TObjRef<AudioSys>()->Play2DSound(_T("team_join"), SOUND_NORMAL);
	return 0;
}

DWORD TeamSys::OnNS_KickMember( tagNS_KickMember* pCmd, DWORD )
{
	if( E_Success != pCmd->dwErrorCode )
	{
		HandleNSErrorCode( pCmd->dwErrorCode );
		return 0;
	}

	if( pCmd->dwDestRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		m_vecTeammates.clear();
		ShowLeaveTeamMsg( g_StrTable[_T("You")] );
	}
	else
	{
		for( vector<tagTeammateData>::iterator itr = m_vecTeammates.begin(); itr != m_vecTeammates.end(); itr++ )
		{
			if( pCmd->dwDestRoleID == itr->dwRoleID )
			{
				ShowLeaveTeamMsg( itr->strName.c_str() );
				m_vecTeammates.erase( itr );
				break;
			}
		}
		tagDelTeammateEvent evtDel;
		evtDel.dwRoleID = pCmd->dwDestRoleID;
		m_pGameFrameMgr->SendEvent( &evtDel );
	}

	// 队伍解散
	if( m_vecTeammates.empty() )
		OnBreakTeam();

	return 0;
}

DWORD TeamSys::OnNS_LeaveTeam( tagNS_LeaveTeam* pCmd, DWORD )
{
	if( E_Success != pCmd->dwErrorCode )
	{
		HandleNSErrorCode( pCmd->dwErrorCode );
		return 0;
	}
	if( pCmd->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		m_vecTeammates.clear();
		ShowLeaveTeamMsg( g_StrTable[_T("You")] );
	}
	else
	{
		for( vector<tagTeammateData>::iterator itr = m_vecTeammates.begin(); itr != m_vecTeammates.end(); itr++ )
		{
			if( pCmd->dwRoleID == itr->dwRoleID )
			{
				ShowLeaveTeamMsg( itr->strName.c_str() );
				m_vecTeammates.erase( itr );
				break;
			}
		}

		// 队长离开了
		if( pCmd->dwLeaderID != m_dwLeaderRoleID && m_vecTeammates.size() > 0 )
		{
			ASSERT( m_dwLeaderRoleID == pCmd->dwRoleID );
			
			// 伪造NS_ChangeLeader
			tagNS_ChangeLeader ccmd;
			ccmd.dwLeaderID = m_dwLeaderRoleID;
			ccmd.dwNewLeaderID = pCmd->dwLeaderID;
			ccmd.dwErrorCode = E_Success;
			OnNS_ChangeLeader( &ccmd, 0 );
		}

		tagDelTeammateEvent evtDel;
		evtDel.dwRoleID = pCmd->dwRoleID;
		m_pGameFrameMgr->SendEvent( &evtDel );
	}

	// 队伍解散
	if( m_vecTeammates.empty() )
		OnBreakTeam();

	return 0;
}

DWORD TeamSys::OnNS_SetPickMode( tagNS_SetPickMode* pCmd, DWORD )
{
	if( E_Success != pCmd->dwErrorCode )
	{
		HandleNSErrorCode( pCmd->dwErrorCode );
		return 0;
	}

	if( m_ePickMode == pCmd->ePickMode )
		return 0;

	m_ePickMode = pCmd->ePickMode;

	tstring strMsg = g_StrTable[_T("Team_SetPickMode")];
	tstringstream stream;
	stream<<_T("LPMenu_PickMode_")<<pCmd->ePickMode;
	strMsg += g_StrTable[stream.str().c_str()];
	AddChatMsg( strMsg.c_str() );

	return 0;
}

DWORD TeamSys::OnNS_ChangeLeader( tagNS_ChangeLeader* pCmd, DWORD )
{
	if( E_Success != pCmd->dwErrorCode )
	{
		HandleNSErrorCode( pCmd->dwErrorCode );
		return 0;
	}

	m_dwLeaderRoleID = pCmd->dwNewLeaderID;
	tagTeamLeaderChangeEvent evt;
	evt.dwNewLeaderID = pCmd->dwNewLeaderID;
	m_pGameFrameMgr->SendEvent( &evt );

	tstring strMsg;
	if( pCmd->dwNewLeaderID == RoleMgr::Inst()->GetLocalPlayerID() )
		strMsg = g_StrTable[_T("You")];
	else
		strMsg = GetTeammateNameByID( pCmd->dwNewLeaderID );
	strMsg += g_StrTable[_T("Team_LeaderChange")];
	AddChatMsg( strMsg.c_str() );

	return 0;
}

DWORD TeamSys::OnNS_RoleStateToTeam( tagNS_RoleStateToTeam* pCmd, DWORD )
{
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		tagTeammateData& data = m_vecTeammates[i];
		if( pCmd->dwRoleID != data.dwRoleID )
			continue;
		data.dwMapID	= pCmd->dwMapID;
		data.eClassEx	= pCmd->eClassEx;
		data.nLevel		= pCmd->nLevel;
		data.nMaxHP		= pCmd->nMaxHP;
		data.nHP		= pCmd->nHP;
		data.nMaxMP		= pCmd->nMaxMP;
		data.nMP		= pCmd->nMP;
		data.fX			= pCmd->fX;
		data.fZ			= pCmd->fZ;
		for( int n = 0; n < TEAM_DISPLAY_NUM; n++ )
		{
			data.dwEquipTypeID[n] = pCmd->dwEquipTypeID[n];
		}
		data.AvatarAtt = pCmd->AvatarAtt;

		tagTeammateStateInitEvent evt;
		evt.dwRoleID = pCmd->dwRoleID;
		m_pGameFrameMgr->SendEvent( &evt );
		
		if( !data.strName.empty() )
			ShowJoinTeamMsg( data.strName.c_str() );

		break;
	}
	return 0;
}

DWORD TeamSys::OnNS_RolePosToTeam( tagNS_RolePosToTeam* pCmd, DWORD )
{
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		tagTeammateData& data = m_vecTeammates[i];
		if( pCmd->dwRoleID != data.dwRoleID )
			continue;
		data.dwMapID = pCmd->dwMapID;
		data.fX = pCmd->fX;
		data.fZ = pCmd->fZ;
		break;
	}
	return 0;
}

DWORD TeamSys::OnNS_RoleLevelChange( tagNS_RoleLevelChange* pCmd, DWORD )
{
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		tagTeammateData& data = m_vecTeammates[i];
		if( pCmd->dwRoleID != data.dwRoleID )
			continue;
		data.nLevel = pCmd->nLevel;
		tagTeammateLevelChangeEvent evt;
		evt.dwRoleID = pCmd->dwRoleID;
		evt.nLevel = pCmd->nLevel;
		m_pGameFrameMgr->SendEvent( &evt );
		break;
	}
	return 0;
}

DWORD TeamSys::OnNS_RemoteAttChangeSingle( tagNS_RemoteAttChangeSingle* pCmd, DWORD )
{
	if( ERRA_MaxHP != pCmd->eType &&
		ERRA_HP    != pCmd->eType &&
		ERRA_MaxMP != pCmd->eType &&
		ERRA_MP    != pCmd->eType )
		return 0;
	
	tagTeammateData* pData = NULL;
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		if( pCmd->dwRoleID == m_vecTeammates[i].dwRoleID )
		{
			pData = &m_vecTeammates[i];
			break;
		}
	}
	if( NULL == pData )
		return 0;

	switch( pCmd->eType )
	{
		case ERRA_MaxHP:	pData->nMaxHP	= pCmd->nValue; break;
		case ERRA_HP:		pData->nHP		= pCmd->nValue; break;
		case ERRA_MaxMP:	pData->nMaxMP	= pCmd->nValue; break;
		case ERRA_MP:		pData->nMP		= pCmd->nValue; break;
		default: break;
	}

	tagTeammateAttChangeEvent evt;
	evt.dwRoleID = pData->dwRoleID;
	evt.eType = pCmd->eType;
	evt.nValue = pCmd->nValue;
	m_pGameFrameMgr->SendEvent( &evt );

	return 0;
}

DWORD TeamSys::OnNS_RemoteAttChangeMutiple( tagNS_RemoteAttChangeMutiple* pCmd, DWORD )
{
	tagNS_RemoteAttChangeSingle tmpCmd;
	tmpCmd.dwRoleID = pCmd->dwRoleID;
	for( int i = 0; i < pCmd->nNum; ++i )
	{
		tmpCmd.eType = pCmd->value[i].eType;
		tmpCmd.nValue = pCmd->value[i].nValue;
		OnNS_RemoteAttChangeSingle( &tmpCmd, 0 );
	}
	return 0;
}

DWORD TeamSys::OnNS_GetTeamID( tagNS_GetTeamID* pCmd, DWORD )
{
	m_dwTeamID = pCmd->dwTeamID;
	m_ePickMode = pCmd->ePickMode;
	return 0;
}

VOID TeamSys::HandleNSErrorCode( const DWORD dwErrorCode )
{
	tstringstream stream;
	stream<<_T("Team_ErrorCode_")<<dwErrorCode;
	tstring strErrorMsg = g_StrTable[stream.str().c_str()];
	AddChatMsg( strErrorMsg.c_str() );
}

DWORD TeamSys::OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent)
{
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		if( pEvent->dwRoleID == m_vecTeammates[i].dwRoleID && !pEvent->strRoleName.empty() )
		{
			if( m_vecTeammates[i].strName.empty() )
				ShowJoinTeamMsg( pEvent->strRoleName.c_str()  );

			m_vecTeammates[i].strName = pEvent->strRoleName;
			tagTeammateNameUpdataEvent evt;
			evt.dwRoleID = pEvent->dwRoleID;
			evt.strName = pEvent->strRoleName;
			m_pGameFrameMgr->SendEvent( &evt );
		}
	}
	if( m_bWaitForInviteRoleName && pEvent->dwRoleID == m_dwInviteRoleID )
	{
		ShowInviteMsgBox( pEvent->strRoleName.c_str() );
		m_bWaitForInviteRoleName = FALSE;
	}
	return 0;
}

DWORD TeamSys::OnInviteMsgBox(tagMsgBoxEvent* pEvent)
{
	if( MBF_OK == pEvent->eResult || MBF_Cancel == pEvent->eResult )
	{
		tagNC_InviteReply cmd;
		cmd.dwLeaderID = m_dwInviteRoleID;
		cmd.bAgree = ( MBF_OK == pEvent->eResult );
		m_pSession->Send( &cmd );
		m_dwInviteRoleID = GT_INVALID;
		m_dwInviteMsgBoxShowTime = 0;
	}
	return 0;
}

DWORD TeamSys::OnLeaveTeamMsgBox( tagMsgBoxEvent* pEvent )
{
	if( MBF_OK == pEvent->eResult )
	{
		tagNC_LeaveTeam cmd;
		m_pSession->Send( &cmd );
	}
	return 0;
}

DWORD TeamSys::OnKickMemberMsgBox( tagMsgBoxEvent* pEvent )
{
	if( MBF_OK == pEvent->eResult )
	{
		tagNC_KickMember cmd;
		cmd.dwDestRoleID = m_dwKickingMemberID;
		cmd.dwSrcRoleID = m_dwLeaderRoleID;
		m_pSession->Send( &cmd );
	}
	m_dwKickingMemberID = GT_INVALID;
	return 0;
}

//是否同意其他玩家加入本地玩家所在的队伍
DWORD TeamSys::OnApplyToJoinTeamMsgBox( tagMsgBoxEvent* pEvent )
{
	if( !P_VALID( pEvent ) || m_dwApplyTeamRPId == GT_INVALID )
		return 0;

	tagNC_ApplyReply cmd;
	cmd.dwApplicantID = m_dwApplyTeamRPId;
	if( MBF_OK == pEvent->eResult )
	{
		cmd.bTeamLeaderAgree = TRUE;
	}
	else
	{
		cmd.bTeamLeaderAgree = FALSE;
	}
	m_pSession->Send( &cmd );

	m_mapApplyRP.Erase( m_dwApplyTeamRPId );
	m_dwApplyTeamRPId = GT_INVALID;
	m_dwWantToJoinTeamTime = 0;
	return 0;
}

DWORD TeamSys::OnInviteJoinTeamEvent(tagInviteJoinTeamEvent* pEvent)
{
	InviteJoinTeam( pEvent->dwRoleID );
	return 0;
}

DWORD  TeamSys::OnApplyJoinTeamEvent( tagApplyJoinTeamEvent* pEvent )
{
	ApplyJoinTeam( pEvent->dwRoleID );
	return 0;
}

bool TeamSys::IsTeammate( const DWORD dwRoleID )
{
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		if( dwRoleID == m_vecTeammates[i].dwRoleID )
			return true;
	}
	return false;
}

void TeamSys::SetPickMode( const EPickMode ePickMode )
{
	if( m_dwLeaderRoleID != RoleMgr::Inst()->GetLocalPlayerID() )
	{
		HandleNSErrorCode( E_Team_Not_Leader );
		return;
	}

	if( ePickMode == m_ePickMode )
		return;

	tagNC_SetPickMode cmd;
	cmd.ePickMode = ePickMode;
	m_pSession->Send(&cmd);
}


void TeamSys::KickTeammate( const DWORD dwTeammateID )
{
	ASSERT( P_VALID( dwTeammateID ) );
	if( m_dwLeaderRoleID != RoleMgr::Inst()->GetLocalPlayerID() )
	{
		HandleNSErrorCode( E_Team_Not_Leader );
		return;
	}
	
	const tagTeammateData* pData = GetTeammateData( dwTeammateID );
	if( !P_VALID( pData ) )
		return;

	ShowKickMemberMsgBox( pData->strName.c_str() );
	m_dwKickingMemberID = dwTeammateID;
}


void TeamSys::HandOutLeader( const DWORD dwNewLeaderID )
{
	ASSERT( P_VALID( dwNewLeaderID ) );
	if( m_dwLeaderRoleID != RoleMgr::Inst()->GetLocalPlayerID() )
	{
		HandleNSErrorCode( E_Team_Not_Leader );
		return;
	}

	if( !IsTeammate( dwNewLeaderID ) )
	{
		HandleNSErrorCode( E_Team_Target_Not_Exit );
		return;
	}

	tagNC_ChangeLeader cmd;
	cmd.dwNewLeader = dwNewLeaderID;
	m_pSession->Send( &cmd );
}


tstring TeamSys::GetTeammateNameByID( const DWORD dwRoleID ) const
{
	ASSERT( P_VALID( dwRoleID ) );
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		if( dwRoleID == m_vecTeammates[i].dwRoleID )
		{
			return m_vecTeammates[i].strName;
		}
	}
	return _T("");
}

DWORD TeamSys::GetTeammateIDByName( const TCHAR* szName ) const
{
	ASSERT( P_VALID( szName ) );
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		if( m_vecTeammates[i].strName == szName )
		{
			return m_vecTeammates[i].dwRoleID;
		}
	}
	return GT_INVALID;
}

void TeamSys::LeaveTeam()
{
	if( GetTeammateNum() == 0 )
		return;

	ShowLeaveTeamMsgBox();
}

void TeamSys::InviteJoinTeam( const DWORD dwRoleID )
{
	ASSERT( P_VALID( dwRoleID ) );
	if( m_dwLeaderRoleID != RoleMgr::Inst()->GetLocalPlayerID() && m_vecTeammates.size() > 0 )
	{
		HandleNSErrorCode( E_Team_Not_Leader );
		return;
	}
	if( IsTeamFull() )
	{
		HandleNSErrorCode( E_Team_Member_Full );
		return;
	}
	if( IsTeammate( dwRoleID ) )
	{
		HandleNSErrorCode( E_Team_Target_Have_Team );
		return;
	}

	tagNC_InviteJoinTeam cmd;
	cmd.dwDestRoleID = dwRoleID;
	m_pSession->Send( &cmd );
}

/**申请加入其他队伍
*/
void TeamSys::ApplyJoinTeam( const DWORD dwRoleID )
{
	ASSERT( P_VALID( dwRoleID ) );
	
	//本地玩家已经有队伍了，不能再加入别的队伍
	if( TeamSys::Inst()->IsInTeam() )
	{
		HandleNSErrorCode( E_Team_You_Have_Team );
		return;
	}

	tagNC_ApplyToJoinTeam cmd;
	cmd.dwTeamMemberID = dwRoleID;
	m_pSession->Send( &cmd );
}

const tagTeammateData* TeamSys::GetTeammateData( const DWORD dwRoleID )
{
	for( size_t i = 0; i < m_vecTeammates.size(); i++ )
	{
		if( dwRoleID == m_vecTeammates[i].dwRoleID )
			return &m_vecTeammates[i];
	}
	return NULL;
}

DWORD TeamSys::GetTeamID() const
{
	if( m_vecTeammates.size() == 0 )
		return GT_INVALID;

	return m_dwTeamID;
}

VOID TeamSys::ShowInviteMsgBox( const TCHAR* szInviteName )
{
	ASSERT( P_VALID( szInviteName ) && szInviteName[0] != _T('\0') );

	if( P_VALID(GetObj("TeamInviteMsgBox")) )
		KillObj("TeamInviteMsgBox");

	CreateObj("TeamInviteMsgBox", "MsgBox");
	MsgBox* pMsgBox = TObjRef<MsgBox>("TeamInviteMsgBox");

	tstring strText = szInviteName;
	strText += g_StrTable[_T("Team_InviteText")];
	pMsgBox->Init( g_StrTable[_T("Team_InviteCaption")], strText.c_str(), 
											   _T("OnInviteMsgBox"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), FALSE);

	m_dwInviteMsgBoxShowTime = timeGetTime();
}

VOID TeamSys::ShowLeaveTeamMsgBox()
{
	if( P_VALID(GetObj("LeaveTeamMsgBox")) )
		KillObj("LeaveTeamMsgBox");

	CreateObj("LeaveTeamMsgBox", "MsgBox");

	TObjRef<MsgBox>("LeaveTeamMsgBox")->Init( _T(""), g_StrTable[_T("Team_LeaveTeamPrompt")], 
		_T("OnLeaveTeamMsgBox"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);

}

VOID TeamSys::ShowKickMemberMsgBox( const TCHAR* szDestName )
{
	ASSERT( P_VALID( szDestName ) && szDestName[0] != _T('\0') );

	if( P_VALID(GetObj("KickTeamMemberMsgBox")) )
		KillObj("KickTeamMemberMsgBox");

	CreateObj("KickTeamMemberMsgBox", "MsgBox");

	TCHAR szText[512];
	_stprintf( szText, g_StrTable[_T("Team_KickMemberPrompt")], szDestName );
	TObjRef<MsgBox>("KickTeamMemberMsgBox")->Init( _T(""), szText, 
		_T("OnKickMemberMsgBox"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
}

VOID TeamSys::AddChatMsg( const TCHAR* szMsg )
{
	ChatFrame* pChatFrame=(ChatFrame*)m_pGameFrameMgr->GetFrame(_T("Chat"));
	if( !P_VALID( pChatFrame ) )
		return;
	pChatFrame->PushInfo( szMsg, ESCC_Troop );
}

VOID TeamSys::ShowJoinTeamMsg( const TCHAR* szPlayerName )
{
	tstring strMsg = szPlayerName;
	strMsg += g_StrTable[_T("Team_MemberJoin")];
	AddChatMsg( strMsg.c_str() );
}

VOID TeamSys::ShowLeaveTeamMsg( const TCHAR* szPlayerName )
{
	tstring strMsg = szPlayerName;
	strMsg += g_StrTable[_T("Team_MemberLeave")];
	AddChatMsg( strMsg.c_str() );
}

VOID TeamSys::OnBreakTeam( const bool bSendEvent/* = true */)
{
	if( m_dwLeaderRoleID != GT_INVALID && bSendEvent )
	{
		// 通知外部队长没有了
		tagTeamLeaderChangeEvent evt;
		evt.dwNewLeaderID = GT_INVALID;
		m_pGameFrameMgr->SendEvent( &evt );

		// 发送离开队伍事件
		tagLeaveTeamEvent evtLeave;
		m_pGameFrameMgr->SendEvent( &evtLeave );
	}

	m_dwTeamID = GT_INVALID;
	m_dwLeaderRoleID = GT_INVALID;
	m_ePickMode = EPUM_Order;
	m_dwInviteRoleID = GT_INVALID;
	m_bWaitForInviteRoleName = FALSE;
	m_dwKickingMemberID = GT_INVALID;
	m_dwInviteMsgBoxShowTime = 0;
	m_dwWantToJoinTeamTime = 0;
	m_eRefuseApplyMode = ERAM_Accept;
}