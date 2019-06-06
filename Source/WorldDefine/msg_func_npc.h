//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_func_npc.h
// author: 
// actor:
// data: 2008-11-10
// last:
// brief: 职能NPC相关消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{	
	E_Dak_NPCNotFound		= 1,	// 没有找到指定NPC
	E_Dak_NPCNotValid		= 2,	// 不是驿站NPC
	E_Dak_TooFar			= 3,	// 距离驿站NPC太远
	E_Dak_NotExist			= 4,	// 在指定NPC身上没有找到驿站
	E_Dak_TargetMap_Invalid	= 5,	// 指定索引内map id和指定id不符
	E_Dak_NotEnough_Money	= 6,	// 消耗品不足
	E_Dak_ChangeMap_Failed	= 7,	// 传送时失败

	E_Ware_NPCNotValid		= 10,	// 不是角色仓库NPC
	
	E_Con_Cannot_Extend		= 50,	// 不可再扩容
	
	E_Silver_NotEnough		= 60,	// 金钱不足
	E_YuanBao_NotEnough		= 61,	// 元宝不足

	E_Item_NotExist			= 70,	// 物品不存在
	E_Item_UseErr			= 71,	// 物品使用失败
};

//----------------------------------------------------------------------------
// 驿站&乾坤石
//----------------------------------------------------------------------------
CMD_START(NC_Dak)
	DWORD	dwNPCID;	// 职能NPC ID
	INT32	nIndex;		// 驿站目的地在该NPC处索引
	DWORD	dwMapID;	// 目的地Map ID
CMD_END

CMD_START(NS_Dak)
	DWORD	dwErrorCode;
CMD_END


//----------------------------------------------------------------------------
// 角色仓库
//----------------------------------------------------------------------------
CMD_START(NC_WareExtend)
	DWORD	dwNPCID;
	INT64	n64Serial;		// true: 使用游戏币；false:使用元宝
CMD_END

CMD_START(NS_WareExtend)
	DWORD	dwErrorCode;
	INT16	n16WareNum;
CMD_END

CMD_START(NC_BagExtend)
	DWORD	dwNPCID;
	INT64	n64Serial;		// 物品id
CMD_END

CMD_START(NS_BagExtend)
	DWORD	dwErrorCode;
	INT16	n16BagNum;
CMD_END

//----------------------------------------------------------------------------
// 金钱&元宝
//----------------------------------------------------------------------------
CMD_START(NC_SaveSilver)
	DWORD	dwNPCID;
	INT64	n64SilverSave;
CMD_END

CMD_START(NS_SaveSilver)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_GetSilver)
	DWORD	dwNPCID;
	INT64	n64SilverGet;
CMD_END

CMD_START(NS_GetSilver)
	DWORD	dwErrorCode;
CMD_END

//CMD_START(NC_SaveYuanBao)
//	DWORD	dwNPCID;
//	INT		nYuanBaoSave;
//CMD_END

CMD_START(NS_SaveYuanBao)
	DWORD	dwErrorCode;
CMD_END

CMD_START(NC_GetYuanBao)	// 从百宝袋中获取元宝
	INT		nYuanBaoGet;
CMD_END

//CMD_START(NS_GetYuanBao)
//	DWORD	dwErrorCode;
//CMD_END


//----------------------------------------------------------------------------
// 使用磨石
//----------------------------------------------------------------------------
CMD_START(NC_Abrase)
	INT64	n64AbraserSerial;	// 磨石64位id
	INT64	n64WeaponSerial;	// 武器64位id
CMD_END

CMD_START(NS_Abrase)
	DWORD	dwErrorCode;
	INT64	n64WeaponSerial;	// 武器64位id
	INT		nAttackTimes;		// 更新后武器使用次数
CMD_END

//----------------------------------------------------------------------------
#pragma pack(pop)