//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: item_handler.cpp
// author: Sxg
// actor:
// data: 2008-07-21
// last:
// brief: 客户端和服务器端间消息处理 -- 物品/装备相关
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/msg_item.h"
#include "../WorldDefine/msg_loot.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/msg_item.h"
#include "../ServerDefine/msg_common.h"

#include "player_session.h"
#include "world_session.h"
#include "role.h"
#include "world.h"
#include "guild_mgr.h"
#include "role_mgr.h"

//-----------------------------------------------------------------------------
// 穿上装备
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleEquip(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_Equip);

	// 消息合法性验证
	if(pRecv->ePosDst <= EEP_Start || pRecv->ePosDst >= EEP_End)
	{
		IMSG(_T("The equip position is invalid!"));
		return GT_INVALID;
	}

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 换装
	DWORD dwErrorCode = pRole->Equip(pRecv->n64Serial, pRecv->ePosDst);

	tagNS_Equip	send;
	send.n64Serial = pRecv->n64Serial;
	send.dwErrorCode = dwErrorCode;
	SendMessage(&send, send.dwSize);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 脱下装备
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleUnequip(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_Unequip);
	
	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 换装
	DWORD dwErrorCode = pRole->Unequip(pRecv->n64Serial, pRecv->n16PosDst);
	if(E_Success == dwErrorCode)
	{
		return E_Success;
	}

	tagNS_Unequip send;
	send.n64Serial = pRecv->n64Serial;
	send.dwErrorCode = dwErrorCode;

	// 反馈给客户端换装结果
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 主副手对换
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSwapWeapon(tagNetCmd* pCmd)
{
	//MGET_MSG(pRecv, pCmd, NC_SwapWeapon);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 换装
	DWORD dwErrorCode = pRole->SwapWeapon();
	if(E_Success == dwErrorCode)
	{
		return E_Success;
	}

	tagNS_SwapWeapon send;
	send.dwErrorCode = dwErrorCode;

	// 反馈给客户端换装结果
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 鉴定装备
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleIdentifyEquip(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_IdentifyEquip);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pRole->GetItemMgr().IdentifyEquip(pRecv->n64SerialReel, 
												pRecv->n64SerialEquip, ELCLD_Equip_Identify);
	if(E_Success == dwErrorCode)
	{
		return E_Success;
	}

	tagNS_IdentifyEquip send;
	send.n64Serial = pRecv->n64SerialEquip;
	send.dwErrorCode = dwErrorCode;

	// 反馈给客户端换装结果
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 同一容器内移动物品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleChangeItemPos(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_ItemPosChange);

	if(pRecv->n16Num < 0)
	{
		return GT_INVALID;
	}
	
	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode;
	switch(pRecv->eConType)
	{
	case EICT_Equip:
		return pRole->MoveRing(pRecv->n64Serial, pRecv->n16PosDst);
		break;
	case EICT_RoleWare:
		{
			if(pRecv->dwNPCID == GT_INVALID)
			{
				break;
			}
			else
			{
				// 如果NPCID为-1，则是使用随身仓库,不必验证npc
				dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_Ware);

				if(dwErrorCode != E_Success)
				{
					return dwErrorCode;
				}
			}
		}
		break;

	case EICT_GuildWare:
		dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_GuildWare);
		if(dwErrorCode == E_Success)
		{
			g_guildMgr.AddGuildWareEvent(pRole->GetID(), EVT_GuildWareMoveTo, pCmd->dwSize, pRecv);
		}
		return dwErrorCode;
		break;
	}

	return pRole->GetItemMgr().Move(pRecv->eConType, pRecv->n64Serial, 
								pRecv->n16Num, pRecv->n16PosDst, ELCLD_Item_Move);
}

//-----------------------------------------------------------------------------
// 容器间移动物品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleChangeItemPosEx(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_ItemPosChangeEx);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 如果和角色仓库有关，则需判断NPC
	if(EICT_RoleWare == pRecv->eConTypeSrc || EICT_RoleWare == pRecv->eConTypeDst)
	{
		DWORD dwErrorCode = E_Success;
		
		if(!GT_VALID(pRecv->dwNPCID)) // 如果NPCID为-1，则是使用随身仓库,不必验证npc 
		{
			// 判断随身仓库的相关物品是否存在

			// 遍历背包
			tagItem* pItem = NULL;
			TMap<INT64, INT16>::TMapIterator iter = pRole->GetItemMgr().GetBag().Begin();

			while(pRole->GetItemMgr().GetBag().GetNextItem(iter, pItem))
			{
				if (!P_VALID(pItem))
				{
					continue;
				}
				
				if (pItem->pProtoType->eSpecFunc == EISF_PortableWare)
				{
					dwErrorCode = E_Success;
					break;
				}

				dwErrorCode = E_UseItem_ItemNotExist;
			}
		}	
		else
		{
			dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_Ware);
		}

		if(dwErrorCode != E_Success)
		{
			return dwErrorCode;
		}
		
// 		DWORD dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_Ware);
// 		if(dwErrorCode != E_Success)
// 		{
// 			return dwErrorCode;
// 		}
	}

	// 如果和帮派仓库有关，则需判断NPC
	if(EICT_GuildWare == pRecv->eConTypeSrc || EICT_GuildWare == pRecv->eConTypeDst)
	{
		DWORD dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_GuildWare);
		if(dwErrorCode == E_Success)
		{
			g_guildMgr.AddGuildWareEvent(pRole->GetID(), EVT_GuildWareMove2Other, pCmd->dwSize, pRecv);
		}
		return dwErrorCode;
	}

	return pRole->GetItemMgr().Move2Other(pRecv->eConTypeSrc, pRecv->n64Serial1, 
									pRecv->eConTypeDst, pRecv->n16PosDst, ELCLD_Item_Move2Other);
}

//------------------------------------------------------------------------
// 按指定顺序排列行囊中物品
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleReorderItem(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_ItemReorder);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 消息合法性弱检测
	if(pRecv->n16ItemNum <= 0 
		|| (pRecv->n16ItemNum > SPACE_ALL_BAG && pRecv->n16ItemNum > SPACE_ALL_WARE))
	{
		return GT_INVALID;
	}

	// 创建消息
	INT nSzMsg = sizeof(tagNS_ItemReorder) + (pRecv->n16ItemNum - 1) * sizeof(INT16);
	MCREATE_MSG(pSend, nSzMsg, NS_ItemReorder);

	// 处理
	DWORD dwErrorCode = E_Success;
	if(GT_INVALID == pRecv->dwNPCID)	
	{
		if(pRecv->eContainerType == EICT_Bag)	// 背包
		{
			pSend->eConType = EICT_Bag;
			dwErrorCode = pRole->GetItemMgr().ReorderBag(pRecv->n16Index, pSend->n16OldIndex, pRecv->n16ItemNum);
		}
		else if (pRecv->eContainerType == EICT_RoleWare)
		{
			pSend->eConType = EICT_RoleWare;
			dwErrorCode = pRole->GetItemMgr().ReorderRoleWare(pRecv->n16Index, pSend->n16OldIndex, pRecv->n16ItemNum);
		}
	}
	else	// 仓库
	{
		pSend->eConType = EICT_RoleWare;
		dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_Ware);
		if(E_Success == dwErrorCode)
		{
			dwErrorCode = pRole->GetItemMgr().ReorderRoleWare(pRecv->n16Index, pSend->n16OldIndex, pRecv->n16ItemNum);
		}
	}

	// 设置消息
	pSend->n16ItemNum = pRecv->n16ItemNum;
	pSend->dwErrorCode = dwErrorCode;

	if(dwErrorCode != E_Success)
	{
		pSend->dwSize = sizeof(tagNS_ItemReorder);
	}

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return dwErrorCode;
}
//------------------------------------------------------------------------
// 使用记录式传送符设置传送坐标
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSetTransportMap(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SetTransportMap);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	//设置点
	pRole->SetItemTransportPoint(pRecv);

	return E_Success;
}
//------------------------------------------------------------------------
// 使用记录式传送符传送
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleUseTransportMap(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_UseTransportMap);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwTypeID	=	GT_INVALID;
	bool bImmediate =	false;
	INT	nRet		=	E_Success;

	// 检查物品是否在背包里
	tagItem* pItem = pRole->GetItemMgr().GetBagItem(pRecv->n64ItemID); 
	if( !P_VALID(pItem) ) 
	{
		nRet = E_UseItem_ItemNotExist;
	}
	else
	{
		//检查状态限制
		DWORD dwSelfStateFlag = pRole->GetStateFlag();
		if( (dwSelfStateFlag & pItem->pProtoType->dwStateLimit) != dwSelfStateFlag )
		{
			nRet = E_UseItem_SelfStateLimit;
		}
		else if(pItem->pProtoType->eSpecFunc != EISF_TransportTool)
		{
			nRet = E_UseItem_SpecFunError;
		}
		else
		{
			// 处理物品消失
			nRet = pRole->GetItemMgr().ItemUsedFromBag(pRecv->n64ItemID, 1, (DWORD)ELCLD_Item_Use);
		}
	}

	if( E_Success != nRet)
	{
		tagNS_UseItem send;
		send.dwSrcRoleID = pRole->GetID();
		send.dwTarRoleID = pRole->GetID();
		send.n64ItemID = pRecv->n64ItemID;
		send.dwTypeID = dwTypeID;
		send.dwSerial = 0;
		send.dwErrorCode = DWORD(nRet);
		send.bInmmediate = bImmediate;

		SendMessage(&send, send.dwSize);
	}
	else
	{
		tagNS_UseItem send;
		send.dwSrcRoleID = pRole->GetID();
		send.dwTarRoleID = pRole->GetID();
		send.n64ItemID = pRecv->n64ItemID;
		send.dwTypeID = dwTypeID;
		send.dwSerial = 0;
		send.nSpellTime = 0;
		send.dwErrorCode = E_Success;
		send.bInmmediate = bImmediate;

		//使用记录式传送符
		pRole->UseNoteTakingItemTransport();
	}

	return E_Success;
}


DWORD PlayerSession::HandleRoleReorderItemEx(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_ItemReorderEx);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 消息合法性弱检测
	if(pRecv->n16ItemNum <= 0 
		|| (pRecv->n16ItemNum > SPACE_ALL_BAG && pRecv->n16ItemNum > SPACE_ALL_WARE))
	{
		return GT_INVALID;
	}

	// 创建消息
	INT nSzMsg = sizeof(tagNS_ItemReorderEx) + pRecv->n16ItemNum * sizeof(tagItemOrder) - sizeof(BYTE);
	MCREATE_MSG(pSend, nSzMsg, NS_ItemReorderEx);

	// 处理
	DWORD dwErrorCode = E_Success;
	if( GT_INVALID == pRecv->dwNPCID)	// 背包
	{
		if( pRecv->eContainerType == EICT_Bag )
		{
			pSend->eConType = EICT_Bag;
			dwErrorCode = pRole->GetItemMgr().ReorderBagEx(pRecv->byData, pSend->byData, pSend->n16ItemNum, pRecv->n16ItemNum);
		}
		else if( pRecv->eContainerType == EICT_RoleWare )		//随身仓库
		{
			pSend->eConType = EICT_RoleWare;
			dwErrorCode = pRole->GetItemMgr().ReorderRoleWareEx(pRecv->byData, pSend->byData, pSend->n16ItemNum, pRecv->n16ItemNum);
		}
	}
	else	// 仓库
	{
		pSend->eConType = EICT_RoleWare;
		dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_Ware);
		if(E_Success == dwErrorCode)
		{
			dwErrorCode = pRole->GetItemMgr().ReorderRoleWareEx(pRecv->byData, 
							pSend->byData, pSend->n16ItemNum, pRecv->n16ItemNum);
		}
	}

	// 设置消息
	pSend->dwErrorCode = dwErrorCode;

	if(pSend->n16ItemNum != pRecv->n16ItemNum)
	{
		IMSG(_T("\n\nCaution:\n\tRole<id: %u> maybe modified bag order msg!\n"), pRole->GetID());
		ASSERT(pSend->n16ItemNum == pRecv->n16ItemNum);

		pSend->dwSize = sizeof(tagNS_ItemReorderEx) + pSend->n16ItemNum * sizeof(tagItemOrder) - sizeof(BYTE);
	}

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 拾取物品
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePickUpItem(tagNetCmd* pCmd)
{
	tagNC_RolePickUpItem* pRecv = (tagNC_RolePickUpItem*)pCmd;

	if( !P_VALID(GetRole()) )
		return GT_INVALID;

	DWORD dwRtv =GetRole()->CanPickUpItem(pRecv->n64Serial);
	if (E_Success == dwRtv)
	{
		dwRtv = GetRole()->PickUpItem(pRecv->n64Serial);
	}
	tagNS_RolePickUpItem send;
	send.dwRoleID		= GetRole()->GetID();
	send.n64Serial		= pRecv->n64Serial;
	send.dwErrorCode	= dwRtv;
	SendMessage(&send, send.dwSize);

	return 0;
}

//------------------------------------------------------------------------
// 扔掉物品
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePutDownItem(tagNetCmd* pCmd)
{
	tagNC_RolePutDownItem* pRecv = (tagNC_RolePutDownItem*)pCmd;

	if( !P_VALID(GetRole()) )
		return GT_INVALID;

	tagNS_RolePutDownItem send;
	send.dwErrorCode = GetRole()->PutDownItem(pRecv->n64Serial);
	SendMessage(&send, send.dwSize);

	return 0;
}

//------------------------------------------------------------------------
// 行囊加密
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSetBagPsd(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_SetBagPsd);
	
	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_BagPsd send;
	
	if(IsHaveBagPsd())	// 已设置
	{
		send.dwErrorCode = E_BagPsd_Exist;
	}
	else	// 未设置，设置
	{
		SetBagPsd(p->dwBagPsdCrc);
		send.dwRoleStateEx	= pRole->m_RoleStateEx.GetState();
		send.dwErrorCode	= E_Success;
	}

	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//------------------------------------------------------------------------
// 取消行囊加密
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleUnsetBagPsd(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_UnsetBagPsd);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}
	
	tagNS_BagPsd send;

	if(!IsHaveBagPsd())
	{
		send.dwErrorCode = E_BagPsd_NoExist;
	}
	else
	{
		if(GetSafeCode() != p->dwSafeCodeCrc)
		{
			send.dwErrorCode = E_BagPsd_SafeCode_Error;
		}
		else
		{
			SetBagPsd(GT_INVALID);
			send.dwRoleStateEx	= pRole->m_RoleStateEx.GetState();
			send.dwErrorCode = E_Success;
		}
	}

	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//------------------------------------------------------------------------
// 验证行囊密码
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleCheckBagPsd(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_OldBagPsd);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}
	
	tagNS_BagPsd send;
	send.dwRoleStateEx	= pRole->m_RoleStateEx.GetState();
	send.dwErrorCode	= E_BagPsd_OK;

	if(GetBagPsd() != p->dwOldBagPsdCrc)
	{
		send.dwErrorCode = E_BagPsd_Error;
	}

	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//------------------------------------------------------------------------
// 修改行囊密码
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleResetBagPsd(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_ResetBagPsd);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}
	
	tagNS_BagPsd send;

	if(!IsHaveBagPsd())
	{
		send.dwErrorCode = E_BagPsd_NoExist;
	}
	else
	{
		if(GetBagPsd() != p->dwOldBagPsdCrc)
		{
			send.dwErrorCode = E_BagPsd_Error;
		}
		else if(p->dwOldBagPsdCrc == p->dwNewBagPsdCrc)
		{
			send.dwErrorCode = GT_INVALID;
		}
		else
		{
			SetBagPsd(p->dwNewBagPsdCrc);
			send.dwRoleStateEx	= pRole->m_RoleStateEx.GetState();
			send.dwErrorCode = E_Success;
		}
	}

	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//------------------------------------------------------------------------
// 打开背包等需要发送密码
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleOpenBagPsd(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_OpenBagPsd);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}
	
	tagNS_BagPsd send;

	if(IsHaveBagPsd() && GetBagPsd() != p->dwBagPsdCrc)
	{
		send.dwErrorCode = E_BagPsd_Error;
	}
	else
	{
		pRole->SetRoleStateEx(ERSE_BagPsdPass, TRUE);
		send.dwRoleStateEx	= pRole->m_RoleStateEx.GetState();
		send.dwErrorCode	= E_Success;
	}

	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//------------------------------------------------------------------------
// 设置行囊密码及发送相关消息
//------------------------------------------------------------------------
VOID PlayerSession::SetBagPsd(DWORD dwNewPsdCrc)
{
	m_sAccountCommon.dwBagPsdCrc = dwNewPsdCrc;

	// 向dbserver发消息
	tagNDBC_ChangeBagPsd send;
	send.dwAccountID = GetSessionID();
	send.dwNewBagPsd = dwNewPsdCrc;
	g_dbSession.Send(&send, send.dwSize);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return;
	}
	
	// 根据新密码设置角色状态
	pRole->m_RoleStateEx.SetState(ERSE_BagPsdPass);
	pRole->m_RoleStateEx.SetState(ERSE_BagPsdExist);

	if(GT_INVALID == dwNewPsdCrc)
	{
		pRole->m_RoleStateEx.UnsetState(ERSE_BagPsdExist);
	}
}

//------------------------------------------------------------------------
// 获得其他玩家的装备信息
//------------------------------------------------------------------------
DWORD PlayerSession::HandleGetSomeoneEquip(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetRemoteRoleEquipInfo);

	DWORD dwRoleID = pRecv->dwRoleID;
	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	if(!P_VALID(pRole)) return GT_INVALID; 

	INT nIndex = 0;
	INT nMaxIndex = EEP_MaxEquip;
	BYTE* pTemp = NULL;

	
	INT nMaxSize = sizeof(tagEquip)*nMaxIndex + sizeof(DWORD) + sizeof(INT) + sizeof(BYTE);
	MCREATE_MSG(pSend, nMaxSize,NS_GetRemoteRoleEquipInfo);

	pSend->dwRoleID = pRecv->dwRoleID;
	pSend->nEquipNum = 0;
	pTemp = pSend->byEquip;

	for(; nIndex < nMaxIndex; ++nIndex)
	{	
		tagEquip* pEquipInfo = pRole->GetItemMgr().GetEquipBarEquip((INT16)nIndex);
		if (!P_VALID(pEquipInfo)) continue;

		memcpy(pTemp, pEquipInfo, sizeof(tagEquip));

		pTemp += sizeof(tagEquip);

		(pSend->nEquipNum)++;
		pEquipInfo = NULL;
	}

	(*pTemp) = _T('\0');
	pSend->dwSize = sizeof(tagNS_GetRemoteRoleEquipInfo) + pSend->nEquipNum * sizeof(tagEquip);

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
	return E_Success;
}

//-----------------------------------------------------------------------------
// 打开随身仓库
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSideWareOpen(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_WareOpen);

	DWORD dwRet = E_Success;

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_WareOpen	send;
	send.dwErrorCode = pRole->OpenSideWare(pRecv->n64ItemID);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}
