//-----------------------------------------------------------------------------
//!\file title_handler.cpp
//!\author xlguo
//!
//!\date 2009-01-20
//! last 2009-02-02
//!
//!\brief 称号消息处理过程
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/msg_role_title.h"


#include "player_session.h"
#include "unit.h"
#include "role.h"
#include "role_mgr.h"
#include "title_mgr.h"
//-----------------------------------------------------------------------------
// 设置角色使用某称号
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleActiveTitle(tagNetCmd* pCmd)
{
	tagNC_ActiveRoleTitle* pRecv = (tagNC_ActiveRoleTitle*)pCmd;
	if( !P_VALID(m_pRole) ) return GT_INVALID;
	
	// 只能设置本角色
	if (GetRole()->GetID() != pRecv->dwRoleID)
		return GT_INVALID;

	// 激活称号
	TitleMgr* pTitleMgr = GetRole()->GetTitleMgr();
	DWORD dwRtv = pTitleMgr->ActiviteTitle(pRecv->u16TitleID);


	// 发送结果
	tagNS_ActiveRoleTitle send;
	send.dwRoleID = GetRole()->GetID();
	send.u16TitleID = pTitleMgr->GetActiviteTitle();
	send.dwErrorCode = dwRtv;
	SendMessage(&send, send.dwSize);

	return 0;
}

//-----------------------------------------------------------------------------
// 请求获得角色拥有的所有称号
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetTitles(tagNetCmd* pCmd)
{
	tagNC_GetRoleTitles* pRecv = (tagNC_GetRoleTitles*)pCmd;
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	Role* pRole = pRecv->dwRoleID == GetRole()->GetID() ? GetRole() : GetOtherInMap(pRecv->dwRoleID);
	if ( !P_VALID(pRole) )
		return 0;

	TitleMgr* pTitleMgr = pRole->GetTitleMgr();

// 	// 查看权限
// 	if ( pRole->GetID() != m_pRole->GetID() && !pTitleMgr.Visibility() )
// 	{	// 不可发送
// 		tagNS_GetRoleTitles send;
// 		send.dwErrorCode = E_Title_NotVisible;
// 		send.dwRoleID = pRecv->dwRoleID;
// 		send.u16TitleNum = 0;
// 		SendMessage(&send, send.dwSize);
// 	}
// 	else
// 	{	// 可发送
		DWORD dwTitlesSize = pTitleMgr->GetObtainedTitlesNum() * sizeof(UINT16);
		DWORD dwMsgSize = sizeof(tagNS_GetRoleTitles) - 1 + (dwTitlesSize > 0 ? dwTitlesSize : 1);

		ASSERT(dwMsgSize >= sizeof(tagNS_GetRoleTitles));

		MCREATE_MSG(pSend, dwMsgSize, NS_GetRoleTitles);
		//tagNS_GetRoleTitles* pSend;
		UINT16* pu16 = reinterpret_cast<UINT16 *>(pSend->byData);
		DWORD dwRtv = pTitleMgr->GetObtainedTitleIDs(pu16, pSend->u16TitleNum);
		pSend->dwErrorCode = dwRtv;
		pSend->dwRoleID = pRecv->dwRoleID;
		SendMessage(pSend, pSend->dwSize);
		MDEL_MSG(pSend);
//	}

	return 0;
}
