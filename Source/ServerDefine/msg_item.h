//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_item.h
// author: 
// actor:
// data: 2008-09-04
// last:
// brief: 物品相关
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push)
#pragma pack(1)
//-----------------------------------------------------------------------------


#include "../WorldDefine/msg_common_errorcode.h"
#include "../WorldDefine/ItemDefine.h"
#include "item_define.h"
//-----------------------------------------------------------------------------
// 枚举
//-----------------------------------------------------------------------------
enum
{
	
};

//-----------------------------------------------------------------------------
// 消息结构
//-----------------------------------------------------------------------------
CMD_START(NDBC_GetItemInfo)	// 获得游戏世界中全局物品相关信息
CMD_END;

CMD_START(NDBS_GetItemInfo)
	DWORD	dwDummy;
	INT64	n64MaxSerial;
	INT64	n64MinSerial;
CMD_END;

CMD_START(NDBC_NewItem)
	tagItem		item;
CMD_END;

CMD_START(NDBC_NewEquip)
	tagEquip	equip;
CMD_END;

CMD_START(NDBC_DelItem)
	INT64		n64Serial;
CMD_END;

CMD_START(NDBC_DelEquip)
	INT64		n64Serial;
CMD_END;

CMD_START(NDBC_NewBaiBaoItem)
	tagItem		item;
CMD_END;

CMD_START(NDBC_NewBaiBaoEquip)
	tagEquip	equip;
CMD_END;

CMD_START(NDBC_DelBaiBaoItem)
	INT64		n64Serial;
CMD_END;

CMD_START(NDBC_DelBaiBaoEquip)
	INT64		n64Serial;
CMD_END;

//CMD_START(NDBC_UpdateItemPos)	// 更新物品位置
//	DWORD		dwAccountID;
//	INT64		n64Serial;
//	DWORD		dwRoleID;
//	INT32		nUseTimes;		// 物品上技能使用次数
//	INT16		n16Num;			// 物品个数
//	INT16		n16Index;		// 容器中位置
//	BYTE		byConType;		// 容器类型
//CMD_END;

CMD_START(NDBC_UpdateEquipAtt)	// 更新装备属性，即装备属性表
	tagEquipSpecUpdate	equipSpecUpdate;
CMD_END;


//CMD_START(NDBC_ReplaceItemCDTime)	// 存储物品冷却时间
//	DWORD			dwRoleID;
//	INT32			nNum;			// 个数
//	BYTE			byCDTime[1];	// tagCDTime
//CMD_END;


//-----------------------------------------------------------------------------
// 压测专用
//-----------------------------------------------------------------------------
CMD_START(NDBC_TestSaveItem)
	tagItem		item;
CMD_END;

CMD_START(NDBC_TestSaveEquip)
	tagEquip	equip; 
CMD_END;

//-----------------------------------------------------------------------------
#pragma pack(pop)
