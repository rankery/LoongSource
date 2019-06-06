//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: TradeYuanBao.cpp
// author: wjiang
// actor:
// data: 2009-3-3
// last:
// brief: 元宝交易类
//-------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "TradeYuanBao.h"
#include "world.h"
#include "role_mgr.h"
#include "role.h"
#include "db_session.h"

#include "../WorldDefine/msg_mall.h"
#include "../ServerDefine/msg_yuanbao.h"

CTradeYB g_tradeYB;

VOID tagYuanBaoAccount::SetSellOrder(BOOL bSubmit) 
{ 
	bSellOrder = bSubmit; 

	// 同步数据库
	tagNDBC_SynSubmitSell	sendDB;
	sendDB.dwRoleID = dwRoleID;
	sendDB.bSellOrder = (bool)bSellOrder;
	g_dbSession.Send(&sendDB, sendDB.dwSize);
}

VOID tagYuanBaoAccount::SetBuyOrder(BOOL bSubmit) 
{
	bBuyOrder = bSubmit;

	// 同步数据库
	tagNDBC_SynSubmitBuy	sendDB;
	sendDB.dwRoleID = dwRoleID;
	sendDB.bBuyOrder = (bool)bBuyOrder;
	g_dbSession.Send(&sendDB, sendDB.dwSize);
}

BOOL tagYuanBaoAccount::IncAccountYB(INT32 nYuanBao, DWORD dwCmdID, BOOL bNoticeClient)
{
	if(nYuanBao <= 0)
	{
		return FALSE;
	}

	INT32 nInc = YuanBao.Gain(nYuanBao);

	// 记录log//??
	
	// 向客户端发消息
	if(bNoticeClient == TRUE)
	{
		Role *pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
		if(P_VALID(pRole))
		{
			tagNS_SynAccoutYB	send;
			send.nYuanBao = GetAccountYB();
			pRole->SendMessage(&send, sizeof(send));
		}
	}

	// 同步数据库
	tagNDBC_SynAccountYB	sendDB;
	sendDB.dwRoleID = dwRoleID;
	sendDB.nYuanBao = GetAccountYB();
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	return TRUE;
}

BOOL tagYuanBaoAccount::IncAccountSilver(INT64 n64Silver, DWORD dwCmdID, BOOL bNoticeClient)
{
	if(n64Silver <= 0)
	{
		return FALSE;
	}

	INT64 n64Inc = Silver.Gain(n64Silver);

	// 记录log//??

	// 向客户端发消息
	if(bNoticeClient == TRUE)
	{
		Role *pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
		if(P_VALID(pRole))
		{
			tagNS_SynAccoutSilver	send;
			send.nSilver = GetAccountSilver();
			pRole->SendMessage(&send, sizeof(send));
		}
	}

	// 同步数据库
	tagNDBC_SynAccountSilver	sendDB;
	sendDB.dwRoleID = dwRoleID;
	sendDB.Money.nGold = MSilver2DBGold(GetAccountSilver());
	sendDB.Money.nSilver = MSilver2DBSilver(GetAccountSilver());
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	return TRUE;
}

BOOL tagYuanBaoAccount::DecAccountYuanBao(INT32 nYuanBao, DWORD dwCmdID, BOOL bNoticeClient)
{
	if(nYuanBao <= 0)
	{
		return FALSE;
	}

	INT64 n64Dec = YuanBao.Spend(nYuanBao);

	// 记录log//??

	// 向客户端发消息
	if(bNoticeClient == TRUE)
	{
		Role *pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
		if(P_VALID(pRole))
		{
			tagNS_SynAccoutYB	send;
			send.nYuanBao = GetAccountYB();
			pRole->SendMessage(&send, sizeof(send));
		}
	}

	// 同步数据库
	tagNDBC_SynAccountYB	sendDB;
	sendDB.dwRoleID = dwRoleID;
	sendDB.nYuanBao = GetAccountYB();
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	return TRUE;
}

BOOL tagYuanBaoAccount::DecAccountSilver(INT64 n64Silver, DWORD dwCmdID, BOOL bNoticeClient)
{
	if(n64Silver <= 0)
	{
		return FALSE;
	}

	INT64 n64Dec = Silver.Spend(n64Silver);

	// 记录log//??

	// 向客户端发消息
	if(bNoticeClient == TRUE)
	{
		Role *pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
		if(P_VALID(pRole))
		{
			tagNS_SynAccoutSilver	send;
			send.nSilver = GetAccountSilver();
			pRole->SendMessage(&send, sizeof(send));
		}
	}

	// 同步数据库
	tagNDBC_SynAccountSilver	sendDB;
	sendDB.dwRoleID = dwRoleID;
	sendDB.Money.nGold = MSilver2DBGold(GetAccountSilver());
	sendDB.Money.nSilver = MSilver2DBSilver(GetAccountSilver());
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	return TRUE;
}


BOOL CTradeYB::Init()
{
	m_mapYBAccount.Clear();
	m_setYBBuy.clear();
	m_setYBSell.clear();
	m_mapYBBuyList.Clear();
	m_mapYBSellList.Clear();
	m_mapOrder2Role.Clear();

	m_dwMaxOrderID	= 0;

	return TRUE;
}

CTradeYB::~CTradeYB()
{
	Destroy();
}

//-------------------------------------------------------------------------------------------------------
// 每小时执行一次
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::OnHour()
{
	// 遍历交易订单
	tagYuanBaoOrder*	pYBOrder = NULL;
	tagDWORDTime		cur_time = GetCurrentDWORDTime();
	DWORD				dwCloseTime = DecreaseTime(cur_time, 2 * 24 * 60 * 60);

	// 订单提交超过两天时间，设为关闭状态
	std::set<tagYuanBaoOrder*, BuyOrderCmp>::iterator itOrderBuy = m_setYBBuy.begin();
	while(itOrderBuy != m_setYBBuy.end())
	{
		pYBOrder = *itOrderBuy;
		itOrderBuy++;

		if(dwCloseTime >= pYBOrder->dwStartTime)
			DeleteYBOrder(pYBOrder, EYBOM_Close);
	}

	std::set<tagYuanBaoOrder*, SellOrderCmp>::iterator itOrderSell = m_setYBSell.begin();
	while(itOrderSell != m_setYBSell.end())
	{
		pYBOrder = *itOrderSell;
		itOrderSell++;

		if(dwCloseTime >= pYBOrder->dwStartTime)
			DeleteYBOrder(pYBOrder, EYBOM_Close);
	}

	// 删除数据库中时间超过七天的所有订单
	DWORD	dwDeleteTime = DecreaseTime(cur_time, 7 * 24 * 60 * 60);
	tagNDBC_DeleteYBOrder	sendDB;
	sendDB.dwDeleteTime = dwDeleteTime;
	g_dbSession.Send(&sendDB, sendDB.dwSize);
}

VOID CTradeYB::Destroy()
{
	tagYuanBaoAccount* pYBAccount = NULL;
	TMap<DWORD, tagYuanBaoAccount*>::TMapIterator itAccount = m_mapYBAccount.Begin();
	while( m_mapYBAccount.PeekNext(itAccount, pYBAccount) )
	{
		SAFE_DEL(pYBAccount);
	}

	tagYuanBaoOrder* pYBOrder = NULL;
	std::set<tagYuanBaoOrder*, BuyOrderCmp>::iterator itOrderBuy = m_setYBBuy.begin();
	while(itOrderBuy != m_setYBBuy.end())
	{
		SAFE_DEL(*itOrderBuy);
		itOrderBuy++;
	}
	m_setYBBuy.clear();

	std::set<tagYuanBaoOrder*, SellOrderCmp>::iterator itOrderSell = m_setYBSell.begin();
	while(itOrderSell != m_setYBSell.end())
	{
		SAFE_DEL(*itOrderSell);
		itOrderSell++;
	}
	m_setYBSell.clear();
	
	m_mapYBBuyList.Clear();
	m_mapYBSellList.Clear();
}

//-------------------------------------------------------------------------------------------------------
// 创建元宝交易账户
//-------------------------------------------------------------------------------------------------------
tagYuanBaoAccount* CTradeYB::CreateTradeAccount(DWORD dwRoleID)
{
	tagYuanBaoAccount *pYBAccount = new tagYuanBaoAccount(dwRoleID);
	if(!P_VALID(pYBAccount))
		return (tagYuanBaoAccount*)GT_INVALID;

	m_mapYBAccount.Add(dwRoleID, pYBAccount);

	// insert to db
	tagNDBC_InsertAccount	sendDB;
	sendDB.dwRoleID = dwRoleID;
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	return pYBAccount;
}

//-------------------------------------------------------------------------------------------------------
// 创建元宝交易账户
//-------------------------------------------------------------------------------------------------------
tagYuanBaoAccount* CTradeYB::CreateTradeAccount(const tagYBAccount *pYBAccount)
{
	tagYuanBaoAccount *pYuanBaoAccount = new tagYuanBaoAccount(pYBAccount->dwRoleID, pYBAccount->nYuanBao,
						pYBAccount->nGold, pYBAccount->nSilver, pYBAccount->bSellOrder, pYBAccount->bBuyOrder);
	if(!P_VALID(pYuanBaoAccount))
		return (tagYuanBaoAccount*)GT_INVALID;

	m_mapYBAccount.Add(pYBAccount->dwRoleID, pYuanBaoAccount);

	return pYuanBaoAccount;
}

//-------------------------------------------------------------------------------------------------------
// 计算元宝寄售列表
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::CalYBSellList()
{
	tagYuanBaoOrder* pYBOrder = NULL;

	set<tagYuanBaoOrder*, SellOrderCmp>::iterator itOrderSell = m_setYBSell.begin();
	while( itOrderSell != m_setYBSell.end())
	{
		pYBOrder = *itOrderSell;
		InserYBSellList(pYBOrder->nPrice, pYBOrder->GetRemainNum());
		++itOrderSell;
	}
}

//-------------------------------------------------------------------------------------------------------
// 计算元宝收购列表
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::CalYBBuyList()
{
	tagYuanBaoOrder* pYBOrder = NULL;

	set<tagYuanBaoOrder*, BuyOrderCmp>::iterator itOrderBuy = m_setYBBuy.begin();
	while( itOrderBuy != m_setYBBuy.end())
	{
		pYBOrder = *itOrderBuy;
		InserYBBuyList(pYBOrder->nPrice, pYBOrder->GetRemainNum());
		++itOrderBuy;
	}
}

//-------------------------------------------------------------------------------------------------------
// 插入元宝寄售列表
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::InserYBSellList(INT nPrice, INT nNum)
{
	m_mapYBSellList.ModifyValue(nPrice, nNum);
}

//-------------------------------------------------------------------------------------------------------
// 插入元宝收购列表
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::InserYBBuyList(INT nPrice, INT nNum)
{
	m_mapYBBuyList.ModifyValue(nPrice, nNum);
}

//-------------------------------------------------------------------------------------------------------
// 删除元宝寄售列表相同价格的订单
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::RemoveYBSellList(INT nPrice, INT nNum, DWORD dwRoleID, BOOL bNoticeClient)
{
	m_mapYBSellList.ModifyValue(nPrice, -nNum);
	INT		nTotalNum = m_mapYBSellList.Peek(nPrice);

	if(nTotalNum == 0)
	{
		m_mapYBSellList.Erase(nPrice);
	}

	if(nTotalNum < 0)
	{
		ILOG->Write(_T("YuanBao Num in m_mapYBSellList < 0  %u\r\n"), nPrice);
	}

	// 同步客户端
	if(bNoticeClient == TRUE)
	{
		Role *pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
		if(P_VALID(pRole))
		{
			tagNS_SynSellPriceList	send;
			send.nPrice = nPrice;
			send.nNum = nTotalNum;
			pRole->SendMessage(&send, send.dwSize);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 删除元宝收购列表相同价格的订单(dwRoleID, 需要同步的玩家ID）
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::RemoveYBBuyList(INT nPrice, INT nNum, DWORD dwRoleID, BOOL bNoticeClient)
{
	m_mapYBBuyList.ModifyValue(nPrice, -nNum);
	INT		nTotalNum = m_mapYBBuyList.Peek(nPrice);

	if(nTotalNum == 0)
	{
		m_mapYBBuyList.Erase(nPrice);
	}

	if(nTotalNum < 0)
	{
		ILOG->Write(_T("YuanBao Num in m_mapYBBuyList < 0  %u\r\n"), nPrice);
	}

	// 同步客户端
	if(bNoticeClient == TRUE)
	{
		Role *pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
		if(P_VALID(pRole))
		{
			tagNS_SynBuyPriceList	send;
			send.nPrice = nPrice;
			send.nNum = nTotalNum;
			pRole->SendMessage(&send, send.dwSize);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 创建元宝交易订单
//-------------------------------------------------------------------------------------------------------
tagYuanBaoOrder* CTradeYB::CreateYBOrder(DWORD dwRoleID, EYBOTYPE eYBOType, INT nPrice, INT nNum)
{
	tagYuanBaoOrder* pYBOrder = new tagYuanBaoOrder;
	if(!P_VALID(pYBOrder))		
		return (tagYuanBaoOrder*)GT_INVALID;

	if(eYBOType != 	EYBOT_BUY && eYBOType != EYBOT_SELL)
		return (tagYuanBaoOrder*)GT_INVALID;

	pYBOrder->dwID = m_dwMaxOrderID;
	pYBOrder->dwRoleID = dwRoleID;
	pYBOrder->eYBOType = eYBOType;
	pYBOrder->nPrice = nPrice;
	pYBOrder->nNum = nNum;
	pYBOrder->eYBOMode = EYBOM_Submit;
	pYBOrder->dwStartTime = g_world.GetWorldTime();

	if(eYBOType == 	EYBOT_BUY)
	{
		m_setYBBuy.insert(pYBOrder);
		InserYBBuyList(nPrice, nNum);
	}
	else
	{
		m_setYBSell.insert(pYBOrder);
		InserYBSellList(nPrice, nNum);
	}

	// insert to db
	tagNDBC_InsertOrder		sendDB;
	sendDB.YBOrder.dwID = pYBOrder->dwID;
	sendDB.YBOrder.dwRoleID = pYBOrder->dwRoleID;
	sendDB.YBOrder.nType = pYBOrder->eYBOType;
	sendDB.YBOrder.nPrice = pYBOrder->nPrice;
	sendDB.YBOrder.nNum = pYBOrder->nNum;
	sendDB.YBOrder.nYBMode = pYBOrder->eYBOMode;
	sendDB.YBOrder.dwStartTime = pYBOrder->dwStartTime;
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	// 加入订单ID与玩家ID的对应索引
	m_mapOrder2Role.Add(m_dwMaxOrderID, dwRoleID);
	// 订单索引加一
	++m_dwMaxOrderID;

	return pYBOrder;
}

//-------------------------------------------------------------------------------------------------------
// 加载元宝交易订单
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::LoadYOOrder(tagYuanBaoOrder* pYuanBaoOrder)
{
	if(!P_VALID(pYuanBaoOrder))
		return;

	tagYuanBaoOrder* pYBOrder = new tagYuanBaoOrder;
	if(!P_VALID(pYBOrder))		
		return;

	*pYBOrder = *pYuanBaoOrder;

	if(pYBOrder->eYBOType == EYBOT_BUY)
	{
		m_setYBBuy.insert(pYBOrder);
		InserYBBuyList(pYBOrder->nPrice, pYBOrder->GetRemainNum());
	}
	else
	{
		m_setYBSell.insert(pYBOrder);
		InserYBSellList(pYBOrder->nPrice, pYBOrder->GetRemainNum());
	}

	// 加入订单ID与玩家ID的对应索引
	m_mapOrder2Role.Add(pYBOrder->dwID, pYBOrder->dwRoleID);

	// 得到最大订单索引
	//if(pYBOrder->dwID > m_dwMaxOrderID)
	//	m_dwMaxOrderID = pYBOrder->dwID +1;
}

//-------------------------------------------------------------------------------------------------------
// 撤销或关闭元宝交易订单
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::DeleteYBOrder(tagYuanBaoOrder* pYBOrder, EYBOMODE eYBOMode)
{
	if(eYBOMode != 	EYBOM_Cancel && eYBOMode != EYBOM_Close)
		return;

	BOOL bNoticeClient = FALSE;
	if(EYBOM_Cancel == eYBOMode)
		bNoticeClient = TRUE;

	// 得到玩家交易账户
	tagYuanBaoAccount* pYBAccount = m_mapYBAccount.Peek(pYBOrder->dwRoleID);
	if(!P_VALID(pYBAccount))
		return;

	if(pYBOrder->eYBOType == EYBOT_BUY)
	{
		pYBOrder->eYBOMode = eYBOMode;
		pYBOrder->dwEndTime = g_world.GetWorldTime();

		RemoveYBBuyList(pYBOrder->nPrice, pYBOrder->GetRemainNum(), pYBOrder->dwRoleID, bNoticeClient);
		m_setYBBuy.erase(pYBOrder);
		pYBAccount->SetBuyOrder(FALSE);
	}
	else
	{
		pYBOrder->eYBOMode = eYBOMode;
		pYBOrder->dwEndTime = g_world.GetWorldTime();

		RemoveYBSellList(pYBOrder->nPrice, pYBOrder->GetRemainNum(), pYBOrder->dwRoleID, bNoticeClient);
		m_setYBSell.erase(pYBOrder);
		pYBAccount->SetSellOrder(FALSE);
	}

	// Update to db
	tagNDBC_CompleteOrder	sendDB;
	sendDB.CompleteOrder.dwID = pYBOrder->dwID;
	sendDB.CompleteOrder.nYBOMode = eYBOMode;
	sendDB.CompleteOrder.dwEndTime = pYBOrder->dwEndTime;
	g_dbSession.Send(&sendDB, sendDB.dwSize);

	// 删除订单ID与玩家ID的对应索引
	m_mapOrder2Role.Erase(pYBOrder->dwID);
	SAFE_DEL(pYBOrder);
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易订单(返回订单剩余元宝数）
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::RefreshYBOrder(tagYuanBaoOrder* pYBOrder, DWORD dwRoleID, INT nPrice, INT nNum, BOOL bNoticeClient)
{
	if(nNum == 0)
		return;

	if(!P_VALID(pYBOrder))
		return;

	tagYuanBaoAccount* pYBAccount = m_mapYBAccount.Peek(pYBOrder->dwRoleID);
	if(!P_VALID(pYBAccount))
		return;

	if(pYBOrder->eYBOType == EYBOT_BUY)
	{
		pYBOrder->nAvgPrice = (pYBOrder->nAvgPrice * pYBOrder->nDealNum + nPrice * nNum) / (pYBOrder->nDealNum + nNum);
		pYBOrder->nDealNum += nNum;
		
		// 更新玩家元宝交易账户
		pYBAccount->IncAccountYB(nNum, GT_INVALID, bNoticeClient);
		pYBAccount->DecAccountSilver(nPrice*nNum, GT_INVALID, bNoticeClient);

		// 删除元宝收购列表相同价格的订单
		RemoveYBBuyList(pYBOrder->nPrice, nNum, dwRoleID, TRUE);
	}
	else
	{
		pYBOrder->nAvgPrice = (pYBOrder->nAvgPrice * pYBOrder->nDealNum + nPrice * nNum) / (pYBOrder->nDealNum + nNum);
		pYBOrder->nDealNum += nNum;

		// 更新玩家元宝交易账户
		pYBAccount->DecAccountYuanBao(nNum, GT_INVALID, bNoticeClient);
		pYBAccount->IncAccountSilver(nPrice*nNum, GT_INVALID, bNoticeClient);

		// 删除元宝寄售列表相同价格的订单
		RemoveYBSellList(pYBOrder->nPrice, nNum, dwRoleID, TRUE);
	}
	
	// Update to db
	tagNDBC_RefreshOrder	sendDB;
	sendDB.RefreshOrder.dwID = pYBOrder->dwID;
	sendDB.RefreshOrder.nAvgPrice = pYBOrder->nAvgPrice;
	sendDB.RefreshOrder.nDealNum = pYBOrder->nDealNum;
	g_dbSession.Send(&sendDB, sendDB.dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 寄售元宝
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::DealYBSell(tagYuanBaoOrder* pYBSellOrder)
{
	BOOL	bNoticeClient = FALSE;	

	// 遍历收购订单，寻找匹配的订单
	tagYuanBaoOrder* pYBBuyOrder = (tagYuanBaoOrder*)GT_INVALID;
	std::set<tagYuanBaoOrder*, BuyOrderCmp>::iterator itBuyOrder = m_setYBBuy.begin();
	while (itBuyOrder != m_setYBBuy.end())
	{
		pYBBuyOrder = (*itBuyOrder);
		++itBuyOrder;

		// 交易订单相互匹配
		if (pYBBuyOrder->nPrice >= pYBSellOrder->nPrice && pYBBuyOrder->dwRoleID != pYBSellOrder->dwRoleID)
		{
			//收购元宝的
			tagYuanBaoAccount* pYBAccount = m_mapYBAccount.Peek(pYBBuyOrder->dwRoleID);
			if(!P_VALID(pYBAccount))
				return;

			INT	nRemainNum = 0;
			nRemainNum = pYBSellOrder->GetRemainNum() >= pYBBuyOrder->GetRemainNum() ? pYBBuyOrder->GetRemainNum():pYBSellOrder->GetRemainNum();
			
			//如果玩家元宝账户里的银不够交易的
			if(pYBSellOrder->nPrice*nRemainNum > pYBAccount->GetAccountSilver())
				continue;

			bNoticeClient = TRUE;

			if(pYBSellOrder->GetRemainNum() > pYBBuyOrder->GetRemainNum())
			{
				RefreshYBOrder(pYBBuyOrder, pYBSellOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				RefreshYBOrder(pYBSellOrder, pYBSellOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				DeleteYBOrder(pYBBuyOrder, EYBOM_Close);
			}
			else if(pYBSellOrder->GetRemainNum() < pYBBuyOrder->GetRemainNum())
			{
				RefreshYBOrder(pYBBuyOrder, pYBSellOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				RefreshYBOrder(pYBSellOrder, pYBSellOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				DeleteYBOrder(pYBSellOrder, EYBOM_Close);
				break;
			}
			else
			{
				RefreshYBOrder(pYBBuyOrder, pYBSellOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				RefreshYBOrder(pYBSellOrder, pYBSellOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				DeleteYBOrder(pYBBuyOrder, EYBOM_Close);
				DeleteYBOrder(pYBSellOrder, EYBOM_Close);
				break;
			}
		}
	}

	// 没有匹配的订单时，同步一下价格列表
	if(bNoticeClient == FALSE)
	{
		INT		nTotalNum = m_mapYBSellList.Peek(pYBSellOrder->nPrice);
		Role	*pRole = g_roleMgr.GetRolePtrByID(pYBSellOrder->dwRoleID);

		if(P_VALID(pRole))
		{
			tagNS_SynSellPriceList	send;
			send.nPrice = pYBSellOrder->nPrice;
			send.nNum = nTotalNum;
			pRole->SendMessage(&send, send.dwSize);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 收购元宝
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::DealYBBuy(tagYuanBaoOrder* pYBBuyOrder)
{
	// 遍历寄售订单，寻找匹配的订单
	BOOL	bNoticeClient = FALSE;

	tagYuanBaoOrder* pYBSellOrder = (tagYuanBaoOrder*)GT_INVALID;
	std::set<tagYuanBaoOrder*, SellOrderCmp>::iterator itSellOrder = m_setYBSell.begin();
	while (itSellOrder != m_setYBSell.end())
	{
		pYBSellOrder = (*itSellOrder);
		++itSellOrder;

		// 交易订单相互匹配
		if(pYBBuyOrder->nPrice >= pYBSellOrder->nPrice && pYBBuyOrder->dwRoleID != pYBSellOrder->dwRoleID)
		{
			//收购元宝的
			tagYuanBaoAccount* pYBAccount = m_mapYBAccount.Peek(pYBBuyOrder->dwRoleID);
			if(!P_VALID(pYBAccount))
				return;

			INT	nRemainNum = 0;
			nRemainNum = pYBSellOrder->GetRemainNum() >= pYBBuyOrder->GetRemainNum() ? pYBBuyOrder->GetRemainNum():pYBSellOrder->GetRemainNum();

			//如果玩家元宝账户里的银不够交易的
			if(pYBSellOrder->nPrice*nRemainNum > pYBAccount->GetAccountSilver())
				continue;

			bNoticeClient = TRUE;

			if(pYBBuyOrder->GetRemainNum() > pYBSellOrder->GetRemainNum())
			{
				RefreshYBOrder(pYBSellOrder, pYBBuyOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				RefreshYBOrder(pYBBuyOrder, pYBBuyOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				DeleteYBOrder(pYBSellOrder, EYBOM_Close);
			}
			else if(pYBBuyOrder->GetRemainNum() < pYBSellOrder->GetRemainNum())
			{
				RefreshYBOrder(pYBSellOrder, pYBBuyOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				RefreshYBOrder(pYBBuyOrder, pYBBuyOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				DeleteYBOrder(pYBBuyOrder, EYBOM_Close);
				break;
			}
			else
			{
				RefreshYBOrder(pYBSellOrder, pYBBuyOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				RefreshYBOrder(pYBBuyOrder, pYBBuyOrder->dwRoleID, pYBSellOrder->nPrice, nRemainNum, TRUE);
				DeleteYBOrder(pYBBuyOrder, EYBOM_Close);
				DeleteYBOrder(pYBSellOrder, EYBOM_Close);
				break;
			}
		}
		else
			break;
	}

	// 没有匹配的订单时，同步一下价格列表
	if(bNoticeClient == FALSE)
	{
		INT		nTotalNum = m_mapYBBuyList.Peek(pYBBuyOrder->nPrice);
		Role	*pRole = g_roleMgr.GetRolePtrByID(pYBBuyOrder->dwRoleID);

		if(P_VALID(pRole))
		{
			tagNS_SynBuyPriceList	send;
			send.nPrice = pYBBuyOrder->nPrice;
			send.nNum = nTotalNum;
			pRole->SendMessage(&send, send.dwSize);
		}
	}
}

//-------------------------------------------------------------------------------------------------------
// 同步客户端整个出售的价格列表
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::SynSellPriceList(Role *pRole)
{
	INT		nListNum = m_mapYBSellList.Size();
	DWORD	dwSize = sizeof(tagNS_GetSellPriceList);
	INT		nIndex = 0;
	INT		nPrice = 0;
	INT		nNum = 0;

	if(!P_VALID(pRole))
		return;

	if(nListNum >0)
	{
		dwSize += (nListNum - 1) * sizeof(tagYBPriceList);
	}

	MCREATE_MSG(pSend, dwSize, NS_GetSellPriceList);

	m_mapYBSellList.ResetIterator();
	while(m_mapYBSellList.PeekNext(nPrice, nNum))
	{
		pSend->YBPriceList[nIndex].nPrice = nPrice;
		pSend->YBPriceList[nIndex].nNum = nNum;
		++nIndex;
	}

	pSend->nListNum = nListNum;
	pRole->SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

//-------------------------------------------------------------------------------------------------------
// 同步客户端整个收购的价格列表
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::SynBuyPriceList(Role *pRole)
{
	INT		nListNum = m_mapYBBuyList.Size();
	DWORD	dwSize = sizeof(tagNS_GetBuyPriceList);
	INT		nIndex = 0;
	INT		nPrice = 0;
	INT		nNum = 0;

	if(!P_VALID(pRole))
		return;

	if(nListNum > 0)
	{
		dwSize += (nListNum - 1) * sizeof(tagYBPriceList);
	}

	MCREATE_MSG(pSend, dwSize, NS_GetBuyPriceList);

	m_mapYBBuyList.ResetIterator();
	while(m_mapYBBuyList.PeekNext(nPrice, nNum))
	{
		pSend->YBPriceList[nIndex].nPrice = nPrice;
		pSend->YBPriceList[nIndex].nNum = nNum;
		++nIndex;
	}

	pSend->nListNum = nListNum;
	pRole->SendMessage(pSend, pSend->dwSize);

	MDEL_MSG(pSend);
}

//-------------------------------------------------------------------------------------------------------
// 同步客户端元宝交易账户
//-------------------------------------------------------------------------------------------------------
VOID CTradeYB::SynYBAccount(Role *pRole)
{
	if(!P_VALID(pRole))
		return;

	tagYuanBaoAccount *pYBAccount = m_mapYBAccount.Peek(pRole->GetID());
	if(!P_VALID(pYBAccount))
		return;

	tagNS_GetYBAccount	send;
	send.dwRoleID = pYBAccount->dwRoleID;
	send.nYuanBao = pYBAccount->GetAccountYB();
	send.nSilver = pYBAccount->GetAccountSilver();
	send.bSellOrder = pYBAccount->bSellOrder;
	send.bBuyOrder = pYBAccount->bBuyOrder;
	pRole->SendMessage(&send, send.dwSize);
};

