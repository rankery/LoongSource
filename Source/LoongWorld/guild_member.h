//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_member.h
// author: Sxg
// actor:
// data: 2009-4-7
// last:
// brief: 帮派成员
//-------------------------------------------------------------------------------
#pragma once

struct	tagGuildMember;
enum	EGuildMemberPos;
//-------------------------------------------------------------------------------
typedef TMap<DWORD, tagGuildMember*>		MapGuildMember;

class GuildMember
{
public:
	GuildMember();
	~GuildMember();

	VOID	Init(DWORD dwGuildID);

	VOID	AddMember(DWORD dwInviteeID, EGuildMemberPos ePos, BOOL bSave2DB = TRUE);
	VOID	LoadMember(const tagGuildMember& sGuildMember);
	VOID	RemoveMember(DWORD dwRoleID, BOOL bSave2DB = TRUE);

	INT32	GetNumber();
	MapGuildMember& GetMemberMap();
	
	VOID	SendGuildMsg(LPVOID pMsg, DWORD dwSize);
	VOID	SendAllMembers2Client(Role *p2Role);

	tagGuildMember* GetMember(DWORD dwRoleID);

	BOOL	IsExist(DWORD dwRoleID);

public:
	// 成员属性相关
	VOID	SetGuildPos(tagGuildMember *pMember, EGuildMemberPos ePos, BOOL bSave2DB = TRUE);
	VOID	SetGuildWarePrivilege(DWORD dwRoleID, BOOL bCanUse, BOOL bSave2DB = TRUE);
	VOID	IncMemberContribution(DWORD dwRoleID, INT32 nContribution, BOOL bSave2DB = TRUE);
	VOID	DecMemberContribution(DWORD dwRoleID, INT32 nContribution, BOOL bSave2DB = TRUE);
	VOID	SetMemberExploit(DWORD dwRoleID, INT32 nExploit, BOOL bSave2DB = TRUE);

private:
	MapGuildMember			m_mapMember;

	// 辅助属性
	DWORD					m_dwGuildID;
};

/**************************** 内联实现 ********************************/

inline VOID GuildMember::Init(DWORD dwGuildID)
{
	ASSERT(m_mapMember.Empty());

	m_dwGuildID = dwGuildID;
}

//-------------------------------------------------------------------------------
// 获取成员
//-------------------------------------------------------------------------------
inline tagGuildMember* GuildMember::GetMember(DWORD dwRoleID)
{
	return m_mapMember.Peek(dwRoleID);
}

//-------------------------------------------------------------------------------
// 成员个数
//-------------------------------------------------------------------------------
inline INT32 GuildMember::GetNumber()
{
	return m_mapMember.Size();
}

//-------------------------------------------------------------------------------
// 是否已加入
//-------------------------------------------------------------------------------
inline BOOL GuildMember::IsExist(DWORD dwRoleID)
{
	return m_mapMember.IsExist(dwRoleID);
}

//-------------------------------------------------------------------------------
// 是否已加入
//-------------------------------------------------------------------------------
inline MapGuildMember& GuildMember::GetMemberMap()
{
	return m_mapMember;
}