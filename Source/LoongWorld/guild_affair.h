//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_affair.h
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 帮派事务发布
//-----------------------------------------------------------------------------
#pragma once

class Guild;

class GuildAffair
{
public:
	GuildAffair();
	~GuildAffair();

	VOID	Init(Guild* pGuild);
	BOOL	IsInitOK()	{ return m_bInit; }

	// 发布帮务
	DWORD	SpreadGuildAffair(Role* pRole, DWORD dwBuffID);

	// 重置帮务发布次数
	VOID	ResetAffairCounter();

private:
	BOOL			m_bInit;
	Guild*			m_pGuild;

	BYTE			m_byCounter;		// 帮务发布次数(保存到数据库?)
};