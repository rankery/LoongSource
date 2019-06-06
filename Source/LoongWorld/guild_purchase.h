//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_purchase.h
// author: sunnee
// actor:
// data: 2009-07-13
// last:
// brief: 帮派团购类
//-------------------------------------------------------------------------------------------------------
#pragma once

struct tagGroupPurchase;
struct tagSimGPInfo;
struct tagMallItemSell;
struct tagItem;
class Guild;

class GuildPurchase
{
public:
	GuildPurchase();
	~GuildPurchase();

	BOOL Init(DWORD dwGuildID);
	VOID Update(DWORD dwTime);
	VOID Destory();

	// 信息管理
	BOOL	Add(tagGroupPurchase* pGPInfo, BOOL bNotify2DB = TRUE);
	BOOL	Remove(tagGroupPurchase* pGPInfo, BOOL bNotify2DB = TRUE);
	BOOL	IsEmpty()	{ return m_mapGPInfo.Empty(); }

	// 获取团购信息
	DWORD	GetAllSimGPInfo(INT &nGPInfoNum, tagSimGPInfo* pData);
	DWORD	GetParticipators(DWORD dwID, DWORD dwRoleID, DWORD *pData);

	INT		GetGroupPurchaseInfoNum();
	INT		GetParticipatorNum(DWORD dwID, DWORD dwRoleID);

	// 发起/响应团购
	DWORD	LaunchGroupPurchase(Role *pRole, DWORD dwID, BYTE byScope,
		BYTE byIndex, INT nUnitPrice, OUT tagGroupPurchase* &pGPInfo, OUT DWORD& dwItemTypeID);
	DWORD	RespondGroupPurchase(Role *pRole, DWORD dwID, DWORD dwRoleID,
		INT nPrice, OUT tagGroupPurchase* &pGPInfo);

	// 生成团购物品
	DWORD	CreateGPItems(DWORD dwID, OUT tagMallItemSell &itemSell);

	// 删除团购信息
	VOID	RemoveGroupPurchaseInfo(tagGroupPurchase* &pGPInfo, BOOL bSuccess = TRUE);
	VOID	RemoveGroupPurchaseInfo();

	// 返还玩家元宝
	VOID	ReturnCost2Participator(tagGroupPurchase* pGPInfo);

	// 更新响应者列表
	VOID	UpdateGPInfo2DB(tagGroupPurchase* pGPInfo);

	// 团购成功处理
	DWORD	DoSuccessStuff(tagGroupPurchase* pGPInfo);

private:
	// 数据库操作
	VOID AddGPInfo2DB(tagGroupPurchase* pGPInfo);
	VOID RemoveGPInfo2DB(tagGroupPurchase* pGPInfo);
	VOID RemoveGuildGPInfo2DB();

private:
	// 生成键值
	INT64	GetKey(DWORD dwRoleID, DWORD dwID);
	
private:
	typedef TMap<INT64, tagGroupPurchase*> MapGPInfo;
	MapGPInfo				m_mapGPInfo;

	Guild*					m_pGuild;
};