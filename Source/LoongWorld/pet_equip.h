//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_equip.h
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物装备
//-------------------------------------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/container_define.h"
#include "../WorldDefine/pet_equip_define.h"
#include "container_template.h"
#include "time_limit_mgr.h"

class PetSoul;
class PetSkill;
struct tagItem;

const INT PET_EQUIP_UPDATE_TIME = 60 * TICK_PER_SECOND;

typedef	Container<tagItem> PetEquipContainer;

//-------------------------------------------------------------------------
// 宠物装备栏
//-------------------------------------------------------------------------
class PetEquipBar: public PetEquipContainer
{
public:
	//---------------------------------------------------------------------
	// 构造与析构
	//---------------------------------------------------------------------
	PetEquipBar(PetSoul* pSoul);
	~PetEquipBar(){}

	//---------------------------------------------------------------------
	// 获得装备
	//---------------------------------------------------------------------
	tagItem*		GetFromRoleBag(INT64 n64ItemID);
	tagItem*		GetFromPet(INT64 n64ItemID);
	VOID			FillClientEquipData(tagPetEquipMsgInfo* pMsgInfo, tagItem* pItem);
	BOOL			HasEquip();
	BOOL			IsEquipExist(INT64 n64Id);

	//---------------------------------------------------------------------
	// 换装与卸装
	//---------------------------------------------------------------------
	DWORD			Equip(INT64 n64ItemID, INT8 n8DstPos, BOOL bSend = FALSE);
	DWORD			UnEquip(INT64 n64ItemID, INT16 n16DstPos, BOOL bSend = FALSE);
	DWORD			SwapEquipPos(INT64 n64ItemID, INT8 n8DstPos, BOOL bSend = FALSE);

	//---------------------------------------------------------------------
	// 更新
	//---------------------------------------------------------------------
	VOID			AddEquip( tagItem* p2Equip, INT16 n16DstPos, BOOL bOnLoad = FALSE);
	VOID			Update();
	VOID			SaveToDB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum);

private:
	//---------------------------------------------------------------------
	// 计算装备效果
	//---------------------------------------------------------------------
	VOID			CalcEffect(PetSoul* pSoul, DWORD dwPetEquipTypeID, BOOL bEquip = TRUE, BOOL bSend = FALSE );
	VOID			DisablePetSkill(std::list<PetSkill*> &listSkill);
	VOID			EnablePetSkill(std::list<PetSkill*> &listSkill);

	//---------------------------------------------------------------------
	// 数据库操作
	//---------------------------------------------------------------------
	VOID			InsertDB( tagItem* p2Equip );
	VOID			RemoveDB( tagItem* p2UnEquip );

	//---------------------------------------------------------------------
	// 其它
	//---------------------------------------------------------------------
	const tagPetEquipProto* GetPetEquipProto( tagItem* pItem );

private:
	TimeLimitMgr<INT64>	m_TimeLimitMgr;	// 时限管理器
	PetSoul*		m_pSoul;			// 宠物灵魂
};