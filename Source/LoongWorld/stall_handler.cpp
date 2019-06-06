//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: stall_handler.cpp
// author: Sxg
// actor:
// data: 2008-12-26
// last:
// brief: 摆摊消息处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/msg_stall.h"
#include "player_session.h"
#include "role.h"
#include "vip_stall.h"

//-----------------------------------------------------------------------------
// 准备摆摊
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallStart(tagNetCmd* pCmd)
{
	if(P_VALID(m_pRole) && GetFatigueGuarder().GetEarnRate() < 10000)
	{
		GetRole()->SendFatigueGuardInfo(E_FatigueLimit_Stall);
		return 0;
	}

	MGET_MSG(pRecv, pCmd, NC_StallStart);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_StallStart send;
	send.dwErrorCode = pRole->StartStall();
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 商品上架
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallSetGoods(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallSetGoods);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_StallSetGoods send;
	send.dwErrorCode = pRole->SetStallGoods(pRecv->n64Serial, pRecv->n64UnitPrice, pRecv->byIndex);
	SendMessage(&send, send.dwSize);

	// 是否需要向周围玩家广播
	if(pRole->IsSetGoodsFinish() && E_Success == send.dwErrorCode && P_VALID(pRole->GetMap()))
	{
		tagNS_StallSetRefresh send;
		send.dwStallRoleID = pRole->GetID();
		send.byIndex = pRecv->byIndex;

		if( P_VALID(pRole->GetMap()) )
		{
			pRole->GetMap()->SendBigVisTileMsg(pRole, &send, send.dwSize);
		}

		// 通知VIP摊位更新物品
		tagVIPStall* pVIPStall = g_VIPStall.GetVIPStallInfo(pRole->GetID());
		if (P_VALID(pVIPStall))
		{
			g_VIPStall.AddEvent(pRole->GetID(), EVT_ChangeStallGoods, 0, NULL);
		}
	}

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 商品下架
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallUnsetGoods(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallUnsetGoods);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_StallUnsetGoods send;
	send.dwErrorCode = pRole->UnsetStallGoods(pRecv->byIndex);
	SendMessage(&send, send.dwSize);

	// 额外处理
	if(pRole->IsSetGoodsFinish() && E_Success == send.dwErrorCode)
	{
		// 检查摊位上是否还有商品
		if(pRole->IsNoGoodsInStall())
		{
			// 没有，则收摊
			pRole->SendCloseStall();
		}
		else if(P_VALID(pRole->GetMap()))
		{
			// 有，则向周围玩家广播
			tagNS_StallUnsetRefresh send;
			send.dwStallRoleID = pRole->GetID();
			send.byIndex = pRecv->byIndex;

			if( P_VALID(pRole->GetMap()) )
			{
				pRole->GetMap()->SendBigVisTileMsg(pRole, &send, send.dwSize);
			}

			// 通知VIP摊位更新物品
			tagVIPStall* pVIPStall = g_VIPStall.GetVIPStallInfo(pRole->GetID());
			if (P_VALID(pVIPStall))
			{
				g_VIPStall.AddEvent(pRole->GetID(), EVT_ChangeStallGoods, 0, NULL);
			}
		}
	}
	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 设置标题
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallSetTitle(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallSetTitle);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	MGET_MSG_STRING(strTitle, pRecv->szData, pRecv->dwSize, NC_StallSetTitle, szData);

	tagNS_StallSetTitle send;
	send.dwErrorCode = pRole->SetStallTitle(strTitle.c_str());
	SendMessage(&send, send.dwSize);

	if (send.dwErrorCode == E_Success)
	{
		// 通知VIP摊位标题变更
		tagVIPStall* pVIPStall = g_VIPStall.GetVIPStallInfo(pRole->GetID());
		if (P_VALID(pVIPStall))
		{
			// 这里保证字符串的结束符
			g_VIPStall.AddEvent(pRole->GetID(), EVT_SetStallTitle, sizeof(TCHAR)*(strTitle.length()+1), (LPVOID)(strTitle.c_str()));
		}
	}

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 设置广告
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallSetAdText(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallSetAdText);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	MGET_MSG_STRING(strAdText, pRecv->szData, pRecv->dwSize, NC_StallSetAdText, szData);
	
	tagNS_StallSetAdText send;
	send.dwErrorCode = pRole->SetStallAdText(strAdText.c_str());
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 设置广告的广播标志位
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallSetAdFlag(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallSetAdFlag);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	tagNS_StallSetAdFlag send;
	send.dwErrorCode = pRole->SetStallAdFlag(pRecv->bFlag);
	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 通知周围玩家，自己开始摆摊
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallSetFinish(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallSetFinish);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	if(!P_VALID(pRole->GetMap()))
	{
		return GT_INVALID;
	}

	tagNS_StallSetFinish send;
	send.dwErrorCode	= pRole->SetStallFinish();
	send.dwRoleID		= pRole->GetID();
	send.byStallLevel	= pRole->GetStallModeLevel();

	if( P_VALID(pRole->GetMap()) )
	{
		pRole->GetMap()->SendBigVisTileMsg(pRole, &send, send.dwSize);
	}

	if( E_Success == send.dwErrorCode )
	{
		pRole->GetMoveData().StopMoveForce();
	}

	// 通知VIP摊位更新状态
	tagVIPStall* pVIPStall = g_VIPStall.GetVIPStallInfo(pRole->GetID());
	if (P_VALID(pVIPStall))
	{
		EVIPStallStatus eStatus = EVSS_Open;
		g_VIPStall.AddEvent(pRole->GetID(), EVT_SetStallStatus, 1, &eStatus);
	}

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 收摊
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallClose(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallClose);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	if(!P_VALID(pRole->GetMap()))
	{
		return GT_INVALID;
	}

	return pRole->SendCloseStall();
}

//-----------------------------------------------------------------------------
// 获得摊位标题
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallGetTitle(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallGetTitle);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 获得map
	Map *pMap = pRole->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	// 找到摊主
	Role *pStallRole = pMap->FindRole(pRecv->dwStallRoleID);
	if(!P_VALID(pStallRole))
	{
		return GT_INVALID;
	}

	INT nChNumTitle = 0;
	
	tagNS_StallGetTitle send;
	send.dwStallRoleID = pRecv->dwStallRoleID;
	send.dwErrorCode = pStallRole->GetStallTitle(send.szTitle);

	// 重新计算消息大小
	if(E_Success == send.dwErrorCode)
	{
		send.dwSize -= ((STALL_MAX_TITLE_NUM - _tcslen(send.szTitle) - 1) * sizeof(TCHAR));
	}
	else
	{
		send.dwSize -= ((STALL_MAX_TITLE_NUM - 1) * sizeof(TCHAR));
	}

	SendMessage(&send, send.dwSize);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获得摊位上所有物品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallGet(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallGet);
	
	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 获得map
	Map *pMap = pRole->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	// 找到摊主
	Role *pStallRole = pMap->FindRole(pRecv->dwStallRoleID);
	if(!P_VALID(pStallRole))
	{
		//tagNS_StallGet send;
		//send.dwErrorCode	= E_Stall_Cannot_Find_StallRole;
		//send.dwStallRoleID	= pRecv->dwStallRoleID;
		//SendMessage(&send, send.dwSize);

		//return send.dwErrorCode;

		return GT_INVALID;
	}

	// 判断距离
	if(!pRole->IsInDistance(*pStallRole, MAX_EXCHANGE_DISTANCE))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	// 为返回消息分配空间
	INT	nSzMsg = pStallRole->CalStallGoodsMemUsed();
	if(0 == nSzMsg)
	{
		// 摊位上没有商品
		return GT_INVALID;
	}

	nSzMsg += (sizeof(tagNS_StallGet) - 1);
	MCREATE_MSG(pSend, nSzMsg, NS_StallGet);

	DWORD dwErrorCode = pStallRole->GetStallGoods(pSend->byData, pSend->byNum, nSzMsg);
	pSend->dwErrorCode		= dwErrorCode;
	pSend->dwStallRoleID	= pRecv->dwStallRoleID;
	
	// 重新计算大小
	pSend->dwSize = nSzMsg + (sizeof(tagNS_StallGet) - 1);

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 购买摊位上的商品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallBuy(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallBuy);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 获得map
	Map *pMap = pRole->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	// 找到摊主
	Role *pStallRole = pMap->FindRole(pRecv->dwStallRoleID);
	if(!P_VALID(pStallRole))
	{
		return GT_INVALID;
	}

	// 判断距离
	if(!pRole->IsInDistance(*pStallRole, MAX_EXCHANGE_DISTANCE))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	INT16 n16RemainNum = 0;
	DWORD dwErrorCode = pStallRole->BuyStallGoods(pRole, 
							pRecv->n64Serial, pRecv->n64UnitPrice, pRecv->n16Num, pRecv->byIndex, n16RemainNum);
	
	if(dwErrorCode != E_Success)
	{
		tagNS_StallBuy send;
		send.dwErrorCode = dwErrorCode;
		SendMessage(&send, send.dwSize);
	}
	else
	{
		// 检查摊位上是否还有商品
		if(pStallRole->IsNoGoodsInStall())
		{
			// 没有，则收摊
			pStallRole->SendCloseStall();
		}
		else
		{
			// 更新物品更新时间
			g_VIPStall.AddEvent(pRole->GetID(), EVT_ChangeStallGoods, 0, NULL);

			// 有，则向周围广播更新后的商品个数
			tagNS_StallBuyRefresh send;
			send.dwStallRoleID = pRecv->dwStallRoleID;
			send.byIndex = pRecv->byIndex;
			send.n16Num = n16RemainNum;
			pMap->SendBigVisTileMsg(pStallRole, &send, send.dwSize);
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取摊位上指定位置的商品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStallGetSpec(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_StallGetSpec);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 获得map
	Map *pMap = pRole->GetMap();
	if(!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	// 找到摊主
	Role *pStallRole = pMap->FindRole(pRecv->dwStallRoleID);
	if(!P_VALID(pStallRole))
	{
		return GT_INVALID;
	}

	// 判断距离
	if(!pRole->IsInDistance(*pStallRole, MAX_EXCHANGE_DISTANCE))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	// 是否收摊
	if(!pStallRole->IsInRoleState(ERS_Stall))
	{
		return GT_INVALID;
	}

	// 为返回消息分配空间
	INT	nSzMsg = (sizeof(tagNS_StallGetSpec) - 1) + (sizeof(tagMsgStallGoods) - 1 + SIZE_EQUIP);
	MCREATE_MSG(pSend, nSzMsg, NS_StallGetSpec);

	DWORD dwErrorCode = pStallRole->GetStallSpecGoods(pRecv->byIndex, pSend->byData, nSzMsg);
	pSend->dwErrorCode		= dwErrorCode;
	pSend->dwStallRoleID	= pRecv->dwStallRoleID;

	// 重新计算大小
	pSend->dwSize = nSzMsg + (sizeof(tagNS_StallGetSpec) - 1);

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取所有VIP摊位信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetAllVIPStallInfo(tagNetCmd* pCmd)
{
	tagNS_GetAllVIPStallInfo send;

	send.dwErrorCode = g_VIPStall.GetAllVIPStallInfo(send.VIPStallInfo, send.dwRequestTime);

	// !!这么搞比较危险，待定
	if (send.dwErrorCode != E_Success)
	{
		// 重新计算大小
		send.dwSize = sizeof(DWORD);
	}

	SendMessage(&send, send.dwSize);

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 更新VIP摊位信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleUpdateVIPStallInfo(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_UpdateVIPStallInfo);
	DWORD dwRequestTime = pRecv->dwRequestTime;

	// 分配足够大的空间
	INT nSzMsg = sizeof(tagNS_UpdateVIPStallInfo) + sizeof(tagVIPStall)*(VIP_STALL_MAX_NUM-1);
	MCREATE_MSG(pSend, nSzMsg, NS_UpdateVIPStallInfo);

	pSend->dwErrorCode = g_VIPStall.GetUpdatedStallInfo(pSend->VIPStallInfo, pSend->nInfoNum, dwRequestTime);

	// 成功时重置时间，失败也不会变更
	pSend->dwRequestTime = dwRequestTime;

	// 重新计算消息大小
	pSend->dwSize = sizeof(tagNS_UpdateVIPStallInfo) + sizeof(tagVIPStall) * (pSend->nInfoNum - 1);

	SendMessage(pSend, pSend->dwSize);

	return pSend->dwErrorCode;
}

//-----------------------------------------------------------------------------
// 申请VIP摊位
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleApplyVIPStall(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	MGET_MSG(pRecv, pCmd, NC_ApplyVIPStall);

	tagNS_ApplyVIPStall send;

	send.dwErrorCode = g_VIPStall.ApplyVIPStall(pRole, pRecv->byIndex, pRecv->nRent);

	SendMessage(&send, send.dwSize);

	if (send.dwErrorCode == E_Success)
	{
		// 取得摊位信息
		tagVIPStall* pStallInfo = g_VIPStall.GetVIPStallInfo(pRecv->byIndex);
		if (!P_VALID(pStallInfo))	
			return GT_INVALID;
		
		// 更新一条摊位信息
		tagNS_UpdateVIPStallInfo updateMsg;
		updateMsg.dwErrorCode		= E_Success;
		updateMsg.dwRequestTime		= pStallInfo->dwLastUpdateTime;
		updateMsg.nInfoNum			= 1;
		IFASTCODE->MemCpy(updateMsg.VIPStallInfo, pStallInfo, sizeof(tagVIPStall));
	}

	return send.dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取某一个VIP摊位上的物品信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleSpecVIPStallGet(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SpecVIPStallGet);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 找到摊主
	Role *pStallRole = g_VIPStall.GetVIPStallRole(pRecv->byIndex);
	if(!P_VALID(pStallRole))
	{
		return GT_INVALID;
	}

	// 不可操作自己的摊位
	if (pStallRole->GetID() == pRole->GetID())
	{
		tagNS_StallGet send;
		send.dwErrorCode = E_Stall_VIP_Self_Stall;
		SendMessage(&send, send.dwSize);

		return send.dwErrorCode;
	}

	// 为返回消息分配空间
	INT	nSzMsg = pStallRole->CalStallGoodsMemUsed();
	if(0 == nSzMsg)
	{
		// 摊位上没有商品
		return GT_INVALID;
	}

	//nSzMsg += (sizeof(tagNS_SpecVIPStallGet) - 1);
	//MCREATE_MSG(pSend, nSzMsg, NS_SpecVIPStallGet);

	//DWORD dwRequestTime		= pRecv->dwRequestTime;
	//DWORD dwErrorCode		= g_VIPStall.GetVIPStallGoods(pSend->byData, pSend->byNum, nSzMsg, dwRequestTime, pRecv->byIndex);
	//pSend->dwErrorCode		= dwErrorCode;
	//pSend->byIndex			= pRecv->byIndex;
	//pSend->dwRequestTime	= dwRequestTime;

	//// 重新计算大小
	//pSend->dwSize = nSzMsg + (sizeof(tagNS_SpecVIPStallGet) - 1);

	//SendMessage(pSend, pSend->dwSize);

	//MDEL_MSG(pSend);

	nSzMsg += (sizeof(tagNS_StallGet) - 1);
	MCREATE_MSG(pSend, nSzMsg, NS_StallGet);

	DWORD dwErrorCode = pStallRole->GetStallGoods(pSend->byData, pSend->byNum, nSzMsg);
	pSend->dwErrorCode		= dwErrorCode;
	pSend->dwStallRoleID	= pStallRole->GetID();

	// 重新计算大小
	pSend->dwSize = nSzMsg + (sizeof(tagNS_StallGet) - 1);

	SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 购买VIP摊位上的商品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleBuyVIPStallGoods(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_VIPStallBuy);

	// 获取人物
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 不可操作自己的摊位
	if (pRecv->dwStallRoleID == pRole->GetID())
	{
		tagNS_VIPStallBuy send;
		send.dwErrorCode = E_Stall_VIP_Self_Stall;
		SendMessage(&send, send.dwSize);

		return send.dwErrorCode;
	}

	// 找到摊位
	tagVIPStall* pVIPStallInfo = g_VIPStall.GetVIPStallInfo(pRecv->dwStallRoleID);
	if (!P_VALID(pVIPStallInfo))
	{
		return GT_INVALID;
	}

	// 找到摊主
	Role *pStallRole = g_VIPStall.GetVIPStallRole(pVIPStallInfo->byStallIndex);
	if(!P_VALID(pStallRole))
	{
		return GT_INVALID;
	}

	INT16 n16RemainNum = 0;
	DWORD dwErrorCode = pStallRole->BuyStallGoods(pRole, 
		pRecv->n64Serial, pRecv->n64UnitPrice, pRecv->n16Num, pRecv->byIndex, n16RemainNum);

	if(dwErrorCode != E_Success)
	{
		tagNS_VIPStallBuy send;
		send.dwErrorCode = dwErrorCode;
		SendMessage(&send, send.dwSize);
	}
	else
	{
		// 检查摊位上是否还有商品
		if(pStallRole->IsNoGoodsInStall())
		{
			// 没有，则收摊
			pStallRole->SendCloseStall();
		}
		else
		{
			// 更新物品更新时间
			g_VIPStall.AddEvent(pRole->GetID(), EVT_ChangeStallGoods, 0, NULL);

			// 有，只向操作玩家发送更新后的商品个数
			tagNS_StallBuyRefresh send;
			send.dwStallRoleID = pRecv->dwStallRoleID;
			send.byIndex = pRecv->byIndex;
			send.n16Num = n16RemainNum;
			
			SendMessage(&send, send.dwSize);
		}
	}

	return dwErrorCode;
}