//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_exchange.h
// author: 
// actor:
// data: 2008-08-21
// last:
// brief: 玩家间交易消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"
#include "ItemDefine.h"

#pragma pack(push, 1)
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
	E_Exchange_Accept				= E_Success,	// 对方同意交易
	E_Exchange_Refuse				= 1,	// 对方拒绝		
	E_Exchange_NotEnough_BagSpace	= 2,	// 背包空间不足
	E_Exchange_RoleBusy				= 3,	// 被申请方忙	
	E_Exchange_WithMore				= 4,	// 不能和多人同时交易
	E_Exchange_OutOfRange			= 5,	// 交易距离范围之外
	E_Exchange_NotInSame_Map		= 6,	// 不在同一区域
	E_Exchange_ItemCanNot_Exchange	= 7,	// 物品不能交易
	E_Exchange_ItemCanNot_Find		= 8,	// 没有找到指定数量的物品
	E_Exchange_NotEnough_Money		= 9,	// 交易的金钱不足
	E_Exchange_ApplicantTgt_Change	= 10,	// 交易申请的交易目标发生变化
	E_Exchange_NotAnswer			= 11,	// 对方没有响应
	E_Exchange_SelfBusy				= 12,	// 玩家自己忙
};




//----------------------------------------------------------------------------
// 消息结构
//----------------------------------------------------------------------------
CMD_START(NC_ExchangeReq)		// 玩家请求交换物品
	DWORD	dwDstID;	// 消息目标ID
CMD_END;

CMD_START(NS_ExchangeReq)		// 服务器转发给目标玩家
	DWORD	dwSrcID;	// 消息发起者ID
CMD_END;

CMD_START(NC_ExchangeReqRes)	// 目标玩家交易请求反馈
	DWORD	dwErrorCode;
	DWORD	dwDstID;
CMD_END;

CMD_START(NS_ExchangeReqRes)	// 服务器转发给目标玩家交易请求反馈
	DWORD	dwErrorCode;
	DWORD	dwSrcID;
	DWORD	dwDstID;
CMD_END;


CMD_START(NC_ExchangeAdd)		// 添加交易的物品装备
//	DWORD	dwDstID;
	INT64	n64Serial;	// 只需要64位序号,服务器负责初始化物品具体信息
CMD_END;

CMD_START(NS_ExchangeAdd_2Src)
	DWORD	dwErrorCode;
	DWORD	dwDstID;
	INT64	n64Serial;
	INT32	nIndex;
CMD_END;

CMD_START(NS_ExchangeAdd_2Dst)
	DWORD	dwSrcID;
	INT32	nIndex;
	BYTE	byData[SIZE_EQUIP];	// 物品或装备(需要通知对方装备的详细信息)
CMD_END;

CMD_START(NC_ExchangeDec)		// 取消交易的物品装备
//	DWORD	dwDstID;
	INT64	n64Serial;
CMD_END;

CMD_START(NS_ExchangeDec)
	DWORD	dwErrorCode;
	DWORD	dwSrcID;
	DWORD	dwDstID;
	INT64	n64Serial;
CMD_END;

CMD_START(NC_ExchangeMoney)		// 金钱发生改变 
//	DWORD	dwDstID;
	INT64	n64Money;
CMD_END

CMD_START(NS_ExchangeMoney)
	DWORD	dwErrorCode;	
	INT64	n64Money;
	DWORD	dwSrcID;
CMD_END


CMD_START(NC_ExchangeLock)		// 锁定交易物品
//	DWORD	dwDstID;
CMD_END;

CMD_START(NS_ExchangeLock)
	DWORD	dwErrorCode;
	DWORD	dwSrcID;
CMD_END;

CMD_START(NC_ExchangeCancel)	// 取消交易
CMD_END;

CMD_START(NS_ExchangeCancel)
	DWORD	dwSrcID;
CMD_END;

CMD_START(NC_ExchangeVerify)	// 玩家确认交易
CMD_END;

CMD_START(NS_ExchangeFinish)	// 交易完成
	DWORD	dwErrorCode;
	DWORD	dwFailedRoleID;		// 导致失败者ID
CMD_END;



//----------------------------------------------------------------------------
#pragma pack(pop)