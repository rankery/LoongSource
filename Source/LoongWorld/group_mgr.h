//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: group_mgr.h
// author: wjiang
// actor:
// data: 2009-1-04
// last:
// brief: 团队管理类
//-------------------------------------------------------------------------------------------------------
#pragma once
#include "event_mgr.h"
#include "../WorldDefine/SocialDef.h"
#include "../WorldDefine/ItemDefine.h"

class Team;

class GroupMgr : public EventMgr<GroupMgr>
{
public:
	//---------------------------------------------------------------------------------------------------
	// CONSTRUCT
	//---------------------------------------------------------------------------------------------------
	~GroupMgr();

	//---------------------------------------------------------------------------------------------------
	// 初始化、更新及销毁
	//---------------------------------------------------------------------------------------------------
	BOOL	Init();
	VOID	Update();
	VOID	Destroy();

	//---------------------------------------------------------------------------------------------------
	// 小队相关
	//---------------------------------------------------------------------------------------------------
	VOID	OnInviteJoinTeam(DWORD dwSenderID, LPVOID pEventMessage);
	VOID	OnInviteJoinTeamReply(DWORD dwSenderID, LPVOID pEventMessage);
	VOID	OnLeaderKickMember(DWORD dwSenderID, LPVOID pEventMessage);
	VOID	OnMemberLeaveTeam(DWORD dwSenderID, LPVOID pEventMessage);
	VOID	OnSetPickMode(DWORD dwSenderID, LPVOID pEventMessage);
	VOID	OnChangeLeader(DWORD dwSenderID, LPVOID pEventMessage);
	VOID	OnChangeLevel(DWORD dwSenderID, LPVOID pEventMessage);
	VOID	OnChangeLeaderRein(DWORD dwSenderID, LPVOID pEventMessage);

	/*
	Jason 2009-11-25 申请入队相关协议处理
	*/
	// 反馈给入队申请者，入队请求情况
	VOID NotifyApplyToJoinTeam(DWORD RoleID, DWORD errcode, DWORD dwTeamLeaderID = 0, DWORD dwApplicantID = 0, BOOL bTeamLeaderAgree = FALSE);
	// 处理入队申请协议的处理函数
	VOID OnApplyJoinTeam(DWORD dwSenderID, LPVOID pEventMessage);
	// 处理队长反馈入队申请协议
	VOID OnReplyForApplingToJoinTeam(DWORD dwSenderID, LPVOID pEventMessage);


	VOID    SendTeamInstanceNotify(Role* pRole, DWORD dwTeamID, LPVOID pMsg, DWORD dwSize);
	VOID	SendTeamMessage(DWORD dwTeamID, LPVOID pMsg, DWORD dwSize);
	VOID	SendTeamMesOutBigVis(Role *pRole, DWORD dwTeamID, LPVOID pMsg, DWORD dwSize);
	VOID	SendTeamateMessage(DWORD dwTeamID, DWORD dwRole, LPVOID pMsg, DWORD dwSize);
	VOID	RoleOutline(Role* pRole);

	const Team* GetTeamPtr(DWORD dwTeamID)	{ return m_mapTeam.Peek(dwTeamID); }

	//---------------------------------------------------------------------------------------------------
	// 给服务器所有在线玩家加一个buff
	//---------------------------------------------------------------------------------------------------
	VOID	OnAddAllRoleBuff(DWORD dwSenderID, LPVOID pEventMessage);

	//---------------------------------------------------------------------------------------------------
	// 脚本触发小队相关事件
	//---------------------------------------------------------------------------------------------------
	VOID	OnCreateTeam(DWORD dwSenderID, LPVOID pEventMessage);
	VOID	OnAddMember(DWORD dwSenderID, LPVOID pEventMessage);

private:
	static VOID	RegisterGroupEventFunc();
	VOID		UpdateTeamatePos();

	Team*		CreateTeam(Role* pInviter, Role* pReplyer);
	VOID		DeleteTeam(Team* pTeam);

private:
	TMap<DWORD, Team*>			m_mapTeam;
	DWORD						m_dwTeamIndex;			// 小队ID索引
};

extern GroupMgr g_groupMgr;