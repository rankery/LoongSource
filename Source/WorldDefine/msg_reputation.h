//-----------------------------------------------------------------------------
// filename: msg_reputation.h
// author: client/server hyu/xlguo
// actor:
// data: 2009-02-19
// last: 
// brief: 声望消息
//-----------------------------------------------------------------------------

#pragma once
#pragma pack(push,1)

#include "reputation.h"

//----------------------------------------------------------------------------
// 获取角色声望信息
//----------------------------------------------------------------------------
CMD_START(NC_GetReputation)
	DWORD					dwRoleID;		//角色ID
CMD_END

CMD_START(NS_GetReputation)
	DWORD					dwRoleID;		//角色ID
	tagReputationData		ReputeData;		//声望数据
CMD_END

#pragma pack(pop)