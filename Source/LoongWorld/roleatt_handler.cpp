//-----------------------------------------------------------------------------
//!\file roleatt_handler.cpp
//!\author Aslan
//!
//!\date 2008-07-28
//! last 2008-07-28
//!
//!\brief 移动消息处理类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/RoleDefine.h"
#include "../WorldDefine/role_att.h"
#include "../WorldDefine/role_info.h"
#include "../WorldDefine/selectrole.h"
#include "../WorldDefine/filter.h"
#include "../ServerDefine/role_data.h"


#include "pet_pocket.h"
#include "pet_soul.h"
#include "player_session.h"
#include "role.h"
#include "creature.h"
#include "role_mgr.h"
#include "map.h"
#include "pet.h"
#include "guild.h"
#include "guild_mgr.h"
#include "title_mgr.h"

//------------------------------------------------------------------------------
// 获取人物属性
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleGetRoleInitAtt(tagNetCmd* pCmd)
{
	tagNC_GetRoleInitState* pRecv = (tagNC_GetRoleInitState*)pCmd;

	if( pRecv->eType <= ERIT_Null || pRecv->eType >= ERIT_End )
		return GT_INVALID;

	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return GT_INVALID;


	switch(pRecv->eType)
	{
	case ERIT_Att:
		pRole->SendInitStateAtt();
		break;
	case ERIT_Skill:
		pRole->SendInitStateSkill();
		break;
	case ERIT_Item:
		pRole->SendInitStateItem();
		pRole->SendInitStateSuit();
		pRole->SendInitStateLongHun();
		break;
	case ERIT_CompleteQuest:
		pRole->SendInitStateCompleteQuest();
		break;
	case ERIT_IncompleteQuest:
		pRole->SendInitStateIncompleteQuest();
		break;
	case ERIT_Money:
		pRole->SendInitStateMoney();
		break;
	case ERIT_Reputation:
		pRole->SendInitStateReputation();
		break;
	case ERIT_FrindAndEnemy:
		pRole->SendFriendAndEnemy();
		break;
	case ERIT_Guild:
		pRole->SendInitStateGuild();
		break;
	default:
		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 获取远程人物或生物属性（是不是要做些处理，如果玩家一直在发怎么办）
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetRemoteUnitAtt(tagNetCmd* pCmd)
{
	tagNC_GetRemoteRoleState* pRecv = (tagNC_GetRemoteRoleState*)pCmd;

	Role* pSelf = GetRole();
	if( !P_VALID(pSelf) || GT_INVALID == pSelf->GetVisTileIndex() ) return GT_INVALID;

	Map* pMap = pSelf->GetMap();
	if( !P_VALID(pMap) ) return GT_INVALID;

	if( pRecv->nRoleNum > 50 ) return GT_INVALID;	// 最多发50个

	// 顺序的轮询地图中的ID
	for(INT n = 0; n < pRecv->nRoleNum; n++)
	{
		if( !P_VALID(pRecv->dwRoleID[n]) )
			break;

		// 检查是人物还是生物
		if( IS_PLAYER(pRecv->dwRoleID[n]) )
		{
			Role* pRemoteRole = pMap->FindRole(pRecv->dwRoleID[n]);
			if( P_VALID(pRemoteRole) )
			{
				INT nBuffNum = pRemoteRole->GetBuffNum();
				DWORD dwSize = sizeof(tagNS_GetRemoteRoleState) + ((nBuffNum > 0) ? (nBuffNum - 1)*sizeof(tagBuffMsgInfo) : 0);

				// 发送远程人物属性同步消息
				MCREATE_MSG(pSend, dwSize, NS_GetRemoteRoleState);

				pSend->RoleData.dwID = pRemoteRole->GetID();
				pSend->RoleData.nLevel = pRemoteRole->GetLevel();

				pSend->RoleData.byStallLevel = pRemoteRole->GetStallModeLevel();
				pSend->RoleData.eClassType = pRemoteRole->GetClass();
				pSend->RoleData.eClassTypeEx = pRemoteRole->GetClassEx();

				pSend->RoleData.dwLoverID = pRemoteRole->GetLoverID();
				pSend->RoleData.bHaveWedding = pRemoteRole->IsRoleWedded();

				// 位置
				Vector3 vPos = pRemoteRole->GetCurPos();
				pSend->RoleData.fPos[0] = vPos.x;
				pSend->RoleData.fPos[1] = vPos.y;
				pSend->RoleData.fPos[2] = vPos.z;

				// 朝向
				Vector3 vFace = pRemoteRole->GetFaceTo();
				pSend->RoleData.fFaceTo[0] = vFace.x;
				pSend->RoleData.fFaceTo[1] = vFace.y;
				pSend->RoleData.fFaceTo[2] = vFace.z;

				// 状态
				pSend->RoleData.dwState = pRemoteRole->GetState();
				pSend->RoleData.dwRoleState = pRemoteRole->GetRoleState();
				pSend->RoleData.ePKState = pRemoteRole->GetPKState();

				// 帮派
				pSend->RoleData.dwGuildID = pRemoteRole->GetGuildID();
				pSend->RoleData.n8GuildPos = EGMP_Null;
				if(pRemoteRole->IsInGuild())
				{
					Guild *pGuild = g_guildMgr.GetGuild(pRemoteRole->GetGuildID());
					if(P_VALID(pGuild))
					{
						tagGuildMember *pMember = pGuild->GetMember(pRemoteRole->GetID());
						if(P_VALID(pMember))
						{
							pSend->RoleData.n8GuildPos = pMember->eGuildPos;
						}
					}
				}

				// 骑乘宠物属性
				PetSoul* pSoul = pRemoteRole->GetPetPocket()->GetMountPetSoul();
				if (P_VALID(pSoul))
				{
					pSend->RoleData.dwMountPetID = pSoul->GetID();
					pSend->RoleData.dwMountPetTypeID = pSoul->GetProtoID();
				}
				else
				{
					pSend->RoleData.dwMountPetID = GT_INVALID;
					pSend->RoleData.dwMountPetTypeID = GT_INVALID;
				}

				if (pRemoteRole->GetPetPocket()->IsRideAfter())
				{
					pSend->RoleData.dwMountPetID = pRemoteRole->GetPetPocket()->GetHostPetID();
				}

				// 属性
				pSend->RoleData.nAtt[ERRA_MaxHP]		= pRemoteRole->GetAttValue(ERA_MaxHP);
				pSend->RoleData.nAtt[ERRA_HP]			= pRemoteRole->GetAttValue(ERA_HP);
				pSend->RoleData.nAtt[ERRA_MaxMP]		= pRemoteRole->GetAttValue(ERA_MaxMP);
				pSend->RoleData.nAtt[ERRA_MP]			= pRemoteRole->GetAttValue(ERA_MP);
				pSend->RoleData.nAtt[ERRA_Rage]			= pRemoteRole->GetAttValue(ERA_Rage);
				pSend->RoleData.nAtt[ERRA_Speed_XZ]		= pRemoteRole->GetAttValue(ERA_Speed_XZ);
				pSend->RoleData.nAtt[ERRA_Speed_Y]		= pRemoteRole->GetAttValue(ERA_Speed_Y);
				pSend->RoleData.nAtt[ERRA_Speed_Swim]	= pRemoteRole->GetAttValue(ERA_Speed_Swim);
				pSend->RoleData.nAtt[ERRA_Speed_Mount]	= pRemoteRole->GetAttValue(ERA_Speed_Mount);
				pSend->RoleData.nAtt[ERRA_Shape]		= pRemoteRole->GetAttValue(ERA_Shape);
				pSend->RoleData.nAtt[ERRA_Hostility]	= pRemoteRole->GetAttValue(ERA_Hostility);
				

				// 对远端玩家公开信息设置
				pSend->RoleData.sRemoteOpenSet			= pRemoteRole->GetRemoteOpenSet();

				// 当前称号
				pSend->RoleData.u16CurActTitleID		= pRemoteRole->GetTitleMgr()->GetActiviteTitle();

				// 外观
				pSend->RoleData.sDisplaySet				= pRemoteRole->GetDisplaySet();
				IFASTCODE->MemCpy(&pSend->RoleData.Avatar, pRemoteRole->GetAvatar(), sizeof(tagAvatarAtt));
				IFASTCODE->MemCpy(&pSend->RoleData.AvatarEquip, &pRemoteRole->GetAvatarEquip(), sizeof(tagAvatarEquip));

				// 状态列表
				pSend->RoleData.nBuffNum = nBuffNum;
				if( nBuffNum > 0 )
				{
					pRemoteRole->GetAllBuffMsgInfo(pSend->RoleData.Buff, nBuffNum);
				}

				SendMessage(pSend, pSend->dwSize);

				MDEL_MSG(pSend);
			}
		}

		else if( IS_CREATURE(pRecv->dwRoleID[n]))
		{
			Creature* pCreature = pMap->FindCreature(pRecv->dwRoleID[n]);
			if( P_VALID(pCreature) )
			{
				INT nBuffNum = pCreature->GetBuffNum();
				DWORD dwSize = sizeof(tagNS_GetRemoteCreatureState) + ((nBuffNum > 0) ? (nBuffNum - 1)*sizeof(tagBuffMsgInfo) : 0);

				// 发送远程生物属性同步消息
				MCREATE_MSG(pSend, dwSize, NS_GetRemoteCreatureState);
				
				pSend->CreatureData.dwID = pCreature->GetID();
				pSend->CreatureData.dwTypeID = pCreature->GetTypeID();
				pSend->CreatureData.nLevel = pCreature->GetLevel();
				
				pSend->CreatureData.bOpen = pCreature->GetDoorStatus();
				pSend->CreatureData.dwMapObjID = pCreature->GetMapObjID();
			
				// 位置
				Vector3 vPos = pCreature->GetCurPos();
				pSend->CreatureData.fPos[0] = vPos.x;
				pSend->CreatureData.fPos[1] = vPos.y;
				pSend->CreatureData.fPos[2] = vPos.z;

				// 状态
				pSend->CreatureData.dwState = pCreature->GetState();

				// 所属
				pSend->CreatureData.dwTaggedOwner = pCreature->GetTaggedOwner();

				// 朝向
				Vector3 vFace = pCreature->GetFaceTo();
				pSend->CreatureData.fFaceTo[0] = vFace.x;
				pSend->CreatureData.fFaceTo[1] = vFace.y;
				pSend->CreatureData.fFaceTo[2] = vFace.z;

				// 属性
				pSend->CreatureData.nAtt[ERRA_MaxHP]		= pCreature->GetAttValue(ERA_MaxHP);
				pSend->CreatureData.nAtt[ERRA_HP]			= pCreature->GetAttValue(ERA_HP);
				pSend->CreatureData.nAtt[ERRA_MaxMP]		= pCreature->GetAttValue(ERA_MaxMP);
				pSend->CreatureData.nAtt[ERRA_MP]			= pCreature->GetAttValue(ERA_MP);
				pSend->CreatureData.nAtt[ERRA_Rage]			= pCreature->GetAttValue(ERA_Rage);
				pSend->CreatureData.nAtt[ERRA_Speed_XZ]		= pCreature->GetAttValue(ERA_Speed_XZ);
				pSend->CreatureData.nAtt[ERRA_Speed_Y]		= pCreature->GetAttValue(ERA_Speed_Y);
				pSend->CreatureData.nAtt[ERRA_Shape]		= pCreature->GetAttValue(ERA_Shape);

				// 状态列表
				pSend->CreatureData.nBuffNum = nBuffNum;
				if( nBuffNum > 0 )
				{
					pCreature->GetAllBuffMsgInfo(pSend->CreatureData.Buff, nBuffNum);
				}

				SendMessage(pSend, pSend->dwSize);

				MDEL_MSG(pSend);

				//-- 再次发送走，防止漂移
				BYTE	byMsg[1024] = {0};
				DWORD	dwSize1 = pMap->CalMovementMsgEx(pMap->FindUnit(pCreature->GetID()), byMsg, 1024);
				SendMessage(byMsg, dwSize1);
			}
		}
		else if (IS_PET(pRecv->dwRoleID[n]))
		{
			Pet* pPet = pMap->FindPet(pRecv->dwRoleID[n]);
			if (P_VALID(pPet))
			{
				tagNS_GetRemotePetState send;
				send.PetData.dwID		= pPet->GetID();
				send.PetData.dwProtoID	= pPet->GetTypeID();
				send.PetData.uState.byPetState		= pPet->GetPetState();
				Role* pMaster			= pPet->GetMaster();
				send.PetData.dwRoleID	= P_VALID(pMaster) ? pMaster->GetID() : GT_INVALID;

				SendMessage(&send, send.dwSize);
			}
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 处理玩家得到其他人的RoleID
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetID(tagNetCmd *pCmd)
{
	tagNC_RoleGetID* pRecv = (tagNC_RoleGetID*)pCmd;
	DWORD dwNameCrc = pRecv->dwRoleNameCrc;
	DWORD dwRoleID = g_roleMgr.GetRoleIDByNameCrc(dwNameCrc);

	tagNS_RoleGetID send;
	send.dwRoleNameCrc = dwNameCrc;
	send.dwRoleID = dwRoleID;
	if (GT_VALID(dwRoleID))
	{
		g_roleMgr.GetRoleNameByNameID(dwRoleID, send.szName);
	}

	SendMessage(&send, send.dwSize);
	return 0;
}

//-----------------------------------------------------------------------------
// 通过NameID得到玩家的名字
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetNameByNameID(tagNetCmd* pCmd)
{
	
	MGET_MSG(pRecv, pCmd, NC_GetNamebyNameID);
	
	tagNS_GetNamebyNameID send;
	send.bResult = TRUE;
	send.dwNameID = pRecv->dwNameID;
	g_roleMgr.GetRoleNameByNameID(pRecv->dwNameID, send.szName);

	SendMessage(&send, send.dwSize);

	return 0;
}

//------------------------------------------------------------------------------
// 客户端获取多个玩家名字
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetSomeName(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_RoleGetSomeName);

	INT		nNum = pRecv->nNum;

	if(nNum <= 0)
		return 0;

	if(nNum > 50)
		return 0;

	DWORD	dwSize = sizeof(tagNS_RoleGetSomeName) + (nNum - 1) * sizeof(tagRoleIDName);
	MCREATE_MSG(pSend, dwSize, NS_RoleGetSomeName);

	pSend->nUserData = pRecv->nUserData;
	pSend->nNum = nNum;

	for(INT n = 0; n < nNum; ++n)
	{
		pSend->IdName[n].dwID = pRecv->dwAllID[n];
		g_roleMgr.GetRoleNameByID(pRecv->dwAllID[n], pSend->IdName[n].szName);
	}

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return 0;
}

//------------------------------------------------------------------------------
// 投点
//------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleBidAttPoint(tagNetCmd* pCmd)
{
	tagNC_RoleBidAttPoint* pRecv = (tagNC_RoleBidAttPoint*)pCmd;
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	DWORD dwErrorCode = m_pRole->BidAttPoint(pRecv->nAttPointAdd);

	tagNS_RoleBidAttPoint send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------
// 洗点
//-------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleClearAttPoint(tagNetCmd* pCmd)
{
	tagNC_RoleClearAttPoint* pRecv = (tagNC_RoleClearAttPoint*)pCmd;
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	DWORD dwErrorCode = m_pRole->ClearAttPoint(pRecv->n64ItemID);

	tagNS_RoleClearAttPoint send;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------
// 角色外观显示模式设置
//-------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSetFashion(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_Fashion);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 获取地图
	Map *pMap = pRole->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}
	
	// 检查是否和当前显示模式相同
	if(pRole->GetDisplaySet().bFashionDisplay == p->bFashion)
	{
		return 0;
	}

	// 设置显示模式
	pRole->SetFashionMode(p->bFashion);

	// 向周围玩家发送新的外观信息
	tagNS_AvatarEquip send;
	send.dwRoleID		= pRole->GetID();
	send.bFashion		= p->bFashion;
	send.sAvatarEquip	= pRole->GetAvatarEquip();
	pMap->SendBigVisTileMsg(pRole, &send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------
// 角色外观显示部位设置
//-------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSetDisplay(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_RoleSetDisplay);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 获取地图
	Map *pMap = pRole->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	// 检查是否和当前部位设置属性相同
	const tagDisplaySet& sDisplaySet = pRole->GetDisplaySet();
	if(sDisplaySet.bHideBack == p->sDisplaySet.bHideBack
		&& sDisplaySet.bHideFace == p->sDisplaySet.bHideFace
		&& sDisplaySet.bHideHead == p->sDisplaySet.bHideHead)
	{
		return 0;
	}

	// 设置部位显示属性
	pRole->SetDisplaySet(p->sDisplaySet.bHideHead, p->sDisplaySet.bHideFace, p->sDisplaySet.bHideBack);

	// 向周围玩家发送消息
	tagNS_RoleSetDisplay send;
	send.dwRoleID		= pRole->GetID();
	send.sDisplaySet	= sDisplaySet;
	pMap->SendBigVisTileMsg(pRole, &send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------
// 角色改名(使用易名贴)
//-------------------------------------------------------------------------------
DWORD PlayerSession::HandleChangeRoleName(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_LPRename);

	DWORD dwErrorCode = E_Success;

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 检查道具的合法性
	tagItem *pItem = pRole->GetItemMgr().GetBagItem(pRecv->n64RenameItemID);
	if(!P_VALID(pItem))
	{
		dwErrorCode = E_UseItem_ItemNotExist;
		//return dwErrorCode;
	}

	if(pItem->pProtoType->eSpecFunc != EISF_RenameItem)
	{
		dwErrorCode = E_UseItem_SpecFunError;
		//return dwErrorCode;
	}

	// 消耗物品
	//DWORD dwErrorCode = GetItemMgr().ItemUsedFromBag(n64ItemSerial, 1, ELCLD_Item_Use);
	//if(dwErrorCode != E_Success)
	//{
	//	return E_UseSkill_CostLimit;
	//}

	// 检查名字合法性
	TCHAR buf[X_SHORT_NAME] = {0};
	_tcsncpy(buf, pRecv->szNewName, X_SHORT_NAME);
	buf[X_SHORT_NAME-1] = _T('\0');
	_tcslwr(buf);
	DWORD dwNameCrc = IUTIL->Crc32(buf);

	if ( g_roleMgr.GetRoleIDByNameCrc(dwNameCrc) != (DWORD)GT_INVALID )
	{
		//RoleName已经存在
		dwErrorCode = E_CreateRole_NameExist;
	}
	else
	{
		// 检查名字长度,合法性由客户端保证
		dwErrorCode = Filter::CheckName(buf, g_attRes.GetVariableLen().nRoleNameMax, g_attRes.GetVariableLen().nRoleNameMin, g_attRes.GetNameFilterWords());
	}

	// 记录旧名字
	TCHAR szOldName[X_SHORT_NAME] = {0};
	g_roleMgr.GetRoleNameByID( pRole->GetID(), szOldName );

	// 改名
	pRole->ReName(buf);

	tagNS_LPRename send;
	send.dwID = pRole->GetID();
	send.dwErrorCode = dwErrorCode;
	_tcscpy_s(send.szNewName, X_SHORT_NAME, buf);
	_tcscpy_s(send.szOldName, X_SHORT_NAME, szOldName );
	SendMessage(&send, send.dwSize);

	//if ( E_Success != dwErrorCode )
	//{
	//	return FALSE;
	//}

	//// 向数据库的改名记录表中添加记录
	//pRole->SaveNewName2DB();

	return TRUE;
}