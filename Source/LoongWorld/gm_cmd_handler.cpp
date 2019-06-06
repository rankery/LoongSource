//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: gm_cmd_handler.h
// author: 
// actor:
// data: 2008-09-11
// last:
// brief: 处理GM命令
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/msg_gm_cmd.h"
#include "../WorldDefine/currency_define.h"
#include "../ServerDefine/msg_vcard.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../WorldDefine/reputation.h"
#include "../WorldDefine/quest.h"
#include "../WorldDefine/msg_combat.h"
#include "../ServerDefine/role_data.h"

#include "role.h"
#include "player_session.h"
#include "gm_net_cmd_mgr.h"
#include "item_creator.h"
#include "map_creator.h"
#include "role_mgr.h"
#include "world_session.h"
#include "pet_pocket.h"
#include "db_session.h"
#include "gm_policy.h"
#include "title_mgr.h"
#include "quest_mgr.h"
#include "mall.h"
#include "vip_netbar.h"
#include "guild.h"
#include "guild_mgr.h"
#include "guild_commodity.h"
#include "world_session.h"
#include "creature.h"

//-----------------------------------------------------------------------------
// 处理GM通用命令
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGMCommand(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GMCommand);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}
	
	//m_GMCommandMgr.SetGMCmdID(pRole->GetID());
	
	// 调用相应控制台命令接口
	return m_GMCommandMgr.Excute(pRecv->szCommand, pRole);
}

//-----------------------------------------------------------------------------
// 通过TypeID获得物品(item dwTypeID nItemNum nQuality)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleCreateItem(const std::vector<DWORD>& vectParam, Role* pGM)
{
	DWORD dwTypeID	= (DWORD)_ttoi((LPCTSTR)vectParam[0]);
	INT16 n16Num	= (INT16)_ttoi((LPCTSTR)vectParam[1]);
	INT32 nQlty		= (INT32)_ttoi((LPCTSTR)vectParam[2]);

	if(n16Num <= 0)
	{
		n16Num = 1;
	}
	
	tagItem *pNewItem = ItemCreator::Create(EICM_GM, pGM->GetID(), dwTypeID, n16Num);
	if(!P_VALID(pNewItem))
	{
		return GT_INVALID;
	}

	if(MIsEquipment(pNewItem->dwTypeID))
	{
		if(nQlty == GT_INVALID)
		{
			// 不鉴定	
		}
		else
		{
			ItemCreator::IdentifyEquip((tagEquip*)pNewItem, (EItemQuality)nQlty);
		}
	}

	pGM->GetItemMgr().Add2Bag(pNewItem, ELCID_GM_CreateItem, TRUE);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获得金钱(silver nGold nSilver)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGetSilver(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT32 nGold		= (INT32)_ttoi((LPCTSTR)vectParam[0]);
	INT32 nSilver	= (INT32)_ttoi((LPCTSTR)vectParam[1]);

	if(nGold < 0 || nSilver < 0)
	{
		return GT_INVALID;
	}

	INT64 nTotalSilver = MGold2Silver(nGold) + (INT64)nSilver;
	pGM->GetCurMgr().IncBagSilver(nTotalSilver, ELCID_GM_GetMoney);
	
	return E_Success;
}

//-----------------------------------------------------------------------------
// 获得元宝(yuanbao nYuanBao)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGetYuanBao(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT32 nYuanBao = (INT32)_ttoi((LPCTSTR)vectParam[0]);

	if(nYuanBao < 0)
	{
		return GT_INVALID;
	}

	pGM->GetCurMgr().IncBagYuanBao(nYuanBao, ELCID_GM_GetYuanBao);
	
	return E_Success;
}

//-----------------------------------------------------------------------------
// 获得赠点(exvolume nExVolume)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGetExVolume(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT32 nExVolume = (INT32)_ttoi((LPCTSTR)vectParam[0]);

	if(nExVolume < 0)
	{
		return GT_INVALID;
	}

	pGM->GetCurMgr().IncExchangeVolume(nExVolume, ELCID_GM_GetExVolume);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 通过TypeID获得物品(clearbag)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleClearBag(const std::vector<DWORD>& vectParam, Role* pGM)
{
	ItemMgr			&itemMgr	= pGM->GetItemMgr();

	tagItem *pItem = NULL;
	INT16 n16SzBag = itemMgr.GetBagCurSize();
	for(INT16 i=0; i<n16SzBag; ++i)
	{
		if(itemMgr.GetBagFreeSize() == n16SzBag)
		{
			break;
		}

		pItem = itemMgr.GetBagItem(i);
		if(!P_VALID(pItem))
		{
			continue;
		}

		itemMgr.DelFromBag(pItem->n64Serial, ELCID_GM_ClearBag);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 移动到指定地点(goto mapName x z)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGoto(const std::vector<DWORD>& vectParam, Role* pGM)
{
	Map *pMap = pGM->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}
	
	DWORD dwMapID = (DWORD)m_pUtil->Crc32((LPCTSTR)vectParam[0]);
	FLOAT fX = (FLOAT)_ttoi((LPCTSTR)vectParam[1]) * TILE_SCALE;
	FLOAT fZ = (FLOAT)_ttoi((LPCTSTR)vectParam[2]) * TILE_SCALE;

	if( FALSE == g_mapCreator.IsMapExist(dwMapID) )
		dwMapID = pGM->GetMapID();

	if( pGM->GotoNewMap(dwMapID, fX, GET_HEIGHT_MAX_Y, fZ) )
	{
		return E_Success;
	}
	else
	{
		return GT_INVALID;
	}
}

//-----------------------------------------------------------------------------
// GM移动到玩家身边(gotorole szRoleName)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGotoRole(const std::vector<DWORD>& vectParam, Role* pGM)
{
	Map *pMap = pGM->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	tstring strRoleName;
	strRoleName = (LPCTSTR)vectParam[0];
	DWORD dwRoleNameCrc = (DWORD)m_pUtil->Crc32(strRoleName.c_str());

	//DWORD dwRoleNameCrc = (DWORD)m_pUtil->Crc32((LPCTSTR)vectParam[0]);
	Role *pRole = pMap->FindRole(g_roleMgr.GetRoleIDByNameCrc(dwRoleNameCrc));
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Vector3& vectGMPos = pRole->GetCurPos();

	if(pGM->GotoNewMap(pGM->GetMapID(), vectGMPos.x, GET_HEIGHT_MAX_Y, vectGMPos.z))
	{
		return E_Success;
	}

	return GT_INVALID;
}

//-----------------------------------------------------------------------------
// 把角色拉到GM身边(moverole szRoleName)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleMoveRole(const std::vector<DWORD>& vectParam, Role* pGM)
{
	Map *pMap = pGM->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	tstring strRoleName;
	strRoleName = (LPCTSTR)vectParam[0];
	DWORD dwRoleNameCrc = (DWORD)m_pUtil->Crc32(strRoleName.c_str());
	//DWORD dwRoleNameCrc = (DWORD)m_pUtil->Crc32((LPCTSTR)vectParam[0]);
	Role *pRole = pMap->FindRole(g_roleMgr.GetRoleIDByNameCrc(dwRoleNameCrc));
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Vector3& vectGMPos = pGM->GetCurPos();

	if(pRole->GotoNewMap(pGM->GetMapID(), vectGMPos.x, GET_HEIGHT_MAX_Y, vectGMPos.z))
	{
		return E_Success;
	}

	return GT_INVALID;
}

//-----------------------------------------------------------------------------
// 充经验(fillexp nExp)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleFillExp(const std::vector<DWORD>& vectParam, Role* pGM)
{
	pGM->ExpChange(_ttoi((LPCTSTR)vectParam[0]), FALSE, TRUE);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 升级(fill nLevel)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleFillLevel(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nLevel = _ttoi((LPCTSTR)vectParam[0]);
	
	// 判断指定等级是否有效，不能指定0级
	if (GT_VALID(nLevel) && nLevel != 0)
		pGM->LevelChange(_ttoi((LPCTSTR)vectParam[0]), FALSE, TRUE);

	// 如果角色处于非死亡状态，返回
	if( !pGM->IsInState(ES_Dead) )
		return E_Success;
	else  // 如果角色处于死亡状态，原地复活
	{
		pGM->Revive( ERRT_Locus, 0, FALSE);
		
		tagNS_RoleRevive send;
		send.dwRoleID		= pGM->GetID();
		send.dwErrorCode	= 0;

		if( P_VALID(pGM->GetMap()) )
		{
			pGM->GetMap()->SendBigVisTileMsg(pGM, &send, send.dwSize);
		}
	
		// 复活到指定地点
		pGM->GotoNewMap(pGM->m_Revive.dwMapID, pGM->m_Revive.fX, pGM->m_Revive.fY, pGM->m_Revive.fZ);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 增加装备潜力值(equippot n16Index nValue) n16Index背包位置索引(从0开始）  nValue潜力值增加量
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleEquipPotInc(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT16	n16Index = (INT16)_ttoi((LPCTSTR)vectParam[0]);
	INT		nValue = (INT)_ttoi((LPCTSTR)vectParam[1]);

	tagEquip *pEquip = 	(tagEquip*)pGM->GetItemMgr().GetBagItem(n16Index);

	if(!P_VALID(pEquip) || !MIsEquipment(pEquip->dwTypeID))
		return GT_INVALID;

	pEquip->ChangePotVal(nValue);
	pGM->GetItemMgr().UpdateEquipSpec(*pEquip);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 踢人
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleKickRole(const std::vector<DWORD>& vectParam, Role* pGM)
{
	tstring strRoleName;
	strRoleName = (LPCTSTR)vectParam[0];
	DWORD dwRoleNameCrc = (DWORD)m_pUtil->Crc32(strRoleName.c_str());

	Role *pRole = g_roleMgr.GetRolePtrByID(g_roleMgr.GetRoleIDByNameCrc(dwRoleNameCrc));
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	PlayerSession *pSession = pRole->GetSession();
	if(P_VALID(pSession))
	{
		g_worldSession.Kick(pSession->GetInternalIndex());
		pSession->SetKicked();
	}

	return E_Success;
}


//-----------------------------------------------------------------------------
// 调整可登陆玩家人数
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleResizeOnlineNum(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT	nValue = (INT)_ttoi((LPCTSTR)vectParam[0]);

	if(nValue < 100)
	{
		return GT_INVALID;
	}

	g_worldSession.SetPlayerNumLimit(nValue);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 重新加载脚本
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleReloadScripts(const std::vector<DWORD>& vectParam, Role* pGM)
{
	g_ScriptMgr.AddEvent(GT_INVALID, EVT_Script_Reload, 0, NULL);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 修改脚本使用的RoleData的值
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleChangeScriptData(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nIndex 	= _ttoi((LPCTSTR)vectParam[0]);
	DWORD dwValue= (DWORD)_ttoi((LPCTSTR)vectParam[1]);

	if (nIndex < ESD_Role && nIndex >= 0)
	{
		pGM->SetScriptData(nIndex, dwValue);
		return E_Success;
	}

	return E_SystemError;
}
//-----------------------------------------------------------------------------
// 开双倍
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleDouble( const std::vector<DWORD>& vectParam, Role* pGM )
{
	if (vectParam.size() < 4)
		return GT_INVALID;

	INT32 nPara1	= (INT32)_ttoi((LPCTSTR)vectParam[0]);
	INT32 nPara2	= (INT32)_ttoi((LPCTSTR)vectParam[1]);
	INT32 nPara3	= (INT32)_ttoi((LPCTSTR)vectParam[2]);
	INT32 nPara4	= (INT32)_ttoi((LPCTSTR)vectParam[3]);

	g_GMPolicy.SetRate((EDoubleType)nPara1, (DWORD)nPara2, (DWORD)nPara3, (DWORD)nPara4);

	return E_Success;
}

// 测试氏族数据用
DWORD GMCommandMgr::HandleClanData(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 3)
		return GT_INVALID;

	INT32 nPara1	= (INT32)_ttoi((LPCTSTR)vectParam[0]);
	INT32 nPara2	= (INT32)_ttoi((LPCTSTR)vectParam[1]);
	INT32 nPara3	= (INT32)_ttoi((LPCTSTR)vectParam[2]);

	if (!P_VALID(pGM))
		return GT_INVALID;

	return pGM->GetClanData().DynamicTest(nPara1, (ECLanType)nPara2, nPara3);
}

// 测试氏族数据用
DWORD GMCommandMgr::HandleVipNetBar(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 3)
		return GT_INVALID;

	DWORD dwPara0	= (DWORD)_ttoi((LPCTSTR)vectParam[0]);
	DWORD dwPara1	= (DWORD)_ttoi((LPCTSTR)vectParam[1]);
	LPCTSTR szPara2	= (LPCTSTR)vectParam[2];

	if (!P_VALID(pGM))
		return GT_INVALID;

	return g_VipNetBarMgr.DynamicTest(dwPara0, dwPara1, szPara2);
}

// 测试宠物
DWORD GMCommandMgr::HandlePet(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 3)
		return GT_INVALID;

	INT32 nPara1	= (INT32)_ttoi((LPCTSTR)vectParam[0]);
	INT32 nPara2	= (INT32)_ttoi((LPCTSTR)vectParam[1]);
	INT32 nPara3	= (INT32)_ttoi((LPCTSTR)vectParam[2]);

	if (!P_VALID(pGM))
		return GT_INVALID;

	return pGM->GetPetPocket()->DynamicTest(nPara1, (ECLanType)nPara2, nPara3);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleAddSkill( const std::vector<DWORD>& vectParam, Role* pGM )
{
	//9001201
	if (vectParam.size() < 1 || !P_VALID(pGM))
		return GT_INVALID;
	INT32 dwTypeID = (INT32)_ttoi((LPCTSTR)vectParam[0]);

	if (!P_VALID(g_attRes.GetSkillProto(dwTypeID)))
	{
		return GT_INVALID;
	}

	Skill* pSkill = new Skill(Skill::GetIDFromTypeID(dwTypeID), 0, Skill::GetLevelFromTypeID(dwTypeID), 0, 0);
	pGM->AddSkill(pSkill);

	return E_Success;
}

// 帮派测试
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGetFund(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 1)
	{
		return GT_INVALID;
	}

	INT32 nFund		= (INT32)_ttoi((LPCTSTR)vectParam[0]);

	if (!P_VALID(pGM))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pGM->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	pGuild->IncGuildFund(pGM->GetID(), nFund, ELCID_GM_GetFund);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGetMaterial(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 1)
	{
		return GT_INVALID;
	}

	INT32 nMaterial	= (INT32)_ttoi((LPCTSTR)vectParam[0]);

	if (!P_VALID(pGM))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pGM->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	pGuild->IncGuildMaterial(pGM->GetID(), nMaterial, ELCID_GM_GetMaterial);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGetContribute(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 1)
	{
		return GT_INVALID;
	}

	INT32 nContribute	= (INT32)_ttoi((LPCTSTR)vectParam[0]);

	if (!P_VALID(pGM))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pGM->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	pGuild->ChangeMemberContribution(pGM->GetID(), nContribute, TRUE);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleResetAffairTimes(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (!P_VALID(pGM))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pGM->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	pGuild->GetGuildAffair().ResetAffairCounter();

	return E_Success;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGetTael(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 1)
	{
		return GT_INVALID;
	}

	INT32 nTael	= (INT32)_ttoi((LPCTSTR)vectParam[0]);

	if (!P_VALID(pGM))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pGM->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}
	GuildCommodity* pCommodity = pGuild->GetGuildCommerce().GetCommodity(pGM->GetID());
	if (!P_VALID(pCommodity))
	{
		return GT_INVALID;
	}

	pCommodity->IncTael(nTael);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGuildStatus(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nType = _ttoi((LPCTSTR)vectParam[0]);
	LONG lValue = (LONG)_ttoi((LPCTSTR)vectParam[1]); // 调整的值，不是设置的值

	Guild* pGuild = g_guildMgr.GetGuild(pGM->GetGuildID());
	if (!P_VALID(pGuild))
		return GT_INVALID;

	// 安定度
	else if (nType == 0)
	{
		if (lValue > 0)
		{
			pGuild->IncGuildPeace(pGM->GetID(), lValue, 0);
		}
		else
		{
			pGuild->DecGuildPeace(pGM->GetID(), -lValue, 0);
		}
	}
	// 声望值
	else if (nType == 1)
	{
		if (lValue > 0)
		{
			pGuild->IncGuildReputation(pGM->GetID(), lValue, 0);
		}
		else
		{
			pGuild->DecGuildReputation(pGM->GetID(), -lValue, 0);
		}
	}
	else
		return E_SystemError;

	return E_Success;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleGuildFacility(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nType = _ttoi((LPCTSTR)vectParam[0]);
	BYTE byValue = (BYTE)_ttoi((LPCTSTR)vectParam[1]);

	Guild* pGuild = g_guildMgr.GetGuild(pGM->GetGuildID());
	if (!P_VALID(pGuild))
		return GT_INVALID;

	pGuild->ChangeFacilityLevel((EFacilitiesType)(nType), byValue);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleRoleGuild(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nType = _ttoi((LPCTSTR)vectParam[0]);
	INT32 n32Value = (INT32)_ttoi((LPCTSTR)vectParam[1]);//调整的值，不是设置的值
	
	// 阅历
	if (nType == 0)
	{
		INT32 n32Temp = pGM->GetAttValue(ERA_Knowledge) + n32Value; 
		if (n32Temp > MAX_GUILDMEM_EXP)
			n32Temp = MAX_GUILDMEM_EXP;
		if (n32Temp < 0)
			n32Temp = 0;
		
		pGM->SetAttValue(ERA_Knowledge, n32Temp);
	} 
	// 功勋
	else if (nType == 1)
	{
		Guild* pGuild = g_guildMgr.GetGuild(pGM->GetGuildID());
		if (!P_VALID(pGuild))
			return GT_INVALID;

		pGuild->ChangeMemberExploit(pGM->GetID(), n32Value);
	}
	else
		return E_SystemError;

	return E_Success;
}

// 帮派团购测试
DWORD GMCommandMgr::HandleLaunchGP(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 4)
	{
		return GT_INVALID;
	}

	DWORD nPara1	= (DWORD)_ttoi((LPCTSTR)vectParam[0]);
	INT nPara2		= (INT)_ttoi((LPCTSTR)vectParam[1]);
	BYTE nPara3		= (BYTE)_ttoi((LPCTSTR)vectParam[2]);
	BYTE nPara4		= (BYTE)_ttoi((LPCTSTR)vectParam[3]);


	if (!P_VALID(pGM))
	{
		return GT_INVALID;
	}

	return g_mall.LaunchGroupPurchase(pGM, nPara1, nPara4, nPara2, nPara3);
}

DWORD GMCommandMgr::HandleRespondGP(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if (vectParam.size() < 3)
	{
		return GT_INVALID;
	}

	DWORD nPara1	= (DWORD)_ttoi((LPCTSTR)vectParam[0]);
	DWORD nPara2	= (DWORD)_ttoi((LPCTSTR)vectParam[1]);
	INT nPara3		= (INT)_ttoi((LPCTSTR)vectParam[2]);

	if (!P_VALID(pGM))
	{
		return GT_INVALID;
	}

	return g_mall.RespondGroupPurchase(pGM, pGM->GetGuildID(), nPara1, nPara2, nPara3);
}

// 测试名帖
DWORD GMCommandMgr::HandleVCard( const std::vector<DWORD>& vectParam, Role* pGM )
{
	if (vectParam.size() < 1)
		return GT_INVALID;

	DWORD dwRoleID	= (DWORD)_ttoi((LPCTSTR)vectParam[0]);

	if(!P_VALID(g_roleMgr.GetRoleInfo(dwRoleID)))
	{
		return GT_INVALID;
	}

	tagNDBC_GetOffLineVCard send;

	send.dwRoleID = dwRoleID;
	send.dwQueryID = pGM->GetID();
	
	g_dbSession.Send(&send, send.dwSize);

	return TRUE;
}

DWORD GMCommandMgr::HandleTitle( const std::vector<DWORD>& vectParam, Role* pGM )
{
	if (vectParam.size() < 3)
		return GT_INVALID;

	EnumTitleEvent	eEvent	= (EnumTitleEvent)_ttoi((LPCTSTR)vectParam[0]);
	DWORD			dwPara1	= (DWORD)_ttoi((LPCTSTR)vectParam[1]);
	DWORD			dwPara2	= (DWORD)_ttoi((LPCTSTR)vectParam[2]);

	pGM->GetTitleMgr()->SigEvent(eEvent, dwPara1, dwPara2);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 添加新的任务
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleAddQuest(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT	nQuestID = _ttoi((LPCTSTR)vectParam[0]);

	const tagQuestProto* pProto = g_questMgr.GetQuestProto(nQuestID);
	if( !P_VALID(pProto) )	return E_QUEST_NOT_EXIST;

	INT nIndex;
	for(nIndex = 0; nIndex < QUEST_MAX_COUNT; ++nIndex)
	{
		if( FALSE == pGM->QuestValid(nIndex) )
		{
			break;
		}
	}

	if( nIndex >= QUEST_MAX_COUNT )
	{
		return E_CanTakeQuest_FAILED_QUEST_NUM_FULL;
	}
	
	pGM->AddQuest(pProto, nIndex);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 清除指定天资类型技能冷却(skillcd talenttype exceptsillid)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleClearSkillCD(const std::vector<DWORD>& vectParam, Role* pGM)
{
	ETalentType	eTalentType		= (ETalentType)_ttoi((LPCTSTR)vectParam[0]);
	DWORD		dwExceptSkillID	= (DWORD)_ttoi((LPCTSTR)vectParam[1]);

	if(eTalentType >= ETT_End || eTalentType < ETT_Null)
	{
		eTalentType = ETT_Null;
	}

	pGM->ClearAllSkillCoodDown(eTalentType, dwExceptSkillID);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 物品冷却开关
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleCoolOff(const std::vector<DWORD>& vectParam, Role* pGM)
{
	pGM->SetObjectCoolMode(TRUE);
	return E_Success;
}

DWORD GMCommandMgr::HandleCoolOn(const std::vector<DWORD>& vectParam, Role* pGM)
{
	pGM->SetObjectCoolMode(FALSE);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 修改角色属性(att eroleatt nval)
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleChangeRoleAtt(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT	nIndex	= _ttoi((LPCTSTR)vectParam[0]);
	INT	nVal	= _ttoi((LPCTSTR)vectParam[1]);

	if(nIndex <= ERA_Null || nIndex >= ERA_End)
	{
		return GT_INVALID;
	}
	
	if ((nIndex == ERA_HP)||(nIndex == ERA_MP)||(nIndex == ERA_Vitality)||(nIndex == ERA_Endurance)
		||(nIndex == ERA_Knowledge)||(nIndex == ERA_Injury)||(nIndex == ERA_Morale)||(nIndex == ERA_Morality)
		||(nIndex == ERA_Culture)||(nIndex == ERA_AttPoint)||(nIndex == ERA_TalentPoint))
	{
		pGM->SetAttValue(nIndex, nVal);
	}
	else
	{
		pGM->SetBaseAttValue(nIndex, nVal);
		pGM->RecalAtt();
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 修改角色声望值
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleSetReputation(const std::vector<DWORD>& vectParam, Role* pGM)
{
	ECLanType eClan	= (ECLanType)_ttoi((LPCTSTR)vectParam[0]);
	INT	nReputation	= _ttoi((LPCTSTR)vectParam[1]);

	if( eClan <= ECLT_BEGIN || eClan >= ECLT_END )
	{
		return GT_INVALID;
	}

	pGM->GetClanData().RepSetVal(eClan, nReputation, TRUE);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 在玩家当前坐标刷出怪物　
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleCreateMonster( const std::vector<DWORD>& vectParam, Role* pGM)
{
	DWORD dwTypeID = _ttoi((LPCTSTR)vectParam[0]);
	BOOL  bCollide = FALSE;
	
	if(vectParam.size() == 2)
		bCollide = _ttoi((LPCTSTR)vectParam[1]);

	Map* pMap = pGM->GetMap();
	if(!P_VALID(pMap))
		return GT_INVALID;

	Vector3 vPos = pGM->GetCurPos();
	Vector3 vFace = pGM->GetFaceTo();

	pMap->CreateCreature(dwTypeID, vPos, vFace, GT_INVALID, bCollide);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 不计算技能的触发率
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleTriggerOff(const std::vector<DWORD>& vectParam, Role* pGM)
{
	pGM->SetTriggerMode(TRUE);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 使用原有的触发率计算
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleTriggerOn(const std::vector<DWORD>& vectParam, Role* pGM)
{
	pGM->SetTriggerMode(FALSE);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 清空该角色所有已完成任务
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleClearRoleQuest(const std::vector<DWORD>& vectParam, Role* pGM)
{
	pGM->ClearCompleteQuest();
	return E_Success;
}

//-----------------------------------------------------------------------------
// 指定某任务物的完成状态
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleRoleQuest(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT	nQuestID = _ttoi((LPCTSTR)vectParam[0]);
	BOOL bDone = (BOOL)_ttoi((LPCTSTR)vectParam[1]);

	pGM->AddDelCompleteQuest(nQuestID, bDone);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 装备的镌刻，使用该命令一次，则镌刻该装备成功一次 gm engrave n64ItemID dwFormulaID 
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleEngrave(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT16 n16ItemIndex = (INT16)_ttoi((LPCTSTR)vectParam[0]);
	DWORD dwFormulaID = (DWORD)_ttoi((LPCTSTR)vectParam[1]);

	pGM->GMEngraveEquip(dwFormulaID, n16ItemIndex);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 铭文，使用该命令一次，则铭文该装备成功一次
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandlePosy(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT16 n16ItemIndex = (INT16)_ttoi((LPCTSTR)vectParam[0]);
	DWORD dwFormulaID = (DWORD)_ttoi((LPCTSTR)vectParam[1]);
	
	pGM->GMPosyEquip(dwFormulaID, n16ItemIndex);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 龙附，使用该命令一次，则该装备完成龙附成功一次
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleLoongSoul(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT16 n16ItemIndex = (INT16)_ttoi((LPCTSTR)vectParam[0]);
	INT64 n64LoongSoulID = (INT64)_ttoi((LPCTSTR)vectParam[1]);

	pGM->GMLoongSoulEquip(n64LoongSoulID, n16ItemIndex);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 为装备增加龙魂属性或特殊属性
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleAddAtt2Weapon(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT16 n16ItemIndex = (INT16)_ttoi((LPCTSTR)vectParam[0]);
	INT nAttType = (INT)_ttoi((LPCTSTR)vectParam[1]); // 0:龙魂属性表 1:龙魂属性里 2:特殊属性
	DWORD dwAttID = (DWORD)_ttoi((LPCTSTR)vectParam[2]);

	// 找到被强化装备
	tagEquip *pEquip = (tagEquip*)pGM->GetItemMgr().GetBagItem(n16ItemIndex);
	if(!P_VALID(pEquip))
		return E_Consolidate_Equip_Not_Exist;

	if(!MIsEquipment(pEquip->dwTypeID))
		return E_Consolidate_NotEquipment;
	// 判断要添加的属性是龙魂属性还是特殊属性
	if (nAttType == 2)
		pEquip->equipSpec.bySpecAtt = dwAttID;
	else if(nAttType == 1)
		pEquip->equipSpec.dwLongHunInnerID = dwAttID;
	else
		pEquip->equipSpec.dwLongHunOuterID = dwAttID;

	ItemCreator::ProcEquipAttBySpecAtt(pEquip);
	pGM->GetItemMgr().UpdateEquipSpec(*pEquip);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 调整生产技能熟练度
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleProficiency(const std::vector<DWORD>& vectParam, Role* pGM)
{
	DWORD dwSkillID = (DWORD)_ttoi((LPCTSTR)vectParam[0]);
	INT nValue = _ttoi((LPCTSTR)vectParam[1]);
	
	Skill* pSkill = pGM->GetSkill(dwSkillID);
	
	if(P_VALID(pSkill))
	{
		// 判断是否是角色的生产技能

		if ((pSkill->GetType() == ESST_Role) &&(pSkill->GetTypeEx() == ESSTE_Produce))
			pGM->ChangeSkillExp(pSkill, nValue);
		else
			return E_SystemError;

		return E_Success;
	}
	else
		return E_SystemError;
}

//-----------------------------------------------------------------------------
// 添加buff
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleAddBuff(const std::vector<DWORD>& vectParam, Role* pGM)
{
	DWORD dwBuffID = (DWORD)_ttoi((LPCTSTR)vectParam[0]);

	const tagBuffProto* pProto = g_attRes.GetBuffProto(dwBuffID);
	if( !P_VALID(pProto) ) return E_SystemError;

	if(pGM->GMTryAddBuff(pGM, pProto, NULL, NULL, NULL))
		return E_Success;
	else
		return E_SystemError;
}

//-----------------------------------------------------------------------------
// 添加Suit
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleAddSuit(const std::vector<DWORD>& vectParam, Role* pGM)
{
	DWORD dwSuitID = (DWORD)_ttoi((LPCTSTR)vectParam[0]);
	INT nQlty = _ttoi((LPCTSTR)vectParam[1]);

	if( TRUE == pGM->AddSuit(dwSuitID, nQlty))
		return E_Success;
	else
		return E_SystemError;
}

//-----------------------------------------------------------------------------
// 添加指定等级区间（包括低5级的）可用的武器或其他装备
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleAddEquip(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nType = _ttoi((LPCTSTR)vectParam[0]);
	INT nLevel = _ttoi((LPCTSTR)vectParam[1]);
	INT nQlty = _ttoi((LPCTSTR)vectParam[2]);

	if ( TRUE == pGM->AddEquip(nType, nLevel, nQlty) )
		return E_Success;
	else
		return E_SystemError;
}

//-----------------------------------------------------------------------------
// 设定角色的职业 nType 0：专精职业 1：英雄职业
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleSetVocation(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nType = _ttoi((LPCTSTR)vectParam[0]);
	INT nVocation = _ttoi((LPCTSTR)vectParam[1]);

	if(nType == 0)
	{
		if(!(nVocation > EV_Null && nVocation < EV_End)) return E_SystemError;
		pGM->SetClass((EClassType)nVocation);
	}
	else if(nType == 1)
	{
		if(!(nVocation > EHV_Begin && nVocation < EHV_End)) return E_SystemError;
		pGM->SetClassEx((EClassTypeEx)nVocation);
	}
	else
		return E_SystemError;

	return E_Success;
}

//-----------------------------------------------------------------------------
// 设置角色开启宝箱数
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleChangeRoleChestSum(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nSum = _ttoi((LPCTSTR)vectParam[0]);

	pGM->SetTreasureSum(nSum);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 设置服务器开启宝箱数
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleChangeServerChestSum(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nSum = _ttoi((LPCTSTR)vectParam[0]);

	g_worldSession.SetTreasureSum(nSum);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 禁止、解禁玩家一切聊天频道
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleNoSpeak(const std::vector<DWORD>& vectParam, Role* pGM)
{
	DWORD dwRoleID = (DWORD)_ttoi((LPCTSTR)vectParam[0]);
	
	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	if(!P_VALID(pRole)) return E_SystemError;

	pRole->SetSpeakOff(TRUE);

	return E_Success;
}

DWORD GMCommandMgr::HandleCancelNoSpeak(const std::vector<DWORD>& vectParam, Role* pGM)
{
	DWORD dwRoleID = (DWORD)_ttoi((LPCTSTR)vectParam[0]);

	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	if(!P_VALID(pRole)) return E_SystemError;

	pRole->SetSpeakOff(FALSE);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 进入、退出隐身状态
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleLurk(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nLevel = _ttoi((LPCTSTR)vectParam[0]);

	if( !pGM->IsInState(ES_Lurk) )
	{
		pGM->Lurk(nLevel);
	}
	else
	{
		pGM->UnLurk();
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 进入、退出无敌状态
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleInvincible(const std::vector<DWORD>& vectParam, Role* pGM)
{
	if( !pGM->IsInState(ES_Invincible) )
	{
		pGM->SetState(ES_Invincible);
	}
	else
	{
		pGM->UnsetState(ES_Invincible);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获得指定玩家的位置（地图，坐标）
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleRolePosition(const std::vector<DWORD>& vectParam, Role* pGM)
{
	return E_Success;
}

//-----------------------------------------------------------------------------
// 杀死指定怪物
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleKillMonster(const std::vector<DWORD>& vectParam, Role* pGM)
{
	DWORD dwMonsterID = (DWORD)_ttoi((LPCTSTR)vectParam[0]);

	Unit* pTarget = pGM->GetMap()->FindUnit(dwMonsterID);

	if( !P_VALID(pTarget)) return E_SystemError;

	if(pTarget->IsCreature())
	{
		Creature* pMonster = (Creature*)pTarget;
		pMonster->OnDead(pGM);
		
		return E_Success;
	}
	else
		return E_SystemError;
}

//-----------------------------------------------------------------------------
// 摊位获得经验
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleStallExp(const std::vector<DWORD>& vectParam, Role* pGM)
{
	INT nExp = _ttoi((LPCTSTR)vectParam[0]);

	return pGM->GainStallExp(nExp);
}

//-----------------------------------------------------------------------------
// 开关门
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleOpenCloseDoor( const std::vector<DWORD>& vectParam, Role* pGM )
{
	if (vectParam.size() < 2)
	{
		return GT_INVALID;
	}

	DWORD nPara1	= (DWORD)_ttoi((LPCTSTR)vectParam[0]);
	BOOL nPara2		= (INT)_ttoi((LPCTSTR)vectParam[1]);


	if (!P_VALID(pGM))
	{
		return GT_INVALID;
	}

	Map* pMap = pGM->GetMap();
	if (!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	if (GT_VALID(nPara1))
	{
		pMap->OpenCloseDoor(nPara1, nPara2);
	}
	else
	{
		pMap->OpenCloseAllDoors(nPara2);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 选人
//-----------------------------------------------------------------------------
DWORD GMCommandMgr::HandleSelectRole( const std::vector<DWORD>& vectParam, Role* pGM )
{
	tagNDBC_LoadRole send;
	send.dwAccountID = pGM->GetSession()->GetSessionID();
	send.dwRoleID = pGM->GetID();

	g_dbSession.Send(&send, send.dwSize);

	pGM->GetSession()->SetRoleLoading();

	return E_Success;
}
