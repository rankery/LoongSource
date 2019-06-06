//------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loot_mgr.h
// author: 
// actor:
// data: 2008-10-27
// last:
// brief: 掉落物品管理器
// remark: 读取掉落表，查询掉落表
//------------------------------------------------------------------------------------------------------
#pragma once

#include "singleton.h"
#include "..\WorldDefine\loot.h"

// 最大掉落位置数
const INT MAX_LOOT_POS		= 24;

struct tagLoot;
struct tagCreatureLoot;
struct tagLootItemSet;
struct tagLootQuestItem;
class Creature;
class Role;
class Team;

//-------------------------------------------------------------------------------------------------------
// 掉落方式
//-------------------------------------------------------------------------------------------------------
enum ELootTo
{
	ELT_BagFirst	= 0,	// 先落入背包，背包满则地面
	ELT_ForceBag	= 1,	// 必须背包
	ELT_Ground		= 2,	// 地面
};

//-----------------------------------------------------------------------------------------------------
// 各种掉落容器
//-----------------------------------------------------------------------------------------------------
typedef TMap<DWORD, tagCreatureLoot*>	CreatureLootMap;	// <掉落ID,掉落结构指针>
typedef TMap<DWORD, tagLootItemSet*>	LootItemSetMap;		// <掉落物品集合ID,掉落物品集合结构指针>
typedef TMap<DWORD, tagLootQuestItem*>	LootQuestItemMap;	// <怪物ID,掉落任务物品结构指针>

//-----------------------------------------------------------------------------------------------------
// 掉落管理器
//-----------------------------------------------------------------------------------------------------
class LootMgr : public Singleton<LootMgr>
{	
public:
	LootMgr();
	~LootMgr();
	
public:
    //-----------------------------------------------------------------------------------------
	// 初始化和销毁
	//-----------------------------------------------------------------------------------------
	BOOL					Init();
	VOID					Destroy();

public:
	//----------------------------------------------------------------------------------------
	// 怪物掉落
	//----------------------------------------------------------------------------------------
	VOID					MonsterLoot(Creature* pCreature, Role* pPicker);

	//----------------------------------------------------------------------------------------
	// 资源掉落
	//----------------------------------------------------------------------------------------
	VOID					ResourceLoot(Creature* pCreature, Role* pPicker);

	//----------------------------------------------------------------------------------------
	// 调查地物掉落
	//----------------------------------------------------------------------------------------
	VOID					InvestigateLoot(Creature* pCreature, Role* pPicker);

private:
	//----------------------------------------------------------------------------------------
	// 各种掉落属性加载
	//----------------------------------------------------------------------------------------
	BOOL					LoadCreatureLoot();
	BOOL					LoadItemSetLoot();
	BOOL					LoadQuestItemLoot();

	//----------------------------------------------------------------------------------------
	// 获取常量指针
	//----------------------------------------------------------------------------------------
	const tagCreatureLoot*	GetCreatureLoot(DWORD dwCreatureID)		{ return m_mapCreatureLoot.Peek(dwCreatureID); }
	const tagLootItemSet*	GetLootItemSet(DWORD dwSetID)			{ return m_mapLootItemSet.Peek(dwSetID); }
	const tagLootQuestItem*	GetLootQuestItem(DWORD dwCreatureID)	{ return m_mapLootQuestItem.Peek(dwCreatureID); }

	//----------------------------------------------------------------------------------------
	// 主要掉落方法
	//----------------------------------------------------------------------------------------
	VOID					NormalLoot(Creature* pCreature, Role* pRole, ELootTo eLootTo, BOOL bSingelLoot, BOOL bTeam);
	VOID					QuestLoot(Creature* pCreature, Role* pRole, BOOL bTeam);

	//----------------------------------------------------------------------------------------
	// 获取下一个掉落位置
	//----------------------------------------------------------------------------------------
	INT						GetPutDownPos(Creature* pCreature, Vector3 &vPos, INT &nIndex);

	//----------------------------------------------------------------------------------------
	// 掉率判断
	//----------------------------------------------------------------------------------------
	FLOAT					GetCreatureLootChanceAdd(Creature* pCreature, Role* pReward,INT addType = ERA_Loot_Add_Rate);
	FLOAT					GetMosterLootChanceAdd(Role* pReward);
	FLOAT					GetResLootChanceAdd(Creature* pCreature, Role* pReward);
	FLOAT					GetInvesLootChanceAdd(Role* pReward);

	// Jason 2009-11-28 掉率衰减
	FLOAT					GetMosterLootChanceAddEx(Creature* pCreature,Role* pReward,INT addType = ERA_Loot_Add_Rate);

	//----------------------------------------------------------------------------------------
	// 得到某个掉落的所属
	//----------------------------------------------------------------------------------------
	VOID					GetLootOwner(Creature* pCreature, Role* pPicker, BOOL bTeam, DWORD& dwOwnerID, Role* &pOwnerRole, DWORD& dwTeamID);

	//-----------------------------------------------------------------------------------------
	// 各种掉落方式
	//-----------------------------------------------------------------------------------------
	BOOL					LootItem(Creature* pCreature, tagItem* &pItem, ELootTo eLootTo, Role* pOwnerRole, DWORD dwOwnerID, DWORD dwTeamID, INT& nIndex);

	//----------------------------------------------------------------------------------------
	// 各种掉落方法
	//----------------------------------------------------------------------------------------
	BOOL					DropItemTo(const tagLoot* pLoot, Creature* pCreature, ELootTo eLootTo, Role* pPicker, BOOL bTeam, FLOAT fChanceAdd, INT &nIndex);
	BOOL					DropSetTo(const tagLoot* pLoot, Creature* pCreature, ELootTo eLootTo, Role* pPicker, BOOL bTeam, FLOAT fChanceAdd, INT &nIndex);
	BOOL					DropMoneyTo(const tagCreatureLoot* pLootProto, Creature* pCreature, ELootTo eLootTo, Role* pPicker, FLOAT fChanceAdd, INT &nIndex);
	BOOL					DropQuestItem(Role* pPicker, const tagItemProto* pItemProto, DWORD dwCreatureID);

private:
	TObjRef<VarContainer>	m_pVar;
	TObjRef<Util>			m_pUtil;

	//-----------------------------------------------------------------------------------------
	// 各种掉落
	//-----------------------------------------------------------------------------------------
	CreatureLootMap			m_mapCreatureLoot;		// 怪物掉落
	LootItemSetMap			m_mapLootItemSet;		// 掉落物品集合
	LootQuestItemMap		m_mapLootQuestItem;		// 掉落任务物品
};

#define sLootMgr LootMgr::getSingleton()

