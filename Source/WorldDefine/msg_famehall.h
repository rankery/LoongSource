//-----------------------------------------------------------------------------
// filename: msg_reputation.h
// author: client/server hyu/xlguo
// actor:
// data: 2009-02-16
// last: 2009-02-16
// brief: 角色声望客户端服务端消息定义
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)

#include "reputation.h"
#include "famehall_define.h"
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
	E_FrameHall_Success					= 0,	// 成功
	E_FrameHall_MemberOrderUnchanged	= 1,	// 名人堂成员列表无变化
	E_FrameHall_RepOrderUnchanged		= 2,	// 声望排名无变化
	E_FrameHall_ItemIsNotTreasure		= 3,	// 物品不是珍宝
	E_FrameHall_TreasureAlreadyAct		= 4,	// 珍宝已激活
	E_FrameHall_ActCountNotEnough		= 5,	// 激活次数不够
	E_FrameHall_ClanColNotEnough		= 6,	// 氏族贡献不够
	E_FrameHall_UnknownError			,		// 未知错误
};


//----------------------------------------------------------------------------
// 获取进入名人堂前50个角色列表
//----------------------------------------------------------------------------
CMD_START(NC_GetFameHallRoles)
	BYTE		byClanType;				// 氏族类型
	DWORD		dwUpdateTime;			// 更新时间戳 tagDWORDTime
CMD_END

CMD_START(NS_GetFameHallRoles)
	BYTE		eClanType;				// 氏族类型EClanType
	BYTE		byErrCode;				// 是否列表无变化则为E_FrameHall_MemberOrderUnchanged
	DWORD		dwUpdateTime;			// 最后更新时间戳
	BYTE		byRoleNum;				// 氏族名人堂列表数量(上限50)
	BYTE		byData[1];				// 成员nameID列表数据(DWORD)
CMD_END


//----------------------------------------------------------------------------
// 声望前50个角色列表
//----------------------------------------------------------------------------
CMD_START(NC_GetReputeTop)
	BYTE 		byClanType;				// 氏族类型
	DWORD		dwUpdateTime;			// 更新时间戳 tagDWORDTime
CMD_END

CMD_START(NS_GetReputeTop)
	BYTE		byClanType;				// 氏族类型EClanType
	BYTE		byErrCode;				// 是否列表无变化则为E_FrameHall_RepOrderUnchanged
	DWORD		dwUpdateTime;			// 最后更新时间戳
	BYTE		byRoleNum;				// 每氏族名人堂列表数量(上限50)
	BYTE		byData[1];				// 变长列表数据(tagRepRankData)
CMD_END



//----------------------------------------------------------------------------
// 获得氏族所有激活珍宝
//----------------------------------------------------------------------------
CMD_START(NC_GetActClanTreasure)
	BYTE		byClanType;				// 氏族类型EClanType
CMD_END

CMD_START(NS_GetActClanTreasure)
	BYTE		byClanType;				// 氏族类型EClanType
	INT16		n16ActTreasureNum;		// 已激活珍宝数量
	BYTE		byData[1];				// 数据tagTreasureData
CMD_END

//----------------------------------------------------------------------------
// 新激活氏族珍宝通知
//----------------------------------------------------------------------------
CMD_START(NS_NewActivedTreasure)
	INT16		n16TreasureID;
	DWORD		dwNameID;
CMD_END

//----------------------------------------------------------------------------
// 激活氏话珍宝
//----------------------------------------------------------------------------
CMD_START(NC_ActiveTreasure)
	UINT16		u16TreasureID;			// 珍宝ID
CMD_END

CMD_START(NS_ActiveTreasure)
	DWORD		dwErrCode;				// 错误码
	UINT16		u16TreasureID;			// 珍宝ID
CMD_END

#pragma pack(pop)