//-----------------------------------------------------------------------------
//!\file msg_show_item.h
//!
//!\date 2008-11-05
//!
//!\brief 客户端与服务器之间关于展示装备,物品的消息
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"
#include "ItemDefine.h"
#include "container_define.h"

#pragma pack(push, 1)

//错误码
enum
{
	E_RoleShowItem_ForbidChat			    = 1,				//禁止展示
	E_RoleShowItem_ServerNotReady			= 2,				//服务器没有准备好
	E_RoleShowItem_SendFailure			    = 3,				//发送物品失败
	E_RoleShowItem_Frequently			    = 4,				//展示过快

	E_RoleShowItem_World_NoItem				= 20,				//世界,没有道具
	E_RoleShowItem_World_NoMoney			= 21,				//世界,没有足够银两

	E_RoleShowItem_Pvt_RemoteRoleLeave		= 30,				//私聊,对方不在线
	E_RoleShowItem_Pvt_NoRoleName			= 31,				//私聊,没有目标玩家 
	E_RoleShowItem_Pvt_NoSelf				= 32,				//私聊,自己不能与自己私聊

	E_RoleShowItem_Guild_NoJoin				= 40,				//门派,没有门派

	E_RoleShowItem_Troop_NoJoin				= 50,				//队伍,没有队伍
};

//-----------------------------------------------------------------------------
//	展示装备
//-----------------------------------------------------------------------------
CMD_START(NC_RoleShowEquip)
	DWORD		 dwDestRoleID;	// 对谁展示
	BYTE		 byChannel;		// 展示频道
	EItemConType eConType;      // 装备位置(背包等等)
	INT64		 n64Serial;		// 装备64位ID
CMD_END;

CMD_START(NS_RoleShowEquip)	
	DWORD		dwSrcRoleID;	// 是谁展示 GT_INVALID代表系统发送
	DWORD		dwDestRoleID;	// 是对谁展示
	BYTE		byChannel;		// 展示频道
	DWORD		dwErrorCode;	// 错误码
	TCHAR	    szEquip[1];		// 装备结构 tagEquip
CMD_END;

//-----------------------------------------------------------------------------
//	展示物品
//-----------------------------------------------------------------------------
CMD_START(NC_RoleShowItem)
	DWORD		 dwDestRoleID;	// 对谁展示
	BYTE		 byChannel;		// 展示频道
	EItemConType eConType;		// 物品位置(背包等等)
	INT64		 n64Serial;		// 物品64位ID
CMD_END;

CMD_START(NS_RoleShowItem)
	DWORD		dwSrcRoleID;	// 是谁展示 GT_INVALID代表系统发送
	DWORD		dwDestRoleID;	// 是对谁展示
	BYTE		byChannel;		// 展示频道
	DWORD		dwErrorCode;	// 错误码
	DWORD	    dwTypeID;		// 物品TypeID tagItem
CMD_END;

// Jason 2009-11-28
// 在队里展示拾取的掉落物品
// 只给队内的成员广播消息
CMD_START(NS_RoleShowDroptItemInTeam)
	DWORD		dwSrcRoleID;	// 是谁展示 GT_INVALID代表系统发送
	BYTE		byChannel;		// 展示频道
	DWORD	    dwTypeID;		// 物品TypeID
	TCHAR		szPickerName[X_SHORT_NAME];
CMD_END


#pragma pack(pop)