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
#pragma pack(push, 1)
//-----------------------------------------------------------------------------
// 玩家间交易服务器端数据结构
//-----------------------------------------------------------------------------
struct tagPetExchangeData
{
	BOOL	bLock;			// 玩家是否确认交换
	BOOL	bVerify;		// 玩家是否再次确认交易
	BYTE	byPetNum;		// 交易物品种类个数

	INT64	n64Money;		// 交易的金钱
	DWORD	dwPetIDs[MAX_EXCHANGE_ITEM];	// 交易的物品，0表示没有交易物品
};
#pragma pack(pop)

//-----------------------------------------------------------------------------
// 玩家间交易管理类
//-----------------------------------------------------------------------------
class PetExchangeMgr
{
public:
	PetExchangeMgr();
	~PetExchangeMgr();

public:
	VOID CreateData();
	VOID DeleteData();

	INT32	AddPet(DWORD dwPetID);	// 返回 -- 成功:插入位置下标；失败:GT_INVALID
	DWORD	DecPet(DWORD dwPetID);	// 返回错误码
	VOID	ResetMoney(INT64 n64Money);

	VOID	Lock();
	VOID	Unlock();
	VOID	Verify();

	VOID	SetTgtRoleID(const DWORD dwTgtRoleID);

public:
	DWORD	GetTgtRoleID() const;
	INT64	GetMoney() const;
	BYTE	GetPetNum() const;
	DWORD*	GetPetIDs() const;
	BOOL	IsLock() const;
	BOOL	IsVerify() const;

private:
	tagPetExchangeData*	m_pExData;
	DWORD				m_dwTgtRoleID;	// 目标玩家，GT_INVALID表示无交易目标
};



/*********************** 玩家间交易管理类中内联函数实现 *******************************/

inline VOID PetExchangeMgr::DeleteData()	{ SAFE_DEL(m_pExData); }

//-----------------------------------------------------------------------------
// 设置交易目标
//-----------------------------------------------------------------------------
inline VOID	PetExchangeMgr::SetTgtRoleID(const DWORD dwTgtRoleID)
{
	m_dwTgtRoleID = dwTgtRoleID;
}

//-----------------------------------------------------------------------------
// 获取交易数据内容
//-----------------------------------------------------------------------------
inline DWORD PetExchangeMgr::GetTgtRoleID() const
{
	return m_dwTgtRoleID;
}

inline INT64 PetExchangeMgr::GetMoney() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->n64Money;
}

inline BYTE  PetExchangeMgr::GetPetNum() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->byPetNum;
}

inline DWORD* PetExchangeMgr::GetPetIDs() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->dwPetIDs;
}

inline BOOL PetExchangeMgr::IsLock() const
{ 
	ASSERT(m_pExData != NULL);
	return m_pExData->bLock;
}

inline BOOL PetExchangeMgr::IsVerify() const
{
	ASSERT(m_pExData != NULL);
	return m_pExData->bVerify;
}

//-----------------------------------------------------------------------------
// 修改交易数据内容
//-----------------------------------------------------------------------------
inline VOID PetExchangeMgr::ResetMoney(INT64 n64Money)	
{ 
	ASSERT(m_pExData != NULL); 
	m_pExData->n64Money = n64Money; 
}


inline VOID PetExchangeMgr::Lock()
{ 
	ASSERT(m_pExData != NULL); 
	m_pExData->bLock = TRUE; 
}

inline VOID PetExchangeMgr::Unlock()
{ 
	ASSERT(m_pExData != NULL); 
	m_pExData->bLock = FALSE; 
}

inline VOID	PetExchangeMgr::Verify()
{
	ASSERT(m_pExData != NULL); 
	m_pExData->bVerify = TRUE; 
}