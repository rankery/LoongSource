//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_mgr.h
// author: Sxg
// actor:
// data: 2009-4-7
// last:
// brief: 帮派管理器
//-------------------------------------------------------------------------------
#pragma once
#include "event_mgr.h"
#include "../WorldDefine/guild_define.h"
#include "../ServerDefine/guild_define.h"

class Guild;
class Role;

struct tagGuildLoad;
struct tagGuildMemLoad;
//-------------------------------------------------------------------------------
class GuildMgr
{
public:
	GuildMgr();
	~GuildMgr();

	BOOL	Init();
	VOID	Update();
	VOID	Destroy();

	INT32	GetGuildNum();
	Guild*	GetGuild(DWORD dwGuildID);

	DWORD	CreateGuild(Role* pCreator, LPCTSTR strGuildName, INT32 nNumTCHAR);
	DWORD	DismissGuild(Role* pLeader, DWORD dwGuildID);

	// 和LoongDB连接后，所有帮派相关初始化
	VOID	InitDBGuild(const tagGuildLoad* pGuildLoad);
	VOID	InitDBGuildMember(const tagGuildMemLoad *pGuildMemberLoad, const INT32 nNum);
	BOOL	IsGuildInitOK();

	// 关闭LoongWorld时，保存所有帮派基本信息
	VOID	SendAllGuild2DB();

	// 客户端连接后，初始化所在帮派信息
	VOID	InitGuildInitState(Role *pRole, 
							OUT tagGuildBase &sGuildBase, 
							OUT tagGuildMember &sGuildMember);
	
	// 客户端请求处理失败后反馈消息
	VOID	SendGuildProcFailed2Client(PlayerSession *pSession, DWORD dwErrorCode);

	// 删除角色时清理相关数据
	DWORD	ClearRoleRemove(DWORD dwRoleID);

	// 配置信息
	const tagGuildCfg& GetGuildCfg() const;

	// 向帮派仓库添加操作事件
	VOID AddGuildWareEvent(DWORD dwSender, EEventType eType, DWORD dwSize, VOID* pMsg);

	// 帮派技能整点升级
	VOID UpgradeGuildSkill();

	// 每天2:00扣除消耗并重置帮务状态
	VOID DailyGuildReset();

private:
	Guild*	CreateGuild(Role* pCreator, const tstring& strGuildName, DWORD dwNewGuildID);

	// 要求更新数据库
	VOID	SendCreateGuild2DB(const tagGuild& sGuild);
	VOID	SendDismissGuild2DB(DWORD dwGuildID);

private:
	typedef TMap<DWORD, Guild*>		MapGuild;	// <dwGuildID, pGuild>

	tagGuildCfg			m_GuildCfg;
	MapGuild			m_mapGuild;
};



/*************************** 内联实现 *******************************/

//-------------------------------------------------------------------------------
// 获取配置信息
//-------------------------------------------------------------------------------
inline const tagGuildCfg& GuildMgr::GetGuildCfg() const
{
	return m_GuildCfg;
}

//-------------------------------------------------------------------------------
// 获取帮派对象个数
//-------------------------------------------------------------------------------
inline INT32 GuildMgr::GetGuildNum()
{
	return m_mapGuild.Size();
}

//-------------------------------------------------------------------------------
// 获取帮派对象
//-------------------------------------------------------------------------------
inline Guild* GuildMgr::GetGuild(DWORD dwGuildID)
{
	return m_mapGuild.Peek(dwGuildID);
}

//-------------------------------------------------------------------------------
extern GuildMgr g_guildMgr;