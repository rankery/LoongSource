#include "StdAfx.h"
#include "player_session.h"
#include "../WorldDefine/exchange_define.h"
#include "../WorldDefine/msg_pet_exchange.h"
#include "map.h"
#include "role.h"

//------------------------------------------------------------------------
// 请求交易
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePetExchangeReq(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_PetExchangeReq);

	if(GT_INVALID == pRecv->dwDstID)
	{
		return GT_INVALID;
	}

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Role* pTarget = NULL;
	DWORD dwErrorCode = pRole->ProcPetExchangeReq(pTarget, pRecv->dwDstID);
	if(E_Success == dwErrorCode)
	{
		if(P_VALID(pTarget))
		{
			// 向被申请者发消息
			tagNS_PetExchangeReq send;
			send.dwSrcID = pRole->GetID();
			pTarget->SendMessage(&send, send.dwSize);
		}
		else
		{
			// 永远不会执行到此
			ASSERT(P_VALID(pTarget));
		}
	}
	else if(dwErrorCode != GT_INVALID)
	{
		//// 向申请者反馈
		//tagNS_ExchangeFinish send;
		//send.dwErrorCode	= dwErrorCode;
		//send.dwFailedRoleID	= pRecv->dwDstID;
		//SendMessage(&send, send.dwSize);

		tagNS_PetExchangeReqRes send;
		send.dwErrorCode	= dwErrorCode;
		send.dwSrcID		= pRole->GetID();
		send.dwDstID		= pRecv->dwDstID;
		SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 玩家对请求交易的反馈
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePetExchangeReqRes(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_PetExchangeReqRes);

	if(GT_INVALID == pRecv->dwDstID)
	{
		return GT_INVALID;
	}

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Role *pApplicant = NULL;
	DWORD dwErrorCode = pRole->ProcPetExchangeReqRes(pApplicant, pRecv->dwDstID, pRecv->dwErrorCode);

	// 检查是否需要重置交易状态
	if(dwErrorCode != E_Pet_Exchange_Accept)
	{
		pRole->GetPetExchMgr().SetTgtRoleID(GT_INVALID);
	}

	// 检查是否需要发反馈消息
	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	// 为简化服务器端逻辑，所以情况均向双方发送消息，由客户端处理
	tagNS_PetExchangeReqRes send;
	send.dwSrcID = pRole->GetID();
	send.dwDstID = pRecv->dwDstID;
	send.dwErrorCode = dwErrorCode;

	if(E_Pet_Exchange_Accept == pRecv->dwErrorCode)
	{
		SendMessage(&send, send.dwSize);
	}

	if(P_VALID(pApplicant))
	{
		pApplicant->SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 添加交易物品
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePetExchangeAdd(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_PetExchangeAdd);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Role	*pTarget	= NULL;
	DWORD	dwErrorCode = pRole->ProcPetExchangeAdd(pTarget, pRecv->dwPetID);

	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	if(E_Success == dwErrorCode)
	{
		tagNS_PetExchangeAdd_2Dst send2Dst;
		send2Dst.dwSrcID	= pRole->GetID();
		send2Dst.dwPetID	= pRecv->dwPetID;

		pTarget->SendMessage(&send2Dst, send2Dst.dwSize);
	}

	tagNS_PetExchangeAdd_2Src send2Src;
	send2Src.dwDstID		= P_VALID(pTarget) ? pTarget->GetID() : GT_INVALID;
	send2Src.dwPetID		= pRecv->dwPetID;
	send2Src.dwErrorCode	= dwErrorCode;

	SendMessage(&send2Src, send2Src.dwSize);

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 取消交易物品
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePetExchangeDec(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_PetExchangeDec);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Role *pTarget = NULL;
	DWORD dwErrorCode = pRole->ProcPetExchangeDec(pTarget, pRecv->dwPetID);
	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_PetExchangeDec send;
	send.dwSrcID	= pRole->GetID();
	send.dwDstID	= P_VALID(pTarget) ? pTarget->GetID() : GT_INVALID;
	send.dwPetID	= pRecv->dwPetID;

	// 从交易清单中删除
	send.dwErrorCode = dwErrorCode;

	SendMessage(&send, send.dwSize);

	if(P_VALID(pTarget))
	{
		tagNS_PetExchangeDec_2Dst send2Dst;
		send2Dst.dwSrcID	= pRole->GetID();
		send2Dst.dwPetID	= pRecv->dwPetID;

		pTarget->SendMessage(&send2Dst, send2Dst.dwSize);
	}

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 修改交易金钱
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePetExchangeMoney(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_PetExchangeMoney);

	if(pRecv->n64Money < 0)
	{
		return GT_INVALID;
	}

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Role *pTarget = NULL;
	DWORD dwErrorCode = pRole->ProcPetExchangeMoney(pTarget, pRecv->n64Money);
	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_PetExchangeMoney send;
	send.dwSrcID	= pRole->GetID();
	send.n64Money	= pRecv->n64Money;
	send.dwErrorCode= dwErrorCode;

	SendMessage(&send, send.dwSize);
	if(P_VALID(pTarget))
	{
		pTarget->SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 锁定交易
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePetExchangeLock(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_PetExchangeLock);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Role *pTarget = NULL;
	DWORD dwErrorCode = pRole->ProcPetExchangeLock(pTarget);
	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	tagNS_PetExchangeLock send;
	send.dwSrcID = pRole->GetID();
	send.dwErrorCode = dwErrorCode;

	SendMessage(&send, send.dwSize);
	if(P_VALID(pTarget))
	{
		pTarget->SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}

//------------------------------------------------------------------------
// 玩家取消交易
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePetExchangeCancel(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_PetExchangeCancel);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Role *pTarget = NULL;
	DWORD dwErrorCode = pRole->ProcPetExchangeCancel(pTarget);

	tagNS_PetExchangeCancel send;
	send.dwSrcID = pRole->GetID();

	SendMessage(&send, send.dwSize);
	if(P_VALID(pTarget) && E_Success == dwErrorCode)
	{
		pTarget->SendMessage(&send, send.dwSize);
	}

	return E_Success;
}

//------------------------------------------------------------------------
// 确认交易
//------------------------------------------------------------------------
DWORD PlayerSession::HandleRolePetExchangeVerify(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_PetExchangeVerify);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Role *pTarget = NULL;
	DWORD dwFailedRoleID = GT_INVALID;
	DWORD dwErrorCode = pRole->ProcPetExchangeVerify(pTarget, dwFailedRoleID);
	if(GT_INVALID == dwErrorCode)
	{
		// 无需向客户端返回
		return dwErrorCode;
	}

	tagNS_PetExchangeFinish send;
	send.dwErrorCode	= dwErrorCode;
	send.dwFailedRoleID	= dwFailedRoleID;

	SendMessage(&send, send.dwSize);
	if(P_VALID(pTarget))
	{
		pTarget->SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}