//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: exchange.h
// author: Sxg
// actor:
// data: 2008-09-02
// last:
// brief: 玩家间交易数据管理
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/exchange_define.h"
//-----------------------------------------------------------------------------
// 玩家间交易服务器端数据结构
//-----------------------------------------------------------------------------
struct tagExchangeData
{
	BOOL	bLock;			// 玩家是否确认交换
	BOOL	bVerify;		// 玩家是否再次确认交易
	BYTE	byTypeNum;		// 交易物品种类个数
	BOOL	bDummy;			// 对齐用	
	INT64	n64Money;		// 交易的金钱
	INT64	n64Serial[MAX_EXCHANGE_ITEM];	// 交易的物品，0表示没有交易物品
	INT16	n16ItemNum[MAX_EXCHANGE_ITEM];	// 待交易物品的个数
};

//-----------------------------------------------------------------------------
// 玩家间交易管理类
//-----------------------------------------------------------------------------
class ExchangeMgr
{
public:
	ExchangeMgr();
	~ExchangeMgr();

public:
	VOID CreateData();
	VOID DeleteData();

	INT32	AddItem(INT64 n64Serial, INT16 n16Num);	// 返回 -- 成功:插入位置下标；失败:GT_INVALID
	DWORD	DecItem(INT64 n64Serial);	// 返回错误码
	VOID	ResetMoney(INT64 n64Money);

	VOID	Lock();
	VOID	Unlock();
	VOID	Verify();

	VOID	SetTgtRoleID(const DWORD dwTgtRoleID);

public:
	DWORD	GetTgtRoleID() const;
	INT64	GetMoney() const;
	BYTE	GetItemTypeNum() const;
	INT64*	GetSerialArray() const;
	INT16*	GetNumArray() const;
	BOOL	IsLock() const;
	BOOL	IsVerify() const;

private:
	tagExchangeData*	m_pExData;
	DWORD				m_dwTgtRoleID;	// 目标玩家，GT_INVALID表示无交易目标
};



/*********************** 玩家间交易管理类中内联函数实现 *******************************/

inline VOID ExchangeMgr::DeleteData()	{ SAFE_DEL(m_pExData); }

//-----------------------------------------------------------------------------
// 设置交易目标
//-----------------------------------------------------------------------------
inline VOID	ExchangeMgr::SetTgtRoleID(const DWORD dwTgtRoleID)
{
	m_dwTgtRoleID = dwTgtRoleID;
}

//-----------------------------------------------------------------------------
// 获取交易数据内容
//-----------------------------------------------------------------------------
inline DWORD ExchangeMgr::GetTgtRoleID() const
{
	return m_dwTgtRoleID;
}

inline INT64 ExchangeMgr::GetMoney() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->n64Money;
}

inline BYTE  ExchangeMgr::GetItemTypeNum() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->byTypeNum;
}

inline INT64* ExchangeMgr::GetSerialArray() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->n64Serial;
}

inline INT16* ExchangeMgr::GetNumArray() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->n16ItemNum;
}

inline BOOL ExchangeMgr::IsLock() const
{ 
	ASSERT(m_pExData != NULL);
	return m_pExData->bLock;
}

inline BOOL ExchangeMgr::IsVerify() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->bVerify;
}

//-----------------------------------------------------------------------------
// 修改交易数据内容
//-----------------------------------------------------------------------------
inline VOID ExchangeMgr::ResetMoney(INT64 n64Money)	
{ 
	ASSERT(m_pExData != NULL); 
	m_pExData->n64Money = n64Money; 
}


inline VOID ExchangeMgr::Lock()
{ 
	ASSERT(m_pExData != NULL); 
	m_pExData->bLock = TRUE; 
}

inline VOID ExchangeMgr::Unlock()
{ 
	ASSERT(m_pExData != NULL); 
	m_pExData->bLock = FALSE; 
}

inline VOID	ExchangeMgr::Verify()
{
	ASSERT(m_pExData != NULL); 
	m_pExData->bVerify = TRUE; 
}