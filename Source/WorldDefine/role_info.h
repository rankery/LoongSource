//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_info.h
// author: 
// actor:
// data: 2008-08-25
// last:
// brief: 根据角色ID得到角色名称或者，根据角色名称CRC值得到角色ID
//-----------------------------------------------------------------------------
#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)

CMD_START(NC_RoleGetID)		// 得到某角色ID
	DWORD	dwRoleNameCrc;	// 角色名字crc32
CMD_END;

CMD_START(NS_RoleGetID)		// 返回某角色ID	
	DWORD	dwRoleNameCrc;	// 角色名称crc32
	DWORD	dwRoleID;		// 请求目标玩家的roleid, = GT_INVALID表示不存在
	TCHAR	szName[X_SHORT_NAME];	// name
CMD_END;

CMD_START(NC_GetNamebyNameID)	// 得到name
	DWORD	dwNameID;			// NameID
CMD_END;

CMD_START(NS_GetNamebyNameID)		// 返回name
	BOOL	bResult;				// TRUE = 成功
	DWORD	dwNameID;				// 请求NameID
	TCHAR	szName[X_SHORT_NAME];	// name
CMD_END;


// 一次请求多个ID对应的名字
CMD_START(NC_RoleGetSomeName)
	INT		nUserData;
	INT		nNum;		// id的数量
	DWORD	dwAllID[1];	// 变长数组,存放dwNum个id
CMD_END


// 玩家ID和名字对应的结构
struct tagRoleIDName
{
	DWORD dwID;
	TCHAR szName[X_SHORT_NAME];
};

CMD_START(NS_RoleGetSomeName)
	INT				nUserData;
	INT				nNum;				// 名字数量
	tagRoleIDName	IdName[1];
CMD_END

#pragma pack(pop)



