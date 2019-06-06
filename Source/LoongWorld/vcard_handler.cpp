//-----------------------------------------------------------------------------
//!\file vcard_handler.cpp
//!\author xlguo
//!
//!\date 2009-02-03
//! last 2008-02-03
//!
//!\brief 角色名贴命令处理器
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "player_session.h"
#include "role_mgr.h"
#include "role.h"
#include "vcard.h"
#include "..\WorldDefine\msg_role_vcard.h"
#include "..\ServerDefine\msg_vcard.h"

//-----------------------------------------------------------------------------
// 获得角色名贴
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleGetVCard(tagNetCmd* pCmd)
{

	MGET_MSG(pRecv, pCmd, NC_GetRoleVCard);
	if( !P_VALID(m_pRole) ) return GT_INVALID;

	// 本玩家
	if(pRecv->dwRoleID == GetRole()->GetID())
		GetRole()->GetVCard().SendVCard2Client(GetRole()->GetID());
	// 其他玩家
	else
	{
		Role* pRole = g_roleMgr.GetRolePtrByID(pRecv->dwRoleID);
		// 在线玩家
		if (P_VALID(pRole) )
		{
			tagRoleVCard* pVCard = &pRole->GetVCard();
			// 名帖可见
			if(pVCard->customVCard.bVisibility)
				pVCard->SendVCard2Client(GetRole()->GetID());
			// 名帖不可见
			else
				GetRole()->GetVCard().NotifyClientGetVCard(pRecv->dwRoleID, E_VCard_NotVisible);
		}
		// 离线玩家
		else
			GetRole()->GetVCard().SendLoadOffLineVCard(pRecv->dwRoleID, GetRole()->GetID());
	}

	return 0;
}

//-----------------------------------------------------------------------------
// 设置角色名贴自定义信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleSetVCard(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SetRoleVCard);
	if( !P_VALID(m_pRole) ) return GT_INVALID;
	
	if( pRecv->dwRoleID != GetRole()->GetID())
	{	// 无改变权限
		GetRole()->GetVCard().NotifyClientSetVCard(pRecv->dwRoleID, E_VCard_NoPrivilege);
		return GT_INVALID;
	}

	tagRoleVCard* pVCard = &GetRole()->GetVCard();

	// 更新数据
	pVCard->SetCustomData(&pRecv->customVCardData);

	// 更新数据库
	pVCard->SendSaveDB();
		
	// 通知客户端
	pVCard->NotifyClientSetVCard(pRecv->dwRoleID, E_VCard_Success);

	return 0;
}

//-----------------------------------------------------------------------------
// 获取在线玩家的头像url
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetHeadPicUrl(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetRoleHeadPicURL);

	if( !P_VALID(GetRole()) ) return GT_INVALID;

	Role*pRole = g_roleMgr.GetRolePtrByID(pRecv->dwRoleID);
	if (!P_VALID(pRole))
		GetRole()->GetVCard().SendNullUrlToMe(pRecv->dwRoleID);
	else
		pRole->GetVCard().SendHeadUrlTo(GetRole()->GetID());
	
	return 0;
}