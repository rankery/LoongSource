//-----------------------------------------------------------------------------
// filename: msg_role_title.h
// author: client/server hyu/xlguo
// actor:
// data: 2009-01-19
// last: 2009-01-20
// brief: 角色称号客户端服务端消息定义
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)
#include "msg_common_errorcode.h"
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
	E_Title_Success					= E_Success,	// 成功
	E_Title_NotVisible				= 1,			// 称号不可见
	E_Title_TitleNotObtained		= 2,			// 称号尚未获得
	E_Title_NoTitleActived			= 3,			// 角色尚未激活称号
	E_Title_TitleActivateFailed		= 4,			// 激活称号失败
	E_Title_UnknownError			= 5,			// 未知错误
};

//-----------------------------------------------------------------------------
// 设置角色称号可见性
//-----------------------------------------------------------------------------
// CMD_START(NC_SetTitlesVisibility)
// 	DWORD			dwRoleID;		// 角色ID
// 	BOOL			bVisibility;	// 称号可见性
// CMD_END
// 
// CMD_START(NS_SetTitlesVisibility)
// 	DWORD			dwErrorCode;	// 错误码
// 	DWORD			dwRoleID;		// 角色ID
// CMD_END

//-----------------------------------------------------------------------------
// 获得角色称号可见性
//-----------------------------------------------------------------------------
// CMD_START(NC_GetTitlesVisibility)
// 	DWORD			dwRoleID;		// 角色ID
// CMD_END
// 
// CMD_START(NS_GetTitlesVisibility)
// 	DWORD			dwErrorCode;	// 错误码
// 	DWORD			dwRoleID;		// 角色ID
// 	BOOL			bVisibility;	// 可见性
// CMD_END

//-----------------------------------------------------------------------------
// 设置角色使用某称号（u16TitleID无效时，表示不使用称号）
//-----------------------------------------------------------------------------
CMD_START(NC_ActiveRoleTitle)
	DWORD			dwRoleID;		// 角色ID
	UINT16			u16TitleID;		// 当前使用的称号ID
CMD_END

CMD_START(NS_ActiveRoleTitle)
	DWORD			dwErrorCode;	// 错误码
	DWORD			dwRoleID;		// 角色ID
	UINT16			u16TitleID;		// 称号ID
CMD_END

//-----------------------------------------------------------------------------
// 请求获得角色正在使用的称号
//-----------------------------------------------------------------------------
CMD_START(NC_GetActivatedTitle)
	DWORD			dwRoleID;		// 角色ID
CMD_END
//-----------------------------------------------------------------------------
// 返回获得角色正在使用的称号
//-----------------------------------------------------------------------------
CMD_START(NS_GetActivatedTitle)
	DWORD			dwErrorCode;	// 错误码
	DWORD			dwRoleID;		// 角色ID
	UINT16			u16TitleID;		// title ID
CMD_END

//-----------------------------------------------------------------------------
// 请求获得角色拥有的所有称号
//-----------------------------------------------------------------------------
CMD_START(NC_GetRoleTitles)
	DWORD			dwRoleID;		// 角色ID
CMD_END
//-----------------------------------------------------------------------------
// 返回角色拥有的所有称号(变长列表)
//-----------------------------------------------------------------------------
CMD_START(NS_GetRoleTitles)
	DWORD			dwErrorCode;	// 错误码
	DWORD			dwRoleID;		// 角色ID
	UINT16			u16TitleNum;	// 当前本地角色已获得的称号数量
	BYTE			byData[1];		// 当前本地角色已获得的称号ID
CMD_END

//-----------------------------------------------------------------------------
// 角色获得新称号(用来通知客户端)
//-----------------------------------------------------------------------------
CMD_START(NS_NewTitles)
	DWORD			dwRoleID;		// 角色ID
	UINT16			u16TitleNum;	// 新获得称号数量
	BYTE			byData[1];		// 新获得称号
CMD_END

//-----------------------------------------------------------------------------
// 角色删除称号(用来通知客户端)
//-----------------------------------------------------------------------------
CMD_START(NS_DelTitles)
	DWORD			dwRoleID;		// 角色ID
	UINT16			u16TitleNum;	// 删除称号数量
	BYTE			byData[1];		// 被删除的称号
CMD_END
//-----------------------------------------------------------------------------
// 角色正在使用称号发生改变，给周围玩家广播
//-----------------------------------------------------------------------------
CMD_START(NS_RoleTitleChangeBroadcast)
	DWORD			dwRoleID;
	UINT16			dwTitleID;		// 若玩家选择隐藏称号，该字段为GT_INVALID
CMD_END;

#pragma pack(pop)