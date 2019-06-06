//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_title.h
// author: 
// actor:
// date: 2009-01-20
// last:
// brief: 称号相关
//-----------------------------------------------------------------------------
#pragma once

#pragma pack(push)
#pragma pack(1)
#include "title_define.h"
//-----------------------------------------------------------------------------
// NDBC:world->DB
// NDBS:DB->world
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// 设置角色使用某称号（u16TitleID无效时，表示不使用称号）
//-----------------------------------------------------------------------------
// CMD_START(NDBC_ActiveRoleTitle)
// 	DWORD			dwRoleID;		// 角色ID
// 	UINT16			u16TitleID;		// 当前使用的称号ID
// CMD_END

//-----------------------------------------------------------------------------
// 角色称号提醒(用来通知数据库)
//-----------------------------------------------------------------------------
CMD_START(NDBC_TitleInsert)
	DWORD			dwRoleID;		// 角色ID
	tagTitleSave	titleSave;		// 称号信息
CMD_END

CMD_START(NDBC_TitleDelete)
	DWORD			dwRoleID;		// 角色ID
	tagTitleSave	titleSave;		// 称号信息
CMD_END

#pragma pack(pop)
