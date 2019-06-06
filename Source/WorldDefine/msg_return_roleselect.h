//-----------------------------------------------------------------------------
//!\file msg_return_roleselect.h
//!
//!\date 2009-08-26
//!
//!\brief 客户端返回角色选择时与服务器之间的消息
//-----------------------------------------------------------------------------

#pragma once
#pragma pack(push, 1)

#include "msg_common_errorcode.h"

//-----------------------------------------------------------------------------
// 返回角色选择界面
//-----------------------------------------------------------------------------
CMD_START(NC_ReturnRoleSelect)
CMD_END

CMD_START(NS_ReturnRoleSelect)
	DWORD		dwErrorCode;		// 错误码
CMD_END

#pragma pack(pop)