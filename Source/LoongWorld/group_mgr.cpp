//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: group_mgr.h
// author: wjiang
// actor:
// data: 2009-1-04
// last:
// brief: 团队管理类
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/group_define.h"
#include "../WorldDefine/msg_group.h"

#include "../ServerDefine/role_data.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "group_mgr.h"
#include "player_session.h"
#include "unit.h"
#include "role.h"
#include "role_mgr.h"
#include "team.h"
#include "map_creator.h"
#include "map.h"
#include "map_instance.h"
#include "map_mgr.h"
#include "TimeChecker.h"

//-------------------------------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------------------------------
GroupMgr::~GroupMgr()
{
}

//-------------------------------------------------------------------------------------------------------
// 管理类初时化
//-------------------------------------------------------------------------------------------------------
BOOL GroupMgr::Init()
{
	m_dwTeamIndex = 3000000001;
	m_mapTeam.Clear();
	RegisterGroupEventFunc();
	return TRUE;
}

//--------------------------------------------------------------------------------------------------------
// 注册事件
//--------------------------------------------------------------------------------------------------------
VOID GroupMgr::RegisterGroupEventFunc()
{
	RegisterEventFunc(EVT_JoinTeam,			&GroupMgr::OnInviteJoinTeam);
	RegisterEventFunc(EVT_JoinTeamRepley,	&GroupMgr::OnInviteJoinTeamReply);
	RegisterEventFunc(EVT_KickMember,		&GroupMgr::OnLeaderKickMember);
	RegisterEventFunc(EVT_LeaveTeam,		&GroupMgr::OnMemberLeaveTeam);
	RegisterEventFunc(EVT_SetPickMol,		&GroupMgr::OnSetPickMode);
	RegisterEventFunc(EVT_ChangeLeader,		&GroupMgr::OnChangeLeader);
	RegisterEventFunc(EVT_ChangeLevel,      &GroupMgr::OnChangeLevel);
	RegisterEventFunc(EVT_ChangeRein,		&GroupMgr::OnChangeLeaderRein);
	RegisterEventFunc(EVT_AddAllRoleBuff,	&GroupMgr::OnAddAllRoleBuff);
	RegisterEventFunc(EVT_CreateTeam,		&GroupMgr::OnCreateTeam);
	RegisterEventFunc(EVT_AddMember,		&GroupMgr::OnAddMember);

	//Jason 2009-11-25 申请入队相关协议处理注册
	RegisterEventFunc(EVT_ApplyToJoinTeam,		&GroupMgr::OnApplyJoinTeam);
	RegisterEventFunc(EVT_ApplyReply,		&GroupMgr::OnReplyForApplingToJoinTeam);
}

//----------------------------------------------------------------------------------------------------------
// 更新
//----------------------------------------------------------------------------------------------------------
VOID GroupMgr::Update()
{
	CHECK_TIME();
	EventMgr<GroupMgr>::Update();

	// 同步队员之间的位置
	UpdateTeamatePos();
}

//----------------------------------------------------------------------------------------------------------
// 销毁
//----------------------------------------------------------------------------------------------------------
VOID GroupMgr::Destroy()
{
	TMap<DWORD, Team*>::TMapIterator it = m_mapTeam.Begin();
	Team* pTeam = NULL;

	while( m_mapTeam.PeekNext(it, pTeam) )
	{
		SAFE_DEL(pTeam);
	}
	m_mapTeam.Clear();
}

//----------------------------------------------------------------------------------------------------------
// 同步每个队伍各个成员的位置
//----------------------------------------------------------------------------------------------------------
VOID GroupMgr::UpdateTeamatePos()
{
	DWORD dwTick = g_world.GetWorldTick();

	Team* pTeam = NULL;
	TMap<DWORD, Team*>::TMapIterator it = m_mapTeam.Begin();

	while( m_mapTeam.PeekNext(it, pTeam) )
	{
		if( dwTick - pTeam->GetTeamSynTick() > TEAM_SYN_TICK )
		{
			pTeam->UpdateTeamatePos();
			pTeam->SetSynTick(dwTick); 
		}
	}
}

//--------------------------------------------------------------------------------------------------------
// 创建队伍
//--------------------------------------------------------------------------------------------------------
Team* GroupMgr::CreateTeam(Role* pInviter, Role* pReplyer)
{
	Team* pTeam = new Team(m_dwTeamIndex, pInviter, pReplyer);
	m_mapTeam.Add(m_dwTeamIndex, pTeam);

	// 小队ID索引加一
	++m_dwTeamIndex;

	pTeam->OnCreate();

	return pTeam;
}

//----------------------------------------------------------------------------------------------------------
// 删除队伍
//----------------------------------------------------------------------------------------------------------
VOID GroupMgr::DeleteTeam(Team* pTeam)
{
	if( !P_VALID(pTeam) ) return;

	pTeam->OnDelete();

	m_mapTeam.Erase(pTeam->GetTeamID());
	SAFE_DEL(pTeam);
}

//---------------------------------------------------------------------------------------------------------
// 邀请玩家加入小队
//---------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnInviteJoinTeam(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagNC_InviteJoinTeam* pRecv = (tagNC_InviteJoinTeam*)pEventMessage;

	DWORD	dwSrcRoleID = dwSenderID;
	DWORD	dwDestRoleID = pRecv->dwDestRoleID;

	if( dwSrcRoleID == dwDestRoleID ) return;	// 邀请自己，外挂！

	Role* pSrcRole	=	g_roleMgr.GetRolePtrByID(dwSrcRoleID);
	Role* pDestRole	=	g_roleMgr.GetRolePtrByID(dwDestRoleID);

	if( !P_VALID(pSrcRole) )
		return;

	DWORD dwErrorCode = E_Success;


	// 判断是否在某地图内允许邀请组队
	Map* pMap = pSrcRole->GetMap();
	if(P_VALID(pMap))
		dwErrorCode = pMap->CanInviteJoinTeam();

	// 被邀请人不在线
	if( !P_VALID(pDestRole) )
	{
		dwErrorCode = E_Team_Target_Not_Online;
	}
	// Jason 2009-12-11 调整bug，要判断指针
	// 判断被邀请者的状态 是否可以接受组队请求
	else if(pDestRole->IsInStateCantBeInviteJoinTeam())
	{
		dwErrorCode = E_Team_Target_Not_Exit;
	}

	// Jason 2009-11-26 被邀请人处于死亡状态
	//else if( pDestRole->IsInState(ES_Dead) )
	//{
	//	dwErrorCode = E_Team_Target_Not_Exit;
	//}

	// 被邀请者有小队
	else if( GT_INVALID != pDestRole->GetTeamID() )
	{
		dwErrorCode = E_Team_Target_Have_Team;
	}

	// 查看被邀请者是不是正在被邀请
	else if( GT_INVALID != pDestRole->GetTeamInvite() )
	{
		dwErrorCode = E_Team_Target_Busy;
	}

	// 邀请者是不是有队伍
	else if( GT_INVALID != pSrcRole->GetTeamID() )
	{	
		Team* pTeam = m_mapTeam.Peek(pSrcRole->GetTeamID());
		if( !P_VALID(pTeam) ) return;

		// 是否为队长
		if( !pTeam->IsLeader(pSrcRole->GetID()) )
			dwErrorCode = E_Team_Not_Leader;

		// 小队人数是否已满
		if( pTeam->GetMemNum() >= MAX_TEAM_NUM )
			dwErrorCode = E_Team_Member_Full;
	}

	// 如果判断成功，则发送邀请
	if( E_Success == dwErrorCode )
	{
		pDestRole->SetTeamInvite(pSrcRole->GetID());

		tagNS_InviteJoinTeam send;
		send.dwLeaderID		=	dwSrcRoleID;
		send.dwDestRoleID	=	dwDestRoleID;
		pDestRole->SendMessage(&send, send.dwSize);
	}
	// 不成功，返回错误
	else
	{
		tagNS_InviteToLeader send;
		send.dwErrorCode = dwErrorCode;
		pSrcRole->SendMessage(&send, send.dwSize);
	}
}

//-------------------------------------------------------------------------------------------------------
// 玩家是否同意加入小队
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnInviteJoinTeamReply(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagNC_InviteReply* pRecv = (tagNC_InviteReply*)pEventMessage;

	DWORD dwReplyRoleID = dwSenderID;

	Role* pReplyRole = g_roleMgr.GetRolePtrByID(dwReplyRoleID);
	if( !P_VALID(pReplyRole) ) return;

	DWORD dwInviteRoleID = pReplyRole->GetTeamInvite();
	if( GT_INVALID == dwInviteRoleID ) return;

	pReplyRole->SetTeamInvite(GT_INVALID);	// 重置被邀请者的邀请人ID

	Role* pSrcRole = g_roleMgr.GetRolePtrByID(dwInviteRoleID);
	if( !P_VALID(pSrcRole) ) return;

	// 判断是否在某地图内允许邀请组队
	Map* pMap = pSrcRole->GetMap();
	if(P_VALID(pMap))
		if(pMap->CanInviteJoinTeam())
			return;

	// 开始做判断
	INT nRet = E_Success;
	BOOL bNewGroup = FALSE;
	Team* pTeam = NULL;

	// 玩家同意加入
	if( pRecv->bAgree )
	{
		// 检查自身是否有队
		if( GT_INVALID != pReplyRole->GetTeamID() )
		{
			nRet = E_Team_Target_Have_Team;
		}
		// 无队
		else
		{
			// 检查邀请者是否有队
			if( GT_INVALID != pSrcRole->GetTeamID() )
			{
				pTeam = m_mapTeam.Peek(pSrcRole->GetTeamID());
				if( !P_VALID(pTeam) ) return;

				nRet = pTeam->AddMember(pSrcRole, pReplyRole);
			}
			// 邀请者无队
			else
			{
				bNewGroup = TRUE;
				pTeam = CreateTeam(pSrcRole, pReplyRole);
			}
		}
	}

	// 如果对方同意，并且判断成功，则发送全体队伍消息
	if( E_Success == nRet && pRecv->bAgree && P_VALID(pTeam) )
	{
		// 发送给所有玩家邀请成功
		tagNS_InviteReply send;
		send.bAgree = pRecv->bAgree;
		send.dwErrorCode = nRet;
		pTeam->ExportAllMemID(send.dwTeamMemID);

		pTeam->SendTeamMessage(&send, send.dwSize);

		// 发送队员初始消息
		pTeam->SendRoleInitStateToTeam(pReplyRole);

		// 发送给新加入的玩家
		pTeam->SendTeamState(pReplyRole);

		// 如果新创建队伍，则设置队长的队伍信息
		if( bNewGroup ) pTeam->SendTeamState(pSrcRole);
	}
	// 如果不同意，或者判断失败，则只发送给双方
	else
	{
		tagNS_InviteReply send;
		send.dwTeamMemID[0] = dwInviteRoleID;
		send.dwTeamMemID[1] = dwReplyRoleID;
		send.bAgree = pRecv->bAgree;
		send.dwErrorCode = nRet;

		pSrcRole->SendMessage(&send, send.dwSize);
		pReplyRole->SendMessage(&send, send.dwSize);
	}
}


//-------------------------------------------------------------------------------------------------------
// 踢玩家出小队
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnLeaderKickMember(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagNC_KickMember* pRecv = (tagNC_KickMember*)pEventMessage;

	DWORD	dwSrcRoleID = pRecv->dwSrcRoleID;
	DWORD	dwDestRoleID = pRecv->dwDestRoleID;

	if( dwSrcRoleID == dwDestRoleID ) return;	// 不能踢出自己

	Role* pSrcRole = g_roleMgr.GetRolePtrByID(dwSrcRoleID);
	Role* pDestRole = g_roleMgr.GetRolePtrByID(dwDestRoleID);

	if( !P_VALID(pDestRole) || !P_VALID(pSrcRole) )
		return;

	// 找到队伍
	Team* pTeam = m_mapTeam.Peek(pSrcRole->GetTeamID());
	if( !P_VALID(pTeam) ) return;

	INT nRet = pTeam->KickMember(pSrcRole, pDestRole);

	if( E_Success == nRet )
	{
		// 如果踢人成功，发送所有玩家消息
		tagNS_KickMember send;
		send.dwDestRoleID = dwDestRoleID;
		send.dwErrorCode = nRet;
		pTeam->SendTeamMessage(&send, send.dwSize);
		pDestRole->SendMessage(&send, send.dwSize);

		// 如果队伍需要删除，则删除队伍
		if( pTeam->IsNeedDelete() )
		{
			DeleteTeam(pTeam);
		}
	}
	else
	{
		// 如果不成功，返回踢人者
		tagNS_KickMember send;
		send.dwDestRoleID = dwDestRoleID;
		send.dwErrorCode = nRet;
		pSrcRole->SendMessage(&send, send.dwSize);
	}
}

//-------------------------------------------------------------------------------------------------------
// 玩家离开小队
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnMemberLeaveTeam(DWORD dwSenderID, LPVOID pEventMessage)
{
	Role* pMember = g_roleMgr.GetRolePtrByID(dwSenderID);
	if( !P_VALID(pMember) ) return;

	// 找到队伍
	Team* pTeam = m_mapTeam.Peek(pMember->GetTeamID());
	if( !P_VALID(pTeam) ) return;

	INT nRet =E_Success; 

	nRet = pTeam->LeaveTeam(pMember);

	if( E_Success == nRet )
	{
		// 成功，发送所有队伍成员消息
		tagNS_LeaveTeam send;
		send.dwRoleID = pMember->GetID();
		send.dwLeaderID = pTeam->GetMemID(0);
		send.dwErrorCode = nRet;
		pTeam->SendTeamMessage(&send, send.dwSize);
		pMember->SendMessage(&send, send.dwSize);

		// 如果队伍要删除，则删除队伍
		if( pTeam->IsNeedDelete() )
		{
			DeleteTeam(pTeam);
		}
	}
	else
	{
		// 如果不成功，返回离开者
		tagNS_LeaveTeam send;
		send.dwRoleID = pMember->GetID();
		send.dwLeaderID = pTeam->GetMemID(0);
		send.dwErrorCode = nRet;
		pMember->SendMessage(&send, send.dwSize);
	}
}

//-------------------------------------------------------------------------------------------------------
// 设置小队拾取模式
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnSetPickMode(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagNC_SetPickMode* pRecv = (tagNC_SetPickMode*)pEventMessage;

	Role* pRole = g_roleMgr.GetRolePtrByID(dwSenderID);
	if( !P_VALID(pRole) ) return;

	// 找到队伍
	Team* pTeam = m_mapTeam.Peek(pRole->GetTeamID());
	if( !P_VALID(pTeam) ) return;

	INT nRet = pTeam->SetPickMode(pRole, pRecv->ePickMode);

	if( E_Success == nRet )
	{
		// 设置成功，发送所有队员消息
		tagNS_SetPickMode send;
		send.ePickMode = pRecv->ePickMode;
		send.dwErrorCode = nRet;
		pTeam->SendTeamMessage(&send, send.dwSize);
	}
	else
	{
		// 设置不成功，发送设置者消息
		tagNS_SetPickMode send;
		send.ePickMode = pRecv->ePickMode;
		send.dwErrorCode = nRet;
		pRole->SendMessage(&send, send.dwSize);

	}
}

//-------------------------------------------------------------------------------------------------------
// 改变小队队长
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnChangeLeader(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagNC_ChangeLeader* pRecv = (tagNC_ChangeLeader*)pEventMessage;

	Role* pSrc = g_roleMgr.GetRolePtrByID(dwSenderID);
	Role* pDest = g_roleMgr.GetRolePtrByID(pRecv->dwNewLeader);

	if( !P_VALID(pSrc) || !P_VALID(pDest) ) return;

	// 找到队伍
	Team* pTeam = m_mapTeam.Peek(pSrc->GetTeamID());
	if( !P_VALID(pTeam) ) return;

	INT nRet = E_Success;
	
	nRet = pTeam->ChangeLeader(pSrc, pDest);

	if( E_Success == nRet )
	{
		// 成功，发送队伍消息
		tagNS_ChangeLeader send;
		send.dwLeaderID = pSrc->GetID();
		send.dwNewLeaderID = pDest->GetID();
		send.dwErrorCode = nRet;
		pTeam->SendTeamMessage(&send, send.dwSize);
	}
	else
	{
		// 不成功，返回交换者
		tagNS_ChangeLeader send;
		send.dwLeaderID = pSrc->GetID();
		send.dwNewLeaderID = pDest->GetID();
		send.dwErrorCode = nRet;
		pSrc->SendMessage(&send, send.dwSize);
	}
}

//-------------------------------------------------------------------------------------------------------
// 发送消息给整个小队
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::SendTeamMessage(DWORD dwTeamID, LPVOID pMsg, DWORD dwSize)
{
	Team* pTeam = m_mapTeam.Peek(dwTeamID);
	if( !P_VALID(pTeam) ) return;

	pTeam->SendTeamMessage(pMsg, dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 发送消息给队友
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::SendTeamateMessage(DWORD dwTeamID, DWORD dwRole, LPVOID pMsg, DWORD dwSize)
{
	Team* pTeam = m_mapTeam.Peek(dwTeamID);
	if( !P_VALID(pTeam) ) return;

	pTeam->SendTeamateMessage(dwRole, pMsg, dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 给可视链表外的小队成员发消息
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::SendTeamMesOutBigVis(Role* pRole, DWORD dwTeamID, LPVOID pMsg, DWORD dwSize)
{
	if( !P_VALID(pRole) ) return;

	Team* pTeam = m_mapTeam.Peek(dwTeamID);
	if( !P_VALID(pTeam) ) return;

	pTeam->SendTeamMesOutBigVis(pRole, pMsg, dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 通知队友进入副本
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::SendTeamInstanceNotify(Role* pRole, DWORD dwTeamID, LPVOID pMsg, DWORD dwSize)
{
	if( !P_VALID(pRole) ) return;

	Team* pTeam = m_mapTeam.Peek(dwTeamID);
	if( !P_VALID(pTeam) ) return;

	pTeam->SendTeamInstanceNotigy(pRole, pMsg, dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 队员下线
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::RoleOutline(Role* pRole)
{
	if( !P_VALID(pRole) ) return;

	Team* pTeam = m_mapTeam.Peek(pRole->GetTeamID());
	if( !P_VALID(pTeam) ) return;

	INT nRet = pTeam->LeaveTeam(pRole);

	if( E_Success == nRet )
	{
		// 成功，发送所有队伍成员消息
		tagNS_LeaveTeam send;
		send.dwRoleID = pRole->GetID();
		send.dwLeaderID = pTeam->GetMemID(0);
		send.dwErrorCode = nRet;
		pTeam->SendTeamMessage(&send, send.dwSize);

		// 如果队伍要删除，则删除队伍
		if( pTeam->IsNeedDelete() )
		{
			DeleteTeam(pTeam);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 小队成员等级改变
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnChangeLevel(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagNS_RoleLevelChange* pRecv = (tagNS_RoleLevelChange*)pEventMessage;
	
	Role* pRole = g_roleMgr.GetRolePtrByID(dwSenderID);
	if( !P_VALID(pRole) ) return;

	// 找到队伍
	Team* pTeam = m_mapTeam.Peek(pRole->GetTeamID());
	if( !P_VALID(pTeam) ) return;

	// 同步队伍等级
	pTeam->OnMemLevelChange(pRole);

	// 同步给小队玩家
	pTeam->SendTeamMesOutBigVis(pRole, pRecv, pRecv->dwSize);	
}

//-------------------------------------------------------------------------------------------------------
// 小队队长统御力改变
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnChangeLeaderRein(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagLeaderRein* pLeaderRein = (tagLeaderRein*)pEventMessage;

	Role* pRole = g_roleMgr.GetRolePtrByID(dwSenderID);
	if( !P_VALID(pRole) ) return;

	Team* pTeam = m_mapTeam.Peek(pRole->GetTeamID());
	if( !P_VALID(pTeam) ) return;

	if( pTeam->IsLeader(pRole->GetID()) ) return;

	pTeam->CalExpFactor();
}

//-------------------------------------------------------------------------------------------------------
// 给服务器所有在线玩家加一个buff
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnAddAllRoleBuff(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagAllRoleBuff* pAllRoleBuff = (tagAllRoleBuff*)pEventMessage;

	g_roleMgr.AddAllRoleBuff(pAllRoleBuff->dwBuffTypeID);
}

//-------------------------------------------------------------------------------------------------------
// 创建一个小队
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnCreateTeam(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagCreateTeam* pCreateTeam = (tagCreateTeam*)pEventMessage;
	DWORD	dwSrcRoleID = pCreateTeam->dwSrcRoleID;
	DWORD	dwDesRoleID = pCreateTeam->dwDesRoleID;

	Role* pSrcRole = g_roleMgr.GetRolePtrByID(dwSrcRoleID);
	if( !P_VALID(pSrcRole) ) return;

	Role* pDesRole = g_roleMgr.GetRolePtrByID(dwDesRoleID);
	if( !P_VALID(pDesRole) ) return;

	// 检查是否有队
	if( GT_INVALID != pSrcRole->GetTeamID() || GT_INVALID != pDesRole->GetTeamID() )
	{
		return;
	}

	Team* pTeam = CreateTeam(pSrcRole, pDesRole);

	// 如果对方同意，并且判断成功，则发送全体队伍消息
	if( P_VALID(pTeam) )
	{
		// 发送给所有玩家邀请成功
		tagNS_InviteReply send;
		send.bAgree = TRUE;
		send.dwErrorCode = E_Success;
		pTeam->ExportAllMemID(send.dwTeamMemID);

		pTeam->SendTeamMessage(&send, send.dwSize);

		// 发送队员初始消息
		pTeam->SendRoleInitStateToTeam(pDesRole);

		// 发送给新加入的玩家
		pTeam->SendTeamState(pDesRole);

		// 如果新创建队伍，则设置队长的队伍信息
		pTeam->SendTeamState(pSrcRole);
	}
}

//-------------------------------------------------------------------------------------------------------
// 增加一个小队队员
//-------------------------------------------------------------------------------------------------------
VOID GroupMgr::OnAddMember(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagAddTeamMember* pCreateTeam = (tagAddTeamMember*)pEventMessage;
	DWORD	dwTeamID = pCreateTeam->dwTeamID;
	DWORD	dwDesRoleID = pCreateTeam->dwDesRoleID;

	Role* pDesRole = g_roleMgr.GetRolePtrByID(dwDesRoleID);
	if( !P_VALID(pDesRole) ) return;

	Team* pTeam = m_mapTeam.Peek(dwTeamID);
	if( !P_VALID(pTeam) ) return;

	Role* pSrcRole =  pTeam->GetMemPtr(0);
	if( !P_VALID(pSrcRole) ) return;

	pTeam->AddMember(pSrcRole, pDesRole);

	// 发送给所有玩家邀请成功
	tagNS_InviteReply send;
	send.bAgree = TRUE;
	send.dwErrorCode = E_Success;
	pTeam->ExportAllMemID(send.dwTeamMemID);

	pTeam->SendTeamMessage(&send, send.dwSize);

	// 发送队员初始消息
	pTeam->SendRoleInitStateToTeam(pDesRole);

	// 发送给新加入的玩家
	pTeam->SendTeamState(pDesRole);
}


// Jason
VOID GroupMgr::NotifyApplyToJoinTeam(DWORD RoleID, DWORD errcode,DWORD dwTeamLeaderID,DWORD dwApplicantID,BOOL bTeamLeaderAgree)
{
	Role* pSender	=	g_roleMgr.GetRolePtrByID(RoleID);
	if( !P_VALID(pSender) )
		return;
	tagNS_ApplyReplyRefuse msg;
	//msg.dwErrorCode		= errcode;
	//if( E_Team_Target_No_Team == errcode )
	//	msg.dwTeamLeaderID	= RoleID;
	//else
		msg.dwTeamLeaderID	= dwTeamLeaderID;
	//msg.dwApplicantID	= dwApplicantID;
	//msg.bTeamLeaderAgree= bTeamLeaderAgree;
	if( GT_VALID(dwTeamLeaderID) )
		g_roleMgr.GetRoleNameByID(dwTeamLeaderID, msg.szName);
	else
		memset(msg.szName,0,sizeof(TCHAR) * X_SHORT_NAME);
	msg.dwErrCode = errcode;
	pSender->SendMessage(&msg, msg.dwSize);
}

// Jason modified from method " OnInviteJoinTeam "
VOID GroupMgr::OnApplyJoinTeam(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagNC_ApplyToJoinTeam* pRecv = (tagNC_ApplyToJoinTeam*)pEventMessage;
	//协议问题，不要处理
	if( !P_VALID(pRecv) )
		return;

	DWORD	dwApplicantID	= dwSenderID;//pRecv->dwApplicantID;
	DWORD	dwTeamMemberID	= pRecv->dwTeamMemberID;

	if( dwApplicantID == dwTeamMemberID )
	{
		NotifyApplyToJoinTeam(dwApplicantID,E_Team_Hack_Tools_Prohibited);
		return;	// 邀请自己，外挂！
	}
	//申请人
	Role* pApplicant	=	g_roleMgr.GetRolePtrByID(dwApplicantID);
	//队成员
	Role* pTeamMember	=	g_roleMgr.GetRolePtrByID(dwTeamMemberID);

	if( !P_VALID(pApplicant) )
	{
		NotifyApplyToJoinTeam(dwApplicantID,E_Team_Apply_Invalid_Param);
		return;
	}
	if( !P_VALID(pTeamMember) )
	{
		NotifyApplyToJoinTeam  (dwApplicantID,E_Team_Peer_Not_Online,dwTeamMemberID);
		return;
	}

	Map* pMap = pApplicant->GetMap();
	if( !P_VALID(pMap) )
	{
		NotifyApplyToJoinTeam(dwApplicantID,E_Team_Cannot_Find_Map);
		return ;
	}
	// 判断是否在某地图内允许邀请组队
	if( E_Success != pMap->CanInviteJoinTeam() )
	{
		NotifyApplyToJoinTeam(dwApplicantID,E_Team_Map_CanNot_Invite);
		return ;
	}

	// 申请人有小队，则禁止
	DWORD dwTeamIDOfApplicant = 0, dwTeamIDOfDest = 0;
	dwTeamIDOfApplicant = pApplicant->GetTeamID();
	dwTeamIDOfDest		= pTeamMember->GetTeamID();
	if( GT_INVALID != dwTeamIDOfApplicant )
	{
		if( dwTeamIDOfApplicant == dwTeamIDOfDest )
			NotifyApplyToJoinTeam( dwApplicantID, E_Team_Already_In_Team );
		else
			NotifyApplyToJoinTeam( dwApplicantID, E_Team_Already_Have_Team );
		return ;
	}
	if( GT_INVALID == dwTeamIDOfDest )
	{
		//NotifyApplyToJoinTeam( dwApplicantID, E_Team_Target_No_Team, dwTeamMemberID );
		// 允许被申请方没有队。
		tagNS_ApplyToJoinTeam msg;
		msg.dwApplicantID = dwApplicantID;
		Role* pTeamLeader = NULL;
		if( GT_VALID(dwApplicantID) )
			g_roleMgr.GetRoleNameByID(dwApplicantID, msg.szName);
		else
			memset(msg.szName,0,sizeof(TCHAR) * X_SHORT_NAME);
		msg.nLevel = pApplicant->GetLevel  ();

		pTeamMember->SendMessage  (&msg,msg.dwSize);

		return ;
	}

	Team* pTeam = m_mapTeam.Peek( dwTeamIDOfDest );

	if( !P_VALID(pTeam) )
	{
		NotifyApplyToJoinTeam( dwApplicantID, E_Team_Target_No_Team );
		return ;
	}
	//队满？
	if( pTeam->GetMemNum() >= MAX_TEAM_NUM )
	{
		NotifyApplyToJoinTeam( dwApplicantID, E_Team_Member_Full );
		return ;
	}

	//得到队长id
	DWORD dwLeaderID = pTeam->GetMemID(0);

	tagNS_ApplyToJoinTeam msg;
	msg.dwApplicantID = dwApplicantID;
	Role* pTeamLeader = NULL;
	if( GT_VALID(dwApplicantID) )
		g_roleMgr.GetRoleNameByID(dwApplicantID, msg.szName);
	else
		memset(msg.szName,0,sizeof(TCHAR) * X_SHORT_NAME);

	//if( GT_VALID(dwApplicantID) )
	//{
	//	msg.nLevel = g_roleMgr.GetRolePtrByID(dwApplicantID)->GetLevel();
	//}

	if( dwLeaderID == dwTeamMemberID )//该人即为队长
	{
		pTeamLeader = pTeamMember ;
	}
	else//普通队员
	{
		//找到队长
		pTeamLeader = g_roleMgr.GetRolePtrByID( dwLeaderID );
	}

	//转发消息
	if( P_VALID(pTeamLeader) )
	{
		msg.nLevel = pApplicant->GetLevel  ();
		pTeamLeader->SendMessage(&msg,msg.dwSize);
	}

}
// Jason modified from method " OnInviteJoinTeamReply "
VOID GroupMgr::OnReplyForApplingToJoinTeam(DWORD dwSenderID, LPVOID pEventMessage)
{
	tagNC_ApplyReply * pRecv = (tagNC_ApplyReply*)pEventMessage;
	if( !P_VALID(pRecv) )
		return ;

	DWORD	dwApplicantID	= pRecv->dwApplicantID;
	DWORD dwLeaderRoleID = /*pRecv->dwTeamLeaderID*/ dwSenderID ;
	//if( dwReplyRoleID != dwSenderID )
	//	return;

	Role* pLeaderRole = g_roleMgr.GetRolePtrByID(dwLeaderRoleID);
	Role* pApplicant	=	g_roleMgr.GetRolePtrByID(dwApplicantID);

	if( !P_VALID(pLeaderRole) || !P_VALID(pApplicant) ) 
		return;

	//DWORD dwInviteRoleID = pReplyRole->GetTeamInvite();
	//if( GT_INVALID == dwInviteRoleID ) return;

	//pReplyRole->SetTeamInvite(GT_INVALID);	// 重置被邀请者的邀请人ID

	//Role* pSrcRole = g_roleMgr.GetRolePtrByID(dwInviteRoleID);
	//if( !P_VALID(pSrcRole) ) return;

	// 申请人应该没有队，防止外挂吧，正常是不会有这个必要处理的
	// 队长应该有队
	DWORD dwLeaderTeamID = pLeaderRole->GetTeamID();
	if( GT_INVALID != pApplicant->GetTeamID() /*|| GT_INVALID == dwLeaderTeamID*/ )
		return;

	// 判断是否在某地图内允许邀请组队
	Map* pMap = pLeaderRole->GetMap();
	if( !P_VALID(pMap) )
		return ;
	if( E_Success != pMap->CanInviteJoinTeam() )
		return;

	// 开始做判断
	INT nRet = -1;
	BOOL bNewGroup = FALSE;
	Team* pTeam = NULL;


	// 队长同意玩家加入
	if( pRecv->bTeamLeaderAgree )
	{
		if(GT_INVALID == dwLeaderTeamID)
		{
			//创建队
			pTeam = CreateTeam(pLeaderRole, pApplicant);
			nRet = E_Success;
			bNewGroup = TRUE;
		}
		else
		{
			pTeam = m_mapTeam.Peek( dwLeaderTeamID );
			// addmember会做一些条件判断的。比如人数满等
			nRet = pTeam->AddMember( pLeaderRole, pApplicant );
		}
	}

	// 如果队长同意，并且判断成功，则发送全体队伍消息
	if( E_Success == nRet )
	{
		if( !P_VALID(pTeam) ) 
			return;

		// 发送给所有玩家邀请成功
		tagNS_InviteReply send;
		send.bAgree = TRUE;
		send.dwErrorCode = nRet;
		pTeam->ExportAllMemID(send.dwTeamMemID);

		pTeam->SendTeamMessage(&send, send.dwSize);

		// 发送新队员初始消息
		pTeam->SendRoleInitStateToTeam( pApplicant );

		// 发送给新加入的玩家
		pTeam->SendTeamState( pApplicant );

		// 如果新创建队伍，则设置队长的队伍信息
		if( bNewGroup ) pTeam->SendTeamState(pLeaderRole);
	}
	// 如果不同意，或者判断失败，则只发送给申请方
	else
	{
		//tagNS_ApplyReplyRefuse msg;
		//msg.dwTeamLeaderID = dwLeaderRoleID;
		//msg.dwApplicantID = dwApplicantID;
		//msg.bTeamLeaderAgree = FALSE;
		//if (GT_VALID(dwLeaderRoleID))
		//	g_roleMgr.GetRoleNameByNameID(dwLeaderRoleID, msg.szName);
		//else
		//	memset(msg.szName,0,sizeof(TCHAR) * X_SHORT_NAME);
		//pApplicant->SendMessage(&msg, msg.dwSize);
		NotifyApplyToJoinTeam(dwApplicantID,E_Team_Refuse,dwLeaderRoleID);
	}
}


GroupMgr g_groupMgr;