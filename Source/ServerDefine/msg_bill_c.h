//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_bill_c.h
// author: 
// actor:
// data: 2009-06-09
// last:
// brief: 直充服务器与网站接口
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/msg_common_errorcode.h"

#pragma pack(push, 1)

//------------------------------------------------------------------------------
// 登陆
//------------------------------------------------------------------------------
CMD_START(NBC_Login)
	DWORD	dwSectionID;		// 大区ID
CMD_END

CMD_START(NBS_Login)
	BYTE	byResult;			// 0-成功 1-失败
CMD_END

//------------------------------------------------------------------------------
// 元宝划拨
//------------------------------------------------------------------------------
CMD_START(NBS_YuanBao)
	CHAR	szTokenID[50];		// 令牌ID
	DWORD	dwWorldID;			// 游戏世界ID
	CHAR	szAccountName[50];	// 帐号名
	INT		nNum;				// 元宝数量
CMD_END

CMD_START(NBC_Yuanbao)
	CHAR	szTokenID[50];		// 令牌ID
	BYTE	byResult;			// 0-成功 1-失败
CMD_END

//-------------------------------------------------------------------------------
// 元宝划拨确认
//-------------------------------------------------------------------------------
CMD_START(NBS_YuanbaoAck)
	CHAR	szTokenID[50];		// 令牌ID
	DWORD	dwWorldID;			// 游戏世界ID
CMD_END

//------------------------------------------------------------------------------
// 道具划拨
//------------------------------------------------------------------------------
CMD_START(NBS_Item)
	CHAR	szTokenID[50];		// 令牌ID
	DWORD	dwWorldID;			// 游戏世界ID
	CHAR	szAccountName[50];	// 帐号名
	DWORD	dwItemID;			// 物品ID
	INT		nNum;				// 数量
CMD_END

CMD_START(NBC_Item)
	CHAR	szTokenID[50];		// 令牌ID
	BYTE	byResult;			// 0-成功 1-失败
CMD_END

//--------------------------------------------------------------------------------
// 道具划拨确认
//--------------------------------------------------------------------------------
CMD_START(NBS_ItemAck)
	CHAR	szTokenID[50];		// 令牌ID
	DWORD	dwWorldID;			// 游戏世界ID
CMD_END

#pragma pack(pop)