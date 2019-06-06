//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_issue.h
// author: 
// actor:
// data: 2008-07-21
// last:
// brief: 玩家关于地图的所有消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)

enum
{
	E_BindRebornMap_NPCInvalid	=	1,	// 职能NPC非法
	E_BindRebornMap_MapInvalid	=	2,	// 地图非法（不是普通地图）
	E_BindRebornMap_Already		=	3,	// 已经绑定在该地图
	E_BindRebornMap_NPCNotFound	=	4,	// 没有找到指定的NPC
	E_BindRebornMap_TooFar		=	5,	// 距离太远

	E_Instance_Not_Exit			=	11,	// 副本不存在
	E_Instance_Role_Not_Exit	=	12,	// 玩家不存在
	E_Instance_Role_Full		=	13,	// 副本人数已到上限
	E_Instance_Level_Down_Limit =	14,	// 副本最低等级限制
	E_Instance_Level_Up_Limit	=	15,	// 副本最高等级限制
	E_Instance_Not_Same_Team	=	16,	// 不是同一小队
	E_Instance_Role_Lack		=	17,	// 副本人数未到下限
	E_Instance_End_Delete		=	18,	// 副本已经结束或者已经关闭
	E_Instance_Already			=	19,	// 已经创建了其它副本
	E_Instance_Full				=	20,	// 副本创建数量已满
};

enum E_SET_TRANSPORT_ERROR_CODE
{	
	EST_SuccessSet				=	E_Success,	//设置成功
	EST_InInstance				=	1,			//处在副本中
	EST_LackNeedItem			=	2,			//缺少需要的物品
	EST_SpecFunError			=	3,			//特殊物品类型错误
	EST_SelfStateLimit			=	4,			//自身状态限制
	EST_ErrEnd
};

//------------------------------------------------------------------------------
// 踩到地图触发器
//------------------------------------------------------------------------------
CMD_START(NC_RoleMapTrigger)
	DWORD	dwTriggerID;
	DWORD	dwMisc;			// 如果是普通或随机副本，代表副本难度；
CMD_END

//------------------------------------------------------------------------------
// 进入副本
//------------------------------------------------------------------------------
CMD_START(NS_EnterInstance)
	DWORD	dwTimeLimit;	// 时限副本所剩时间，单位是秒( 非时限副本为GT_INVALID）
	DWORD	dwErrorCode;
CMD_END;

//-----------------------------------------------------------------------------
// 去新地图
//-----------------------------------------------------------------------------
CMD_START(NS_GotoNewMap)
	DWORD	dwMapID;
	Vector3	pos;
	Vector3	faceTo;
CMD_END

//-----------------------------------------------------------------------------
// 通知本地玩家是否召唤队友
//-----------------------------------------------------------------------------
CMD_START(NS_InstanceNofity)
CMD_END

CMD_START(NC_InstanceNotify)	
	bool	bNotify;
CMD_END

//-----------------------------------------------------------------------------
// 通知所有队友是否进入副本
//-----------------------------------------------------------------------------
CMD_START(NS_InstanceAgree)
	DWORD   dwInviterID;
	DWORD	dwMapID;
	DWORD	dwMisc;
CMD_END

CMD_START(NC_InstanceAgree)
	bool	bAgree;
CMD_END

//-----------------------------------------------------------------------------
// 副本完成条件达成
//-----------------------------------------------------------------------------
CMD_START(NS_InstanceComplete)
CMD_END

//-----------------------------------------------------------------------------
// 请求离开副本
//-----------------------------------------------------------------------------
CMD_START(NC_LeaveInstance)
CMD_END

//-----------------------------------------------------------------------------
// 同步副本倒计时
//-----------------------------------------------------------------------------
CMD_START(NS_SyncInstanceTime)
	DWORD	dwTimeLeft;			// 副本剩余时间：单位毫秒
CMD_END

//-----------------------------------------------------------------------------
// 绑定复活点
//-----------------------------------------------------------------------------
CMD_START(NC_BindRebornMap)
	DWORD	dwNPCID;		// 职能NPCID
CMD_END

CMD_START(NS_BindRebornMap)
	DWORD	dwErrorCode;
	DWORD	dwBornMapID;
	Vector3	vBornPos;
CMD_END

//-----------------------------------------------------------------------------
// 设置可记录传送符
//-----------------------------------------------------------------------------
CMD_START(NC_SetTransportMap)
INT64   n64ItemID;      // 可记录传送符
CMD_END

CMD_START(NS_SetTransportMap)
E_SET_TRANSPORT_ERROR_CODE  dwErrorCode;
DWORD                       dwTransportMapID;
Vector3                     vTransportPoint;
CMD_END

//-----------------------------------------------------------------------------
// 使用可记录传送符
//-----------------------------------------------------------------------------
CMD_START(NC_UseTransportMap)
INT64		n64ItemID;			//物品64位ID
CMD_END

#pragma pack(pop)

