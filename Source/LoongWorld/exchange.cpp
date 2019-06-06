//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: exchange.cpp
// author: Sxg
// actor:
// data: 2008-09-02
// last:
// brief: 玩家间交易数据管理
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "exchange.h"
#include "../WorldDefine/msg_exchange.h"

//-----------------------------------------------------------------------------
// 构造&析构函数
//-----------------------------------------------------------------------------
ExchangeMgr::ExchangeMgr()
{
	m_pExData		= NULL;
	m_dwTgtRoleID	= GT_INVALID;
}

ExchangeMgr::~ExchangeMgr()
{
	SAFE_DEL(m_pExData);
}

//-----------------------------------------------------------------------------
// 创建交易空间 -- 如果已存在，则重新初始化
//-----------------------------------------------------------------------------
VOID ExchangeMgr::CreateData()
{
	if(NULL == m_pExData)
	{
		m_pExData = new tagExchangeData;
	}

	ASSERT(m_pExData != NULL);

	ZeroMemory(m_pExData, sizeof(tagExchangeData));
}

//-----------------------------------------------------------------------------
// 向交易数据结构中添加交易物品 -- 返回 -- 成功:插入位置下标；失败:GT_INVALID
//-----------------------------------------------------------------------------
INT32 ExchangeMgr::AddItem(INT64 n64Serial, INT16 n16Num)
{
	ASSERT(m_pExData != NULL);

	INT32 nInsIndex = GT_INVALID;
	for(INT32 i=0; i<MAX_EXCHANGE_ITEM; ++i)
	{
		if(m_pExData->n64Serial[i] == n64Serial)
		{
			return GT_INVALID;
		}
		else if(0 == m_pExData->n64Serial[i] && GT_INVALID == nInsIndex)
		{
			nInsIndex = i;
		}
	}

	if(nInsIndex != GT_INVALID)
	{
		m_pExData->n64Serial[nInsIndex] = n64Serial;
		m_pExData->n16ItemNum[nInsIndex] = n16Num;

		++m_pExData->byTypeNum;
	}

	return nInsIndex;
}

//-----------------------------------------------------------------------------
// 从交易物品中取出指定物品 -- 成功返回E_Success，失败返回相应错误码
//-----------------------------------------------------------------------------
DWORD ExchangeMgr::DecItem(INT64 n64Serial)
{
	ASSERT(m_pExData != NULL);

	for(INT32 i=0; i<MAX_EXCHANGE_ITEM; ++i)
	{
		if(m_pExData->n64Serial[i] == n64Serial)
		{
			m_pExData->n64Serial[i] = 0;
			--m_pExData->byTypeNum;
			return E_Success;
		}
	}

	return E_Exchange_ItemCanNot_Find;
}