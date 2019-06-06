#pragma once

#include "msg_common_errorcode.h"

#pragma pack(push, 1)

//----------------------------------------------------------
// 通知客户端删除某个远程玩家或怪物
//----------------------------------------------------------
CMD_START(NS_RemoveRemote)
	DWORD	dwRoleID[1];
CMD_END

#pragma pack(pop)