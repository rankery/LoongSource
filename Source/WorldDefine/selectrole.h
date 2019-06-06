#pragma once

#pragma pack(push)
#pragma pack(1)

#include "msg_common_errorcode.h"
#include "RoleDefine.h"
#define MAX_ROLE_NUM		5
enum										// 人物创建、删除、选择相关的错误代码
{
	E_JoinGame_AccountOnline			=	1,
	E_JoinGame_VerifyCodeError			=	2,

	E_CreateRole_RoleCreating			=	11,
	E_CreateRole_RoleEnumNotDone		=	12,
	E_CreateRole_RoleDeleting			=	13,
	E_CreateRole_RoleNumFull			=	14,
	E_CreateRole_NameExist				=	15,
	E_CreateRole_NameInvalid			=	16,
	E_CreateRole_SafeCodeInvalid		=	17,
	E_CreateRole_InWorld				=	18,
	E_CreateRole_RoleLoading			=	19,
	E_CreateRole_RoleEnumNotSuccess		=	20,

	E_DelRole_RoleNotExist				=	21,
	E_DelRole_RoleDeleting				=	22,
	E_DelRole_RoleEnumNotDone			=	23,
	E_DelRole_RoleCreating				=	24,
	E_DelRole_SafeCodeIncorrect			=	25,
	E_DelRole_InWorld					=	26,
	E_DelRole_RoleLoading				=	27,
	E_DelRole_LeaveGuild1st				=	28,	// 请先脱离帮派
	E_DelRole_BreakMarriage1st			=	29,	// 请先离婚

	E_EnumRole_RoleEnumed				=	31,
	E_EnumRole_RoleEnuming				=	32,
	E_EnumRole_InWorld					=	33,

	E_SelectRole_RoleEnumNotDone		=	41,
	E_SelectRole_RoleCreating			=	42,
	E_SelectRole_RoleDeleting			=	43,
	E_SelectRole_RoleNotExist			=	44,
	E_SelectRole_InWorld				=	45,
	E_SelectRole_RoleLoading			=	46,
};

//-----------------------------------------------------------------------------
// 进入游戏
//-----------------------------------------------------------------------------
CMD_START(NC_JoinGame)
	DWORD		dwAccountID;		// 帐号ID
	DWORD		dwVerifyCode;		// 登陆服务器发送给客户端的验证码
CMD_END

CMD_START(NS_JoinGame)
	DWORD		dwErrorCode;
	INT			nGMPrivilege;		// GM权限
CMD_END

//-----------------------------------------------------------------------------
// 请求角色列表
//-----------------------------------------------------------------------------
CMD_START(NC_EnumRole)
CMD_END

CMD_START(NS_EnumRole)
	DWORD			dwErrorCode;
	DWORD			dwSafeCodeCrc;
	tagDWORDTime	dwTimeReset;
	tagDWORDTime	dwTimeLastLogin;	//上次登录时间
	DWORD			dwIPLast;			//上次登录IP
	DWORD			dwIP;				//本次登录IP
	INT				nNum;
	BYTE			bySimpleInfo[1];
CMD_END


//创建安全码
CMD_START(NC_SafeCode)
	DWORD			dwSafeCode;
CMD_END

CMD_START(NS_SafeCode)
	DWORD			dwErrorCode;
CMD_END

//重置安全码
CMD_START(NC_ResetSafeCode)
CMD_END

CMD_START(NS_ResetSafeCode)
	tagDWORDTime	dwTimeReset;
CMD_END

//取消安全码
CMD_START(NC_CancelSafeCodeReset)
CMD_END

CMD_START(NS_CancelSafeCodeReset)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 创建角色
//-----------------------------------------------------------------------------
CMD_START(NC_CreateRole)	
	TCHAR			szRoleName[X_SHORT_NAME];
	EPreChapStatus	eStatus;
	tagAvatarAtt	AvatarAtt;
CMD_END

CMD_START(NS_CreateRole)	
	DWORD			dwErrorCode;
	BYTE			bySimRoleInfo[1];
CMD_END

//-----------------------------------------------------------------------------
// 删除角色
//-----------------------------------------------------------------------------
CMD_START(NC_DeleteRole)
	DWORD			dwRoleID;						
	DWORD			dwSafeCodeCrc;
CMD_END

CMD_START(NS_DeleteRole)
	DWORD			dwErrorCode;
	DWORD			dwRoleID;
CMD_END

//------------------------------------------------------------------------------
// 选择角色
//------------------------------------------------------------------------------
CMD_START(NC_SelectRole)
	DWORD			dwRoleID;
CMD_END

CMD_START(NS_SelectRole)
	DWORD			dwErrorCode;
	DWORD			dwRoleID;
CMD_END

#pragma pack(pop)