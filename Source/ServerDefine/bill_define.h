//-----------------------------------------------------------------------------
//!\file bill_define.h
//!\author Aslan
//!
//!\date 2009-06-22
//! last 2009-06-22
//!
//!\brief 直充划拨的相关事件定义
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 直充类型
//-----------------------------------------------------------------------------
enum EBillType
{
	EBBT_Null			=	-1,

	EBBT_Yuanbao		=	0,		// 直充元宝
	EBBT_Item			=	1,		// 直充物品

	EBBT_End			=	2,
};

//-----------------------------------------------------------------------------
// 划拨状态
//-----------------------------------------------------------------------------
enum EBillStateType
{
	EBST_Null			=	-1,

	EBST_Undeal			=	0,		// 未处理
	EBST_Success		=	1,		// 处理成功

	EBST_End			=	2,
};

#define MIBAO_LEN			240		// 密保卡程度
#define BILLINFO_PERTICK	10		// 每次最多处理划拨信息

//-----------------------------------------------------------------------------
// 划拨信息结构
//-----------------------------------------------------------------------------
struct tagBillInfo
{
	DWORD		dwTokenID;			// 令牌ID
	EBillType	eBillType;			// 直冲类型
	DWORD		dwTypeID;			// 物品ID(直冲元宝时无效)
	INT32		nNum;				// 数量
	CHAR		szAccountName[X_SHORT_NAME];// 账号名

	tagBillInfo()
	{
		Reset();
	}

	VOID Reset()
	{
		dwTokenID	= GT_INVALID;
		eBillType	= EBBT_Null;
		dwTypeID	= GT_INVALID;
		nNum		= 0;
		ZeroMemory(szAccountName, sizeof(szAccountName));
	}
};
