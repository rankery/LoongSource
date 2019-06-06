
#include "stdafx.h"
#include "vocation.h"
#include "role.h"
#include "creature.h"
#include "../ServerDefine/log_cmdid_define.h"

//-------------------------------------------------------------
// 角色获得专精职业
//-------------------------------------------------------------
DWORD Role::RoleSetVocation( DWORD dwNPCID, EClassType eVocation)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC)
		|| !pNPC->IsFunctionNPC(EFNPCT_Class) 
		|| !pNPC->CheckNPCTalkDistance(this))
	{
		return ECEC_NPCError;
	}

	// 判断等级是否大于80级
	if(this->GetLevel() < 80) return ECEC_NotUpToLevel;
	
	// 该角色是否已经选择了专精职业
	if( !(this->GetClass() == EV_Base) ) return ECEC_IsNotBaseClass;
	ASSERT(this->GetClassEx() == EHV_Base);

	// 是否有就职凭证
	TList<tagItem*> ItemList;
	GetItemMgr().GetBagSameItemList(ItemList, ITEM_SetClass);
	if(ItemList.Empty()) return ECEC_LackOfNewclassItem;

	// 就职
	this->SetClass(eVocation);

	// 洗天资
	ClearTalent((INT64)0, FALSE);

	// 扣除就职凭证
	TList<tagItem*>::TListIterator it = ItemList.Begin();
	GetItemMgr().DelFromBag((*it)->n64Serial, ELCLD_Class_Get, 1);

	tagNS_RoleClassChangeBroadcast send;
	send.dwRoleID	= GetID();
	send.eClass = m_eClass;
	GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);

	return ECEC_Success;
}

//-------------------------------------------------------------
// 角色获得英雄职业
//-------------------------------------------------------------
DWORD Role::RoleSetHeroVocation( DWORD dwNPCID, EClassTypeEx eHVocation)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC)
		|| !pNPC->IsFunctionNPC(EFNPCT_Class) 
		|| !pNPC->CheckNPCTalkDistance(this))
	{
		return ECEC_NPCError;
	}

	// 判断等级是否大于120级
	if(this->GetLevel() < 120) return ECEC_NotUpToLevel;

	// 该角色是否已经选择了英雄职业
	if( !(this->GetClassEx() == EHV_Base) ) return ECEC_IsNotBaseClassEx;
	ASSERT(this->GetClass()>EHV_Base && this->GetClassEx()<EHV_End);

	// 选择的英雄职业是否与专精职业相匹配
	Vocation::tagHeroVocation* pTemp = g_VocationMap.GetVocationMatchMap().Peek(this->GetClass());
	if ( pTemp->eHVocation_1 != eHVocation && pTemp->eHVocation_2 != eHVocation) return ECEC_ClassNotMatch;

	this->SetClassEx(eHVocation);

	return ECEC_Success;
}

//-------------------------------------------------------------
// 角色更改职业
//-------------------------------------------------------------
DWORD Role::ChangeVocation( DWORD dwNPCID, EClassType eVocation)
{
	// 获得地图
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(P_VALID(pMap));
		return GT_INVALID;
	}

	// 找到NPC，并检查合法性
	Creature* pNPC = pMap->FindCreature(dwNPCID);
	if(!P_VALID(pNPC)
		|| !pNPC->IsFunctionNPC(EFNPCT_Class) 
		|| !pNPC->CheckNPCTalkDistance(this))
	{
		return ECEC_NPCError;
	}

	// 判断角色目前是否已经就职，未就职也就不需要更改
	if (this->GetClass() == EV_Base) return ECEC_ClassNotSet;

	// 判断是否有相关的IM道具
	TList<tagItem*> ItemList;
	GetItemMgr().GetBagSameItemList(ItemList, IM_ChangeClass);
	if(ItemList.Empty()) return ECEC_LackOfReclassItem;

	// 转职
	this->SetClassEx(EHV_Base);
	this->SetClass(eVocation);

	// 洗天资
	ClearTalent((INT64)0, FALSE);

	// 扣除IM道具
	TList<tagItem*>::TListIterator it = ItemList.Begin();
	GetItemMgr().DelFromBag((*it)->n64Serial, ELCLD_Class_Get, 1);

	tagNS_RoleClassChangeBroadcast send;
	send.dwRoleID	= GetID();
	send.eClass = GetClass();
	GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);

	return ECEC_Success;
}
