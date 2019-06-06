//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_commodity.cpp
// author: Sunnee
// actor:
// data: 2009-07-03
// last:
// brief: 跑商任务用容器
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#include "../ServerDefine/msg_guild.h"
#include "../WorldDefine/msg_guild.h"

#include "att_res.h"
#include "db_session.h"
#include "guild_commodity.h"
#include "role_mgr.h"
#include "role.h"

//------------------------------------------------------------------------------
// 构造与析构
//------------------------------------------------------------------------------
GuildCommodity::GuildCommodity()
{
	m_nCurTael	= 0;
	m_mapGoods.Clear();
}

GuildCommodity::~GuildCommodity()
{
	Destory();
}

//------------------------------------------------------------------------------
// 初始化和销毁
//------------------------------------------------------------------------------
BOOL GuildCommodity::Init( DWORD dwRoleID, INT nLevel, const tagTaelInfo* pTaelInfo /*= NULL*/, const tagRedoundInfo* pRedoundInfo /*= NULL*/ )
{
	if (!GT_VALID(dwRoleID))
	{
		return FALSE;
	}

	m_dwOwnerID	= dwRoleID;
	m_nLevel	= nLevel;
	m_mapGoods.Clear();

	if (P_VALID(pTaelInfo) && P_VALID(pRedoundInfo))
	{
		m_sRedoundInfo	= *pRedoundInfo;
		m_sTaelInfo		= *pTaelInfo;
		m_nCurTael		= pTaelInfo->nBeginningTael;
	}

	return TRUE;
}

VOID GuildCommodity::Destory()
{
	tagCommerceGoodInfo* pInfo	= NULL;
	MapGoods::TMapIterator iter	= m_mapGoods.Begin();
	while (m_mapGoods.PeekNext(iter, pInfo))
	{
		SAFE_DEL(pInfo);
	}
	m_mapGoods.Clear();
}

//------------------------------------------------------------------------------
// 从数据库中载入跑商信息
//------------------------------------------------------------------------------
DWORD GuildCommodity::LoadCommodityInfo( tagGuildCommerceInfo* pLoadInfo )
{
	if (!P_VALID(pLoadInfo))
	{
		return GT_INVALID;
	}

	// 所属
	m_dwOwnerID		= pLoadInfo->dwRoleID;

	// 静态属性
	const tagCommerceInfo* pInfo = g_attRes.GetGuildCommerceInfo(pLoadInfo->nLevel);
	if (!P_VALID(pInfo))
	{
		return GT_INVALID;
	}
	m_nLevel		= pLoadInfo->nLevel;
	m_sTaelInfo		= pInfo->sTaelInfo;
	m_sRedoundInfo	= pInfo->sRedoundInfo;

	// 商银
	m_nCurTael		= pLoadInfo->nTael;

	// 商货
	m_mapGoods.Clear();
	for (int n=0; n<MAX_COMMODITY_CAPACITY; n++)
	{
		if (!pLoadInfo->sGoodInfo[n].IsValid())
		{
			break;
		}
		tagCommerceGoodInfo* pGoodInfo = new tagCommerceGoodInfo(pLoadInfo->sGoodInfo[n]);
		m_mapGoods.Add(pLoadInfo->sGoodInfo[n].dwGoodID, pGoodInfo);
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 当前商银操作
//------------------------------------------------------------------------------
BOOL GuildCommodity::IncTael( INT32 nTael )
{
	if (nTael <= 0 || m_nCurTael > m_sTaelInfo.nMaxTael)
	{
		return FALSE;
	}

	m_nCurTael += nTael;
	if (m_nCurTael > m_sTaelInfo.nMaxTael || m_nCurTael <= 0)	// 考虑到INT32溢出
	{
		m_nCurTael = m_sTaelInfo.nMaxTael;
	}

	// 通知数据库
	SaveTael2DB();

	// 通知客户端
	Role* pRole = g_roleMgr.GetRolePtrByID(m_dwOwnerID);
	if (P_VALID(pRole))
	{
		tagNS_GetCommodityInfo	send;
		send.nCurTael		= m_nCurTael;
		send.nLevel			= m_nLevel;
		send.nGoodNum		= 0;
		pRole->SendMessage(&send, send.dwSize);
	}

	return TRUE;
}

BOOL GuildCommodity::DecTael( INT32 nTael )
{
	if (nTael <= 0 || m_nCurTael <= 0)
	{
		return FALSE;
	}

	m_nCurTael -= nTael;
	if (m_nCurTael < 0)
	{
		m_nCurTael = 0;
	}

	// 通知数据库
	SaveTael2DB();

	// 通知客户端
	Role* pRole = g_roleMgr.GetRolePtrByID(m_dwOwnerID);
	if (P_VALID(pRole))
	{
		tagNS_GetCommodityInfo	send;
		send.nCurTael		= m_nCurTael;
		send.nLevel			= m_nLevel;
		send.nGoodNum		= 0;
		pRole->SendMessage(&send, send.dwSize);
	}

	return TRUE;
}

//------------------------------------------------------------------------------
// 容器物品操作
//------------------------------------------------------------------------------
BOOL GuildCommodity::AddGood( DWORD dwGoodID, BYTE byNum, INT32 nPrice /*= 0*/ )
{
	if (!GT_VALID(dwGoodID))
	{
		return FALSE;
	}

	tagCommerceGoodInfo* pGoodInfo = NULL;
	if (m_mapGoods.IsExist(dwGoodID))
	{
		// 取得当前该商货数量
		pGoodInfo = m_mapGoods.Peek(dwGoodID);
		if (!P_VALID(pGoodInfo))
		{
			return FALSE;
		}
		INT16 n16Num = pGoodInfo->byGoodNum + byNum;
		if (n16Num > MAX_COMMODITY_GOOD_NUM)
		{
			n16Num = MAX_COMMODITY_GOOD_NUM;
		}
		pGoodInfo->byGoodNum = (BYTE)n16Num;
	}
	else if (m_mapGoods.Size() >= MAX_COMMODITY_CAPACITY)
	{
		return FALSE;
	}
	else
	{
		// byNum不可能超过255，这里不做参数判断
		pGoodInfo = new tagCommerceGoodInfo;
		pGoodInfo->dwGoodID		= dwGoodID;
		pGoodInfo->byGoodNum	= byNum;
		pGoodInfo->nCost		= nPrice;
		m_mapGoods.Add(dwGoodID, pGoodInfo);
	}

	// 通知数据库
	SaveCommodity2DB();

	// 通知客户端
	Role* pRole = g_roleMgr.GetRolePtrByID(m_dwOwnerID);
	if (P_VALID(pRole))
	{
		tagNS_GetCommodityInfo	send;
		send.nCurTael		= m_nCurTael;
		send.nLevel			= m_nLevel;
		send.nGoodNum		= 1;
		send.sGoodInfo[0]	= *pGoodInfo;
		pRole->SendMessage(&send, send.dwSize);
	}

	return TRUE;
}

BOOL GuildCommodity::RemoveGood( DWORD dwGoodID, BYTE byNum, INT32 nPrice /*= 0*/ )
{
	if (!GT_VALID(dwGoodID) || !m_mapGoods.IsExist(dwGoodID))
	{
		return FALSE;
	}

	tagCommerceGoodInfo* pGoodInfo = m_mapGoods.Peek(dwGoodID);
	if (!P_VALID(pGoodInfo))
	{
		return FALSE;
	}

	BYTE byRemainNum = pGoodInfo->byGoodNum;
	if (byRemainNum < byNum)
	{
		return FALSE;
	}
	else
	{
		pGoodInfo->byGoodNum	= byRemainNum - byNum;
		pGoodInfo->nCost		= nPrice;
	}

	// 通知客户端
	Role* pRole = g_roleMgr.GetRolePtrByID(m_dwOwnerID);
	if (P_VALID(pRole))
	{
		tagNS_GetCommodityInfo	send;
		send.nCurTael		= m_nCurTael;
		send.nLevel			= m_nLevel;
		send.nGoodNum		= 1;
		send.sGoodInfo[0]	= *pGoodInfo;
		pRole->SendMessage(&send, send.dwSize);
	}

	if (pGoodInfo->byGoodNum == 0)
	{
		m_mapGoods.Erase(dwGoodID);
		SAFE_DEL(pGoodInfo);
	}

	// 通知数据库
	SaveCommodity2DB();

	return TRUE;
}

//------------------------------------------------------------------------------
// 获取容器中商货信息(包括商银)
//------------------------------------------------------------------------------
DWORD GuildCommodity::GetCommodityInfo( tagCommerceGoodInfo* pGoods, INT& nGoodNum, INT32& nTael, INT32& nLevel )
{
	// 上层保证参数容量
	if (!P_VALID(pGoods))
	{
		return GT_INVALID;
	}

	// 商银
	nTael	= m_nCurTael;
	nLevel	= m_nLevel;

	// 商货
	nGoodNum		= 0;

	DWORD dwGoodID	= GT_INVALID;
	tagCommerceGoodInfo* pGoodInfo = NULL;

	MapGoods::TMapIterator iter = m_mapGoods.Begin();
	while (m_mapGoods.PeekNext(iter, dwGoodID, pGoodInfo))
	{
		if (!P_VALID(pGoodInfo))
		{
			m_mapGoods.Erase(dwGoodID);
			continue;
		}

		if (!pGoodInfo->IsValid() || nGoodNum >= MAX_COMMODITY_CAPACITY)
		{
			m_mapGoods.Erase(dwGoodID);
			continue;
		}

		pGoods[nGoodNum++]	= *pGoodInfo;
	}

	return E_Success;
}

//------------------------------------------------------------------------------
// 数据库操作
//------------------------------------------------------------------------------
VOID GuildCommodity::SaveTael2DB()
{
	tagNDBC_ChangeTael send;
	send.dwRoleID	= m_dwOwnerID;
	send.nTael		= m_nCurTael;

	g_dbSession.Send(&send, send.dwSize);
}

VOID GuildCommodity::SaveCommodity2DB()
{
	tagNDBC_SaveCommodity send;
	send.dwRoleID	= m_dwOwnerID;

	DWORD dwGoodID					= GT_INVALID;
	tagCommerceGoodInfo* pGoodInfo	= NULL;
	INT nGoodType					= 0;

	MapGoods::TMapIterator iter = m_mapGoods.Begin();
	while (m_mapGoods.PeekNext(iter, dwGoodID, pGoodInfo))
	{
		if (!P_VALID(pGoodInfo))
		{
			m_mapGoods.Erase(dwGoodID);
			continue;
		}
		if (!pGoodInfo->IsValid() || nGoodType >= MAX_COMMODITY_CAPACITY)
		{
			m_mapGoods.Erase(dwGoodID);
			continue;
		}
		send.sGoodInfo[nGoodType++] = *pGoodInfo;
	}
	// 空位处理
	for (int n=nGoodType; n<MAX_COMMODITY_CAPACITY; n++)
	{
		send.sGoodInfo[n].dwGoodID	= GT_INVALID;
		send.sGoodInfo[n].byGoodNum	= 0;
		send.sGoodInfo[n].nCost		= 0;
	}

	g_dbSession.Send(&send, send.dwSize);
}

//------------------------------------------------------------------------------
// 是否可容纳该商货
//------------------------------------------------------------------------------
DWORD GuildCommodity::IsFull( DWORD dwGoodID, BYTE byNum )
{
	tagCommerceGoodInfo* pGoodInfo = m_mapGoods.Peek(dwGoodID);

	INT nResultNum = byNum;
	if (P_VALID(pGoodInfo))
	{
		nResultNum += pGoodInfo->byGoodNum;
		if (nResultNum > MAX_COMMODITY_GOOD_NUM)
		{
			return E_GuildCommodity_ItemMaxHold;
		}
	}
	else if (GetGoodsNum() >= MAX_COMMODITY_CAPACITY)
	{
		return E_GuildCommodity_NotEnough_Space;
	}
	else if (nResultNum > MAX_COMMODITY_GOOD_NUM)
	{
		return E_GuildCommodity_NotEnough_Space;
	}
	
	return E_Success;
}

//------------------------------------------------------------------------------
// 商货是否足够
//------------------------------------------------------------------------------
DWORD GuildCommodity::IsExist( DWORD dwGoodID, BYTE byNum )
{
	tagCommerceGoodInfo* pGoodInfo = m_mapGoods.Peek(dwGoodID);
	if (!P_VALID(pGoodInfo))
	{
		return E_CofC_ItemNotFind;
	}
	else if (pGoodInfo->byGoodNum < byNum)
	{
		return E_CofC_ItemNotEnough;
	}
	
	return E_Success;
}

//------------------------------------------------------------------------------
// 跑商利润
//------------------------------------------------------------------------------
INT32 GuildCommodity::GetGain()
{
	if (m_nCurTael > m_sTaelInfo.nBeginningTael)
	{
		return m_nCurTael - m_sTaelInfo.nBeginningTael;
	}

	return 0;
}

//------------------------------------------------------------------------------
// 死亡惩罚
//------------------------------------------------------------------------------
VOID GuildCommodity::DeadPenalty()
{
	// 商银惩罚
	DecTael((INT32)(m_nCurTael * 0.2f + 0.5f));

	// 商货惩罚

	// 统计需要惩罚的商货数量
	MapGoods::TMapIterator iter = m_mapGoods.Begin();
	INT32 nGoodNum = 0;
	tagCommerceGoodInfo* pGoodInfo = NULL;
	while (m_mapGoods.PeekNext(iter, pGoodInfo))
	{
		if (!P_VALID(pGoodInfo))
			continue;

		nGoodNum += pGoodInfo->byGoodNum;
	}

	nGoodNum = (INT32)(nGoodNum * 0.2f);

	// 随机选取商货扣除
	DWORD dwGoodID = GT_INVALID;
	while (nGoodNum > 0)
	{
		m_mapGoods.RandPeek(dwGoodID, pGoodInfo);
		if (!GT_VALID(dwGoodID) || !P_VALID(pGoodInfo))
		{
			continue;
		}

		// 随机算出扣除该商货的数量
		BYTE byNum = IUTIL->RandomInRange(1, min(pGoodInfo->byGoodNum, nGoodNum));

		// 扣除商货
		RemoveGood(pGoodInfo->dwGoodID, byNum);

		nGoodNum -= byNum;
	}
}