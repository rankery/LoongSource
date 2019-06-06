//-----------------------------------------------------------------------------
//!\file move_handler.cpp
//!\author Aslan
//!
//!\date 2008-06-10
//! last 2008-06-10
//!
//!\brief 移动消息处理类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/msg_pk.h"

#include "player_session.h"
#include "role.h"
#include "map.h"

//-----------------------------------------------------------------------------
// 设置/关闭PK保护
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSafeGuard(tagNetCmd* pCmd)
{
	tagNC_SafeGuard* pRecv = (tagNC_SafeGuard*)pCmd;

	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return GT_INVALID;

	INT nRet = E_Success;

	if( pRecv->bOpen )
		nRet = pRole->OpenSafeGuard(pRecv->n64ItemID);
	else
		nRet = pRole->CloseSafeGuard();

	// 返回给客户端
	tagNS_SafeGuard send;
	send.bOpen = pRecv->bOpen;
	send.dwErrorCode = nRet;
	SendMessage(&send, send.dwSize);

	return 0;
}

//----------------------------------------------------------------------------
// 开启行凶状态
//----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSetPK(tagNetCmd* pCmd)
{
	tagNC_PK* pRecv = (tagNC_PK*)pCmd;

	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return GT_INVALID;

	INT nRet = pRole->SetPK(pRecv->bOnlyPK);

	// 返回给客户端
	tagNS_PK send;
	send.dwErrorCode = nRet;
	send.bReset = FALSE;
	send.bOnlyPK = pRecv->bOnlyPK;
	SendMessage(&send, send.dwSize);

	return 0;
}