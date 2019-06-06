/********************************************************************
created:	2008/12/09
created:	09:12:2008   00:05
filename: 	d:\loong_client\Client\ToolTipScriptFuncImpl.cpp
file path:	d:\loong_client\Client
file base:	ToolTipScriptFuncImpl
file ext:	cpp
author:		lynx

purpose:	脚本函数接口实现，供策划调用
*********************************************************************/
#include "StdAfx.h"
#include "RoleMgr.h"
#include "Role.h"
#include "Item.h"
#include "PlayerNameTab.h"
#include "LocalPlayer.h"
#include "SkillMgr.h"
#include "ItemMgr.h"
#include "CurrencyMgr.h"
#include "LevelUpData.h"
#include "ItemProtoData.h"
#include "SkillProtoData.h"
#include "..\WorldDefine\shop_define.h"
#include "..\WorldDefine\currency_define.h"
#include "ConsolidateData.h"
#include "..\WorldDefine\compose_define.h"
#include "..\WorldDefine\suit_define.h"
#include "LonghunAttData.h"
#include "..\WorldDefine\longhun_define.h"
#include "ServerTime.h"
#include "TreasureMgr.h"
#include "ReputeMgr.h"
#include "PetProtoData.h"
#include "GuildDefine.h"
#include "GuildMgr.h"


//-------------------------------------------------------------------------------------
//							       本地玩家信息相关函数
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// 获取本地玩家等级
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerLevel(lua_State* L)
{
	//GetLocalPlayerLevel()
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(P_VALID(pLp))
	{
		lua_pushnumber(L, pLp->GetRoleLevel());
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取本地玩家性别
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerSex(lua_State* L)
{
	//GetLocalPlayerSex()
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(P_VALID(pLp))
	{
		lua_pushnumber(L, pLp->GetPlayerSex());
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取本地玩家职业
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerClass(lua_State* L)
{
	//GetLocalPlayerClass()

	return 0;
}

//-------------------------------------------------------------------------------------
// 获取本地玩家某项氏族声望等级
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerRepution(lua_State* L)
{
	//GetLocalPlayerRepution(eClan)
	ECLanType eClan = (ECLanType)(DWORD)(lua_tonumber(L, -1));
	if(eClan > ECLT_NULL && eClan < ECLT_END)
	{
		lua_pushnumber(L, ReputeMgr::Inst()->GetReputeLevel(eClan)->eLevel);
		return 1;
	}

	return 0;
}
//-------------------------------------------------------------------------------------
// 获取本地玩家某项氏族贡献值
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerContribution(lua_State* L)
{
	//GetLocalPlayerContribution(eClan)
	ECLanType eClan = (ECLanType)(DWORD)(lua_tonumber(L, -1));
	if(eClan > ECLT_NULL && eClan < ECLT_END)
	{
		lua_pushnumber(L, CurrencyMgr::Inst()->GetClanContribute(eClan));
		return 1;
	}

	return 0;
}
//-------------------------------------------------------------------------------------
// 获取本地玩家某项天资投放点
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerTalentBidden(lua_State* L)
{
	//GetLocalPlayerTalentBidden(eTalent)
	ETalentType eTalent = (ETalentType)(DWORD)(lua_tonumber(L, -1));
	if(eTalent > ETT_Null && eTalent < ETT_End)
	{
		lua_pushnumber(L, SkillMgr::Inst()->GetTalentPoint(eTalent));
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取本地玩家属性
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerAttribute(lua_State* L)
{
	//GetLocalPlayerAttribute(eAtt)
	ERoleAttribute eAtt = (ERoleAttribute)(DWORD)(lua_tonumber(L, -1));
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(P_VALID(pLp))
	{
		if(eAtt > ERA_Null && eAtt < ERA_End)
		{
			lua_pushnumber(L, pLp->GetAttribute(eAtt));
			return 1;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取本地玩家属性投放值
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerAttBidden(lua_State* L)
{
	//GetLocalPlayerAttBidden(eAtt)
	ERoleAttribute eAtt = (ERoleAttribute)(DWORD)(lua_tonumber(L, -1));
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(P_VALID(pLp))
	{
		if(eAtt >= ERA_AttA_Start && eAtt <= ERA_AttA_End)
		{
			lua_pushnumber(L, pLp->GetAttPointAdd(eAtt));
			return 1;
		}
	}
	return 0;
}
//------------------------------------------------------------------------------------
// 获取本地玩家升级属性增加值
//------------------------------------------------------------------------------------
int SC_GetLocalPlayerAttLevelup(lua_State* L)
{
	//GetLocalPlayerAttLevelup(eAtt)
	ERoleAttribute eAtt = (ERoleAttribute)(DWORD)lua_tonumber(L, -1);
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(P_VALID(pLp))
	{
		const tagExperience* pLevelup = LevelUpData::Inst()->FindExpData(pLp->GetRoleLevel());
		if(P_VALID(pLevelup))
		{
			if(eAtt >= ERA_AttA_Start && eAtt <= ERA_AttA_End)
			{
				lua_pushnumber(L, pLevelup->nAtt[eAtt]);
				return 1;
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取本地玩家可消耗属性当前值
//-------------------------------------------------------------------------------------
int SC_GetLocalPlayerCostAttCurValue(lua_State* L)
{
	ESkillCostType eCost = (ESkillCostType)(DWORD)lua_tonumber(L, -1);
	LocalPlayer* pLp = RoleMgr::Inst()->GetLocalPlayer();
	if(P_VALID(pLp))
	{
		ERoleAttribute eType = ERA_Null;
		switch (eCost)
		{
		case ESCT_HP:
			eType = ERA_HP;	
			break;
		case ESCT_MP:
			eType = ERA_MP;
			break;
		case ESCT_Rage:
			eType = ERA_Rage;
			break;
		case ESCT_Endurance:
			eType = ERA_Endurance;
			break;
		case ESCT_Valicity:
			eType = ERA_Vitality;
			break;
		}
		if (ERA_Null != eType)
		{
			lua_pushnumber(L, pLp->GetAttribute(eType));
			return 1;
		}
	}	
	return 0;
}


//-------------------------------------------------------------------------------------
//							          物品信息相关函数
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// 获取物品名称
//-------------------------------------------------------------------------------------
int SC_GetItemName(lua_State *L)
{
	//GetItemName(obj)
	Item* pItem = (Item*)(lua_touserdata(L, -1));
	if(P_VALID(pItem) && pItem->IsItem())
	{
		tstring strTmp;
		//如果珍宝激活名称有效
		if(P_VALID(pItem->GetData()->dwNameID))
			strTmp += PlayerNameTab::Inst()->FindNameByNameID(pItem->GetData()->dwNameID);
		strTmp += pItem->GetDisplay()->szName;
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(strTmp.c_str());
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, strTmp.c_str(), strTmp.size());
#endif		
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品名称
//-------------------------------------------------------------------------------------
int SC_GetItemNameEx(lua_State *L)
{
	//GetItemNameEx(proto,name)
	const tagItemProto* pProto = (const tagItemProto*)(lua_touserdata(L, -2));
	DWORD dwNameId = (DWORD)(lua_tonumber(L, -1));
	if(P_VALID(pProto))
	{
		LPCTSTR szName;
		//如果珍宝激活名称有效
		if(P_VALID(dwNameId))
			szName = TreasureMgr::Inst()->GetTreasureName(dwNameId).c_str();
		else
			szName = pProto->pDisplayInfo->szName;
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(szName);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, szName, strlen(szName));
#endif		
		return 1;
	}
	
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品描述
//-------------------------------------------------------------------------------------
int SC_GetItemDesc(lua_State *L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->pDisplayInfo->szDesc);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, pProto->pDisplayInfo->szDesc, strlen(pProto->pDisplayInfo->szDesc));
#endif
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品类型
//-------------------------------------------------------------------------------------
int SC_GetItemType(lua_State *L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->pDisplayInfo->szType);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else 
		lua_pushlstring(L, pProto->pDisplayInfo->szType, strlen(pProto->pDisplayInfo->szType));
#endif
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品品级
//-------------------------------------------------------------------------------------
int SC_GetItemQuality(lua_State *L)
{
	Item*				pItem  = (Item*)		(lua_touserdata(L, -2)); 
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		// 如果是装备，不能根据静态属性获得物品品级
		if(MIsEquipment(pProto->dwTypeID))
		{
			if(P_VALID(pItem) && pItem->IsItem())
			{
				Equipment* pEquip = (Equipment*)pItem;
				lua_pushnumber(L, pEquip->GetItemQuality());
				return 1;
			}
			else 
			{
				lua_pushnumber(L, (BYTE)GT_INVALID);
				return 1;
			}
		}
		else
		{
			lua_pushnumber(L, pProto->byQuality);
			return 1;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品绑定类别
//-------------------------------------------------------------------------------------
int SC_GetItemBindType(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->byBindType);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品绑定角色名
//-------------------------------------------------------------------------------------
int SC_GetItemBindRoleName(lua_State* L)
{
    Item* pItem = (Item*)(lua_touserdata(L, -1)); 
    if(P_VALID(pItem))
    {
        LPCTSTR szRoleName = PlayerNameTab::Inst()->FindNameByID(pItem->GetItemBindingRole());
        if(szRoleName != _T(""))
        {
#ifdef UNICODE
            LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(szRoleName);
            lua_pushlstring(L, szTmp, strlen(szTmp));
#else
            lua_pushlstring(L, szRoleName, strlen(szRoleName));
#endif
            return 1;
        }
    }
    return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品绑定状态
//-------------------------------------------------------------------------------------
int SC_GetItemBindState(lua_State* L)
{
	Item* pItem = (Item*)(lua_touserdata(L, -1)); 
	if(P_VALID(pItem) && pItem->IsItem())
	{
		lua_pushboolean(L, pItem->IsItemBinded());
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品锁定状态
//-------------------------------------------------------------------------------------
int SC_GetItemLockState(lua_State* L)
{
	Item* pItem = (Item*)(lua_touserdata(L, -1)); 
	if(P_VALID(pItem) && pItem->IsItem())
	{
		lua_pushboolean(L, pItem->IsItemLocked());
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品性别限制
//-------------------------------------------------------------------------------------
int SC_GetItemSexLimit(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->eSexLimit);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 物品等级限制
//-------------------------------------------------------------------------------------
int SC_GetItemLevelLimit(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->byMinUseLevel);
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 物品氏族声望限制
//-------------------------------------------------------------------------------------
int SC_GetItemClanReputeLimit(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->eClanRepute);
		lua_pushnumber(L, pProto->nClanReputeVal);
		return 2;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 物品其他声望限制
//-------------------------------------------------------------------------------------
int SC_GetItemOtherClanReputeLimit(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->eOtherClanRepute);
		lua_pushnumber(L, pProto->nOtherClanReputeVal);
		return 2;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 物品恢复时间
//-------------------------------------------------------------------------------------
int SC_GetItemCoolDownTime(lua_State* L)
{
    const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
    if (P_VALID(pProto))
    {
        int nCdTime = pProto->dwCooldownTime;
        lua_pushnumber(L, nCdTime);
        return 1;
    }
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品最大使用次数
//-------------------------------------------------------------------------------------
int SC_GetItemMaxUseTimes(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->nMaxUseTimes);
		return 1;
	} 
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品当前使用次数
//-------------------------------------------------------------------------------------
int SC_GetItemUseTimes(lua_State* L)
{
	Item* pItem = (Item*)(lua_touserdata(L, -1)); 
	if(P_VALID(pItem) && pItem->IsItem())
	{
		lua_pushnumber(L, pItem->GetItemUseTimes());
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品制造者
//-------------------------------------------------------------------------------------
int SC_GetItemCreator(lua_State* L)
{
	Item* pItem = (Item*)(lua_touserdata(L, -1));
	if(P_VALID(pItem) && pItem->IsItem())
	{
		DWORD dwCreatorID = pItem->GetItemCreatorID();
		if(GT_VALID(dwCreatorID))
		{
			LPCTSTR szRoleName = PlayerNameTab::Inst()->FindNameByID(dwCreatorID);
			if(szRoleName != _T(""))
			{
#ifdef UNICODE
				LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(szRoleName);
				lua_pushlstring(L, szTmp, strlen(szTmp));
#else
				lua_pushlstring(L, szRoleName, strlen(szRoleName));
#endif
				return 1;
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品使用类型
//-------------------------------------------------------------------------------------
INT SC_GetItemUseType(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		INT nType = 0;
		if (GT_VALID(pProto->dwBuffID0))
		{
			nType = 1;
		}
		else
		{
			if(EISF_Null != pProto->eSpecFunc)
			{
				if(EISF_AcceptQuest == pProto->eSpecFunc ||			// 接取任务
					EISF_RemoveAttPt == pProto->eSpecFunc ||		// 属性洗点	
					EISF_RemoveTalentPt == pProto->eSpecFunc ||		// 天资洗点
					EISF_LearnSkill == pProto->eSpecFunc ||			// 学习技能	
					EISF_FriendGift == pProto->eSpecFunc)			// 好友礼物
				{
					nType = 1;
				}
				else if(EISF_IdetifyEquip == pProto->eSpecFunc ||	// 鉴定装备
					EISF_HoleGem == pProto->eSpecFunc ||			// 镶嵌
					EISF_Brand == pProto->eSpecFunc ||				// 烙印
					EISF_LongHunShi == pProto->eSpecFunc ||			// 龙附
					EISF_Grind == pProto->eSpecFunc ||				// 磨石
					EISF_Chisel == pProto->eSpecFunc)				// 开凿
				{
					nType = 2;
				}
				else if(EISF_Dye == pProto->eSpecFunc)				// 染料
				{
					nType = 3;
				}
				else if(EISF_ProtectSign == pProto->eSpecFunc)		// 保底符
				{
					nType = 4;
				}
				else if(EISF_ColorProbability == pProto->eSpecFunc)	// 颜色提升
				{
					nType = 5;
				}
			}
		}
		lua_pushnumber(L, nType);
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品消耗潜力值
//-------------------------------------------------------------------------------------
INT SC_GetItemCostPotVal(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		if(EISF_HoleGem == pProto->eSpecFunc ||
			EISF_Brand == pProto->eSpecFunc ||
			EISF_LongHunShi == pProto->eSpecFunc)
		{
			const tagConsolidateItem* pItem = ConsolidateData::Inst()->FindConsolidateItemProto(pProto->dwTypeID);
			if(P_VALID(pItem))
			{
				lua_pushnumber(L, pItem->dwPotValConsume);
				return 1;
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品可镶部位
//-------------------------------------------------------------------------------------
INT SC_GetItemHoldEquipPos(lua_State* L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -2));
	INT					nIndex =		   (INT)(lua_tonumber(L, -1));	
	if(P_VALID(pProto))
	{
		const tagConsolidateItem* pItem = ConsolidateData::Inst()->FindConsolidateItemProto(pProto->dwTypeID);
		if(P_VALID(pItem))
		{
			lua_pushnumber(L, pItem->ConsolidatePos[nIndex].byConsolidate);
			return 1;
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品总使用时限
//-------------------------------------------------------------------------------------
INT SC_GetItemAllTimeLimit(lua_State* L)
{
    Item*				pItem  = (Item*)(lua_touserdata(L, -1));
    if(P_VALID(pItem) && pItem->IsItem())
    {
        int timeLimit = (int)pItem->GetProto()->dwTimeLimit;
        if (timeLimit > 0)
        {
            lua_pushnumber(L, timeLimit);
            return 1;
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品总使用时限
//-------------------------------------------------------------------------------------
INT SC_GetItemProtoAllTimeLimit(lua_State* L)
{
    tagItemProto*   pItemProto  = (tagItemProto*)(lua_touserdata(L, -1));
    if(P_VALID(pItemProto))
    {
        int timeLimit = (int)pItemProto->dwTimeLimit;
        if (timeLimit > 0)
        {
            lua_pushnumber(L, timeLimit);
            return 1;
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品使用时限
//-------------------------------------------------------------------------------------
INT SC_GetItemTimeLimit(lua_State* L)
{
	Item*				pItem  = (Item*)(lua_touserdata(L, -1));
	if(P_VALID(pItem) && pItem->IsItem())
	{
		INT nPostSecord = (INT)CalcTimeDiff(ServerTime::Inst()->CalCurrentServerDwordTime(), pItem->GetData()->dw1stGainTime);
		if(nPostSecord > 0)
		{
			INT nTimeLimit = (INT)pItem->GetProto()->dwTimeLimit - nPostSecord;
			if(nTimeLimit > 0)
			{
				lua_pushnumber(L, nTimeLimit);
				return 1;
			}
		}
	}
	
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取物品可学技能（技能书）
//-------------------------------------------------------------------------------------
int SC_GetItemLearnSkill(lua_State* L)
{
	const tagItemProto* pProto = (const tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		if(EISF_LearnSkill == pProto->eSpecFunc)
		{
			const tagSkillProtoClient* pSkill = SkillProtoData::Inst()->FindSkillProto(pProto->nSpecFuncVal1);
			if(P_VALID(pSkill))
			{
				lua_pushlightuserdata(L, (void*)pSkill);
				return 1;	
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品是否可出售
//-------------------------------------------------------------------------------------
INT SC_GetItemCanSell(lua_State* L)
{
    tagItemProto*   pItemProto  = (tagItemProto*)(lua_touserdata(L, -1));
    if(P_VALID(pItemProto))
    {
        int bCanSell = (int)pItemProto->bCanSell;
        lua_pushnumber(L, bCanSell);
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品起手时间
//-------------------------------------------------------------------------------------
INT SC_GetItemPreTime(lua_State* L)
{
    tagItemProto*   pItemProto  = (tagItemProto*)(lua_touserdata(L, -1));
    if(P_VALID(pItemProto))
    {
        int timeLimit = (int)pItemProto->nPrepareTime;
        if (timeLimit > 0)
        {
            lua_pushnumber(L, timeLimit);
            return 1;
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------
// 获取物品可堆叠数量
//-------------------------------------------------------------------------------------
INT SC_GetItemMaxItem(lua_State* L)
{
    tagItemProto*   pItemProto  = (tagItemProto*)(lua_touserdata(L, -1));
    if(P_VALID(pItemProto))
    {
        int timeLimit = (int)pItemProto->n16MaxLapNum;
        if (timeLimit > 0)
        {
            lua_pushnumber(L, timeLimit);
            return 1;
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------
//							            装备信息相关函数
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// 获取装备特性描述
//-------------------------------------------------------------------------------------
int SC_GetEquipDescEx(lua_State *L)
{
	const tagItemProto* pProto = (tagItemProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->pDisplayInfo->szDescEx);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, pProto->pDisplayInfo->szDescEx, strlen(pProto->pDisplayInfo->szDescEx));
#endif
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备职业限制
//-------------------------------------------------------------------------------------
int SC_GetEquipClassLimit(lua_State* L)
{
	const tagEquipProto* pProto = (tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{

	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备位置
//-------------------------------------------------------------------------------------
int SC_GetEquipPos(lua_State* L)
{
	const tagEquipProto* pProto = (tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->eEquipPos);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取武器伤害
//-------------------------------------------------------------------------------------
int SC_GetEquipWeaponDamage(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -2));
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquipMinDmg());
		lua_pushnumber(L, pEquip->GetEquipMaxDmg());
		return 2;
	}
	else if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->n16MinDmg);
		lua_pushnumber(L, pProto->n16MaxDmg);

		return 2;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取武器武魂值
//-------------------------------------------------------------------------------------
int SC_GetEquipWeaponWuhun(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -2));
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquipWuhun());
		return 1;
	}
	else if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->n16WuHun);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取防具护甲值
//-------------------------------------------------------------------------------------
int SC_GetEquipArmorDefine(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -2));
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquipArmor());
		return 1;
	}
	else if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->n16Armor);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取武器暂新度
//-------------------------------------------------------------------------------------
int SC_GetEquipNewNess(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -2));
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquipNewness());
		return 1;
	}
	else if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->n16Newness);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备类型
//-------------------------------------------------------------------------------------
int SC_GetEquipType(lua_State* L)
{
    Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -2));
    const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -1));

    if(P_VALID(pProto))
    {
        lua_pushnumber(L, pProto->eType);
        return 1;
    }
    return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备潜力值
//-------------------------------------------------------------------------------------
int SC_GetEquipPotVal(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -2));
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		if( pEquip->GetProto()->eType != EIT_Fashion )
		{
			lua_pushnumber(L, pEquip->GetEquipPotVal());
            lua_pushnumber(L, pEquip->GetEquipCurrPotMax());
			lua_pushnumber(L, pEquip->GetEquipPotMax());
			return 3;
		}
		else
		{
			lua_pushnumber(L, GT_INVALID );
			lua_pushnumber(L, GT_INVALID );
            lua_pushnumber(L, GT_INVALID );
			return 3;
		}
	}
	else if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->nPotVal);
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取装备潜力可提升的次数
//-------------------------------------------------------------------------------------
int SC_GetEquipPotAdvanceTime(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -2));
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber( L, pEquip->GetEquipPotAdvanceTime() );
		return 1;
	}
	else if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->nPotIncTimes);
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取装备属性原始加成
//-------------------------------------------------------------------------------------
int SC_GetEquipProtoAtt(lua_State* L)
{
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_newtable(L);
		for(INT i=0; i< MAX_ROLEATT_BASE_EFFECT; ++i)	
		{
			lua_pushnumber(L, pProto->BaseEffect[i].eRoleAtt);
			lua_pushnumber(L, pProto->BaseEffect[i].nValue);
			lua_settable(L, -3);
		}	
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取装备属性加成
//-------------------------------------------------------------------------------------
int SC_GetEquipAttribute(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		TMap<ERoleAttribute, INT> Map;
		pEquip->GetEquipAttribute(Map);
		if(Map.Size() > 0)
		{
			lua_newtable(L);

			Map.ResetIterator();
			ERoleAttribute eAtt;
			INT nValue;
			while(Map.PeekNext(eAtt, nValue))
			{
				lua_pushnumber(L, eAtt);
				lua_pushnumber(L, nValue);
				lua_settable(L, -3);
			}	
			return 1;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备属性百分比加成
//-------------------------------------------------------------------------------------
int SC_GetEquipAttributePct(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		TMap<ERoleAttribute, INT> Map;
		pEquip->GetEquipAttributePct(Map);
		if(Map.Size() > 0)
		{
			lua_newtable(L);

			Map.ResetIterator();
			ERoleAttribute eAtt;
			INT nValue;
			while(Map.PeekNext(eAtt, nValue))
			{
				lua_pushnumber(L, eAtt);
				lua_pushnumber(L, nValue);
				lua_settable(L, -3);
			}	
			return 1;
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取镌刻、铭文、烙印的生产状态
//-------------------------------------------------------------------------------------
int SC_GetEquipProduceState(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquipEngraveTimes());
		lua_pushnumber(L, pEquip->GetEquipPosyTimes());
		lua_pushnumber(L, pEquip->GetEquipBrandLevel());
		return 3;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备宝石镶嵌数量
//-------------------------------------------------------------------------------------
int SC_GetEquipHoldNum(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquipHoleNum());
		lua_pushnumber(L, pEquip->GetEquipMaxHoleNum());
		return 2;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备已镶嵌宝石
//-------------------------------------------------------------------------------------
int SC_GetEquipHoldGem(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -2));
	INT		   nIndex = (INT)		(lua_tonumber(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		if(nIndex >= 0 && nIndex < MAX_EQUIPHOLE_NUM)
		{
			DWORD dwGemID = pEquip->GetEquipHoldGemID(nIndex);
			const tagItemProto* pGem = ItemProtoData::Inst()->FindItemProto(dwGemID);
			if(P_VALID(pGem))
			{
#ifdef UNICODE
				LPCSTR szTmp1 = TObjRef<Util>()->UnicodeToUnicode8(pGem->pDisplayInfo->szName);
				lua_pushlstring(L, szTmp1, strlen(szTmp1));
#else
				lua_pushlstring(L, pGem->pDisplayInfo->szName, strlen(pGem->pDisplayInfo->szName));
#endif
#ifdef UNICODE
				LPCSTR szTmp2 = TObjRef<Util>()->UnicodeToUnicode8(pGem->pDisplayInfo->szDesc);
				lua_pushlstring(L, szTmp2, strlen(szTmp2));
#else
				lua_pushlstring(L, pGem->pDisplayInfo->szDesc, strlen(pGem->pDisplayInfo->szDesc));
#endif
				lua_pushnumber(L, pGem->byQuality);
				return 3;
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备龙附等级
//-------------------------------------------------------------------------------------
int SC_GetEquipLongfu(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -2));
	EWuXing	   eType  = (EWuXing)(DWORD)(lua_tonumber(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		if(eType > EWX_Null && eType < EWX_End)
		{
			lua_pushnumber(L, pEquip->GetEquipWuxing(eType));
			return 1;
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取装备龙魂能力（名称+描述）
//-------------------------------------------------------------------------------------
int SC_GetEquipLonghun(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -2));
	bool bInner = (lua_toboolean(L, -1) == 0 ? false : true);
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		DWORD dwID;
		if(bInner)
			dwID = pEquip->GetEquipInnerLonghun();
		else 
			dwID = pEquip->GetEquipOuterLonghun();

		if(P_VALID(dwID))
		{
			const tagLongHunProtoClient* pProto = LonghunAttData::Inst()->FindLonghunData(dwID);
			if(P_VALID(pProto))
			{
				lua_pushlightuserdata(L, (void*)pProto);
#ifdef UNICODE				
				LPCSTR szTmp1 = TObjRef<Util>()->UnicodeToUnicode8(pProto->szName);
				lua_pushlstring(L, szTmp1, strlen(szTmp1));
#else
				lua_pushlstring(L, pProto->szName, strlen(pProto->szName));
#endif
#ifdef UNICODE					
				LPCSTR szTmp2 = TObjRef<Util>()->UnicodeToUnicode8(pProto->szDesc);
				lua_pushlstring(L, szTmp2, strlen(szTmp2));
#else
				lua_pushlstring(L, pProto->szDesc, strlen(pProto->szDesc));
#endif
				return 3;
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备龙魂能力激活条件
//-------------------------------------------------------------------------------------
int SC_GetEquipLonghunActive(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -2));
	bool bInner = (lua_toboolean(L, -1) == 0 ? false : true);
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushboolean(L, ItemMgr::Inst()->GetLonghun((EEquipPos)pEquip->GetPos(), pEquip->GetItemId()));
		EEquipPos ePos = pEquip->GetEquipPos();
		
		if(bInner)
		{
            if(EICT_Equip != pEquip->GetConType())
            {
                if (EEP_Finger1 == pEquip->GetEquipPos())
                {
                    lua_pushnumber(L, g_sLongHunActivePos[EEP_Finger1].eInner[0]);
                    lua_pushnumber(L, g_sLongHunActivePos[EEP_Finger1].eInner[1]);
                    lua_pushnumber(L, g_sLongHunActivePos[EEP_Finger2].eInner[0]);
                    lua_pushnumber(L, g_sLongHunActivePos[EEP_Finger2].eInner[1]);
                    return 5;
                }
                else
                {
                    ePos = pEquip->GetEquipPos();
                    lua_pushnumber(L, g_sLongHunActivePos[ePos].eInner[0]);
                    lua_pushnumber(L, g_sLongHunActivePos[ePos].eInner[1]);
                    lua_pushnumber(L, -1);
                    lua_pushnumber(L, -1);
                    return 5;
                }
            }
            else
            {
                ePos = (EEquipPos)pEquip->GetPos();
                lua_pushnumber(L, g_sLongHunActivePos[ePos].eInner[0]);
                lua_pushnumber(L, g_sLongHunActivePos[ePos].eInner[1]);
                lua_pushnumber(L, -1);
                lua_pushnumber(L, -1);
                return 5;
            }
		}
		else
		{
            if(EICT_Equip != pEquip->GetConType())
            {
                if (EEP_Finger1 == pEquip->GetEquipPos())
                {
                    lua_pushnumber(L, g_sLongHunActivePos[EEP_Finger1].eOuter);
                    lua_pushnumber(L, g_sLongHunActivePos[EEP_Finger2].eOuter);
                    return 3;
                }
                else
                {
                    ePos = pEquip->GetEquipPos();
                    lua_pushnumber(L, g_sLongHunActivePos[ePos].eOuter);
                    lua_pushnumber(L, -1);
                    return 3;
                }

            }
            else
            {
                ePos = (EEquipPos)pEquip->GetPos();
                lua_pushnumber(L, g_sLongHunActivePos[ePos].eOuter);
                lua_pushnumber(L, -1);
                return 3;
            }
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------
// 获取装备龙魂能力五行属性
//-------------------------------------------------------------------------------------
int SC_GetEquipLonghunFive(lua_State* L)
{
	const tagLongHunProtoClient* pProto = (tagLongHunProtoClient*)(lua_touserdata(L, -2));
	INT nIndex = (INT)(lua_tonumber(L, -1));

	if(P_VALID(pProto) && nIndex >= 0 && nIndex < X_WUXING_TYPE_NUM)
	{
		lua_pushnumber(L, pProto->byWuXingActive[nIndex]);
		return 1;
	}
	return 0;
};

//-------------------------------------------------------------------------------------
// 获取装备所属套装名称
//-------------------------------------------------------------------------------------
int SC_GetEquipSuitName(lua_State* L)
{
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -3));
	BYTE				 byQual = (BYTE)(lua_tonumber(L, -2));		
	INT					 nIndex	= (INT)(lua_tonumber(L, -1));
	if(P_VALID(pProto) && nIndex >= 0 && nIndex < MAX_PEREQUIP_SUIT_NUM
		&& pProto->bySuitMinQlty[nIndex] <= byQual)
	{
		const tagSuitProtoClient* pSuit = ItemProtoData::Inst()->FindSuitProto(pProto->dwSuitID[nIndex]);
		if (P_VALID(pSuit))
		{
			lua_pushlightuserdata(L, (void*)pSuit);
#ifdef UNICODE
			LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pSuit->szSuitName);
			lua_pushlstring(L, szTmp, strlen(szTmp));
#else
			lua_pushlstring(L, pSuit->szSuitName, strlen(pSuit->szSuitName));
#endif
			INT nNum = ItemMgr::Inst()->GetSuitNum(pSuit->dwID);
			lua_pushnumber(L, GT_INVALID == nNum ? 0 : nNum);
			
			// 计算套装件数最大值
			INT nMax = 0;
			for(INT i=0; i<MAX_SUIT_ATT_NUM; ++i)
			{
				if(!P_VALID(pSuit->dwTriggerID[i]))
					break;

				nMax = pSuit->n8ActiveNum[i];
			}
			lua_pushnumber(L, nMax);

			return 4;
		}
		
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备当前套装属性
//-------------------------------------------------------------------------------------
int SC_GetEquipSuitAttribute(lua_State* L)
{
	const tagSuitProtoClient* pSuit	= (const tagSuitProtoClient*)(lua_touserdata(L, -2));
	INT	nIndex = (INT)(lua_tonumber(L, -1));
	if (P_VALID(pSuit) && nIndex >= 0 && nIndex < MAX_SUIT_ATT_NUM)
	{
		
		lua_pushnumber(L, pSuit->n8ActiveNum[nIndex]);
		const tagBuffProtoClient* pBuff = SkillProtoData::Inst()->FindBuffProto(pSuit->dwBuffID[nIndex]);
		if (P_VALID(pBuff))
		{
#ifdef UNICODE
			LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pBuff->szDesc);
			lua_pushlstring(L, szTmp, strlen(szTmp));
#else 
			lua_pushlstring(L, pBuff->szDesc, strlen(pBuff->szDesc));
#endif
			
			return 2;
		}
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取装备当前属性限制
//-------------------------------------------------------------------------------------
int SC_GetEquipAttributeLimit(lua_State* L)
{
	Equipment*			 pEquip = (Equipment*)			(lua_touserdata(L, -3));
	const tagEquipProto* pProto = (const tagEquipProto*)(lua_touserdata(L, -2));
	INT					 nIndex	=  	   			   (INT)(lua_tonumber(L, -1));
	if (P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquipAttLimit((ERoleAttribute)nIndex));
		return 1;
	}
	else if (P_VALID(pProto) && nIndex >= ERA_AttA_Start && nIndex <= ERA_AttA_End)
	{
		lua_pushnumber(L, pProto->n16AttALimit[nIndex]);
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
// 获取装备特殊属性
//-------------------------------------------------------------------------------------
int SC_GetEquipSpecAttribute(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquipSpecAttribute()+1);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备鉴定修正率
//-------------------------------------------------------------------------------------
int SC_GetEquipQltyModPct(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		lua_pushnumber(L, pEquip->GetEquip()->n16QltyModPct);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备的颜色（主要是时装）
//-------------------------------------------------------------------------------------
int SC_GetEquipColor(lua_State* L)
{
	Equipment* pEquip = (Equipment*)(lua_touserdata(L, -1));
	if(P_VALID(pEquip) && pEquip->IsItem())
	{
		if(pEquip->GetProto()->eType == EIT_Fashion && pEquip->IsCanDye())
		{
			lua_pushnumber(L, pEquip->GetEquip()->n8ColorID);
			return 1;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取装备的品级影响比率
//------------------------------------------------------------------------------------- 
int SC_GetEquipQltyEffect(lua_State* L)
{
	INT nQlty = (INT)(lua_tonumber(L, -1));
	const tagEquipQltyEffect* pAtt = ItemProtoData::Inst()->FindEquipQltyEffect(nQlty);
	if(P_VALID(pAtt))
	{
		lua_pushnumber(L, pAtt->fWeaponFactor);
		lua_pushnumber(L, pAtt->fArmorFactor);
		lua_pushnumber(L, pAtt->fPotFactor);
		return 3;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
//										技能信息相关函数
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// 获取技能名称
//-------------------------------------------------------------------------------------
int SC_GetSkillName(lua_State *L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->szName);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, pProto->szName, strlen(pProto->szName));
#endif
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能描述
//-------------------------------------------------------------------------------------
int SC_GetSkillDesc(lua_State *L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->szDesc);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, pProto->szDesc, strlen(pProto->szDesc));
#endif
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能类型
//-------------------------------------------------------------------------------------
int SC_GetSkillType(lua_State *L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->nType2);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能职业限制
//-------------------------------------------------------------------------------------
int SC_GetSkillClassLimit(lua_State* L)
{
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能施展类型
//-------------------------------------------------------------------------------------
int SC_GetSkillUseType(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->eUseType);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能性别限制
//-------------------------------------------------------------------------------------
int SC_GetSkillSexLimit(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->eSexLimit);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能位置限制
//-------------------------------------------------------------------------------------
int SC_GetSkillPosLimit(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->ePosType);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能骑战限制
//-------------------------------------------------------------------------------------
int SC_GetSkillMountLimit(lua_State* L)
{
	//const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	//if(P_VALID(pProto))
	//{
	//	lua_pushboolean(L, pProto->bMountLimit);
	//	return 1;
	//}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能是否已学习
//-------------------------------------------------------------------------------------
int SC_GetSkillIsLearn(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushboolean(L, SkillMgr::Inst()->IsHaveSkillByTypeID(pProto->dwID));
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能升级方式
//-------------------------------------------------------------------------------------
int SC_GetSkillLevelupType(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->eLevelUpType);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能前置技能
//-------------------------------------------------------------------------------------
int SC_GetSkillPreSkill(lua_State* L)
{
	const tagSkillData*		   pSkill = (tagSkillData*)		  (lua_touserdata(L, -2));	
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pSkill))
	{
		const tagSkillProtoClient* pNext = SkillProtoData::Inst()->FindSkillProto(pSkill->dwTypeID + 1);
		if(P_VALID(pNext))
		{
			const tagSkillProtoClient* pPre = SkillProtoData::Inst()->FindSkillProto(pNext->dwPreLevelSkillID);
			if(P_VALID(pPre))
			{
#ifdef UNICODE
				LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pPre->szName);
				lua_pushlstring(L, szTmp, strlen(szTmp));
#else 
				lua_pushlstring(L, pPre->szName, strlen(pPre->szName));
#endif
				lua_pushnumber(L, pPre->nLevel);
				lua_pushboolean(L, SkillMgr::Inst()->IsHaveSkillByTypeID(pPre->dwID));
				return 3;
			}
		}
	}
	else if(P_VALID(pProto))
	{
		const tagSkillProtoClient* pPre = SkillProtoData::Inst()->FindSkillProto(pProto->dwPreLevelSkillID);
		if(P_VALID(pPre))
		{
#ifdef UNICODE
			LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pPre->szName);
			lua_pushlstring(L, szTmp, strlen(szTmp));
#else
			lua_pushlstring(L, pPre->szName, strlen(pPre->szName));
#endif
			lua_pushnumber(L, pPre->nLevel);
			lua_pushboolean(L, SkillMgr::Inst()->IsHaveSkillByTypeID(pPre->dwID));
			return 3;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能学习等级限制
//-------------------------------------------------------------------------------------
int SC_GetSkillLevelLimit(lua_State* L)
{
	const tagSkillData*		   pSkill = (tagSkillData*)		  (lua_touserdata(L, -2));
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pSkill))
	{
		const tagSkillProtoClient* pNext = SkillProtoData::Inst()->FindSkillProto(pSkill->dwTypeID + 1);
		if(P_VALID(pNext))
		{
			lua_pushnumber(L, pNext->nNeedRoleLevel);
			return 1;
		}
	}
	else if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->nNeedRoleLevel);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能学习天资限制
//-------------------------------------------------------------------------------------
int SC_GetSkillTalentLimit(lua_State* L)
{
	const tagSkillData*		   pSkill = (tagSkillData*)		  (lua_touserdata(L, -2));
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pSkill))
	{
		const tagSkillProtoClient* pNext = SkillProtoData::Inst()->FindSkillProto(pSkill->dwTypeID + 1);
		if(P_VALID(pNext))
		{
			lua_pushnumber(L, pNext->nNeedTalentPoint);
			lua_pushnumber(L, pNext->eTalentType);
			return 2;
		}
	}
	else if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->nNeedTalentPoint);
		lua_pushnumber(L, pProto->eTalentType);
		return 2;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能伤害类型
//-------------------------------------------------------------------------------------
int SC_GetSkillDamageType(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->eDmgType);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能等级
//-------------------------------------------------------------------------------------
int SC_GetSkillLevel(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->nLevel);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能最大等级
//-------------------------------------------------------------------------------------
int SC_GetSkillMaxLevel(lua_State* L)
{
    const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
    if(P_VALID(pProto))
    {
        lua_pushnumber(L, pProto->nMaxLevel);
        return 1;
    }
    return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能熟练度
//-------------------------------------------------------------------------------------
int SC_GetSkillExp(lua_State* L)
{
	const tagSkillData* pSkill = (tagSkillData*)(lua_touserdata(L, -1));
	if(P_VALID(pSkill))
	{
		lua_pushnumber(L, pSkill->nExp);
		lua_pushnumber(L, pSkill->pProto->nLevelUpExp);
		return 2;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能伤害
//-------------------------------------------------------------------------------------
int SC_GetSkillDamage(lua_State* L)
{
	const tagSkillData*		   pSkill = (tagSkillData*)		  (lua_touserdata(L, -2));
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pSkill))
	{
        INT nSingle = 0;
        if (pSkill->pProto->nDmgTimes > 0 && pSkill->pProto->nChannelDmg[0] > 100000)
        {
            nSingle = pSkill->pProto->nChannelDmg[0];
        }
        lua_pushnumber(L, nSingle);
		lua_pushnumber(L, pSkill->nSkillDmg);
		return 2;
	}
	else if(P_VALID(pProto))
	{
		//如果技能没找到，则只跟据相应模板属性
        INT nSingle = 0;
        if (pProto->nDmgTimes > 0 && pProto->nChannelDmg[0] > 100000)
        {
            nSingle = pProto->nChannelDmg[0];
        }

		INT nSum = 0;
		for(INT i=0; i<pProto->nDmgTimes; ++i)
		{
			nSum += pProto->nChannelDmg[i];
		}
        lua_pushnumber(L, nSingle);
		lua_pushnumber(L, nSum);
		return 2;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能施展距离
//-------------------------------------------------------------------------------------
int SC_GetSkillOPDistance(lua_State* L)
{
	const tagSkillData*		   pSkill = (tagSkillData*)		  (lua_touserdata(L, -2));
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pSkill))
	{
		lua_pushnumber(L, pSkill->fOPDist);
		return 1;
	}
	else if(P_VALID(pProto))
	{
		//如果技能没找到，则只跟据相应模板属性
		lua_pushnumber(L, pProto->fOPDist);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能起手时间
//-------------------------------------------------------------------------------------
int SC_GetSkillPrepareTime(lua_State* L)
{
	const tagSkillData*		   pSkill = (tagSkillData*)		  (lua_touserdata(L, -2));
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pSkill))
	{
		lua_pushnumber(L, pSkill->nPrepareTime);
		return 1;
	}
	else if(P_VALID(pProto))
	{
		//如果技能没找到，则只跟据相应模板属性
		lua_pushnumber(L, pProto->nPrepareTime);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能恢复时间
//-------------------------------------------------------------------------------------
int SC_GetSkillCooldownTime(lua_State* L)
{
	const tagSkillData*		   pSkill = (tagSkillData*)		  (lua_touserdata(L, -2));
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pSkill))
	{
		lua_pushnumber(L, pSkill->nCooldown);
		return 1;
	}
	else if(P_VALID(pProto))
	{
		//如果技能没找到，则只跟据相应模板属性
		lua_pushnumber(L, pProto->nCoolDown);
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能属性加强值
//-------------------------------------------------------------------------------------
int SC_GetSkillAttribute(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_newtable(L);
		pProto->mapRoleAttMod.ResetIterator();
		ERoleAttribute eAtt;
		INT nValue;
		while(pProto->mapRoleAttMod.PeekNext(eAtt, nValue))
		{
			lua_pushnumber(L, eAtt);
			lua_pushnumber(L, nValue);
			lua_settable(L, -3);
		}
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能属性百分比加强值
//-------------------------------------------------------------------------------------
int SC_GetSkillAttributePct(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -2));
	if(P_VALID(pProto))
	{
		lua_newtable(L);
		pProto->mapRoleAttModPct.ResetIterator();
		ERoleAttribute eAtt;
		INT nValue;
		while(pProto->mapRoleAttModPct.PeekNext(eAtt, nValue))
		{
			lua_pushnumber(L, eAtt);
			lua_pushnumber(L, nValue);
			lua_settable(L, -3);
		}
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能消耗
//-------------------------------------------------------------------------------------
int SC_GetSkillCost(lua_State* L)
{
	const tagSkillData*		   pSkill = (tagSkillData*)		   (lua_touserdata(L, -3));	
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*) (lua_touserdata(L, -2));
	ESkillCostType				eCost = (ESkillCostType)(DWORD)(lua_tonumber(L, -1));
	if(P_VALID(pSkill))
	{
		if(eCost >= ESCT_HP && eCost < ESCT_End)
		{
			lua_pushnumber(L, pSkill->nCost[eCost]);
			return 1;
		}
	}
	else if(P_VALID(pProto))
	{
		if(eCost >= ESCT_HP && eCost < ESCT_End)
		{
			lua_pushnumber(L, pProto->nSkillCost[eCost]);
			return 1;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取技能下一级描述
//-------------------------------------------------------------------------------------
int SC_GetSkillNextLevel(lua_State* L)
{
	const tagSkillProtoClient* pProto = (tagSkillProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->szNextDesc);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, pProto->szNextDesc, strlen(pProto->szNextDesc));
#endif
		return 1;
	}
	return 0;
}


//-------------------------------------------------------------------------------------
//							               状态信息相关函数
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// 获取状态名称
//-------------------------------------------------------------------------------------
int SC_GetBuffName(lua_State *L)
{
	const tagBuffProtoClient* pProto = (tagBuffProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->szName);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else 
		lua_pushlstring(L, pProto->szName, strlen(pProto->szName));
#endif
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取状态描述
//-------------------------------------------------------------------------------------
int SC_GetBuffDesc(lua_State *L)
{
	const tagBuffProtoClient* pProto = (tagBuffProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->szDesc);
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else 
		lua_pushlstring(L, pProto->szDesc, strlen(pProto->szDesc));
#endif
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
// 获取状态类型
//-------------------------------------------------------------------------------------
int SC_GetBuffType(lua_State *L)
{
	const tagBuffProtoClient* pProto = (tagBuffProtoClient*)(lua_touserdata(L, -1));
	if(P_VALID(pProto))
	{
		lua_pushnumber(L, pProto->nType3);
		return 1;
	}
	return 0;
}
//-----------------------------------------------------------------------------------
// 获取状态剩余时间
//-----------------------------------------------------------------------------------
int SC_GetBuffRemainTime(lua_State* L)
{
	const tagRoleBuff* pBuff = (tagRoleBuff*)(lua_touserdata(L, -1));
	if(P_VALID(pBuff))
	{
		if(pBuff->nPersistTimeLeft > 0)
		{	
			INT nvalue = (INT)( Kernel::Inst()->GetAccumTimeDW() - pBuff->dwStartTime );
			INT	nRemainTime = pBuff->nPersistTimeLeft - nvalue + nvalue % 1000 ;
			lua_pushnumber(L, nRemainTime/1000);
		}
		else
		{
			lua_pushnumber(L, 0);
		}
		return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------
//											价格相关
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// 获取价格金钱数
//-------------------------------------------------------------------------------------
int SC_GetPriceSilver(lua_State* L)
{
	const tagItemCost* pCost = (tagItemCost*)(lua_touserdata(L, -1));
	if(P_VALID(pCost))
	{
		lua_pushnumber(L, MSilver2DBGold(pCost->n64CostSilver));
		lua_pushnumber(L, MSilver2DBSilver(pCost->n64CostSilver));
		return 2;
	}
	return 0;
}

//------------------------------------------------------------------------------------
// 获取价格特殊消耗类型
//------------------------------------------------------------------------------------
int SC_GetPriceSpecCost(lua_State* L)
{
	const tagItemCost* pCost = (tagItemCost*)(lua_touserdata(L, -1));
	if(P_VALID(pCost) && ECCT_Null != pCost->eCostType)
	{
		lua_pushnumber(L, pCost->eCostType);
		lua_pushnumber(L, pCost->nCostNum);
		lua_pushnumber(L, pCost->nRepLevel);

		if(pCost->eCostType == ECCT_ItemExchange)
		{
			const tagItemDisplayInfo* pInfo = ItemProtoData::Inst()->FindItemDisplay(pCost->dwCostItemTypeID);
			if(P_VALID(pInfo))
			{
#ifdef UNICODE
				LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pInfo->szName);
				lua_pushlstring(L, szTmp, strlen(szTmp));
#else
				lua_pushlstring(L, pInfo->szName, strlen(pInfo->szName));
#endif		
				return 4;
			}
		}

		return 3;
	}
	return 0;
}


//------------------------------------------------------------------------------------
// 获取商货名字
//------------------------------------------------------------------------------------
int SC_GetCommerceName(lua_State *L)
{
	
	const tagGoodInfo* pInfo = (const tagGoodInfo*)(lua_touserdata(L, -1));
	if (P_VALID(pInfo))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pInfo->strName.c_str());
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, pInfo->strName.c_str(), strlen(pInfo->strName.c_str()));
#endif
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------------
// 获取商货类型
//------------------------------------------------------------------------------------
int SC_GetCommerceType(lua_State *L)
{
	const tagGoodInfo* pInfo = (const tagGoodInfo*)(lua_touserdata(L, -1));
	if (P_VALID(pInfo))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pInfo->strDisplayType.c_str());
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, pInfo->strDisplayType.c_str(), strlen(pInfo->strDisplayType.c_str()));
#endif
		return 1;
	}
	return 0;
}


//------------------------------------------------------------------------------------
// 获取商货描述
//------------------------------------------------------------------------------------
int SC_GetCommerceDesc(lua_State *L)
{
	const tagGoodInfo* pInfo = (const tagGoodInfo*)(lua_touserdata(L, -1));
	if (P_VALID(pInfo))
	{
#ifdef UNICODE
		LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pInfo->strDesc.c_str());
		lua_pushlstring(L, szTmp, strlen(szTmp));
#else
		lua_pushlstring(L, pInfo->strDesc.c_str(), strlen(pInfo->strDesc.c_str()));
#endif
		return 1;
	}
	return 0;
}



//------------------------------------------------------------------------------------
// 获取宠物技能类型
//------------------------------------------------------------------------------------
int SC_GetPetSkillType(lua_State *L)
{
    const tagPetSkillData* pProto = (const tagPetSkillData*)(lua_touserdata(L, -1));
    if(P_VALID(pProto))
    {
#ifdef UNICODE
        LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->strType.c_str());
        lua_pushlstring(L, szTmp, strlen(szTmp));
#else 
        lua_pushlstring(L, pProto->strType.c_str(), strlen(pProto->strType.c_str()));
#endif
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------------
// 获取宠物技能名称
//------------------------------------------------------------------------------------
int SC_GetPetSkillName(lua_State *L)
{
    const tagPetSkillData* pProto = (tagPetSkillData*)(lua_touserdata(L, -1));
    if(P_VALID(pProto))
    {
#ifdef UNICODE
        LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->strName.c_str());
        lua_pushlstring(L, szTmp, strlen(szTmp));
#else 
        lua_pushlstring(L, pProto->strName.c_str(), strlen(pProto->strName.c_str()));
#endif
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------------
// 获取宠物技能描述
//------------------------------------------------------------------------------------
int SC_GetPetSkillDesc(lua_State *L)
{
    const tagPetSkillData* pProto = (tagPetSkillData*)(lua_touserdata(L, -1));
    if(P_VALID(pProto))
    {
#ifdef UNICODE
        LPCSTR szTmp = TObjRef<Util>()->UnicodeToUnicode8(pProto->strDesc.c_str());
        lua_pushlstring(L, szTmp, strlen(szTmp));
#else 
        lua_pushlstring(L, pProto->strDesc.c_str(), strlen(pProto->strDesc.c_str()));
#endif
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------------
// 获取宠物技能等级
//------------------------------------------------------------------------------------
int SC_GetPetSkillLevel(lua_State *L)
{
    const tagPetSkillData* pProto = (tagPetSkillData*)(lua_touserdata(L, -1));
    if(P_VALID(pProto))
    {
        lua_pushnumber(L, pProto->nLevel);
        return 1;
    }
    return 0;
}


//------------------------------------------------------------------------------------
// 获取升级帮派技能所需资金
//------------------------------------------------------------------------------------
int SC_GetResearchGuildSkillCostFund(lua_State *L)
{
	const tagSkillProtoClient* pAtt = (const tagSkillProtoClient*)(lua_touserdata(L, -1));

	if (P_VALID(pAtt))
	{
		DWORD dwID = pAtt->dwID;
		const tagGuildSkillInfo *pInfo = GuildMgr::Inst()->FindSkillInfo(dwID);
		if (P_VALID(pInfo))
		{
			lua_pushnumber(L, (lua_Number)pInfo->nResearchFund);
			return 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------------
// 获取升级帮派技能所需资材
//------------------------------------------------------------------------------------
int SC_GetResearchGuildSkillCostMaterial(lua_State *L)
{
	const tagSkillProtoClient* pAtt = (const tagSkillProtoClient*)(lua_touserdata(L, -1));

	if (P_VALID(pAtt))
	{
		DWORD dwID = pAtt->dwID;
		const tagGuildSkillInfo *pInfo = GuildMgr::Inst()->FindSkillInfo(dwID);
		if (P_VALID(pInfo))
		{
			lua_pushnumber(L, (lua_Number)pInfo->nResearchMaterial);
			return 1;
		}
	}
	return 0;
}




//-----------------------------------------------------------------------------
// table of function names and entry points that will be registered with Lua
//-----------------------------------------------------------------------------
static const luaL_reg tooltipLib[] = 
{
	{"GetLocalPlayerLevel",				SC_GetLocalPlayerLevel},
	{"GetLocalPlayerSex",				SC_GetLocalPlayerSex},
	{"GetLocalPlayerClass",				SC_GetLocalPlayerClass},
	{"GetLocalPlayerRepution",			SC_GetLocalPlayerRepution},
	{"GetLocalPlayerContribution",		SC_GetLocalPlayerContribution},
	{"GetLocalPlayerTalentBidden",		SC_GetLocalPlayerTalentBidden},
	{"GetLocalPlayerAttribute",			SC_GetLocalPlayerAttribute},
	{"GetLocalPlayerAttBidden",			SC_GetLocalPlayerAttBidden},
	{"GetLocalPlayerAttLevelup",		SC_GetLocalPlayerAttLevelup},
	{"GetLocalPlayerCostAttCurValue",	SC_GetLocalPlayerCostAttCurValue},
	//more
	{"GetItemName",						SC_GetItemName},
	{"GetItemNameEx",					SC_GetItemNameEx},
	{"GetItemDesc",						SC_GetItemDesc},
	{"GetItemType",						SC_GetItemType},
	{"GetItemQuality",					SC_GetItemQuality},
	{"GetItemBindType",					SC_GetItemBindType},
	{"GetItemBindState",				SC_GetItemBindState},
	{"GetItemLockState",				SC_GetItemLockState},
	{"GetItemSexLimit",					SC_GetItemSexLimit},
	{"GetItemLevelLimit",				SC_GetItemLevelLimit},
	{"GetItemMaxUseTimes",				SC_GetItemMaxUseTimes},
	{"GetItemUseTimes",					SC_GetItemUseTimes},
	{"GetItemCreator",					SC_GetItemCreator},
	{"GetItemUseType",					SC_GetItemUseType},
	{"GetItemCostPotVal",				SC_GetItemCostPotVal},
	{"GetItemHoldEquipPos",				SC_GetItemHoldEquipPos},
    {"GetItemAllTimeLimit",             SC_GetItemAllTimeLimit},
    {"GetItemProtoAllTimeLimit",        SC_GetItemProtoAllTimeLimit},
	{"GetItemTimeLimit",				SC_GetItemTimeLimit},
	{"GetItemLearnSkill",				SC_GetItemLearnSkill},
	{"GetItemClanReputeLimit",			SC_GetItemClanReputeLimit},
	{"GetItemOtherClanReputeLimit",		SC_GetItemOtherClanReputeLimit},
    {"GetItemCoolDownTime",             SC_GetItemCoolDownTime},
    {"GetItemCanSell",                  SC_GetItemCanSell},
    {"GetItemPreTime",                  SC_GetItemPreTime},
    {"GetItemMaxItem",                  SC_GetItemMaxItem},
    {"GetItemBindRoleName",             SC_GetItemBindRoleName},
    
	//more
	{"GetEquipDescEx",					SC_GetEquipDescEx},
	{"GetEquipClassLimit",				SC_GetEquipClassLimit},
	{"GetEquipPos",						SC_GetEquipPos},
	{"GetEquipWeaponDamage",			SC_GetEquipWeaponDamage},
	{"GetEquipWeaponWuhun",				SC_GetEquipWeaponWuhun},
	{"GetEquipArmorDefine",				SC_GetEquipArmorDefine},
	{"GetEquipNewNess",					SC_GetEquipNewNess},
	{"GetEquipPotTime",					SC_GetEquipPotAdvanceTime},
	{"GetEquipPotVal",					SC_GetEquipPotVal},
	{"GetEquipProtoAtt",				SC_GetEquipProtoAtt},
	{"GetEquipAttribute",				SC_GetEquipAttribute},
	{"GetEquipAttributePct",			SC_GetEquipAttributePct},
	{"GetEquipProduceState",			SC_GetEquipProduceState},
	{"GetEquipHoldNum",					SC_GetEquipHoldNum},
	{"GetEquipHoldGem",					SC_GetEquipHoldGem},
	{"GetEquipLongfu",					SC_GetEquipLongfu},
	{"GetEquipLonghun",					SC_GetEquipLonghun},
	{"GetEquipLonghunActive",			SC_GetEquipLonghunActive},
	{"GetEquipLonghunFive",				SC_GetEquipLonghunFive},
	{"GetEquipSpecAttribute",			SC_GetEquipSpecAttribute},
	{"GetEquipSuitName",				SC_GetEquipSuitName},
	{"GetEquipSuitAttribute",			SC_GetEquipSuitAttribute},
	{"GetEquipAttributeLimit",			SC_GetEquipAttributeLimit},
	{"GetEquipQltyModPct",				SC_GetEquipQltyModPct},
	{"GetEquipColor",					SC_GetEquipColor},
	{"GetEquipQltyEffect",				SC_GetEquipQltyEffect},
    {"GetEquipType",                    SC_GetEquipType},
	//more
	{"GetSkillName",					SC_GetSkillName},
	{"GetSkillDesc",					SC_GetSkillDesc},
	{"GetSkillType",					SC_GetSkillType},
	{"GetSkillClassLimit",				SC_GetSkillClassLimit},
	{"GetSkillUseType",					SC_GetSkillUseType},
	{"GetSkillSexLimit",				SC_GetSkillSexLimit},
	{"GetSkillPosLimit",				SC_GetSkillPosLimit},
	{"GetSkillMountLimit",				SC_GetSkillMountLimit},
	{"GetSkillIsLearn",					SC_GetSkillIsLearn},
	{"GetSkillLevelupType",				SC_GetSkillLevelupType},
	{"GetSkillPreSkill",				SC_GetSkillPreSkill},
	{"GetSkillLevelLimit",				SC_GetSkillLevelLimit},
	{"GetSkillTalentLimit",				SC_GetSkillTalentLimit},
	{"GetSkillDamageType",				SC_GetSkillDamageType},
	{"GetSkillLevel",					SC_GetSkillLevel},
    {"GetSkillMaxLevel",				SC_GetSkillMaxLevel},
	{"GetSkillExp",						SC_GetSkillExp},
	{"GetSkillDamage",					SC_GetSkillDamage},
	{"GetSkillOPDistance",				SC_GetSkillOPDistance},
	{"GetSkillPrepareTime",				SC_GetSkillPrepareTime},
	{"GetSkillCooldownTime",			SC_GetSkillCooldownTime},
	{"GetSkillAttribute",				SC_GetSkillAttribute},
	{"GetSkillAttributePct",			SC_GetSkillAttributePct},
	{"GetSkillCost",					SC_GetSkillCost},
	{"GetSkillNextLevel",				SC_GetSkillNextLevel},
	//more
	{"GetBuffName",						SC_GetBuffName},
	{"GetBuffDesc",						SC_GetBuffDesc},
	{"GetBuffType",						SC_GetBuffType},
	{"GetBuffRemainTime",				SC_GetBuffRemainTime},
	//more
	{"GetPriceSilver",					SC_GetPriceSilver},
	{"GetPriceSpecCost",				SC_GetPriceSpecCost},
    //more
    {"GetPetSkillType",                 SC_GetPetSkillType},
    {"GetPetSkillName",                 SC_GetPetSkillName},
    {"GetPetSkillDesc",                 SC_GetPetSkillDesc},
    {"GetPetSkillLevel",                SC_GetPetSkillLevel},
	// 商货
	{"GetCommerceName",					SC_GetCommerceName},
	{"GetCommerceType",					SC_GetCommerceType},
	{"GetCommerceDesc",					SC_GetCommerceDesc},
	// 帮派技能
	{"GetResearchGuildSkillCostFund",				SC_GetResearchGuildSkillCostFund},
	{"GetResearchGuildSkillCostMaterial",			SC_GetResearchGuildSkillCostMaterial},
	{NULL, NULL}
};

//-----------------------------------------------------------------------------
// LUA OPEN SCRIPT LIB
// Registers all the basic script support functions with Lua.
//-----------------------------------------------------------------------------
VOID LuaOpenToolTipLib(lua_State*  l)
{
	// register our library of routines under the table name Script
	luaI_openlib(l, "Tooltip",	tooltipLib, 0);
}