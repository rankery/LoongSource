//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild.h
// author: Sxg
// actor:
// data: 2009-4-7
// last:
// brief: 帮派
//-------------------------------------------------------------------------------
#pragma once

#include "state_mgr.h"
#include "guild_pos.h"
#include "guild_member.h"
#include "guild_warehouse.h"
#include "guild_upgrade.h"
#include "guild_affair.h"
#include "guild_skill.h"
#include "guild_commerce.h"
#include "../ServerDefine/guild_define.h"
#include "../WorldDefine/msg_guild.h"

class Role;
struct tagInvite;
struct tagGuildLoad;
//-------------------------------------------------------------------------------
class Guild
{
public:
	Guild();
	Guild(const tagGuildLoad *pGuildLoad);
	~Guild();

	VOID	InitDBGuildMember(const tagGuildMember& sGuildMember);
	VOID	InitCreate(Role* pCreator, const tstring& strGuildName, DWORD dwGuildID, const tagGuildCfg &sGuildCfg);
	VOID	Update();

	BOOL	IsGuildInitOK();

public:
	DWORD	DismissGuild(Role* pRole);

	DWORD	CanInviteJoin(DWORD dwInviterID, DWORD dwInviteeID, Role **ppInvitee, BOOL bInsert = TRUE);
	DWORD	InviteJoin(DWORD dwInviterID, DWORD dwInviteeID);
	BOOL	EraseInviteJoin(DWORD dwInviteeID);
	DWORD	Appoint(DWORD dwAppointorID, DWORD dwAppointeeID, EGuildMemberPos eGuildPos);
	DWORD	KickMember(DWORD dwRoleID, DWORD dwKickID);
	DWORD	TurnoverLeader(DWORD dwOldLeaderID, DWORD dwNewLeaderID);
	DWORD	ModifyTenet(DWORD dwRoleID, LPCTSTR strNewTenet, INT32 nStrLen);
	DWORD	ModifySgin(DWORD dwRoleID, LPCTSTR strNewURL, INT32 nStrLen);
	DWORD	ModifyGroupPurchase(BOOL bInc);
	DWORD	DemissPostion(DWORD dwRoleID, OUT INT8 &n8OldGuildPos);
	DWORD	LeaveGuild(Role* pRole);

	// 获取操作
	VOID	SendAllMembers2Client(Role *p2Role);
	DWORD	SendSpecMemberEx2Client(Role *p2Role, DWORD dwSpecRoleID);
	DWORD	SendSpecMember2Client(Role *p2Role, DWORD dwSpecRoleID);
	DWORD	SendGuildName2Client(PlayerSession *pSession);
	DWORD	SendGuildTenet2Client(PlayerSession *pSession);
	DWORD	SendFacilitiesInfo2Client(Role* p2Role);

	// 删除角色时清理相关数据
	DWORD	ClearRoleRemove(DWORD dwRoleID);

public:
	// 帮派信息获得
	const tagGuild& GetGuildAtt() const;
	GuildWarehouse& GetGuildWarehouse()		{ return m_GuildWarehouse; }
	GuildUpgrade&	GetGuildFacilities()	{ return m_GuildUpgrade; }
	GuildAffair&	GetGuildAffair()		{ return m_GuildAffair; }
	GuildSkill&		GetGuildSkill()			{ return m_GuildSkill; }
	GuildCommerce&	GetGuildCommerce()		{ return m_GuildCommerce; }

	BOOL	IsHoldCity(BYTE byCityIndex);
	VOID	SendGuildMsg(LPVOID pMsg, DWORD dwSize);
	
	template<typename UnitOperation>
	VOID ForEachRoleInGuild( UnitOperation oper );
public:
	// 帮派仓库相关
	DWORD	SetGuildWarePrivilege(Role* pRole, DWORD dwRoleID, BOOL bCanUse);

public:
	// 帮派升级
	VOID	ReInitbyGuildUpgrade(BYTE byNewLevel = 0);

public:
	// 帮派状态相关
	DWORD GetGuildState() const { return m_GuildState.GetState(); }

	BOOL IsInGuildState(EGuildSpecState eState) const
	{
		return m_GuildState.IsInState(eState);
	}

	BOOL IsInGuildStateAll(DWORD dwState) const
	{
		return m_GuildState.IsInStateAll(dwState);
	}

	BOOL IsInGuildStateAny(DWORD dwState) const
	{
		return m_GuildState.IsInStateAny(dwState);
	}

	VOID SetGuildState(EGuildSpecState eState, BOOL bSendMsg=TRUE)
	{
		if( TRUE == IsInGuildState(eState) ) return;

		m_GuildState.SetState(eState);
		m_Att.dwSpecState = m_GuildState.GetState();
		SaveGuildAtt2DB();

		if( bSendMsg )
		{
			tagNS_GuildStateSet send;
			send.eState = eState;
			SendGuildMsg(&send, send.dwSize);
		}
	}

	VOID UnsetGuildState(EGuildSpecState eState, BOOL bSendMsg=TRUE)
	{
		if( FALSE == IsInGuildState(eState) ) return;

		m_GuildState.UnsetState(eState);
		m_Att.dwSpecState = m_GuildState.GetState();
		SaveGuildAtt2DB();

		if( bSendMsg )
		{
			tagNS_GuildStateUnset send;
			send.eState = eState;
			SendGuildMsg(&send, send.dwSize);
		}
	}

public:
	// 帮派属性操作
	BOOL	IncGuildFund(DWORD dwRoleID, INT32 nFund, DWORD dwCmdID);
	BOOL	IncGuildMaterial(DWORD dwRoleID, INT32 nMaterial, DWORD dwCmdID);
	BOOL	IncGuildReputation(DWORD dwRoleID, INT32 nReputation, DWORD dwCmdID);
	BOOL	IncGuildPeace(DWORD dwRoleID, INT16 nPeace, DWORD dwCmdID);

	BOOL	DecGuildFund(DWORD dwRoleID, INT32 nFund, DWORD dwCmdID);
	BOOL	DecGuildMaterial(DWORD dwRoleID, INT32 nMaterial, DWORD dwCmdID);
	BOOL	DecGuildReputation(DWORD dwRoleID, INT32 nReputation, DWORD dwCmdID);
	BOOL	DecGuildPeace(DWORD dwRoleID, INT16 nPeace, DWORD dwCmdID);

	VOID	ReCalAffairRemainTimes(BYTE bySpreadTimes);

	// 记录帮派资金LOG
	VOID LogGuildFund(DWORD dwRoleID, INT32 nFund, INT32 nTotalFund, DWORD dwCmdID);
	VOID LogGuildMaterial(DWORD dwRoleID, INT32 nMaterial, INT32 nTotalMaterial, DWORD dwCmdID);
	VOID LogGuildReputation(DWORD dwRoleID, INT32 nReputation, INT32 nTotalReputation, DWORD dwCmdID);

	// 每天2:00扣除消耗并重置帮务状态
	VOID DailyGuildReset();

public:
	// 成员属性操作
	tagGuildMember*	GetMember(DWORD dwRoleID);
	INT32			GetGuildMemberNum();
	MapGuildMember&	GetGuildMemberMap();
	VOID			ChangeMemberContribution(DWORD dwRoleID, INT32 nContribution, BOOL bInc, BOOL bSave2DB = TRUE);
	VOID			ChangeMemberExploit(DWORD dwRoleID, INT32 n32Exploit, BOOL bSave2DB = TRUE);

public:
	// 帮派设施相关
	VOID	ChangeFacilityLevel(EFacilitiesType eType, BYTE byValue);

private:
	// 管理对象方法再封装
	DWORD	AddMember(DWORD dwRoleID, EGuildMemberPos eGuildPos, BOOL bSave2DB = TRUE);
	DWORD	LoadMember(const tagGuildMember& sGuildMember);
	VOID	RemoveMember(DWORD dwRoleID, EGuildMemberPos eGuildPos);
	DWORD	ChangeGuildPos(tagGuildMember *pMember, EGuildMemberPos eNewGuildPos);
	VOID	SetGuildBangzhu(DWORD dwNewLeaderID);

	// 保存帮派属性到数据库
	VOID	SaveGuildAtt2DB();

public:
	// 操作权限
	const tagGuildPower&	GetGuildPower(EGuildMemberPos eGuildPos) const;
	const tagGuildKick&		GetGuildKick(EGuildMemberPos eGuildPos) const;
	const tagGuildAppoint&	GetGuildAppoint(EGuildMemberPos eGuildPos) const;

private:
	typedef State<DWORD, EGuildSpecState>	GuildState;

	//-------------------------------------------------------------------------------
	// Null Obj
	//-------------------------------------------------------------------------------
	static const tagGuildPower		m_NullGuildPower;
	static const tagGuildKick		m_NullGuildKick;
	static const tagGuildAppoint	m_NullGuildAppoint;

	//-------------------------------------------------------------------------------
	// 帮派属性
	//-------------------------------------------------------------------------------
	tagGuild				m_Att;
	//tagGuildPower			m_Power[X_GUILD_POS_NUM];

	//-------------------------------------------------------------------------------
	// 邀请帖
	//-------------------------------------------------------------------------------
	typedef TMap<DWORD, tagInvite*>	MapInvite;	// <dwInviteeID>
	MapInvite				m_mapInvite;

	//-------------------------------------------------------------------------------
	// 相关管理器
	//-------------------------------------------------------------------------------
	GuildMember				m_MemberMgr;
	GuildPos				m_GuildPos;

	//-------------------------------------------------------------------------------
	// 帮派仓库
	//-------------------------------------------------------------------------------
	GuildWarehouse			m_GuildWarehouse;

	//-------------------------------------------------------------------------------
	// 帮派状态
	//-------------------------------------------------------------------------------
	GuildState				m_GuildState;

	//-------------------------------------------------------------------------------
	// 帮派设施
	//-------------------------------------------------------------------------------
	GuildUpgrade			m_GuildUpgrade;

	//-------------------------------------------------------------------------------
	// 帮派事务
	//-------------------------------------------------------------------------------
	GuildAffair				m_GuildAffair;

	//-------------------------------------------------------------------------------
	// 帮派技能
	//-------------------------------------------------------------------------------
	GuildSkill				m_GuildSkill;

	//-------------------------------------------------------------------------------
	// 帮派跑商
	//-------------------------------------------------------------------------------
	GuildCommerce			m_GuildCommerce;
};

/*************************** 内联实现 *******************************/

//-------------------------------------------------------------------------------
// 获取帮派属性
//-------------------------------------------------------------------------------
inline const tagGuild& Guild::GetGuildAtt() const
{
	return m_Att;
}

//-------------------------------------------------------------------------------
// 帮派内广播
//-------------------------------------------------------------------------------
inline VOID Guild::SendGuildMsg(LPVOID pMsg, DWORD dwSize)
{
	m_MemberMgr.SendGuildMsg(pMsg, dwSize);
}

//-------------------------------------------------------------------------------
// 添加成员
//-------------------------------------------------------------------------------
inline DWORD Guild::AddMember(DWORD dwRoleID, EGuildMemberPos eGuildPos, BOOL bSave2DB/* = TRUE*/)
{
	if(m_MemberMgr.IsExist(dwRoleID))
	{
		return E_Success;
	}
	
	DWORD dwErrorCode = m_GuildPos.AddMember(dwRoleID, eGuildPos);
	if(E_Success == dwErrorCode)
	{
		m_MemberMgr.AddMember(dwRoleID, eGuildPos, bSave2DB);
		m_Att.n16MemberNum = GetGuildMemberNum();
	}
	
	ASSERT(E_Success == dwErrorCode);
	return dwErrorCode;
}

inline DWORD Guild::LoadMember(const tagGuildMember& sGuildMember)
{
	if(m_MemberMgr.IsExist(sGuildMember.dwRoleID))
	{
		return E_Success;
	}

	DWORD dwErrorCode = m_GuildPos.AddMember(sGuildMember.dwRoleID, sGuildMember.eGuildPos);
	if(E_Success == dwErrorCode)
	{
		m_MemberMgr.LoadMember(sGuildMember);
		m_Att.n16MemberNum = GetGuildMemberNum();
	}

	return dwErrorCode;
}

//-------------------------------------------------------------------------------
// 删除成员
//-------------------------------------------------------------------------------
inline VOID Guild::RemoveMember(DWORD dwRoleID, EGuildMemberPos eGuildPos)
{
	m_GuildPos.RemoveMember(dwRoleID, eGuildPos);
	// 职位变更导致其他属性变化
	m_GuildCommerce.AbandonCommerce(dwRoleID, TRUE);

	m_MemberMgr.RemoveMember(dwRoleID);
	m_Att.n16MemberNum = GetGuildMemberNum();
}

//-------------------------------------------------------------------------------
// 成员职位变化
//-------------------------------------------------------------------------------
inline DWORD Guild::ChangeGuildPos(tagGuildMember *pMember, EGuildMemberPos eNewGuildPos)
{
	DWORD dwErrorCode = m_GuildPos.ChangePos(pMember->dwRoleID, pMember->eGuildPos, eNewGuildPos);
	if(E_Success == dwErrorCode)
	{
		// 职位变更导致其他属性变化
		if (!GetGuildPower(eNewGuildPos).bCommerce)
		{
			m_GuildCommerce.AbandonCommerce(pMember->dwRoleID);
		}
		// 职位确定
		m_MemberMgr.SetGuildPos(pMember, eNewGuildPos, TRUE);
	}
	
	ASSERT(E_Success == dwErrorCode);

	return dwErrorCode;
}

//-------------------------------------------------------------------------------
// 获取成员属性
//-------------------------------------------------------------------------------
inline tagGuildMember* Guild::GetMember(DWORD dwRoleID)
{
	return m_MemberMgr.GetMember(dwRoleID);
}

//-------------------------------------------------------------------------------
// 获取成员属性
//-------------------------------------------------------------------------------
inline INT32 Guild::GetGuildMemberNum()
{
	ASSERT(m_MemberMgr.GetNumber() == m_GuildPos.GetNumber());
	return m_GuildPos.GetNumber();
}

//-------------------------------------------------------------------------------
// 获取成员属性表
//-------------------------------------------------------------------------------
inline MapGuildMember& Guild::GetGuildMemberMap()
{
	return m_MemberMgr.GetMemberMap();
}

//-------------------------------------------------------------------------------
// 获取所有成员基本信息
//-------------------------------------------------------------------------------
inline VOID Guild::SendAllMembers2Client(Role *p2Role)
{
	ASSERT(P_VALID(p2Role));

	ASSERT(m_MemberMgr.GetNumber() == m_GuildPos.GetNumber());

	if(m_GuildPos.GetNumber() == 1)
	{
		// 只有一个成员，应该是本人
		return;
	}

	m_MemberMgr.SendAllMembers2Client(p2Role);
}

//-------------------------------------------------------------------------------
// 设置帮主
//-------------------------------------------------------------------------------
inline VOID Guild::SetGuildBangzhu(DWORD dwNewLeaderID)
{
	m_Att.dwLeaderRoleID = dwNewLeaderID;

	// 保存到数据库
	SaveGuildAtt2DB();
}

//-------------------------------------------------------------------------------
// 成员贡献变化
//-------------------------------------------------------------------------------
inline VOID Guild::ChangeMemberContribution( DWORD dwRoleID, INT32 nContribution, BOOL bInc, BOOL bSave2DB /*= TRUE*/ )
{
	if (bInc)
	{
		m_MemberMgr.IncMemberContribution(dwRoleID, nContribution, bSave2DB);
	}
	else
	{
		m_MemberMgr.DecMemberContribution(dwRoleID, nContribution, bSave2DB);
	}
}

inline VOID Guild::ChangeMemberExploit(DWORD dwRoleID, INT32 n32Exploit, BOOL bSave2DB /* = TRUE */)
{
	m_MemberMgr.SetMemberExploit(dwRoleID, n32Exploit, bSave2DB);
}

inline VOID Guild::ChangeFacilityLevel(EFacilitiesType eType, BYTE byValue)
{
	m_GuildUpgrade.ChangeFacilitiesLevel(eType, byValue);
}

template<typename UnitOperation>
VOID Guild::ForEachRoleInGuild( UnitOperation oper )
{
	MapGuildMember& map = GetGuildMemberMap();

	tagGuildMember* pMember = NULL;
	MapGuildMember::TMapIterator iter = map.Begin();
	while(map.PeekNext(iter, pMember))
	{
		Role *pRole = g_roleMgr.GetRolePtrByID(pMember->dwRoleID);
		if(P_VALID(pRole))
		{
			oper(pRole);
		}
	}
}