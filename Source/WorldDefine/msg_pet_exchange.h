//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_pet_exchange.h
// author:  hyu
// actor:   hyu
// data:    2009-08-11
// last:    2009-08-11
// brief: 玩家间宠物交易消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
    E_Pet_Exchange_Accept				= E_Success,	// 对方同意交易
    E_Pet_Exchange_Refuse				= 1,	// 对方拒绝		
    E_Pet_Exchange_NotEnough_Space	    = 2,	// 宠物栏空间不足
    E_Pet_Exchange_RoleBusy				= 3,	// 被申请方忙	
    E_Pet_Exchange_WithMore				= 4,	// 不能和多人同时交易
    E_Pet_Exchange_OutOfRange			= 5,	// 交易距离范围之外
    E_Pet_Exchange_NotInSame_Map		= 6,	// 不在同一区域
    E_Pet_Exchange_PetCanNot_Exchange	= 7,	// 宠物不能交易
    E_Pet_Exchange_PetCanNot_Find		= 8,	// 没有找到指定的宠物
    E_Pet_Exchange_NotEnough_Money		= 9,	// 交易的金钱不足
    E_Pet_Exchange_ApplicantTgt_Change	= 10,	// 交易申请的交易目标发生变化
    E_Pet_Exchange_NotAnswer			= 11,	// 对方没有响应
    E_Pet_Exchange_SelfBusy				= 12,	// 玩家自己忙

	E_Pet_Exchange_PetHasEquip			= 20,	// 宠物有装备
	E_Pet_Exchange_RoleLvlNotEnough		= 21,	// 目标玩家等级不够
	E_Pet_Exchange_PetLocked			= 22,	// 宠物被锁定
	E_Pet_Exchange_PetBinded			= 23,	// 宠物被绑定
	E_Pet_Exchange_PetStateNotAllow		= 24,	// 宠物状态不允许

    E_Pet_Exchange_End
};




//----------------------------------------------------------------------------
// 交易握手
//----------------------------------------------------------------------------

// 玩家请求交换宠物
CMD_START(NC_PetExchangeReq)
    DWORD	dwDstID;	// 消息目标ID
CMD_END;

// 服务器转发给目标玩家
CMD_START(NS_PetExchangeReq)
    DWORD	dwSrcID;	// 消息发起者ID
CMD_END;

// 目标玩家交易请求反馈
CMD_START(NC_PetExchangeReqRes)
    DWORD	dwErrorCode;
    DWORD	dwDstID;
CMD_END;

// 服务器转发给双方玩家交易请求反馈
CMD_START(NS_PetExchangeReqRes)
    DWORD	dwErrorCode;
    DWORD	dwSrcID;    // 发起者id
    DWORD	dwDstID;    // 目标id
CMD_END;

//----------------------------------------------------------------------------
// 交易过程
//----------------------------------------------------------------------------


// 添加交易的宠物
CMD_START(NC_PetExchangeAdd)
    DWORD	dwPetID;
CMD_END;

// 反馈，如成功则本地添加
CMD_START(NS_PetExchangeAdd_2Src)
    DWORD	dwErrorCode;
    DWORD	dwSrcID;        // 添加发起者id
    DWORD	dwDstID;
    DWORD	dwPetID;
CMD_END;

// 如成功则通知目标玩家添加宠物
CMD_START(NS_PetExchangeAdd_2Dst)
    DWORD	dwSrcID;        // 对方发起者id
    DWORD	dwPetID;
CMD_END;

// 取消交易的宠物
CMD_START(NC_PetExchangeDec)
    DWORD	dwSrcID;        // 添加发起者id
    DWORD	dwPetID;
CMD_END;

// 反馈，如成功则本地减少
CMD_START(NS_PetExchangeDec)
    DWORD	dwErrorCode;
    DWORD	dwSrcID;        // 减少发起者id
    DWORD	dwDstID;
    DWORD	dwPetID;
CMD_END;

// 如成功则通知目标玩家取消宠物
CMD_START(NS_PetExchangeDec_2Dst)
    DWORD	dwSrcID;        // 减少发起者id
    DWORD	dwPetID;
CMD_END;

// 金钱发生改变 
CMD_START(NC_PetExchangeMoney)
    INT64	n64Money;
CMD_END

// 通知双方
CMD_START(NS_PetExchangeMoney)
    DWORD	dwErrorCode;	
    INT64	n64Money;
    DWORD	dwSrcID;       // 改变金钱发起者id
CMD_END

CMD_START(NC_PetExchangeLock)		// 锁定交易宠物
CMD_END;

// 通知双方
CMD_START(NS_PetExchangeLock)
    DWORD	dwErrorCode;
    DWORD	dwSrcID;       // 锁定交易发起者id
CMD_END;

// 取消交易
CMD_START(NC_PetExchangeCancel)
CMD_END;

// 通知双方
CMD_START(NS_PetExchangeCancel)
    DWORD	dwSrcID;        // 取消交易发起者id
CMD_END;

// 玩家确认交易
CMD_START(NC_PetExchangeVerify)	
CMD_END;

// 交易完成
CMD_START(NS_PetExchangeFinish)
    DWORD	dwErrorCode;
    DWORD	dwFailedRoleID;		// 导致失败者ID
CMD_END;

//----------------------------------------------------------------------------
#pragma pack(pop)