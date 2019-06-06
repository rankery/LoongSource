//-----------------------------------------------------------------------------
// filename: msg_role_info.h
// author: client/server hyu/xlguo
// actor:
// data: 2009-02-02
// last: 2009-02-03
// brief: 角色名贴客户端服务端消息定义
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)

#include "vcard_define.h"


//----------------------------------------------------------------------------
// 获取角色名贴信息
//----------------------------------------------------------------------------
CMD_START(NC_GetRoleVCard)
	DWORD			dwRoleID;			// 角色ID
CMD_END

CMD_START(NS_GetRoleVCard)
	DWORD			dwErrorCode;		// 错误码
	DWORD			dwRoleID;			// 角色ID
//	tagVCardData	VCardData;			// 名帖数据
	BYTE			pData[1];				// 名帖数据
CMD_END

//----------------------------------------------------------------------------
// 设置角色名贴信息
//----------------------------------------------------------------------------
CMD_START(NC_SetRoleVCard)
	DWORD				dwRoleID;			// 角色ID
	tagCustomVCardData	customVCardData;	// 自定义信息
CMD_END

CMD_START(NS_SetRoleVCard)
	DWORD			dwErrorCode;		// 错误码
	DWORD			dwRoleID;			// 角色ID
CMD_END

//----------------------------------------------------------------------------
// 获得角色头像URL信息
//----------------------------------------------------------------------------
CMD_START(NC_GetRoleHeadPicURL)
    DWORD           dwRoleID;           //!< 角色ID
CMD_END

CMD_START(NS_GetRoleHeadPicURL)
    DWORD           dwRoleID;
    BYTE            byHeadUrlSize;      //!< 头像尺寸(byte长度)
    BYTE            byData[1];          //!< 头像URL串，以'\0'结束
CMD_END

#pragma pack(pop)