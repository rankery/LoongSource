//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_skill.h
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 帮派技能
//-----------------------------------------------------------------------------
#pragma once

class Guild;
class Role;
struct tagGuildSkill;
struct tagGuildSkillInfo;

class GuildSkill
{
public:
	GuildSkill();
	~GuildSkill();

	VOID	Init(Guild* pGuild, BOOL bRequest = FALSE);
	VOID	Destroy();

	BOOL	IsInitOK()	{ return m_bInit; }

public:
	// 载入帮派技能信息
	BOOL	LoadGuildSkillIInfo(tagGuildSkillInfo* pSkillInfo, INT nSkillNum);

	// 获取所有帮派技能信息
	DWORD	GetGuildSkillInfo(DWORD& dwSkillID, INT16& n16Progress, INT& nSkillNum, DWORD* pLevelInfo);

	// 整点升级
	DWORD	SkillUpgradeOnClock();

	// 上缴物品升级
	DWORD	SkillUpgradeByHandIn(Role* pRole, INT64 n64Serial, DWORD& dwSkillID, INT16& n16Progress);

	// 学习技能
	DWORD	LearnGuildSkill(Role* pRole, DWORD dwSkillID, INT& nLevel, BOOL& bLearnError);

	// 创建帮派技能
	VOID	CreateGuildSkill();

	// 设置当前升级技能
	DWORD	SetCurUpgradeGuildSkill(DWORD dwRoleID, DWORD dwSkillID, INT& nLevel, INT16& n16Progress);

	// 清除所有帮派技能信息
	VOID	RemoveGuildSkillInfo();

private:
	// 与数据库通讯
	VOID	SaveSkillInfo2DB(DWORD dwSkillID);
	VOID	ChangeResearchSkill(DWORD dwNewSkillID);
	
private:
	typedef TMap<DWORD, tagGuildSkill*>	MapGuildSkill;

	BOOL			m_bInit;
	Guild*			m_pGuild;

	// 技能升级信息
	DWORD			m_dwCurSkill;
	MapGuildSkill	m_mapGuildSkill;
};