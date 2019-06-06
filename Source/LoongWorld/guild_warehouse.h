//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_warehouse.h
// author: Sunnee
// actor:
// data: 2009-06-23
// last:
// brief: 帮派仓库管理器 -- 负责物品管理,与客户端同步,及记录物品
//-----------------------------------------------------------------------------
#pragma once

#include "event_mgr.h"
#include "container.h"

struct	tagGuildBase;
class	Guild;
//-----------------------------------------------------------------------------

class GuildWarehouse : public EventMgr<GuildWarehouse>
{
public:
	GuildWarehouse();
	~GuildWarehouse();

	VOID	Init(Guild* pGuild, BOOL bRequest = FALSE);
	VOID	Update();

	BOOL	IsInitOK()	{ return m_bInitOK; }

	// 帮派升级时重新设置大小
	VOID	ReInit();

	// 初始化时载入
	VOID	LoadWarehouseItems(const BYTE* pData, INT nItemNum);
	// 帮派解散时删除所有物品
	VOID	RemoveAllItems();

	// 事件操作
	VOID	OnMoveToFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnMove2OtherFunc(DWORD dwSenderID, VOID* pEventMessage);

	// 地图上层事件
	DWORD	GetGuildWareInfo(BYTE* pBuff, INT& nItemNum, DWORD& dwLastUpdateTime, INT16& nSzWare, INT32& nSize);
	DWORD	SendGuildWarePriList2Client(Role* p2Role);

	// 外部接口
	DWORD	AddItem(tagItem *&pItem, DWORD dwCmdID, BOOL bInsert2DB = TRUE);

private:
	// 移动动作
	DWORD MoveAllowable(Role* pRole);
	DWORD Move(Role* pRole, INT64 n64Serial, INT16 n16Num, INT16 n16PosDst, DWORD dwCmdID);
	DWORD Move2Other(Role* pRole, EItemConType eConTypeSrc, INT64 n64Serial1, 
		EItemConType eConTypeDst, INT16 n16PosDst, DWORD dwCmdID);

private:
	// 数据库通讯
	VOID InsertItem2DB(tagItem &item);
	VOID DeleteItemFromDB(INT64 n64Serial, INT32 dwTypeID);
	VOID SaveUpdateItem2DB();

private:
	// 客户端通讯
	VOID SendMessage(Role* pRole, LPVOID pMsg, DWORD dwSize);

private:
	// 记录Log
	BOOL IsItemNeedLog(const tagItem &item) const { return item.pProtoType->bNeedLog; }
	__forceinline VOID LogItem(DWORD dwRoleID, const tagItem &item1, const tagItem *pItem2, INT16 n16OptNum, DWORD dwCmdID);

private:
	static	VOID RegisterGuildWareEventFunc();

private:
	// 更新时间管理
	VOID SetUpdateTime(INT16 nPos);				// 记录对应栏位的最后更新时间

private:
	BOOL				m_bInitOK;
	Guild*				m_pGuild;

	ItemContainer*		m_pContainer;			// 放到帮派仓库中的物品,所属为帮派(m_dwOwnerID <==> GuildID)

	// 更新时间管理
	DWORD				m_dwLastSaveTime;		// 最后保存时间
	DWORD				m_dwGuildWareTime;		// 帮派仓库最后更新时间
	std::vector<DWORD>	m_vecUpdateTime;		// 仓库每个栏位的最后更新时间
};