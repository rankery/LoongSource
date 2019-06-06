//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: item_creator.cpp
// author: Sxg
// actor:
// data: 2008-07-29
// last:
// brief: 物品&装备生成器 -- 物品&装备生产,及武器鉴定(按品级生成属性)
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/ItemDefine.h"
#include "../WorldDefine/RoleDefine.h"
#include "../ServerDefine/base_define.h"
#include "../ServerDefine/item_define.h"

#include "att_res.h"
#include "item_creator.h"
#include "world.h"
#include "script_mgr.h"

INT64			ItemCreator::m_n64MaxSerial = 0;
INT64			ItemCreator::m_n64MinSerial = 0;
Mutex			ItemCreator::m_Mutex;

//-------------------------------------------------------------------------------------------------------
// 构造函数
//-------------------------------------------------------------------------------------------------------
ItemCreator::ItemCreator()
{}

ItemCreator::~ItemCreator()
{}

//-------------------------------------------------------------------------------------------------------
// 根据已有物品生成新的堆物品
//-------------------------------------------------------------------------------------------------------
tagItem* ItemCreator::Create(const tagItem &item, INT16 n16Num)
{
	if(n16Num > item.pProtoType->n16MaxLapNum)
	{
		ASSERT(n16Num <= item.pProtoType->n16MaxLapNum);
		return NULL;
	}
	
	tagItem *pNewItem;
	if(MIsEquipment(item.dwTypeID))
	{
		pNewItem = new tagEquip;
		IFASTCODE->MemCpy(pNewItem, &item, SIZE_EQUIP);
	}
	else
	{
		pNewItem = new tagItem;
		IFASTCODE->MemCpy(pNewItem, &item, SIZE_ITEM);
	}

	CreateItemSerial(pNewItem->n64Serial);
	pNewItem->n16Num = n16Num;

	return pNewItem;
}

//-------------------------------------------------------------------------------------------------------
// 生成物品&装备
// Jason 2009-12-1 增加紫色品级影响，装备潜力值受材料影响
//-------------------------------------------------------------------------------------------------------
tagItem* ItemCreator::Create(EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID,
							 INT16 n16Num/* = 1*/, DWORD dwCreator/* = GT_INVALID*/,
							 INT16 n16QltyModPct/* = 0*/, INT16 n16QltyModPctEx/* = 0*/, INT16 n16PotValPct/* = 10000*/, INT nPotAdding)
{
	if(n16Num <= 0)
	{
		ASSERT(n16Num > 0);
		return NULL;
	}
	
	tagItem *pRetItem;
	if(MIsEquipment(dwTypeID))
	{
		pRetItem = new tagEquip;
		if(NULL == pRetItem)
		{
			ASSERT(pRetItem != NULL);
			return NULL;
		}

		InitEquipSpec(((tagEquip*)pRetItem)->equipSpec, n16QltyModPct, n16QltyModPctEx, n16PotValPct);
		pRetItem->pProtoType = g_attRes.GetEquipProto(dwTypeID);
	}
	else
	{		
		pRetItem = new tagItem;
		if(NULL == pRetItem)
		{
			ASSERT(pRetItem != NULL);
			return NULL;
		}

		pRetItem->pProtoType = g_attRes.GetItemProto(dwTypeID);
	}

	if(!P_VALID(pRetItem->pProtoType) || pRetItem->pProtoType->n16MaxLapNum < n16Num)
	{
		MAttResCaution(_T("item or equip proto"), _T("typeid"), dwTypeID);
		ASSERT(P_VALID(pRetItem->pProtoType));
		ASSERT(P_VALID(pRetItem->pProtoType) && pRetItem->pProtoType->n16MaxLapNum >= n16Num);
		::Destroy(pRetItem);
		return NULL;
	}

	INT64 n64Serial;
	CreateItemSerial(n64Serial);

	InitItem(*pRetItem, eCreateMode, pRetItem->pProtoType, dwCreateID, n64Serial, n16Num, dwCreator, g_world.GetWorldTime(), GT_INVALID, GT_INVALID);

	// 装备
	if(MIsEquipment(pRetItem->dwTypeID))
	{
		// Jason 2009-12-1 装备潜力值提升次数
		((tagEquip*)pRetItem)->equipSpec.nPotIncTimes	= ((tagEquip*)pRetItem)->pEquipProto->nPotIncTimes + nPotAdding;	
		// 检查是否为掉落或者生成后即时鉴定
		if( (pRetItem->pEquipProto->bIdentifyLoot && EICM_Loot == eCreateMode)
			|| (pRetItem->pEquipProto->bIdentifyProduct && EICM_Product == eCreateMode)
			|| (EICM_LessingLoong == eCreateMode && pRetItem->pEquipProto->bIdentifyProduct ) 
			|| (EICM_GoldStone == eCreateMode && pRetItem->pEquipProto->bIdentifyProduct ) )
		{
			ItemCreator::IdentifyEquip((tagEquip*)pRetItem);
		}
	}

	return pRetItem;
}
// Jason 2009-12-1 增加紫色品级后的重载
tagItem* ItemCreator::CreateByProduce(EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, EItemQuality eQlty,INT nPotAdding,DWORD dwPurpleQltyIdfPct, 
							 INT16 n16Num/* = 1*/, DWORD dwCreator/* = GT_INVALID*/,
							 INT16 n16QltyModPct/* = 0*/, INT16 n16QltyModPctEx/* = 0*/, INT16 n16PotValPct/* = 10000*/)
{
	if(n16Num <= 0)
	{
		ASSERT(n16Num > 0);
		return NULL;
	}

	tagItem *pRetItem;
	if(MIsEquipment(dwTypeID))
	{
		pRetItem = new tagEquip;
		if(NULL == pRetItem)
		{
			ASSERT(pRetItem != NULL);
			return NULL;
		}

		InitEquipSpec(((tagEquip*)pRetItem)->equipSpec, n16QltyModPct, n16QltyModPctEx, n16PotValPct);
		pRetItem->pProtoType = g_attRes.GetEquipProto(dwTypeID);
	}
	else
	{		
		pRetItem = new tagItem;
		if(NULL == pRetItem)
		{
			ASSERT(pRetItem != NULL);
			return NULL;
		}

		pRetItem->pProtoType = g_attRes.GetItemProto(dwTypeID);
	}

	if(!P_VALID(pRetItem->pProtoType) || pRetItem->pProtoType->n16MaxLapNum < n16Num)
	{
		MAttResCaution(_T("item or equip proto"), _T("typeid"), dwTypeID);
		ASSERT(P_VALID(pRetItem->pProtoType));
		ASSERT(P_VALID(pRetItem->pProtoType) && pRetItem->pProtoType->n16MaxLapNum >= n16Num);
		::Destroy(pRetItem);
		return NULL;
	}

	INT64 n64Serial;
	CreateItemSerial(n64Serial);

	InitItem(*pRetItem, eCreateMode, pRetItem->pProtoType, dwCreateID, n64Serial, n16Num, dwCreator, g_world.GetWorldTime(), GT_INVALID, GT_INVALID);

	// 装备
	if(MIsEquipment(pRetItem->dwTypeID))
	{
		// Jason 2009-12-1 装备潜力值提升次数
		((tagEquip*)pRetItem)->equipSpec.nPotIncTimes	= ((tagEquip*)pRetItem)->pEquipProto->nPotIncTimes + nPotAdding;
		((tagEquip*)pRetItem)->equipSpec.dwPurpleQltyIdfPct = dwPurpleQltyIdfPct;
		// 检查是否为掉落或者生成后即时鉴定
		if((pRetItem->pEquipProto->bIdentifyLoot && EICM_Loot == eCreateMode)
			|| (pRetItem->pEquipProto->bIdentifyProduct && EICM_Product == eCreateMode)
			|| (EICM_LessingLoong == eCreateMode && pRetItem->pEquipProto->bIdentifyProduct ) 
			|| (EICM_GoldStone == eCreateMode && pRetItem->pEquipProto->bIdentifyProduct ) )
		{
			ItemCreator::IdentifyEquip((tagEquip*)pRetItem,eQlty);
		}
	}

	return pRetItem;
}


//-------------------------------------------------------------------------------------------------------
// 根据数据库读取的数据创建物品
//-------------------------------------------------------------------------------------------------------
tagItem* ItemCreator::CreateItemByData( PVOID pData )
{
	MTRANS_POINTER(pItem, pData, tagItem);
	tagItem* pNewItem = new tagItem;

	IFASTCODE->MemCpy(pNewItem, pItem, sizeof(tagItem));
	pNewItem->pProtoType = g_attRes.GetItemProto(pItem->dwTypeID);

	if (P_VALID(pNewItem->pProtoType) && IsGMItemNoInit(pNewItem))
	{
		InitItem(*pNewItem, pItem->eCreateMode, pNewItem->pProtoType, pNewItem->dwCreateID, pNewItem->n64Serial, pNewItem->n16Num, pNewItem->dwCreatorID, pNewItem->dwCreateTime, pNewItem->dwAccountID, pNewItem->dwOwnerID);
	}

	return pNewItem;
}

//-------------------------------------------------------------------------------------------------------
// 根据数据库读取的数据创建装备
//-------------------------------------------------------------------------------------------------------
tagEquip* ItemCreator::CreateEquipByData( PVOID pData )
{
	MTRANS_POINTER(pEquip, pData, tagEquip);
	tagEquip* pNewEquip = new tagEquip;

	IFASTCODE->MemCpy(pNewEquip, pEquip, sizeof(tagEquip));
	pNewEquip->pProtoType = g_attRes.GetEquipProto(pEquip->dwTypeID);

	if (IsGMItemNoInit(pEquip))
	{
		InitItem(*pNewEquip, pNewEquip->eCreateMode, pNewEquip->pProtoType, pNewEquip->dwCreateID, pNewEquip->n64Serial, pNewEquip->n16Num, pNewEquip->dwCreatorID, pNewEquip->dwCreateTime, pNewEquip->dwAccountID, pNewEquip->dwOwnerID);

		EItemQuality eQuality = (EItemQuality)pNewEquip->equipSpec.byQuality;
		InitEquipSpec(pNewEquip->equipSpec);
		IdentifyEquip(pNewEquip, eQuality);
	}

	return pNewEquip;
}

//-------------------------------------------------------------------------------------------------------
// 生成世界唯一号(注意要做互锁机制)
//-------------------------------------------------------------------------------------------------------
VOID ItemCreator::CreateItemSerial(INT64 &n64NewSerial)
{
	m_Mutex.Acquire();
	n64NewSerial = ++m_n64MaxSerial;
	m_Mutex.Release();
}

//-------------------------------------------------------------------------------------------------------
// 创建物品
//-------------------------------------------------------------------------------------------------------
VOID ItemCreator::InitItem( tagItem &item, EItemCreateMode eCreateMode, const tagItemProto *pProto, DWORD dwCreateID, INT64 n64Serial, INT16 n16Num, DWORD dwCreator, DWORD dwCreateTime, DWORD dwAccountID, DWORD dwOwnerID )
{
	ZeroMemory(&item, SIZE_ITEM);

	item.pProtoType		= pProto;
	
	item.n64Serial		= n64Serial;
	item.n16Num			= n16Num;
	item.dwTypeID		= pProto->dwTypeID;

	item.byBind			= EBS_Unknown;
	item.bLock			= FALSE;

	item.eCreateMode	= eCreateMode;
	item.dwCreateID		= dwCreateID;	// 生成该物品的ID,如: QuestID,GMID等
	
	item.dwCreatorID	= dwCreator;	/* 多为RoleID, 如某位玩家完成任务后奖励物品,该位可能标示为该玩家创建;
											当该物品通过生成系统生成时,则该位同dwCreateID*/
	item.dwNameID		= GT_INVALID;
	item.dwCreateTime	= dwCreateTime;

	item.dwOwnerID		= dwOwnerID;
	item.dwAccountID	= dwAccountID;
	item.eConType		= EICT_Null;
	item.n16Index		= GT_INVALID;

	item.eStatus		= EUDBS_Insert;

	item.pScript		= g_ScriptMgr.GetItemScript(pProto->dwTypeID);
}

//-------------------------------------------------------------------------------------------------------
// 创建未鉴定装备(装备专用部分属性)
//-------------------------------------------------------------------------------------------------------
VOID ItemCreator::InitEquipSpec(tagEquipSpec &equipSpec, INT16 n16QltyModPct, INT16 n16QltyModPctEx, INT16 n16PotValPct)
{
	ZeroMemory(&equipSpec, SIZE_EQUIPSPEC);

	equipSpec.byQuality		= EIQ_Null;
	equipSpec.bCanCut		= TRUE;

	for(INT32 i=0; i<MAX_ROLEATT_POSY_EFFECT; ++i)
	{
		equipSpec.PosyEffect[i].eRoleAtt = ERA_Null;
	}

	equipSpec.dwLongHunInnerID	= GT_INVALID;
	equipSpec.dwLongHunOuterID	= GT_INVALID;

	equipSpec.bySpecAtt			= EESA_Null;
	equipSpec.n8ColorID			= EC_Null;

	equipSpec.n16QltyModPct		= n16QltyModPct;
	equipSpec.n16QltyModPctEx	= n16QltyModPctEx;

	equipSpec.n16PotValModPct	= n16PotValPct;
}

//-----------------------------------------------------------------------------
// 鉴定装备(没有品级或品级非法，则重新计算品级)
//-----------------------------------------------------------------------------
VOID ItemCreator::IdentifyEquip(IN OUT tagEquip *pEquip, 
								EItemQuality eQlty/* = EIQ_Null*/, 
								const tagIMEffect *pIMEffect/* = NULL*/)
{
	ASSERT(P_VALID(pEquip));
	ASSERT(P_VALID(pEquip->pEquipProto));
	ASSERT(!MIsIdentified(pEquip));

	// 检查是否是已鉴定过装备
	if(MIsIdentified(pEquip))
	{
		return;
	}
	
	INT32 nQuality = eQlty;
	
	// 没有品级或品级非法，则重新计算品级
	bool bPurpleQltyIdf = false;
	if( pEquip->equipSpec.dwPurpleQltyIdfPct )
	{
		FLOAT fRnd = ( (FLOAT) IUTIL->RandomInRange(0,100) ) / 100;
		FLOAT fPct = (FLOAT)pEquip->equipSpec.dwPurpleQltyIdfPct / 100;
		if( fRnd <= fPct )//成功
			bPurpleQltyIdf = true;
	}
	
	if( eQlty <= EIQ_Start || eQlty >= EIQ_End ) 
	{
		if( !bPurpleQltyIdf ) //紫色之外的，还走原品级生成方式
		{
			// 根据装备品级生成几率生成品级
			nQuality = GenBaseEquipQlty(pEquip->dwTypeID);

			// 根据生产品级修正几率修正品级
			nQuality = ModEquipQltyByProduce(pEquip, nQuality);
		}
		else
			nQuality = EIQ_Purple;
	}

	// 初始化品级
	pEquip->equipSpec.byQuality = (BYTE)nQuality;

	// 初始化等级限制
	pEquip->equipSpec.byMinUseLevel = pEquip->pEquipProto->byMinUseLevel;	// 等级限制
	pEquip->equipSpec.byMaxUseLevel = pEquip->pEquipProto->byMaxUseLevel;	// 等级上限

	// 根据品级修正相关属性
	if(!MIsFashion(pEquip))
	{
		CreateEquipQltyRel(pEquip, pEquip->pEquipProto, (EItemQuality)nQuality);
	}
	else
	{
		CreateFashionQltyRel(pEquip, pEquip->pEquipProto, (EItemQuality)nQuality, pIMEffect);
	}
}

//-----------------------------------------------------------------------------
// 根据装备品级生成几率生成品级
//-----------------------------------------------------------------------------
INT32 ItemCreator::GenBaseEquipQlty(DWORD dwTypeID)
{
	tagEquipQltyPct *pEquipQltyPct = g_attRes.GetEquipQltyPct(dwTypeID);
	if(!P_VALID(pEquipQltyPct))
	{
		ASSERT(P_VALID(pEquipQltyPct));
		return EIQ_White;
	}

	INT32 nEquipQltySumPct = 0;
	INT32 nRandPct = IUTIL->Rand() % 10000;
	// Jason 2009-11-30 不能自动生成紫色品级装备
	// 注意，紫色品级装备是不能自动生成的。
	// 因此att_res.cpp中的InitOneEquipQltyPct，不需要改动。
	for(INT32 i=EIQ_Purple-1/*EIQ_End - 1*/; i>EIQ_Start; --i)
	{
		nEquipQltySumPct += pEquipQltyPct->nEquipQltyPct[i]; // Jason nEquipQltyPct的维度是EIQ_End - EIQ_Start - 1，它包含了紫装，因此，我在上面做了限制
		if(nRandPct < nEquipQltySumPct)
		{
			return i;
		}
	}

	return EIQ_White;
}

//-----------------------------------------------------------------------------
// 根据生产品级修正几率修正品级
// 由于增加了紫色品级，而，
// 不能自动生成紫色装备，因此，将EIQ_END改为EIQ_Purple
//-----------------------------------------------------------------------------
INT32 ItemCreator::ModEquipQltyByProduce(const tagEquip *pEquip, INT32 nQuality)
{
	if(pEquip->equipSpec.n16QltyModPctEx != 0
		&& (IUTIL->Rand() % 10000) < pEquip->equipSpec.n16QltyModPctEx)	// 二级修正
	{
		return min(nQuality + 2, EIQ_Purple - 1);
	}
	else	// 一级修正
	{
		if(pEquip->equipSpec.n16QltyModPct > 0)
		{
			if((IUTIL->Rand() % 10000) < pEquip->equipSpec.n16QltyModPct)
			{
				return min(nQuality + 1, EIQ_Purple - 1);
			}
		}
		else if(pEquip->equipSpec.n16QltyModPct < 0)
		{
			if((IUTIL->Rand() % 10000) < -pEquip->equipSpec.n16QltyModPct)
			{
				if( nQuality >= EIQ_Purple )
					nQuality = EIQ_Purple - 1 ;
				return max(nQuality - 1, EIQ_White);
			}
		}
	}

	return nQuality;
}

//-----------------------------------------------------------------------------
// 根据指定品级生成装备相关属性
//-----------------------------------------------------------------------------
DWORD ItemCreator::GenLongHunRandID(INT nLongHunType, INT nEquipPos, INT nQlty)
{
	if(nEquipPos < EEP_Equip_Start || nEquipPos > EEP_Equip_End)
	{
		ASSERT(0);
		return GT_INVALID;
	}
	
	const std::vector<DWORD> vectLongHun 
		= g_attRes.GetLongHunSpecVect(nLongHunType, g_eEquipGenLongHunTypeEx[nEquipPos], nQlty);

	INT nSzVect = vectLongHun.size();
	if(nSzVect > 0)
	{
		INT nRandPct = IUTIL->Rand() % nSzVect;
		return vectLongHun[nRandPct];
	}

	return GT_INVALID;
}

//-----------------------------------------------------------------------------
// 根据指定品级生成装备相关属性
//-----------------------------------------------------------------------------
VOID ItemCreator::CreateEquipQltyRel(OUT tagEquip *pEquip, const tagEquipProto *pEquipProto, EItemQuality eQlty)
{
	ASSERT(eQlty > EIQ_Start && eQlty < EIQ_End);
	
	// 得到指定品级装备属性参数
	const tagEquipQltyEffect *pEquipQltyEffect = g_attRes.GetEquipQltyEffect(eQlty);
	if(!P_VALID(pEquipQltyEffect))
	{
		ASSERT(P_VALID(pEquipQltyEffect));
		return;
	}
	
	// 修正系数
	FLOAT fValueModFactor;

	// 装备基础属性 -- "武器：原始编辑值; 防具：原始编辑值"
	fValueModFactor = pEquipQltyEffect->fWeaponFactor;
	pEquip->equipSpec.n16WuHun	= (INT16)(pEquipProto->n16WuHun * fValueModFactor);		// 内功伤害计算用
	pEquip->equipSpec.n16MinDmg	= (INT16)(pEquipProto->n16MinDmg * fValueModFactor);	// 武器最小伤害
	pEquip->equipSpec.n16MaxDmg	= (INT16)(pEquipProto->n16MaxDmg * fValueModFactor);	// 武器最大伤害

	fValueModFactor = pEquipQltyEffect->fArmorFactor;
	pEquip->equipSpec.n16Armor	= (INT16)(pEquipProto->n16Armor * fValueModFactor);		// 防具护甲

	// 角色一级属性	--	A4 数值4=0.5+等级×0.05	将数值4随机分配在任意1或2个或3个一级属性之中							
	ZeroMemory(pEquip->equipSpec.nRoleAttEffect, sizeof(pEquip->equipSpec.nRoleAttEffect));
	
	fValueModFactor = pEquipQltyEffect->fAttAFactor;
	INT32 nValueModLeft = (INT32)(pEquipQltyEffect->nAttAFactor + pEquipProto->byLevel * fValueModFactor);
	INT32 nValueMod, nAttAIndex, nAttANumMod = 0;

	while(nValueModLeft != 0)
	{
		++nAttANumMod;
		nValueMod = IUTIL->Rand() % nValueModLeft + 1;
		nAttAIndex = IUTIL->Rand() % X_ERA_ATTA_NUM;

		if(nAttANumMod >= pEquipQltyEffect->nAttANumEffect)
		{
			nValueMod = nValueModLeft;
		}

		pEquip->equipSpec.nRoleAttEffect[nAttAIndex] += nValueMod;

		nValueModLeft -= nValueMod;
	}

	// 装备潜力值 -- 原始编辑值(±10%浮动)×4
	fValueModFactor = (1.0f + FLOAT(IUTIL->Rand() % 21 - 10) / 100.0f) * pEquipQltyEffect->fPotFactor * (FLOAT)(pEquip->equipSpec.n16PotValModPct) / 10000.0f;
	pEquip->equipSpec.nPotVal = (INT)(pEquipProto->nPotVal * fValueModFactor);
	// Jason 2009-12-22 修正bug，当品级高时，会超过最大潜力值，比如橙色品级
	if( pEquip->equipSpec.nPotVal > pEquip->pEquipProto->nMaxPotVal )
		pEquip->equipSpec.nPotVal = pEquip->pEquipProto->nMaxPotVal;
	else if( pEquip->equipSpec.nPotVal < 0 )
		pEquip->equipSpec.nPotVal = 0;

	
	// 随机数[0,10000)
	INT32 nRandPct;
	
	// 镶嵌孔数量 -- 无孔(0%) 1孔(40%) 2孔(30%) 3孔(20%) 4孔(0%) 5孔(0%)
	pEquip->equipSpec.byHoleNum = 0;

	INT32 nHolePctSum = 0;
	nRandPct = IUTIL->Rand() % 10000;
	for(INT32 i=MAX_EQUIPHOLE_NUM; i>0; --i)
	{
		nHolePctSum += pEquipQltyEffect->nHoleNumPct[i];
		if(nRandPct < nHolePctSum)
		{
			pEquip->equipSpec.byHoleNum = i;
			break;
		}
	}

	// 龙魂能力	-- 	"50%几率出现表·龙魂能力, 25%几率出现里·龙魂能力"
	pEquip->equipSpec.dwLongHunInnerID	= GT_INVALID;
	pEquip->equipSpec.dwLongHunOuterID	= GT_INVALID;

	// .1表里只能随出一个 -- 先里或表
	nRandPct = IUTIL->Rand() % 10000;
	if(nRandPct < pEquipQltyEffect->nLonghunPct[ELHT_Inner])
	{
		pEquip->equipSpec.dwLongHunInnerID = GenLongHunRandID(ELHT_Inner, pEquipProto->eEquipPos, eQlty);
	}
	else
	{
		nRandPct = IUTIL->Rand() % 10000;
		if(nRandPct < pEquipQltyEffect->nLonghunPct[ELHT_Outer])
		{
			pEquip->equipSpec.dwLongHunOuterID = GenLongHunRandID(ELHT_Outer, pEquipProto->eEquipPos, eQlty);
		}
	}

	// 装备特殊属性 -- 5%几率1个B类属性
	nRandPct = IUTIL->Rand() % 10000;
	if(nRandPct < pEquipQltyEffect->nSpecAttPct)
	{
		nRandPct = IUTIL->Rand() % 10000;
		INT32 nSpecAttPctSum = 0;
		for(INT32 i=0; i<X_EQUIP_SPECATT_NUM; ++i)
		{
			nSpecAttPctSum += g_nEquipSpecAttPct[i];
			if(nRandPct < nSpecAttPctSum)
			{
				pEquip->equipSpec.bySpecAtt = i;
				break;
			}
		}

		if(pEquip->equipSpec.bySpecAtt <= (BYTE)EESA_EquipSpec_RelEnd)
		{
			// 计算特殊属性对装备属性的影响
			ProcEquipAttBySpecAtt(pEquip);
		}
		else
		{
			// 对装备强化影响或死亡掉落影响 -- 生产或调落时处理
		}
	}
}

//-----------------------------------------------------------------------------
// 计算特殊属性对装备属性的影响
//-----------------------------------------------------------------------------
VOID ItemCreator::ProcEquipAttBySpecAtt(tagEquip *pEquip)
{
	tagEquipSpec &equipSpec = pEquip->equipSpec;

	switch(equipSpec.bySpecAtt)
	{
	case EESA_LevelLim_Simple:
		// 简易:该装备等级限制-5，最低可减少至0
		equipSpec.byMinUseLevel = (equipSpec.byMinUseLevel > 5 ? equipSpec.byMinUseLevel - 5 : 0);
		break;
	case EESA_LevelLim_Fine:
		// 精简		该装备等级限制-10，最低可减少至0
		equipSpec.byMinUseLevel = (equipSpec.byMinUseLevel > 10 ? equipSpec.byMinUseLevel - 10 : 0);
		break;
	case EESA_LevelLim_None:
		// 无级别	该装备无等级限制
		equipSpec.byMinUseLevel = 0;
		break;

	case EESA_AttALim_Simple:
		// 轻便		该装备属性限制减少10%，取整
		equipSpec.n16AttALimModPct = -1000;
		break;
	case EESA_AttALim_Comfort:
		// 舒适		该装备属性限制减少25%，取整
		equipSpec.n16AttALimModPct = -2500;
		break;
	case EESA_AttALim_Light:
		// 轻盈		该装备属性限制减少50%，取整
		equipSpec.n16AttALimModPct = -5000;
		break;

	case EESA_Potential_YinFeng:
		// 隐凤		该装备的初始潜力值+200
		equipSpec.nPotVal = min(equipSpec.nPotVal + 200, pEquip->pEquipProto->nMaxPotVal);
		break;
	case EESA_Potential_YinHuang:
		// 隐凰		该装备的初始潜力值+400
		equipSpec.nPotVal = min(equipSpec.nPotVal + 400, pEquip->pEquipProto->nMaxPotVal);
		break;
	case EESA_Potential_FeiFeng:
		// 飞凤		该装备的初始潜力值+800
		equipSpec.nPotVal = min(equipSpec.nPotVal + 800, pEquip->pEquipProto->nMaxPotVal);
		break;
	case EESA_Potential_MingHuang:
		// 鸣凰		该装备的初始潜力值+1200
		equipSpec.nPotVal = min(equipSpec.nPotVal + 1200, pEquip->pEquipProto->nMaxPotVal);
		break;
	case EESA_Potential_WoLong:
		// 卧龙		装备的初始潜力值提高5%
		equipSpec.nPotVal = (INT32)min(equipSpec.nPotVal * 1.05, pEquip->pEquipProto->nMaxPotVal);
		break;
	case EESA_Potential_CangLong:
		// 藏龙		装备的初始潜力值提高10%
		equipSpec.nPotVal = (INT32)min(equipSpec.nPotVal * 1.1, pEquip->pEquipProto->nMaxPotVal);
		break;
	case EESA_Potential_FuLong:
		// 伏龙		装备的初始潜力值提高20%
		equipSpec.nPotVal = (INT32)min(equipSpec.nPotVal * 1.2, pEquip->pEquipProto->nMaxPotVal);
		break;
	case EESA_Potential_ShengLong:
		// 升龙		装备的初始潜力值提高30%
		equipSpec.nPotVal = (INT32)min(equipSpec.nPotVal * 1.3, pEquip->pEquipProto->nMaxPotVal);
		break;
	}
}

//-----------------------------------------------------------------------------
// 根据指定品级生成时装相关属性
//-----------------------------------------------------------------------------
BOOL ItemCreator::CreateFashionQltyRel(OUT tagEquip *pEquip, const tagEquipProto *pEquipProto, 
									EItemQuality eQlty, const tagIMEffect *pIMEffect/* = NULL*/)
{
	ASSERT(eQlty > EIQ_Start && eQlty < EIQ_End);

	// 获取时装生成相关资源
	const tagFashionGen *pGen = g_attRes.GetFashionQltyEffect(eQlty);
	if(!P_VALID(pGen))
	{
		MAttResCaution(_T("fashion_qlty_effect"), _T("Quality"), (INT)eQlty);
		return FALSE;
	}

	const tagFashionColorPct *pColor = g_attRes.GetFashionColorPct(eQlty);
	if(!P_VALID(pColor))
	{
		MAttResCaution(_T("fashion_color_pct"), _T("Quality"), (INT)eQlty);
		return FALSE;
	}

	// 保存随机数用
	INT nRand;
	
	// 颜色处理
	INT nColor		= EC_Null;
	INT nColorPct	= 0;
	if(pIMEffect != NULL && EIME_Color == pIMEffect->eEffect)
	{
		nColor		= (INT)pIMEffect->dwParam1;
		nColorPct	= (INT)pIMEffect->dwParam2;
	}

	// .0原型中有颜色，则用原型中颜色
	if(pEquipProto->dwColor <= EC_End)
	{
		pEquip->equipSpec.n8ColorID = (INT8)pEquipProto->dwColor;
	}
	// .1修正几率是否为100%
	else if(10000 == nColorPct && nColor >= EC_Start && nColor <= EC_End)
	{
		pEquip->equipSpec.n8ColorID = nColor;
	}
	// .2根据颜色表及修正率生成颜色
	else
	{
		INT nPctSum = 0;
		nRand = IUTIL->Rand() % 10000;
		pEquip->equipSpec.n8ColorID = EC_Val0;
		for(INT i=X_COLOR_NUM-1; i>=0; --i)
		{
			nPctSum += pColor->n16ColorPct[i];
			if(nColor == i)
			{
				nPctSum += nColorPct;
			}

			if(nRand < nPctSum)
			{
				pEquip->equipSpec.n8ColorID = i;
				break;
			}
		}
	}

	// 白装没有属性加成
	if(EIQ_White == eQlty)
	{
		return TRUE;
	}

	// 仪容属性 -- 原始编辑值×1.5
	if(ERA_Appearance == pEquipProto->BaseEffect[0].eRoleAtt)
	{
		pEquip->equipSpec.n16Appearance = (INT16)(pEquipProto->BaseEffect[0].nValue * (pGen->fAppearanceFactor - 1));
	}

	// 统御属性加成 -- 10%几率出现, 值=物品等级÷5[取整]
	nRand = IUTIL->Rand() % 10000;
	if(nRand < pGen->n16ReinPct && pGen->n8ReinVal > 0)
	{
		pEquip->equipSpec.byRein = pEquip->pEquipProto->byLevel / pGen->n8ReinVal;
	}

	// 悟性属性加成 -- 10%几率出现, 值=物品等级÷20[取整]
	nRand = IUTIL->Rand() % 10000;
	if(nRand < pGen->n16SavvyPct && pGen->n8SavvyVal > 0)
	{
		pEquip->equipSpec.bySavvy = pEquip->pEquipProto->byLevel / pGen->n8SavvyVal;
	}

	// 福缘属性加成 -- 1%几率出现, 值=5（上下浮动20%）+装备等级/30
	nRand = IUTIL->Rand() % 10000;
	if(nRand < pGen->n16FortunePct && pGen->n8FortuneVal2 > 0)
	{
		FLOAT fFactor = 1.0f + (FLOAT(IUTIL->Rand() % 41 - 20) / 100.0f);
		pEquip->equipSpec.byFortune 
			= (BYTE)(pGen->n8FortuneVal1 * fFactor + (DOUBLE)pEquipProto->byLevel / pGen->n8FortuneVal2);
	}

	// 时装均不可开凿
	pEquip->equipSpec.bCanCut = FALSE;

	return TRUE;
}

tagItem* ItemCreator::CreateTreasure(DWORD dwNameID, EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, INT16 n16Num /*= 1*/, DWORD dwCreator /*= GT_INVALID*/, INT16 n16QltyModPct /*= 0*/, INT16 n16QltyModPctEx /*= 0*/ )
{
	ASSERT(GT_VALID(dwNameID));
	tagItem* pNew = Create(eCreateMode, dwCreateID, dwTypeID, n16Num, dwCreator, n16QltyModPct, n16QltyModPctEx);
	if (P_VALID(pNew))
	{
		pNew->dwNameID = dwNameID;
	}	
	return pNew;
}

tagItem* ItemCreator::CreateTreasureEx(DWORD dwNameID, EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, 
								 INT16 n16Num/* = 1*/, EItemQuality eQlty/* = EIQ_Null*/, DWORD dwCreator/* = GT_INVALID*/, 
								 const tagIMEffect *pIMEffect/* = NULL*/)
{
	ASSERT(GT_VALID(dwNameID));
	tagItem* pNew = CreateEx(eCreateMode, dwCreateID, dwTypeID, n16Num, eQlty, dwCreator, pIMEffect);
	if (P_VALID(pNew))
	{
		pNew->dwNameID = dwNameID;
	}
	return pNew;
}

BOOL ItemCreator::IsGMItemNoInit(tagItem* pTmpItem)
{
	ASSERT(P_VALID(pTmpItem));

	if (EICT_Baibao == pTmpItem->eConType 
		&& !GT_VALID(pTmpItem->dwCreateID)
		&& !GT_VALID(pTmpItem->dwCreatorID))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL ItemCreator::InitGMItem(tagItem* pTmpItem)
{
	// 初始化物品属性

	// 初始化装备属性
	if (MIsEquipment(pTmpItem->dwTypeID))
	{
		MTRANS_POINTER(pEquip, pTmpItem, tagEquip);
		ItemCreator::IdentifyEquip(pEquip, EItemQuality(pEquip->equipSpec.byQuality));
	}

	return TRUE;
}
