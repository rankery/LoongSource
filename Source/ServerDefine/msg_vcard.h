//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_vcard.h
// author: 
// actor:
// date: 2009-02-04
// last:
// brief: 保存名帖更改命令
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push)
#pragma pack(1)
#include "..\WorldDefine\vcard_define.h"
//-----------------------------------------------------------------------------
// NDBC:world->DB
// NDBS:DB->world
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 更改名帖自定义信息
//-----------------------------------------------------------------------------
CMD_START(NDBC_VCardChg)
	DWORD				dwRoleID;			// 角色ID
	tagVCardData		VCardData;	// 自定义信息
CMD_END

//-----------------------------------------------------------------------------
// 获取离线玩家名帖
//-----------------------------------------------------------------------------
CMD_START(NDBC_GetOffLineVCard)
	DWORD				dwRoleID;			// 被查询者ID
	DWORD				dwQueryID;			// 查询者ID
CMD_END

CMD_START(NDBS_GetOffLineVCard)
	DWORD				dwRoleID;			// 被查询者ID
	DWORD				dwQueryID;			// 查询者ID
	DWORD				dwErrCode;
	BYTE				byData[1];	
CMD_END


#pragma pack(pop)
