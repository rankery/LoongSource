//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: LoongDBSave.cpp
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 数据库操作应用层实现 -- 保存数据相关
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/QuestDef.h"
#include "../WorldDefine/SocialDef.h"
#include "../WorldDefine/ItemDefine.h"
#include "../WorldDefine/RoleDefine.h"
#include "../WorldDefine/vcard_define.h"
#include "../ServerDefine/msg_item.h"
#include "../ServerDefine/role_data.h"
#include "../ServerDefine/clandata_define.h"
#include "../ServerDefine/item_define.h"
#include "../ServerDefine/activity_define.h"

#include "LoongDB.h"

//-------------------------------------------------------------------------------------------------------
// 删除物品装备相关定义
//-------------------------------------------------------------------------------------------------------
enum EDelItemTable
{
	EDIT_Item		= 0,	// item表
	EDIT_ItemBaiBao	= 1,	// item_baibao表
};

struct tagDelItem
{
	INT64	n64Serial;
	CHAR*	pTable;
};

struct tagNewItemEquip
{
	union
	{
		const tagItem*	pItem;
		const tagEquip* pEquip;
	};

	const CHAR*		pTable;
};

//#define MGET_TABLE(pTable, pDelItem)		\
//	char *pTable = "item";					\
//	if(EDIT_ItemBaiBao == pDelItem->eTable)	\
//		pTable = "item_baibao";

////-------------------------------------------------------------------------------------------------------
//// 删除玩家的buff，功夫，item/equip等.保存玩家数据前用于清除多余的记录
////-------------------------------------------------------------------------------------------------------
//VOID CLoongDB::DeleteRecord(DWORD dwRoleID, tagRecordDel *pRecord)
//{
//	/*************************************************************************************
//	* 依次删除各表中指定的记录: 技能列表，状态列表，物品列表，装备列表，任务列表
//	**************************************************************************************/
//	
//	BYTE *pData = pRecord->byData; // 记录当前指针位置
//
//	// 1.技能列表
//	Update(dwRoleID, pData, pRecord->nSkillNum, sizeof(DWORD), (&CLoongDB::FormatDelRecSkill));
//	pData += pRecord->nSkillNum * sizeof(DWORD);
//
//	// 2.状态列表
//	Update(dwRoleID, pData, pRecord->nBuffNum, sizeof(DWORD), (&CLoongDB::FormatDelRecBuff));
//	pData += pRecord->nBuffNum * sizeof(DWORD);
//
//	//// 4.物品列表
//	//Update(dwRoleID, pData, pRecord->nItemNum, sizeof(INT64), (&CLoongDB::FormatDelRecItem));
//	//pData += pRecord->nItemNum * sizeof(INT64);
//
//	//// 5.装备列表
//	//Update(dwRoleID, pData, pRecord->nEquipNum, sizeof(INT64), (&CLoongDB::FormatDelRecEquip));
//	//pData += pRecord->nEquipNum * sizeof(INT64);
//
//	// 6.任务列表
//	Update(dwRoleID, pData, pRecord->nQuestNum, sizeof(DWORD), (&CLoongDB::FormatDelRecTask));
//	pData += pRecord->nQuestNum * sizeof(DWORD);
//}


//-------------------------------------------------------------------------------------------------------
// 存储角色相关信息 -- 除需即时存储的，其他属性均在此处初始化存储数据
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::SaveRole(DWORD dwRoleID, tagRoleDataSave *pRoleData)
{
	// 保存角色基本属性信息
	SaveRoleData(dwRoleID, pRoleData);

	/**************************************************************************
	* 依次保存: 技能列表，状态列表，称号列表，物品列表，装备列表，
	*			好友列表，仇敌列表，任务列表，已完成任务列表, 物品冷却时间表
	***************************************************************************/

	LPVOID pData = pRoleData->byData; // 记录当前指针位置

	// 1.技能列表
	SaveSkill(dwRoleID, pData, pRoleData->nSkillNum, &pData);

	// 2.状态列表
	SaveRoleBuff(dwRoleID, pData, pRoleData->nBuffNum, &pData);

	// 4.称号列表
	SaveTitle(dwRoleID, pData, pRoleData->nTitleNum, &pData);

	// 5.物品列表 6.装备列表 // 分消息保存
	//SaveItemEquip(dwRoleID, pData, pRoleData->nItemNum, &pData);
	UpdateItemPosAndUseRel(dwRoleID, pData, pRoleData->nItemNum, &pData);

	// 7.好友列表 
	// 8.仇敌列表 // 这2个表中的数据是即时保存的

	// 9.任务列表
	SaveTask(dwRoleID, pData, pRoleData->nQuestNum, &pData);

	// 10.已完成任务列表
	//SaveTaskDone(dwRoleID, pData, pRoleData->nQuestDoneNum);

	// 11.物品冷却时间表
	ReplaceItemCDTime(dwRoleID, pData, pRoleData->nItemCDTimeNum, &pData);

	// 12.保存氏族数据
	SaveClanData(dwRoleID, pData, pRoleData->bClanDataChg, &pData);

	// 13.保存宠物
	SavePets(dwRoleID, pData, pRoleData->nPetsNum, &pData);

}


//********************************************************************************************************
//-------------------------------------------------------------------------------------------------------
// 插入新物品
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertItem(const tagItem &item, const CHAR* pTable)
{
	tagNewItemEquip sNewItem;
	sNewItem.pItem	= &item;
	sNewItem.pTable	= pTable;
	return Update(GT_INVALID, (LPVOID)&sNewItem, 1, SIZE_ITEM, (&CLoongDB::FormatInsertItem));
}

//-------------------------------------------------------------------------------------------------------
// 插入新装备专用属性
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertEquipSpec(const tagEquip &equip, const CHAR* pTable)
{
	tagNewItemEquip sNewItem;
	sNewItem.pEquip	= &equip;
	sNewItem.pTable	= pTable;
	return Update(GT_INVALID, (LPVOID)&sNewItem, 1, SIZE_EQUIP, (&CLoongDB::FormatInsertEquipSpec));
}

//-------------------------------------------------------------------------------------------------------
// 插入新装备 -- 装备要更新item表和equip表，需要保证数据的完整性
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertEquip(const tagEquip &equip, BOOL bBaiBao/* = FALSE*/)
{
	CHAR* pTableItem	= "item";
	CHAR* pTableEquip	= "equip";

	if(bBaiBao)
	{
		pTableItem	= "item_baibao";
		pTableEquip = "equip_baibao";
	}
	
	BOOL bRes = InsertItem(equip, pTableItem);
	if(!bRes)
	{
		return bRes;
	}

	bRes = InsertEquipSpec(equip, pTableEquip);
	if(!bRes)
	{
		// equip表插入失败，从item表中删除相应记录
		DelItem(equip.n64Serial, pTableItem);
	}

	return bRes;
}

//-------------------------------------------------------------------------------------------------------
// 物品
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveItem(const tagItem &item)
{
	return Update(GT_INVALID, (LPVOID)&item, 1, SIZE_ITEM, (&CLoongDB::FormatSaveItem));
}

//-------------------------------------------------------------------------------------------------------
// 装备专用属性
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveEquipSpec(const tagEquip &equip)
{
	return Update(GT_INVALID, (LPVOID)&equip, 1, SIZE_EQUIP, (&CLoongDB::FormatSaveEquipSpec));
}

//-------------------------------------------------------------------------------------------------------
// 装备专用属性
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateEquipSpec(const tagEquipSpecUpdate &equipSpecUpdate)
{
	return Update(GT_INVALID, (LPVOID)&equipSpecUpdate, 1, sizeof(tagEquipSpecUpdate), (&CLoongDB::FormatUpdateEquipSpec));
}


//-------------------------------------------------------------------------------------------------------
// 装备 -- 装备要更新item表和equip表，需要保证数据的完整性
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveEquip(const tagEquip &equip)
{
	BOOL bRes = SaveItem(equip);
	if(!bRes)
	{
		return bRes;
	}

	bRes = SaveEquipSpec(equip);
	if(!bRes)
	{
		// equip表插入失败，从item表中删除相应记录
		//DelItem(equip.n64Serial);//??
	}

	return bRes;
}

//-------------------------------------------------------------------------------------------------------
// 删除物品
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelItem(INT64 n64Serial, CHAR *pTable/* = "item"*/)
{
	tagDelItem delItem;
	delItem.n64Serial	= n64Serial;
	delItem.pTable		= pTable;
	return Update(GT_INVALID, (LPVOID)&delItem, 1, sizeof(INT64), (&CLoongDB::FormatDelItem));
}

//-------------------------------------------------------------------------------------------------------
// 从装备表中删除装备专用属性
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelEquipSpec(INT64 n64Serial, CHAR *pTable/* = "equip"*/)
{
	tagDelItem delItem;
	delItem.n64Serial	= n64Serial;
	delItem.pTable		= pTable;
	return Update(GT_INVALID, (LPVOID)&delItem, 1, sizeof(INT64), (&CLoongDB::FormatDelEquipSpec));
}

//-------------------------------------------------------------------------------------------------------
// 删除装备
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelEquip(INT64 n64Serial)
{
	if ( !(SaveDelEquipInfo(n64Serial) && SaveDelItemInfo(n64Serial)) )
		return FALSE;	

	if(DelItem(n64Serial))
	{
		return DelEquipSpec(n64Serial);
	}

	return FALSE;
}

BOOL CLoongDB::SaveDelEquipInfo(INT64 n64Serial)
{
	// 获取流和连接
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();
	ASSERT(P_VALID(pStream));
	ASSERT(P_VALID(pCon));

	// 删除操作
	pStream->SetCopyItem("equip_del", "*", "equip");
	pStream->SetWhere();
	pStream->FillString("SerialNum=") << n64Serial;

	// 执行
	BOOL bRet = m_pDBLoong->Execute(pStream);

	// 释放流和连接
	m_pDBLoong->ReturnConnection(pCon);
	m_pDBLoong->ReturnStream(pStream);

	return bRet;
}

BOOL CLoongDB::SaveDelItemInfo(INT64 n64Serial)
{
	// 获取流和连接
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();
	ASSERT(P_VALID(pStream));
	ASSERT(P_VALID(pCon));

	// 删除操作
	pStream->SetCopyItem("item_del", "*", "item");
	pStream->SetWhere();
	pStream->FillString("SerialNum=") << n64Serial;

	// 执行
	BOOL bRet = m_pDBLoong->Execute(pStream);

	// 释放流和连接
	m_pDBLoong->ReturnConnection(pCon);
	m_pDBLoong->ReturnStream(pStream);

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 保存角色属性信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveRoleData(DWORD dwRoleID, const tagRoleDataSave *pRoleData)
{
	return Update(dwRoleID, (LPVOID)pRoleData, 1, GT_INVALID, (&CLoongDB::FormatSaveRoleData));
}

//-------------------------------------------------------------------------------------------------------
// 保存氏族信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveClanData(DWORD dwRoleID, IN LPVOID pData, BOOL bChange, OUT LPVOID* ppOutPointer)
{
	if (!bChange)
	{
		*ppOutPointer = pData;
		return TRUE;
	}
	*ppOutPointer = reinterpret_cast<BYTE*>(pData) + sizeof(tagDBReputeData);
	return Update(dwRoleID, pData, 1, sizeof(tagDBReputeData), (&CLoongDB::FormatUpdateClanData));
}


//-------------------------------------------------------------------------------------------------------
// 保存物品信息
//-------------------------------------------------------------------------------------------------------
//BOOL CLoongDB::SaveItemEquip(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer)
//{
//	tagItem *pItem;
//	BYTE	*pCurPointer = (BYTE*)pData;
//	for(INT32 i=0; i<nNum; ++i)
//	{
//		pItem = (tagItem*)pCurPointer;
//		if(MIsEquipment(pItem->dwTypeID))
//		{
//			SaveEquip(*(tagEquip*)pItem);
//			pCurPointer += SIZE_EQUIP;
//		}
//		else
//		{
//			SaveItem(*pItem);
//			pCurPointer += SIZE_ITEM;
//		}
//	}
//
//	*ppOutPointer = pCurPointer;
//
//	return TRUE;
//}

//-------------------------------------------------------------------------------------------------------
// 更新物品位置及使用相关属性
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateItemPosAndUseRel(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer)
{
	return Update(dwRoleID, pData, nNum, sizeof(tagItemUpdate), (&CLoongDB::FormatUpdateItemPosAndUseRel), ppOutPointer);
}

//-------------------------------------------------------------------------------------------------------
// 插入角色技能信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertSkill(DWORD dwRoleID, tagSkillSave *pData)
{
	return Update(dwRoleID, pData, 1, sizeof(tagSkillSave), (&CLoongDB::FormatInsertSkill));
}

//-------------------------------------------------------------------------------------------------------
// 删除角色技能信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelSkill(DWORD dwRoleID, DWORD dwSkillID)
{
	return Update(dwRoleID, &dwSkillID, 1, sizeof(DWORD), (&CLoongDB::FormatDelSkill));
}

//-------------------------------------------------------------------------------------------------------
// 删除角色技能信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateSkill(DWORD dwRoleID, tagSkillSave *pData)
{
	LPVOID pSkillData = pData;
	return SaveSkill(dwRoleID, pSkillData, 1, &pSkillData);
}

//-------------------------------------------------------------------------------------------------------
// 保存角色技能信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveSkill(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer)
{
	return Update(dwRoleID, pData, nNum, sizeof(tagSkillSave), (&CLoongDB::FormatSaveSkill), ppOutPointer);
}

//-------------------------------------------------------------------------------------------------------
// 保存角色buff信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveRoleBuff(DWORD dwRoleID, LPVOID pBuff, INT32 nNum, OUT LPVOID *ppOutPointer)
{
	// 先删除buff
	Update(dwRoleID, NULL, 1, 0, (&CLoongDB::FormatDelAllRoleBuff));

	BYTE		*pCurPointer = (BYTE*)pBuff;
	tagBuffSave *pBuffSave;
	for(INT32 i=0; i<nNum; ++i)
	{
		pBuffSave = (tagBuffSave *)pCurPointer;
		Update(dwRoleID, pBuffSave, 1, sizeof(tagBuffSave), (&CLoongDB::FormatSaveRoleBuff));
		pCurPointer = pBuffSave->byData + pBuffSave->n8ModifierNum * sizeof(DWORD);
	}

	*ppOutPointer = pCurPointer;
	
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 保存称号信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveTitle(DWORD dwRoleID, LPVOID pData, INT32 nNum, OUT LPVOID *ppOutPointer)
{
	return Update(dwRoleID, pData, nNum, sizeof(tagTitleSave), (&CLoongDB::FormatSaveTitle), ppOutPointer);
}

//-------------------------------------------------------------------------------------------------------
// 接受任务
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertTask(DWORD dwRoleID, LPVOID pAcceptQuest)
{
	return Update(dwRoleID, pAcceptQuest, 1, sizeof(tagAcceptQuest), (&CLoongDB::FormatInsertTask));
}


//-------------------------------------------------------------------------------------------------------
// 保存任务信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveTask(DWORD dwRoleID, LPVOID pTask, INT32 nNum, OUT LPVOID *ppOutPointer)
{
	return Update(dwRoleID, pTask, nNum, sizeof(tagQuestSave), (&CLoongDB::FormatSaveTask), ppOutPointer);
}

//-------------------------------------------------------------------------------------------------------
// 保存已完成任务信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::SaveTaskDone(DWORD dwRoleID, LPVOID pTask, INT32 nNum)
{
	return Update(dwRoleID, pTask, nNum, sizeof(tagQuestDoneSave), (&CLoongDB::FormatSaveTaskDone));
}

//-------------------------------------------------------------------------------------------------------
// 创建角色名贴信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertVCard(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, 0, (&CLoongDB::FormatInsertVCard));
}

//-------------------------------------------------------------------------------------------------------
// 更新角色名贴信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateVCard(DWORD dwRoleID, const PVOID pData)
{
	return Update(dwRoleID, pData, 1, sizeof(tagVCardData), (&CLoongDB::FormatUpdateVCard));
}

//-------------------------------------------------------------------------------------------------------
// 删除角色名贴信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelVCard(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, 0, (&CLoongDB::FormatDelVCard));
}

//-------------------------------------------------------------------------------------------------------
// 创建氏族信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertClanData(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, 0, (&CLoongDB::FormatInsertClanData));
}

//-------------------------------------------------------------------------------------------------------
// 更新氏族信息
//-------------------------------------------------------------------------------------------------------
// BOOL CLoongDB::UpdateClanData(DWORD dwRoleID, const PVOID pData)
// {
// 	return Update(dwRoleID, pData, 1, sizeof(tagDBReputeData), (&CLoongDB::FormatUpdateClanData));
// }

//-------------------------------------------------------------------------------------------------------
// 删除氏族信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DelClanData(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, 0, (&CLoongDB::FormatDelClanData));
}

BOOL CLoongDB::SaveActivityData(DWORD dwID, LPVOID pActivityData)
{
	return Update(dwID, pActivityData, 1, sizeof(tagActivityData), (&CLoongDB::FormatSaveActivity));
}

//********************************************************************************************************
//-------------------------------------------------------------------------------------------------------
// 插入新物品
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertItem(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(pEx, pData, tagNewItemEquip);
	const tagItem *p = pEx->pItem;

	pStream->SetInsert(pEx->pTable);

	pStream->FillString("SerialNum=") << p->n64Serial;
	pStream->FillString(",TypeID=") << p->dwTypeID;

	pStream->FillString(",Num=") << p->n16Num;
	pStream->FillString(",Bind=") << p->byBind;
	pStream->FillString(",LockStat=") << p->bLock;
	pStream->FillString(",UseTimes=") << p->nUseTimes;
	
	pStream->FillString(",CreateMode=") << p->eCreateMode;
	pStream->FillString(",CreateID=") << p->dwCreateID;

	pStream->FillString(",CreatorID=") << p->dwCreatorID;
	pStream->FillString(",NameID=") << p->dwNameID;

	DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->dwCreateTime);
	pStream->FillString(",CreateTime='").FillString(m_szTime);
	pStream->FillString("'");

	DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->dw1stGainTime);
	pStream->FillString(",FirstGainTime='").FillString(m_szTime);
	pStream->FillString("'");

	pStream->FillString(",OwnerID=") << p->dwOwnerID;
	pStream->FillString(",AccountID=") << p->dwAccountID;
	pStream->FillString(",ContainerTypeID=") << p->eConType;
	pStream->FillString(",Suffix=") << p->n16Index;
}

//-------------------------------------------------------------------------------------------------------
// 格式化装备属性通用部分(insert,update)
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatCommonEquipSpec(Beton::MyStream *pStream, const tagEquipSpec *pEquipSpec)
{
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->FillString("Quality=") << pEquipSpec->byQuality;
	pStream->FillString(",MinUseLevel=") << pEquipSpec->byMinUseLevel;
	pStream->FillString(",MaxUseLevel=") << pEquipSpec->byMaxUseLevel;
	pStream->FillString(",AttALimMod=") << pEquipSpec->n16AttALimMod;
	pStream->FillString(",AttALimModPct=") << pEquipSpec->n16AttALimModPct;

	pStream->FillString(",WuHun=") << pEquipSpec->n16WuHun;
	pStream->FillString(",MinDmg=") << pEquipSpec->n16MinDmg;
	pStream->FillString(",MaxDmg=") << pEquipSpec->n16MaxDmg;
	pStream->FillString(",Armor=") << pEquipSpec->n16Armor;
	pStream->FillString(",PotVal=") << pEquipSpec->nPotVal;
	pStream->FillString(",PotValUsed=") << pEquipSpec->nPotValUsed;
	// Jason 2009-12-1装备潜力值提升次数，默认0
	pStream->FillString(",PotIncTimes=") << pEquipSpec->nPotIncTimes;
	// Jason 2009-12-2 补充装备紫色品级鉴定几率，默认0
	pStream->FillString  (",PurpleQltyIdfPct=") << pEquipSpec->dwPurpleQltyIdfPct;

	pStream->FillString(",PosyTimes=") << pEquipSpec->byPosyTimes;
	pStream->FillString(",EngraveTimes=") << pEquipSpec->byEngraveTimes;
	pStream->FillString(",HoleNum=") << pEquipSpec->byHoleNum;
	pStream->FillString(",BrandLevel=") << pEquipSpec->byBrandLevel;
	pStream->FillString(",LongFuLevel=") << pEquipSpec->byLongfuLevel;

	pStream->FillString(",CanCut=") << pEquipSpec->bCanCut;
	pStream->FillString(",LongInnerID=") << pEquipSpec->dwLongHunInnerID;
	pStream->FillString(",LongOuterID=") << pEquipSpec->dwLongHunOuterID;
	pStream->FillString(",SpecAtt=") << pEquipSpec->bySpecAtt;
	pStream->FillString(",FlareVal=") << pEquipSpec->byFlareVal;
	pStream->FillString(",QltyModPct=") << pEquipSpec->n16QltyModPct;
	pStream->FillString(",QltyModPctEx=") << pEquipSpec->n16QltyModPctEx;
	pStream->FillString(",PotValModPct=") << pEquipSpec->n16PotValModPct;

	// 时装属性
	pStream->FillString(",Appearance=") << pEquipSpec->n16Appearance;
	pStream->FillString(",Rein=") << pEquipSpec->byRein;
	pStream->FillString(",Savvy=") << pEquipSpec->bySavvy;
	pStream->FillString(",Fortune=") << pEquipSpec->byFortune;
	pStream->FillString(",ColorID=") << pEquipSpec->n8ColorID;

	pStream->FillString(",RoleAttEffect='").FillBlob(pEquipSpec->nRoleAttEffect, sizeof(pEquipSpec->nRoleAttEffect), pCon);
	pStream->FillString("',PosyEffect='").FillBlob(pEquipSpec->PosyEffect, sizeof(pEquipSpec->PosyEffect), pCon);
	pStream->FillString("',EngraveAtt='").FillBlob(pEquipSpec->nEngraveAtt, sizeof(pEquipSpec->nEngraveAtt), pCon);
	pStream->FillString("',HoleGemID='").FillBlob(pEquipSpec->dwHoleGemID, sizeof(pEquipSpec->dwHoleGemID), pCon);
	pStream->FillString("',DerateVal='").FillBlob(pEquipSpec->nBrandAtt, sizeof(pEquipSpec->nBrandAtt), pCon);
	pStream->FillString("',WuXing='").FillBlob(pEquipSpec->byLongfuAtt, sizeof(pEquipSpec->byLongfuAtt), pCon);
	pStream->FillString("'");

	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 插入新装备
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertEquipSpec(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagNewItemEquip);

	pStream->SetInsert(p->pTable);
	
	pStream->FillString("SerialNum=") << p->pEquip->n64Serial;
	pStream->FillString(",");
	FormatCommonEquipSpec(pStream, &p->pEquip->equipSpec);
}

//-------------------------------------------------------------------------------------------------------
// 保存角色的物品信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveItem(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pItem)
{
	MTRANS_POINTER(p, pItem, tagItem);

	pStream->SetUpdate("item");

	//pStream->FillString("SerialNum=") << p->n64Serial;
	//pStream->FillString(",TypeID=") << p->dwTypeID;

	pStream->FillString("Num=") << p->n16Num;
	pStream->FillString(",Bind=") << p->byBind;
	pStream->FillString(",LockStat=") << p->bLock;
	pStream->FillString(",UseTimes=") << p->nUseTimes;
	
	//pStream->FillString(",CreateMode=") << p->eCreateMode;
	//pStream->FillString(",CreateID=") << p->dwCreateID;

	//pStream->FillString(",CreatorID=") << p->dwCreatorID;

	//DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->dwCreateTime);
	//pStream->FillString(",CreateTime='").FillString(m_szTime);
	//pStream->FillString("'");

	DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->dw1stGainTime);
	pStream->FillString(",FirstGainTime='").FillString(m_szTime);
	pStream->FillString("'");

	pStream->FillString(",OwnerID=") << p->dwOwnerID;
	pStream->FillString(",AccountID=") << p->dwAccountID;
	pStream->FillString(",ContainerTypeID=") << p->eConType;
	pStream->FillString(",Suffix=") << p->n16Index;

	pStream->SetWhere();
	pStream->FillString("SerialNum=") << p->n64Serial;
}

//-------------------------------------------------------------------------------------------------------
// 保存装备信息 -- 修改equip表
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveEquipSpec(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pEquip)
{
	MTRANS_POINTER(p, pEquip, tagEquip);

	pStream->SetUpdate("equip");
	FormatCommonEquipSpec(pStream, &p->equipSpec);

	pStream->SetWhere();
	pStream->FillString("SerialNum=") << p->n64Serial;
}

//-------------------------------------------------------------------------------------------------------
// 更新装备属性
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateEquipSpec(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pEquipSpecUpdate)
{
	MTRANS_POINTER(p, pEquipSpecUpdate, tagEquipSpecUpdate);

	pStream->SetUpdate("equip");
	FormatCommonEquipSpec(pStream, &p->equipSpec);

	pStream->SetWhere();
	pStream->FillString("SerialNum=") << p->n64Serial;
}

//-------------------------------------------------------------------------------------------------------
// 保存角色属性信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDelItem(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagDelItem);

	pStream->SetDelete(p->pTable);

	pStream->SetWhere();
	pStream->FillString("SerialNum=") << p->n64Serial;
}

//-------------------------------------------------------------------------------------------------------
// 保存角色属性信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDelEquipSpec(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagDelItem);

	pStream->SetDelete(p->pTable);

	pStream->SetWhere();
	pStream->FillString("SerialNum=") << p->n64Serial;
}


//-------------------------------------------------------------------------------------------------------
// 保存角色属性信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveRoleData(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	tagRoleDataSave *p = (tagRoleDataSave *)pData;

	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();
	
	pStream->SetUpdate("roledata");
	pStream->FillString("AvatarEquip='").FillBlob(&p->AvatarEquip, SIZE_AVATAR_EQUIP, pCon);
	pStream->FillString("',DisplaySet=")<< (BYTE)p->DisplaySet;
	pStream->FillString(",MapID=") << p->dwMapID;
	pStream->FillString(",X=") << p->fCoordinate[0];// 坐标
	pStream->FillString(",Y=") << p->fCoordinate[1];
	pStream->FillString(",Z=") << p->fCoordinate[2];
	pStream->FillString(",FaceX=") << p->fFace[0];// 朝向
	pStream->FillString(",FaceY=") << p->fFace[1];
	pStream->FillString(",FaceZ=") << p->fFace[2];

	pStream->FillString(",RebornMapID=") << p->dwRebornMapID;

	pStream->FillString(",Class=") << (INT32)p->eClass;
	pStream->FillString(",ClassEx=") << p->eClassEx;
	pStream->FillString(",Level=") << p->nLevel;
	pStream->FillString(",ExpCurLevel=") << p->nCurExp;

	pStream->FillString(",HP=") << p->nHP;
	pStream->FillString(",MP=") << p->nMP;
	pStream->FillString(",Rage=") << p->nRage;
	pStream->FillString(",Endurance=") << p->nEndurance;
	pStream->FillString(",Vitality=") << p->nVitality;
	pStream->FillString(",Injury=") << p->nInjury;
	pStream->FillString(",Knowledge=") << p->nKnowledge;
	pStream->FillString(",Morale=") << p->nMorale;
	pStream->FillString(",Morality=") << p->nMorality;
	pStream->FillString(",Culture=") << p->nCulture;
	pStream->FillString(",Hostility=") << p->nHostility;
	pStream->FillString(",Credit=") << p->nCredit;
	pStream->FillString(",Identity=") << p->nIdentity;
	pStream->FillString(",VIPPoint=") << p->nVIPPoint;

	pStream->FillString(",AttPtAvail=") << p->nAttPoint;
	pStream->FillString(",TalentPtAvail=") << p->nTalentPoint;

	pStream->FillString(",Strength=") << (p->Strength.m_dwHeroism + p->Strength.m_dwWit + p->Strength.m_dwErrantry + p->Strength.m_dwValor);

	pStream->FillString(",PhysiqueAdded=") << p->nAttPointAdd[ERA_Physique];
	pStream->FillString(",StrengthAdded=") << p->nAttPointAdd[ERA_Strength];
	pStream->FillString(",PneumaAdded=") << p->nAttPointAdd[ERA_Pneuma];
	pStream->FillString(",InnerforceAdded=") << p->nAttPointAdd[ERA_InnerForce];
	pStream->FillString(",TechniqueAdded=") << p->nAttPointAdd[ERA_Technique];
	pStream->FillString(",AgilityAdded=") << p->nAttPointAdd[ERA_Agility];

	pStream->FillString(",TalentType1=") << p->talent[0].eType;
	pStream->FillString(",TalentType2=") << p->talent[1].eType;
	pStream->FillString(",TalentType3=") << p->talent[2].eType;
	pStream->FillString(",TalentType4=") << p->talent[3].eType;
	pStream->FillString(",TalentVal1=") << p->talent[0].nPoint;
	pStream->FillString(",TalentVal2=") << p->talent[1].nPoint;
	pStream->FillString(",TalentVal3=") << p->talent[2].nPoint;
	pStream->FillString(",TalentVal4=") << p->talent[3].nPoint;

	pStream->FillString(",SGFlag=") << p->bSafeGuard;
	
	pStream->FillString(",BagSize=") << p->n16BagSize;		// 背包
	pStream->FillString(",BagGold=") << p->nBagGold;
	pStream->FillString(",BagSilver=") << p->nBagSilver;
	pStream->FillString(",BagYuanBao=") << p->nBagYuanBao;
	pStream->FillString(",ExchangeVolume=") << p->nExVolume;

	//pStream->FillString(",WareSize=") << p->n16WareSize;		// 账号仓库
	//pStream->FillString(",WareGold=") << p->nWareGold;
	//pStream->FillString(",WareSilver=") << p->nWareSilver;
	//pStream->FillString(",WareYuanBao=") << p->nWareYuanBao;

	pStream->FillString(",TotalTax=") << p->nTotalTax;	// 缴税总额
	pStream->FillString(",RemoteOpenSet=") << (DWORD)p->sRemoteOpenSet;
	pStream->FillString(",CurTitleID=") << p->u16ActiveTitleID;

	if(DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->LoginTime))
	{
		pStream->FillString(",LoginTime='").FillString(m_szTime);
		pStream->FillString("'");

		DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->LogoutTime);
		pStream->FillString(",LogoutTime='").FillString(m_szTime);
		pStream->FillString("'");
		
		DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->CloseSafeGuardTime);
		pStream->FillString(",CloseSGTime='").FillString(m_szTime);
		pStream->FillString("'");

		DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->dwTimeGetMallFree);
		pStream->FillString(",GetMallFreeTime='").FillString(m_szTime);
		pStream->FillString("'");
	}
	else
	{
		IMSG(_T("Error: Func DwordTime2DataTime() run error in CLoongDB::FormatSaveRoleData()!!!!!\r\n"));
		ASSERT(0);
	}

	pStream->FillString(",OnlineTime=") << p->nOnlineTime;
	pStream->FillString(",CurOnlineTime=") << p->nCurOnlineTime;

	pStream->FillString(",ScriptData='");
	pStream->FillBlob(p->Data.dwData, sizeof(DWORD)*ESD_Role, pCon);
	pStream->FillString("'");

	pStream->FillString(",StallLevel=") << p->byStallLevel;
	pStream->FillString(",StallDailyExp=") << p->nStallDailyExp;
	pStream->FillString(",StallCurExp=") << p->nStallCurExp;
	pStream->FillString(",StallLastTime=") << p->dwLastStallTime;

	pStream->FillString(",DeadUnSetSafeGuardCountDown=") << p->nDeadUnSetSafeGuardCountDown;
	pStream->FillString(",NeedPrisonRevive=") << p->bNeedPrisonRevive;

	pStream->FillString(",LoverID=") << p->dwLoverID;
	pStream->FillString(",HaveWedding=") << p->bHaveWedding;

	// Jason 2009-12-7 增加神龙赐福相关字段
	pStream->FillString(",LastLessingTime=") << (DWORD)p->timeLastLessing;
	pStream->FillString(",LastLessingLevel=") << p->nLessingLevel;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;

	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 创建角色名贴信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertVCard(Beton::MyStream *pStream, DWORD dwRoleID, PVOID pDum)
{
	ASSERT(!P_VALID(pDum));

	pStream->SetInsert("visiting_card");
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 更新角色名贴信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateVCard(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagVCardData);
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetUpdate("visiting_card");

	pStream->FillString("Level=") << (INT32)p->dwLevel;
	pStream->FillString(",Job=") << (INT32)p->dwJob;
	pStream->FillString(",MateRoleID=") << (INT32)p->dwMateRoleID;
	pStream->FillString(",FactionID=") << (INT32)p->dwFactionID;
	pStream->FillString(",Position=") << (INT32)p->dwPosition;

	pStream->FillString(",Visibility=") << p->customVCardData.bVisibility;
	pStream->FillString(",Sex=") << p->customVCardData.bySex;
	pStream->FillString(",Constellation=") << p->customVCardData.byConstellation;
	pStream->FillString(",ChineseZodiac=") << p->customVCardData.byChineseZodiac;
	pStream->FillString(",Area=") << p->customVCardData.byArea;
	pStream->FillString(",Province=") << p->customVCardData.byProvince;
	pStream->FillString(",City='").FillString(p->customVCardData.chCity, pCon).FillString("'");
	pStream->FillString(",HeadUrl='").FillString(reinterpret_cast<TCHAR *>(p->customVCardData.byData), pCon).FillString("'");
	pStream->FillString(",Signature='").FillString(reinterpret_cast<TCHAR *>(&(p->customVCardData.byData[p->customVCardData.byHeadUrlSize])), pCon).FillString("'");

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;

	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 更新角色名贴信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDelVCard(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum)
{
	ASSERT(!P_VALID(pDum));
	pStream->SetDelete("visiting_card");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 创建角色名贴信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertClanData(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum)
{
	ASSERT(!P_VALID(pDum));
	
	pStream->SetInsert("clan_data");
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 更新角色名贴信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateClanData(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	ASSERT(P_VALID(pData));
	MTRANS_POINTER(pDBRepData, pData, tagDBReputeData);

	pStream->SetUpdate("clan_data");

	pStream->FillString("RepXuanYuan=")		<< pDBRepData->m_nReputation[ECLT_XuanYuan];
	pStream->FillString(",ConXuanYuan=")	<< pDBRepData->m_nContribution[ECLT_XuanYuan];
	pStream->FillString(",ActCntXuanYuan=")	<< pDBRepData->m_n8ActCount[ECLT_XuanYuan];

	pStream->FillString(",RepShenNong=")	<< pDBRepData->m_nReputation[ECLT_ShenNong];
	pStream->FillString(",ConShenNong=")	<< pDBRepData->m_nContribution[ECLT_ShenNong];
	pStream->FillString(",ActCntShenNong=")	<< pDBRepData->m_n8ActCount[ECLT_ShenNong];

	pStream->FillString(",RepFuXi=")		<< pDBRepData->m_nReputation[ECLT_FuXi];
	pStream->FillString(",ConFuXi=")		<< pDBRepData->m_nContribution[ECLT_FuXi];
	pStream->FillString(",ActCntFuXi=")		<< pDBRepData->m_n8ActCount[ECLT_FuXi];

	pStream->FillString(",RepSanMiao=")		<< pDBRepData->m_nReputation[ECLT_SanMiao];
	pStream->FillString(",ConSanMiao=")		<< pDBRepData->m_nContribution[ECLT_SanMiao];
	pStream->FillString(",ActCntSanMiao=")	<< pDBRepData->m_n8ActCount[ECLT_SanMiao];

	pStream->FillString(",RepJiuLi=")		<< pDBRepData->m_nReputation[ECLT_JiuLi];
	pStream->FillString(",ConJiuLi=")		<< pDBRepData->m_nContribution[ECLT_JiuLi];
	pStream->FillString(",ActCntJiuLi=")	<< pDBRepData->m_n8ActCount[ECLT_JiuLi];

	pStream->FillString(",RepYueZhi=")		<< pDBRepData->m_nReputation[ECLT_YueZhi];
	pStream->FillString(",ConYueZhi=")		<< pDBRepData->m_nContribution[ECLT_YueZhi];
	pStream->FillString(",ActCntYueZhi=")	<< pDBRepData->m_n8ActCount[ECLT_YueZhi];

	pStream->FillString(",RepNvWa=")		<< pDBRepData->m_nReputation[ECLT_NvWa];
	pStream->FillString(",ConNvWa=")		<< pDBRepData->m_nContribution[ECLT_NvWa];
	pStream->FillString(",ActCntNvWa=")		<< pDBRepData->m_n8ActCount[ECLT_NvWa];

	pStream->FillString(",RepGongGong=")	<< pDBRepData->m_nReputation[ECLT_GongGong];
	pStream->FillString(",ConGongGong=")	<< pDBRepData->m_nContribution[ECLT_GongGong];
	pStream->FillString(",ActCntGongGong=")	<< pDBRepData->m_n8ActCount[ECLT_GongGong];

	pStream->FillString(",FameMask=")		<< pDBRepData->m_u16FameMask;
	pStream->FillString(",RepRstTimeStamp=")<< pDBRepData->m_dwtLastResetTime;

	pStream->SetWhere();
	pStream->FillString("RoleID=")			<< dwRoleID;

}

//-------------------------------------------------------------------------------------------------------
// 删除角色名贴信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDelClanData(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDum)
{
	ASSERT(!P_VALID(pDum));

	pStream->SetDelete("clan_data");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 插入角色技能信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateItemPosAndUseRel(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagItemUpdate);

	pStream->SetUpdate("item");
	pStream->FillString("Num=") << p->n16Num;
	pStream->FillString(",UseTimes=") << p->nUseTimes;
	pStream->FillString(",OwnerID=") << p->dwOwnerID;
	pStream->FillString(",AccountID=") << p->dwAccountID;
	pStream->FillString(",ContainerTypeID=") << p->byConType;
	pStream->FillString(",Suffix=") << p->n16Index;
	pStream->FillString(",Bind=") << p->byBindType;
	pStream->SetWhere();
	pStream->FillString("SerialNum=") << p->n64Serial;
}

//-------------------------------------------------------------------------------------------------------
// 插入角色技能信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertSkill(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagSkillSave);

	pStream->SetInsert("skill");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",ID=") << p->dwID;
	pStream->FillString(",BiddenLevel=") << p->nLearnLevel;
	pStream->FillString(",SelfLevel=") << p->nSelfLevel;
	pStream->FillString(",Proficiency=") << p->nProficiency;
	pStream->FillString(",CoolDown=") << p->nCoolDown;
}

//-------------------------------------------------------------------------------------------------------
// 插入角色技能信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDelSkill(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetDelete("skill");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and ID=") << (*(DWORD*)pData);
}

//-------------------------------------------------------------------------------------------------------
// 保存角色技能信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveSkill(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagSkillSave);

	pStream->SetReplace("skill");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",ID=") << p->dwID;
	pStream->FillString(",BiddenLevel=") << p->nLearnLevel;
	pStream->FillString(",SelfLevel=") << p->nSelfLevel;
	pStream->FillString(",Proficiency=") << p->nProficiency;
	pStream->FillString(",CoolDown=") << p->nCoolDown;
}

//-------------------------------------------------------------------------------------------------------
// 保存角色buff信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveRoleBuff(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pBuff)
{
	MTRANS_POINTER(p, pBuff, tagBuffSave);

	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetInsert("buff");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",SrcUnitID=") << p->dwSrcUnitID;
	pStream->FillString(",SrcSkillID=") << p->dwSrcSkillID;
	pStream->FillString(",ItemTypeID=") << p->dwItemTypeID;
	pStream->FillString(",ItemSerialID=") << p->n64Serial;

	pStream->FillString(",BuffID=") << p->dwBuffID;
	pStream->FillString(",CurTick=") << p->nPersistTick;
	pStream->FillString(",Level=") << p->n8Level;
	pStream->FillString(",CurLapTimes=") << p->n8CurLapTimes;

	if(p->n8ModifierNum > 0)
	{
		pStream->FillString(",EffectSkillID='");
		pStream->FillBlob(p->byData, p->n8ModifierNum * sizeof(DWORD), pCon);
		pStream->FillString("'");
	}

	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 保存称号信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveTitle(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagTitleSave);

	pStream->SetUpdate("title");
	pStream->FillString("Count=") << p->dwStateMark;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and ID=") << p->u16TitleID;
}

//-------------------------------------------------------------------------------------------------------
// 保存任务信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveTask(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTask)
{
	tagQuestSave *p = (tagQuestSave *)pTask;

	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetReplace("task");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",TaskID=") << p->u16QuestID;
	pStream->FillString(",TaskFlag=") << p->dwQuestFlag;
	pStream->FillString(",StartTime=") << p->dwAcceptTime;

	for(INT32 i=0; i<QUEST_CREATURES_COUNT; ++i)
	{
		pStream->FillString(",Monster") << i;
		pStream->FillString("Num=") << (INT16)p->n16MonsterNum[i];
	}

	pStream->FillString(",ScriptData='");
	pStream->FillBlob(p->Data.dwData, sizeof(DWORD)*ESD_Quest, pCon);
	pStream->FillString("'");

	if(p->DynamicTarget.eTargetType != EQTT_NULL)
	{
		pStream->FillString(",DynamicTarget='");
		pStream->FillBlob(&p->DynamicTarget, sizeof(tagQuestDynamicTarget), pCon);
		pStream->FillString("'");
	}
	
	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 保存已完成任务信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveTaskDone(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTask)
{
	tagQuestDoneSave *p = (tagQuestDoneSave *)pTask;

	pStream->SetReplace("task_done");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",TaskID=") << p->u16QuestID;
	pStream->FillString(",StartTime=") << p->dwStartTime;
	pStream->FillString(",Times=") << p->nTimes;
}

//-------------------------------------------------------------------------------------------------------
// 从状态表中删除指定记录
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDelAllRoleBuff(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDummy)
{
	pStream->SetDelete("buff");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 保存固定活动脚本数据
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSaveActivity(Beton::MyStream *pStream, DWORD dwID, LPVOID pActivityData)
{
	tagActivityData *p = (tagActivityData *)pActivityData;
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();
	pStream->SetReplace("activity");
	pStream->FillString("dwID=") << dwID;
	pStream->FillString(",ScriptData='");
	pStream->FillBlob(&p->Data.dwData, sizeof(DWORD)*ESD_Activity, pCon);
	pStream->FillString("'");
	m_pDBLoong->ReturnConnection(pCon);
}
////-------------------------------------------------------------------------------------------------------
//// 从物品表中删除指定记录
////-------------------------------------------------------------------------------------------------------
//VOID CLoongDB::FormatDelRecItem(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
//{
//	INT64 *p = (INT64 *)pData;
//
//	pStream->SetDelete("item");
//	pStream->SetWhere();
//	pStream->FillString("RoleID=") << dwRoleID;
//	pStream->FillString(" and SerialNum=") << *p;
//}
//
////-------------------------------------------------------------------------------------------------------
//// 从装备表中删除指定记录
////-------------------------------------------------------------------------------------------------------
//VOID CLoongDB::FormatDelRecEquip(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
//{
//	INT64 *p = (INT64 *)pData;
//
//	pStream->SetDelete("equip");
//	pStream->SetWhere();
//	pStream->FillString("RoleID=") << dwRoleID;
//	pStream->FillString(" and SerialNum=") << *p;
//}
//
////-------------------------------------------------------------------------------------------------------
//// 从任务表中删除指定记录
////-------------------------------------------------------------------------------------------------------
//VOID CLoongDB::FormatDelRecTask(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
//{
//	DWORD *p = (DWORD *)pData;
//
//	pStream->SetDelete("task");
//	pStream->SetWhere();
//	pStream->FillString("RoleID=") << dwRoleID;
//	pStream->FillString(" and TaskID=") << *p;
//}

