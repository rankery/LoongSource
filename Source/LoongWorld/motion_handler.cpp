#include "stdafx.h"
#include "player_session.h"
#include "role.h"
#include "role_mgr.h"
#include "../WorldDefine/msg_motion.h"

//-------------------------------------------------------------------------------
// 玩家播放个性动作
//-------------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleStyleAction(tagNetCmd* pCmd)
{
	tagNC_RoleStyleAction* pRecv = (tagNC_RoleStyleAction*)pCmd;

	Role* pRole = GetRole();
	if( !P_VALID(pRole) ) return GT_INVALID;

	// 广播
	tagNS_RoleStyleAction send;
	send.dwDstRoleID = GT_INVALID;
	send.dwRoleID = pRole->GetID();
	send.dwActionID = pRecv->dwActionID;

	if( P_VALID(pRole->GetMap()) )
	{
		pRole->GetMap()->SendBigVisTileMsg(pRole, &send, send.dwSize);
	}
	return 0;
}


//-------------------------------------------------------------------------
// 双人动作邀请
//-------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleDuetMotionInvite( tagNetCmd* pCmd )
{
	if (!P_VALID(GetRole()))		return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_DuetMotionInvite);

	DWORD dwRtv = E_Success;

	Role* pDestRole = g_roleMgr.GetRolePtrByID(pRecv->dwDstRoleID);
	if (!P_VALID(pDestRole))
	{
		dwRtv = E_Motion_RoleNotFround;
	}
	else
	{
		dwRtv = GetRole()->CanCastMotion(pDestRole, pRecv->dwActionID);
	}

	if (E_Motion_Success == dwRtv)
	{
		GetRole()->SetMotionInviteStatus(TRUE, pRecv->dwDstRoleID);
		pDestRole->SetMotionInviteStatus(TRUE, GetRole()->GetID());

		tagNS_DuetMotionOnInvite send;
		send.dwActionID = pRecv->dwActionID;
		send.dwSrcRoleID = GetRole()->GetID();

		pDestRole->SendMessage(&send, send.dwSize);
	}
	else
	{
		tagNS_DuetMotionInvite send;
		send.dwErrCode = dwRtv;
		SendMessage(&send, send.dwSize);
	}

	return E_Success;
}

//-------------------------------------------------------------------------
// 双人动作应答
//-------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleDuetMotionOnInvite( tagNetCmd* pCmd )
{
	if (!P_VALID(GetRole()))		return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_DuetMotionOnInvite);

	tagNS_DuetMotionInvite send;
	send.dwErrCode = E_Motion_Success;

	Role* pSrcRole = g_roleMgr.GetRolePtrByID(pRecv->dwSrcRoleID);
	if (!P_VALID(pSrcRole))
	{
		send.dwErrCode = E_Motion_RoleNotFround;
	}
	else if (!pRecv->bAccpet)
	{
		send.dwErrCode = E_Motion_DstRoleRefuse;
	}
	else
	{
		send.dwErrCode = GetRole()->CanCastMotion(pSrcRole, pRecv->dwActionID);
	}

	if (send.dwErrCode != E_Motion_Success)
	{
		GetRole()->SetMotionInviteStatus(FALSE, pRecv->dwSrcRoleID);
		if (P_VALID(pSrcRole))
		{
			pSrcRole->SetMotionInviteStatus(FALSE, GetRole()->GetID());
		}		
	}

	if (P_VALID(pSrcRole))
	{
		pSrcRole->SendMessage(&send, send.dwSize);
	}	

	return E_Success;
}

//-------------------------------------------------------------------------
// 双人动作开始
//-------------------------------------------------------------------------
DWORD PlayerSession::HandleRoleDuetMotionStart( tagNetCmd* pCmd )
{
	MTRANS_POINTER(pRecv, pCmd, tagNC_DuetMotionStart);

	Role* pDestRole = GetOtherInMap(pRecv->dwDstRoleID);
	if (!P_VALID(GetRole()))		return GT_INVALID;
	if (!P_VALID(pDestRole))		return GT_INVALID;

	GetRole()->CastMotion(pDestRole, pRecv->dwActionID);

	GetRole()->SetMotionInviteStatus(FALSE, pRecv->dwDstRoleID);
	pDestRole->SetMotionInviteStatus(FALSE, GetRole()->GetID());

	return E_Success;
}

