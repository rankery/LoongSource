//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: exchange_define.h
// author: Sxg
// actor:
// data: 2008-08-25
// last:
// brief: 玩家间交易
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
const INT32 MAX_EXCHANGE_ITEM			= 12;	// 一次最多交换12件物品//??
const INT32 MAX_EXCHANGE_MONEY			= 0;	// 0
const INT32 MAX_EXCHANGE_DISTANCE		= 100;


//-----------------------------------------------------------------------------
struct tagExchangeData
{
	DWORD	dwRoleID;		// 目标玩家，GT_INVALID表示无交易目标
	BOOL	bLock;			// 玩家是否确认交换
	BOOL	bVerify;		// 玩家是否再次确认交易
	BYTE	byTypeNum;		// 交易物品种类个数
	BOOL	bDummy[2];		// 对齐用	
	INT64	n64Money;		// 交易的金钱
	INT64	n64Serial[MAX_EXCHANGE_ITEM];	// 交易的物品，0表示没有交易物品
	INT16	n16ItemNum[MAX_EXCHANGE_ITEM];	// 待交易物品的个数

	tagExchangeData(DWORD dwTgtRoleID) { ZeroMemory(this, sizeof(tagExchangeData)); dwRoleID = dwTgtRoleID;}
};
