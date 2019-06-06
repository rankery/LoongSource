//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: item_creator.cpp
// author: Sxg
// actor:
// data: 2008-07-29
// last:
// brief: 物品&装备生成器 -- 物品&装备生产,及武器鉴定(按品级生成属性)
//-----------------------------------------------------------------------------
#pragma once

#include "mutex.h"

struct tagEquip;
struct tagEquipProto;
struct tagIMEffect;
//-----------------------------------------------------------------------------
class ItemCreator
{
	friend class GMCommandMgr;

public:
	ItemCreator();
	~ItemCreator();

public:
	// 根据数据库读取的数据创建物品
	static tagItem* CreateItemByData(PVOID pData);
	static tagEquip* CreateEquipByData(PVOID pData);
	// 根据已有物品生成新的堆物品
	static tagItem* Create(const tagItem &item, INT16 n16Num);
	
	// 生成未鉴定物品&装备
	static tagItem* Create(EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, INT16 n16Num = 1,  
							DWORD dwCreator = GT_INVALID, INT16 n16QltyModPct = 0, INT16 n16QltyModPctEx = 0, INT16 n16PotValPct = 10000,INT nPotAdding = 0);
	// Jason 2009-12-1 增加紫色品级
	static tagItem* CreateByProduce(EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID,EItemQuality eQlty, INT nPotAdding,DWORD dwPurpleQltyIdfPct = 0, INT16 n16Num = 1, 
		DWORD dwCreator = GT_INVALID, INT16 n16QltyModPct = 0, INT16 n16QltyModPctEx = 0, INT16 n16PotValPct = 10000);

	// 生成未鉴定珍宝(物品&装备)
	static tagItem* CreateTreasure(DWORD dwNameID, EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, INT16 n16Num = 1, 
							DWORD dwCreator = GT_INVALID, INT16 n16QltyModPct = 0, INT16 n16QltyModPctEx = 0);

	// 生成指定品级的物品&装备
	static tagItem* CreateEx(EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, 
							INT16 n16Num = 1, EItemQuality eQlty = EIQ_Null, DWORD dwCreator = GT_INVALID, 
							const tagIMEffect *pIMEffect = NULL)
	{
		tagItem *pNewItem = Create(eCreateMode, dwCreateID, dwTypeID, n16Num);
		if(!P_VALID(pNewItem))
		{
			return NULL;
		}

		if(MIsEquipment(dwTypeID)/* && eQlty > EIQ_Start && eQlty < EIQ_End*/)
		{
			IdentifyEquip((tagEquip*)pNewItem, eQlty, pIMEffect);
		}

		return pNewItem;
	}

	// 生成指定品级的珍宝(物品&装备)
	static tagItem* CreateTreasureEx(DWORD dwNameID, EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, 
		INT16 n16Num = 1, EItemQuality eQlty = EIQ_Null, DWORD dwCreator = GT_INVALID, 
		const tagIMEffect *pIMEffect = NULL);


	// 设置物品装备序号
	static VOID SetItemSerial(INT64 n64Max, INT64 n64Min) { m_n64MaxSerial = n64Max; m_n64MinSerial = n64Min; }

	// 生成世界唯一号(注意要做互锁机制)
	static VOID CreateItemSerial(INT64 &n64NewSerial);

public:
	static VOID IdentifyEquip(IN OUT tagEquip *pEquip, 
						EItemQuality eQlty = EIQ_Null, const tagIMEffect *pIMEffect = NULL);

private: 	
	// 参数合法性由上层调用函数保证
	static VOID InitItem(tagItem &item, EItemCreateMode eCreateMode, const tagItemProto *pProto, DWORD dwCreateID, INT64 n64Serial, INT16 n16Num, DWORD dwCreator, DWORD dwCreateTime, DWORD dwAccountID, DWORD dwOwnerID);
	static VOID InitEquipSpec(tagEquipSpec &equipSpec, INT16 n16QltyModPct = 0, INT16 n16QltyModPctEx = 0, INT16 n16PotValPct = 10000);

	static INT32 GenBaseEquipQlty(DWORD dwTypeID);
	static INT32 ModEquipQltyByProduce(const tagEquip *pEquip, INT32 nQuality);
	static DWORD GenLongHunRandID(INT nLongHunType, INT nEquipPos, INT nQlty);
	
	static VOID CreateEquipQltyRel(OUT tagEquip *pEquip, const tagEquipProto *pEquipProto, EItemQuality eQlty);
	static BOOL CreateFashionQltyRel(OUT tagEquip *pEquip, const tagEquipProto *pEquipProto, 
									EItemQuality eQlty, const tagIMEffect *pIMEffect = NULL);

	static VOID ProcEquipAttBySpecAtt(tagEquip *pEquip);

	static BOOL IsGMItemNoInit(tagItem* pTmpItem);
	static BOOL InitGMItem(tagItem* pTmpItem);


private:
	static INT64			m_n64MaxSerial;
	static INT64			m_n64MinSerial;
	static Mutex			m_Mutex;
};