//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_pos.h
// author: Sxg
// actor:
// data: 2009-4-20
// last:
// brief: 帮派职位管理
//-------------------------------------------------------------------------------
#pragma once

#include "guild_tang.h"
//-------------------------------------------------------------------------------
class GuildPos
{
public:
	GuildPos();
	~GuildPos();

	VOID	Init(BYTE byGuildLevel);

	BOOL	IsMemberFull() const;
	BOOL	IsPosFull(EGuildMemberPos eGuildPos);
	DWORD	AddMember(DWORD dwRoleID, EGuildMemberPos eGuildPos);
	VOID	RemoveMember(DWORD dwRoleID, EGuildMemberPos eGuildPos);
	DWORD	ChangePos(DWORD dwRoleID, EGuildMemberPos eOldGuildPos, EGuildMemberPos eNewGuildPos);
	DWORD	SpecGuildPosLimit(DWORD dwRoleID, EGuildMemberPos eGuildPos);
	INT32	GetNumber() const;

private:
	BOOL	IsBangZhuFull() const;
	BOOL	IsFuBangZhuFull() const;
	BOOL	IsHongHuFaFull() const;
	BOOL	IsZiHuFaFull() const;
	BOOL	IsJingYingFull() const;

	DWORD	AddBangZhu(DWORD dwRoleID);
	DWORD	AddFuBangZhu(DWORD dwRoleID);
	DWORD	AddHongHuFa(DWORD dwRoleID);
	DWORD	AddZiHuFa(DWORD dwRoleID);
	DWORD	AddJingYing(DWORD dwRoleID);
	DWORD	AddPuTong(DWORD dwRoleID);

	VOID	RemoveBangZhu(DWORD dwRoleID);
	VOID	RemoveFuBangZhu(DWORD dwRoleID);
	VOID	RemoveHongHuFa(DWORD dwRoleID);
	VOID	RemoveZiHuFa(DWORD dwRoleID);
	VOID	RemoveJingYing(DWORD dwRoleID);
	VOID	RemovePuTong(DWORD dwRoleID);

private:
	Tang		m_Tang[X_GUILD_TANG_NUM];

	// 考虑记录相应职位的roleid，当前系统还没有必要

	// 辅助属性
	INT8		m_n8CurNumBangZhu;
	INT8		m_n8CurNumFuBangZhu;
	INT8		m_n8CurNumHongHuFa;
	INT8		m_n8CurNumZiHuFa;
	INT8		m_n8CurNumJingYing;

	INT8		m_n8MaxNumJingYing;

	INT16		m_n16CurNumMember;
	INT16		m_n16MaxNumMember;
};


/*************************** 内联实现 *******************************/

inline INT32 GuildPos::GetNumber() const
{
	return m_n16CurNumMember;
}

inline BOOL GuildPos::IsMemberFull() const
{
	return m_n16CurNumMember >= m_n16MaxNumMember;
}

inline BOOL GuildPos::IsBangZhuFull() const
{
	return m_n8CurNumBangZhu >= MAX_GUILDPOS_BANGZHU;
}

inline BOOL GuildPos::IsFuBangZhuFull() const
{
	return m_n8CurNumFuBangZhu >= MAX_GUILDPOS_FUBANGZHU;
}

inline BOOL GuildPos::IsHongHuFaFull() const
{
	return m_n8CurNumHongHuFa >= MAX_GUILDPOS_HONGHUFA;
}

inline BOOL GuildPos::IsZiHuFaFull() const
{
	return m_n8CurNumZiHuFa >= MAX_GUILDPOS_ZIHUFA;
}

inline BOOL GuildPos::IsJingYingFull() const
{
	return m_n8CurNumJingYing >= m_n8MaxNumJingYing;
}

inline DWORD GuildPos::ChangePos(DWORD dwRoleID, EGuildMemberPos eOldGuildPos, EGuildMemberPos eNewGuildPos)
{
	RemoveMember(dwRoleID, eOldGuildPos);
	return AddMember(dwRoleID, eNewGuildPos);
}