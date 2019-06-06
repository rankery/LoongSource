//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_equip.cpp
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物装备
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "pet_equip.h"

#include "../WorldDefine/msg_pet.h"
#include "../WorldDefine/msg_pet_equip.h"

#include "../ServerDefine/msg_item.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/msg_pet.h"

#include "pet_soul.h"
#include "pet_skill.h"
#include "pet_att.h"
#include "role.h"
#include "item_mgr.h"

//----------------------------------------------------------------------------------------------------
// 通过64位ID来装备物品（物品在玩家背包中未被移除）
//----------------------------------------------------------------------------------------------------
DWORD PetEquipBar::Equip( INT64 n64ItemID, INT8 n8DstPos, BOOL bSend /*= FALSE*/ )
{
	INT16 n16DstPos = (INT16)n8DstPos;

 	MTRANS_ELSE_RET(pMaster, m_pSoul->GetMaster(), Role, E_Pets_PetEquip_Master_NotExist);

	// 玩家背包中获取物品
	MTRANS_ELSE_RET(p2Equip, GetFromRoleBag(n64ItemID), tagItem, E_Pets_PetEquip_PetEquipNotInBag);

	// 获得原型
	MTRANS_ELSE_RET(p2EquipPetEquipProto, GetPetEquipProto(p2Equip), const tagPetEquipProto, E_Pets_PetEquip_UnknownEquipProto)

	// 背包中移除
	pMaster->GetItemMgr().TakeOutFromBag(n64ItemID, ELCLD_PET_ITEM_PET_EQUIP, TRUE);

	//对于装备栏每个位置装备
	for (INT16 i=0; i<PetEquipContainer::GetCurSpaceSize(); ++i)
	{
		tagItem* pCurItem = PetEquipContainer::GetItem(i);
		if (!P_VALID(pCurItem)) 
			continue;

		const tagPetEquipProto* pCurProto = GetPetEquipProto(pCurItem);
		if (!P_VALID(pCurProto)) 
			continue;
		//若新装备同类型唯一且与该位置装备类型相同或新装备唯一且与该位置装备typeid相同
		if (p2EquipPetEquipProto->bTypeUnique && p2EquipPetEquipProto->nType == pCurProto->nType ||
			p2EquipPetEquipProto->bUnique && p2Equip->dwTypeID == pCurItem->dwTypeID)
		{
			//卸载该位置装备
			UnEquip(pCurItem->GetKey(), GT_INVALID, TRUE);
		}
	}
		
	//若指定位置
	if (GT_VALID(n16DstPos))
	{
		//目标位置不空闲
		if (!PetEquipContainer::IsOnePlaceFree(n16DstPos))
		{
			//目标位置卸装
			tagItem* pItem = PetEquipContainer::GetItem(n16DstPos);
			if (P_VALID(pItem))
			{
				UnEquip(pItem->GetKey(), GT_INVALID, TRUE);
			}
		}
	}
	//若不指定位置
	else
	{
		//目标位置设置为找到一个空闲位置
		if (PetEquipContainer::GetFreeSpaceSize() != 0)
			for (INT16 i=0; i<PetEquipContainer::GetCurSpaceSize(); ++i)
			{
				if (!PetEquipContainer::IsOnePlaceFree(i)) continue;
				n16DstPos = i;
				break;
			}
		else
		{
			n16DstPos = 0;
			tagItem* pCurItem = PetEquipContainer::GetItem(n16DstPos);
			ASSERT(P_VALID(pCurItem)) ;
			DWORD dwRtv = UnEquip(pCurItem->GetKey(), GT_INVALID, TRUE);
			ASSERT(dwRtv == E_Success);
		}
	}

	// 标准化
	if (n16DstPos > SIZE_PET_EQUIP_BAR || n16DstPos < 0)
	{
		n16DstPos = 0;
	}
	
	AddEquip(p2Equip, n16DstPos);

	// 插入到数据库
	InsertDB(p2Equip);
	p2Equip->SetUpdate(EUDBS_Null);

	if (bSend)
	{
		// 同步客户端
		tagNS_PetEquip send;
		send.dwErrCode = E_Success;
		send.dwPetID = m_pSoul->GetID();	
		FillClientEquipData(&send.itemData, p2Equip);
		pMaster->SendMessage(&send, send.dwSize);
	}
 	return E_Pets_Success;
}

//----------------------------------------------------------------------------------------------------
// 通过64位ID来卸装，物品加入到任务背包
//----------------------------------------------------------------------------------------------------
DWORD PetEquipBar::UnEquip( INT64 n64ItemID, INT16 n16DstPos, BOOL bSend /*= FALSE*/ )
{
	MTRANS_ELSE_RET(pMaster, m_pSoul->GetMaster(), Role, E_Pets_PetEquip_Master_NotExist);

	//若指定位置
	if (GT_VALID(n16DstPos))
	{
		//目标位置不空闲
		if (!pMaster->GetItemMgr().IsBagOneSpaceFree(n16DstPos))
		{
			//返回错误
			return E_Pets_PetEquip_BagIndexNotFree;
		}
	}
	//若不指定位置
	else
	{
		//找到空闲目标位置
		n16DstPos = m_pSoul->GetMaster()->GetItemMgr().GetBagOneFreeSpace();
		//若无空闲位置
		if (!GT_VALID(n16DstPos))
		{
			//返回错误	
			return E_Pets_PetEquip_BagIndexNotFree;
		}
	}

	// 从宠物装备栏移除
	tagItem* p2UnEquip = PetEquipContainer::Remove(n64ItemID);

	//从时限管理器移除
	if(!P_VALID(p2UnEquip)) return E_Pets_PetEquip_BagIndexNotFree;

	if (p2UnEquip->IsTimeLimit())
	{
		m_TimeLimitMgr.RemoveFromTimeLeftList(p2UnEquip->n64Serial);
	}	

	// 从数据库移除
	RemoveDB(p2UnEquip);
	p2UnEquip->SetUpdate(EUDBS_Null);


	// 计算效果
	CalcEffect(m_pSoul, p2UnEquip->pProtoType->nSpecFuncVal1, FALSE, TRUE);

	DWORD dwRtv = pMaster->GetItemMgr().Add2BagByIndexAndInsertDB(p2UnEquip, ELCLD_PET_ITEM_PET_EQUIP, n16DstPos);
	if (E_Pets_Success != dwRtv)
	{
		::Destroy(p2UnEquip);
	}

	// 同步客户端
	if (E_Success == dwRtv && bSend)
	{
		tagNS_PetUnequip send;
		send.n64ItemID	= p2UnEquip->GetKey();
		send.dwErrCode = E_Success;
		pMaster->SendMessage(&send, send.dwSize);
	}

	return dwRtv ;
}

//----------------------------------------------------------------------------------------------------
// 计算装备效果
//----------------------------------------------------------------------------------------------------
VOID PetEquipBar::CalcEffect(PetSoul* pSoul, DWORD dwPetEquipTypeID, BOOL bEquip, BOOL bSend)
{
	const tagPetEquipProto* pProto = g_attRes.GetPetEquipProto(dwPetEquipTypeID);
	if (!P_VALID(pProto) || !P_VALID(pSoul))
	{
		ASSERT(0);
		return ;
	}

	// 是否卸装
	INT nCoef = bEquip ? 1 : -1;

	std::list<PetSkill*> listSkills;

	// 计算效果
	for (INT i=0; i<MAX_PET_EQUIP_MOD_ATT && GT_VALID(pProto->nPetAtt[i]); ++i)
	{
		INT nPetAtt = pProto->nPetAtt[i];
		listSkills.clear();
		m_pSoul->ExportSpecSkill(nPetAtt, listSkills);

		DisablePetSkill(listSkills);
		pSoul->GetPetAtt().ModAttVal(nPetAtt, nCoef * pProto->nPetAttMod[i], bSend);
		EnablePetSkill(listSkills);
	}
}

//----------------------------------------------------------------------------------------------------
// 构造函数
//----------------------------------------------------------------------------------------------------
PetEquipBar::PetEquipBar(PetSoul* pSoul) 
	:PetEquipContainer(EICT_PetEquip, SIZE_PET_EQUIP_BAR, SIZE_PET_EQUIP_BAR), m_TimeLimitMgr(PET_EQUIP_UPDATE_TIME), m_pSoul(pSoul)
{
	ASSERT(P_VALID(m_pSoul));
}

//----------------------------------------------------------------------------------------------------
// 更新
//----------------------------------------------------------------------------------------------------
VOID PetEquipBar::Update()
{
	m_TimeLimitMgr.Update();
	TList<INT64>& list2Del = m_TimeLimitMgr.GetNeedDelList();
	while (!list2Del.Empty())
	{
		// 若物品到期，则简单的把移除工作交给人物背包
		INT64 n64ItemID = list2Del.PopFront();
		if(IsEquipExist(n64ItemID))
			UnEquip(n64ItemID, GT_INVALID, TRUE);
	}
}

//----------------------------------------------------------------------------------------------------
// 从人物背包中获取宠物装备
//----------------------------------------------------------------------------------------------------
tagItem* PetEquipBar::GetFromRoleBag( INT64 n64ItemID )
{
	MTRANS_ELSE_RET(pMaster, m_pSoul->GetMaster(), Role, NULL);
	tagItem* pToGet = pMaster->GetItemMgr().GetBagItem(n64ItemID);
	if (!P_VALID(pToGet))
	{
		ASSERT(0);
	}
	return pToGet;
}

//----------------------------------------------------------------------------------------------------
// 从宠物身上获取宠物装备
//----------------------------------------------------------------------------------------------------
tagItem* PetEquipBar::GetFromPet( INT64 n64ItemID )
{
	tagItem* pToGet = PetEquipContainer::GetItem(n64ItemID);
	if (!P_VALID(pToGet))
	{
		ASSERT(0);	
	}
	return pToGet;
}

//----------------------------------------------------------------------------------------------------
// 获取客户端装备结构
//----------------------------------------------------------------------------------------------------
VOID PetEquipBar::FillClientEquipData( tagPetEquipMsgInfo* pMsgInfo, tagItem* pItem)
{
	if (P_VALID(pItem))
	{
		pMsgInfo->n64ItemID		= pItem->GetKey();
		pMsgInfo->dwItemProtoID	= pItem->dwTypeID;
		pMsgInfo->n8Pos			= (INT8)pItem->n16Index;
	}
}

//----------------------------------------------------------------------------------------------------
// 宠物装备位置交换
//----------------------------------------------------------------------------------------------------
DWORD PetEquipBar::SwapEquipPos( INT64 n64ItemID, INT8 n8DstPos, BOOL bSend /*= FALSE*/ )
{
	
	INT16 n16DstPos = (INT16)n8DstPos;
	tagItem* pItem = GetFromPet( n64ItemID);
	if (!P_VALID(pItem))
	{
		return E_Pets_PetEquip_EquipIsNotOnPet;
	}else if (n16DstPos < 0 || n16DstPos >= PetEquipContainer::GetCurSpaceSize())
	{
		return E_Pets_PetEquip_InvalidDstPos;
	}
	else if (pItem->n16Index == n16DstPos)
	{
		return E_Pets_PetEquip_EquipAlreadyInPos;
	}

	INT16 n16SrcPos = pItem->n16Index;

	if (PetEquipContainer::IsOnePlaceFree(n16DstPos))
	{
		// 直接放到这个位置
		tagItem* pItem = PetEquipContainer::Remove(n64ItemID);
		ASSERT(P_VALID(pItem));
		PetEquipContainer::Add(pItem, n16DstPos);
	}
	else
	{
		// 交换
		PetEquipContainer::Swap(pItem->n16Index, n16DstPos);
	}

	if (bSend)
	{
		tagNS_PetEquipPosSwap send;
		send.dwErrCode = E_Success;
		send.n64ItemID = pItem->GetKey();
		send.n8PosSrc = (INT8)n16SrcPos;
		send.n8PosDst = (INT8)pItem->n16Index;
		m_pSoul->GetMaster()->SendMessage(&send, send.dwSize);
	}

	return E_Pets_Success;
}

//----------------------------------------------------------------------------------------------------
// 存数据库
//----------------------------------------------------------------------------------------------------
VOID PetEquipBar::SaveToDB( OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum )
{
	nNum = 0;

	MTRANS_POINTER(pCurPointer, pData, tagItemUpdate);

	tagItem	*pTemp	= NULL;
	for(INT16 i=0; i<PetEquipContainer::GetCurSpaceSize(); ++i)
	{
		pTemp = PetEquipContainer::GetItem(i);
		if(P_VALID(pTemp) && pTemp->eStatus != EUDBS_Null)
		{
			pCurPointer[nNum].n64Serial		= pTemp->n64Serial;
			pCurPointer[nNum].dwOwnerID		= pTemp->dwOwnerID;
			pCurPointer[nNum].nUseTimes		= pTemp->nUseTimes;
			pCurPointer[nNum].n16Num		= pTemp->n16Num;
			pCurPointer[nNum].n16Index		= pTemp->n16Index;
			pCurPointer[nNum].byConType		= pTemp->eConType;

			pTemp->SetUpdate(EUDBS_Null);

			++nNum;
		}
	}

	pOutPointer = &pCurPointer[nNum];
}

//----------------------------------------------------------------------------------------------------
// 插入数据库
//----------------------------------------------------------------------------------------------------
VOID PetEquipBar::InsertDB( tagItem* p2Equip )
{
	tagNDBC_NewItem send;
	IFASTCODE->MemCpy(&send.item, p2Equip, SIZE_ITEM);
	g_dbSession.Send(&send, send.dwSize);
}

//----------------------------------------------------------------------------------------------------
// 从数据库移除
//----------------------------------------------------------------------------------------------------
VOID PetEquipBar::RemoveDB( tagItem* p2UnEquip )
{
	tagNDBC_DelItem send;
	send.n64Serial = p2UnEquip->n64Serial;
	g_dbSession.Send(&send, send.dwSize);
}

//----------------------------------------------------------------------------------------------------
// 获取宠物装备原型
//----------------------------------------------------------------------------------------------------
const tagPetEquipProto* PetEquipBar::GetPetEquipProto( tagItem* pItem )
{
	if (!P_VALID(pItem)) return NULL;
	MTRANS_ELSE_RET(pItemProto, pItem->pProtoType, const tagItemProto, NULL);
	if (EISF_PetEquipment != pItemProto->eSpecFunc) return NULL;
	return g_attRes.GetPetEquipProto(pItem->pProtoType->nSpecFuncVal1);
}

//----------------------------------------------------------------------------------------------------
// 添加装备
//----------------------------------------------------------------------------------------------------
VOID PetEquipBar::AddEquip( tagItem* p2Equip, INT16 n16DstPos, BOOL bOnLoad)
{
	// 获得原型
	MTRANS_ELSE_RET(p2EquipPetEquipProto, GetPetEquipProto(p2Equip), const tagPetEquipProto, );

	//添加到装备栏
	PetEquipContainer::Add(p2Equip, n16DstPos);
	p2Equip->SetOwner(m_pSoul->GetID(), GT_INVALID);

	// 计算时限物品
	if (p2Equip->IsTimeLimit())
	{
		m_TimeLimitMgr.Add2TimeLeftList(p2Equip->n64Serial, p2Equip->pProtoType->dwTimeLimit, p2Equip->dw1stGainTime);
	}	

	// 计算物品效果
	CalcEffect(m_pSoul, p2EquipPetEquipProto->dwTypeID, TRUE, !bOnLoad);
}

BOOL PetEquipBar::HasEquip()
{
	return  PetEquipContainer::GetMaxSpaceSize() - PetEquipContainer::GetFreeSpaceSize() != 0;
}

VOID PetEquipBar::DisablePetSkill( std::list<PetSkill*> &listSkill)
{
	if (!m_pSoul->IsCalled())
	{
		return;
	}
	std::list<PetSkill*>::iterator itr = listSkill.begin();
	while (itr != listSkill.end())
	{
		PassiveSkill* pPassive = dynamic_cast<PassiveSkill*>(*itr);
		if (P_VALID(pPassive))
		{
			pPassive->DeActive();
		}
		EnhanceSkill* pEnhance = dynamic_cast<EnhanceSkill*>(*itr);
		if (P_VALID(pEnhance))
		{
			pEnhance->Close();
		}
		++itr;
	}
}

VOID PetEquipBar::EnablePetSkill( std::list<PetSkill*> &listSkill)
{
	if (!m_pSoul->IsCalled())
	{
		return;
	}
	std::list<PetSkill*>::iterator itr = listSkill.begin();
	while (itr != listSkill.end())
	{
		PassiveSkill* pPassive = dynamic_cast<PassiveSkill*>(*itr);
		if (P_VALID(pPassive))
		{
			pPassive->Active();
		}
		EnhanceSkill* pEnhance = dynamic_cast<EnhanceSkill*>(*itr);
		if (P_VALID(pEnhance))
		{
			pEnhance->Open();
		}
		++itr;
	}
}

BOOL PetEquipBar::IsEquipExist( INT64 n64Id )
{
	//对于装备栏每个位置装备
	for (INT16 i=0; i<PetEquipContainer::GetCurSpaceSize(); ++i)
	{
		tagItem* pCurItem = PetEquipContainer::GetItem(i);
		if (P_VALID(pCurItem)) 
			return TRUE;
	}

	return FALSE;
}