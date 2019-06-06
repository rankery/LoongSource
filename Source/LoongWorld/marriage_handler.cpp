//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: marriage_handler.cpp
// author: Sxg
// actor:
// data: 2009-09-10
// last:
// brief: 玩家结婚和婚礼的消息函数
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "player_session.h"
#include "map.h"
#include "role.h"
#include "role_mgr.h"
#include "title_mgr.h"
#include "marriage.h"
#include "../WorldDefine/msg_marriage.h"
#include "../WorldDefine/msg_social.h"
#include "../ServerDefine/msg_marriage.h"
#include "../ServerDefine/role_data.h"
//////////////////////////////////////////////////////////////////////////
//结婚请求
//////////////////////////////////////////////////////////////////////////
DWORD PlayerSession::HandleGetMarriageReq(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_GetMarriageReq);

	if(GT_INVALID == pRecv->dwDstID)
	{
		return GT_INVALID;
	}

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		IMSG(_T("Cant't find the role object of the session when tackling the GetMarriageReq message\r\n"));
		return GT_INVALID;
	}

	Role* pTarget = NULL;
	DWORD dwErrorCode = pRole->ProcGetMarriageReq(pTarget, pRecv->dwDstID,pRecv->dwNpcID);
	if(E_Marriage_Accept == dwErrorCode)
	{
		if(P_VALID(pTarget))
		{
			// 向被申请者发消息
			tagNS_GetMarriageReq send;
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
		tagNS_GetMarriageReqRes send;
		send.dwErrorCode	= dwErrorCode;
		send.dwSrcID		= pRole->GetID();
		send.dwDstID		= pRecv->dwDstID;
		SendMessage(&send, send.dwSize);
	}
	return dwErrorCode;
}

//////////////////////////////////////////////////////////////////////////
//将被请求者的反馈信息 转发给请求者
//////////////////////////////////////////////////////////////////////////
DWORD PlayerSession::HandleGetMarriageReqRes(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_GetMarriageReqRes);

	if(GT_INVALID == pRecv->dwDstID)
	{
		return GT_INVALID;
	}

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		IMSG(_T("Cant't find the role object of the session when tackling the GetMarriageReqRes message\r\n"));
		return GT_INVALID;
	}
	Role *pApplicant = NULL;
	DWORD dwErrorCode = pRole->ProcGetMarriageReqRes(pApplicant, pRecv->dwDstID, pRecv->dwErrorCode);

	// 检查是否需要发反馈消息
	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	//两边都返信息
	tagNS_GetMarriageReqRes send;
	send.dwSrcID = pRole->GetID();
	send.dwDstID = pRecv->dwDstID;
	send.dwErrorCode = dwErrorCode;
	
	SendMessage(&send, send.dwSize);
	if(P_VALID(pApplicant))
	{
		pApplicant->SendMessage(&send, send.dwSize);
	}

	if(E_Marriage_Accept == dwErrorCode)
	{	
		tagFriend* pFriRole = pRole->GetFriendPtr(pApplicant->GetID());
		if(P_VALID(pFriRole))
		{
			//称号
			pApplicant->GetTitleMgr()->SigEvent(ETE_MARRY_JOIN, GT_INVALID,	pFriRole->dwFriVal);
			pRole->GetTitleMgr()->SigEvent(ETE_MARRY_JOIN, GT_INVALID,	pFriRole->dwFriVal);
		}

		//更新loverid
		pApplicant->UpdateLoverID(pRole->GetID());
		pRole->UpdateLoverID(pApplicant->GetID());

		// 通知客户端
		tagNS_RemoteChangeSpouse sendBigVis;
		sendBigVis.dwRoleID	= pRole->GetID();
		sendBigVis.dwLoverID = pRole->GetLoverID();
		sendBigVis.bHaveWedding = pRole->IsRoleWedded();
		pRole->GetMap()->SendBigVisTileMsg(pRole, &sendBigVis, sendBigVis.dwSize);

		sendBigVis.dwRoleID	= pApplicant->GetID();
		sendBigVis.dwLoverID = pApplicant->GetLoverID();
		sendBigVis.bHaveWedding = pApplicant->IsRoleWedded();
		pApplicant->GetMap()->SendBigVisTileMsg(pApplicant, &sendBigVis, sendBigVis.dwSize);

		//往db发消息 记录结婚信息
		tagNDBC_RoleGetMarriage send;
		send.dwRoleID1 = pRole->GetID();
		send.dwRoleID2 = pRecv->dwDstID;
		g_dbSession.Send(&send,send.dwSize);

		//调用脚本
		g_ScriptMgr.GetRoleScript()->OnGetMarriage(pApplicant, pApplicant->GetID(), pRole->GetID());
	}
	return dwErrorCode;
}

//////////////////////////////////////////////////////////////////////////
//离婚申请
//////////////////////////////////////////////////////////////////////////
DWORD PlayerSession::HandleBreakMarriageReq(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_BreakMarriageReq);

	if(GT_INVALID == pRecv->dwDstID)
	{
		return GT_INVALID;
	}

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		IMSG(_T("Cant't find the role object of the session when tackling the BreakMarriageReq message\r\n"));
		return GT_INVALID;
	}

	Role* pTarget = NULL;
	DWORD dwErrorCode = pRole->ProcBreakMarriageReq(pTarget, pRecv->dwDstID);
	if(E_Marriage_Accept == dwErrorCode)
	{
		if(P_VALID(pTarget))
		{
			// 向被申请者发消息
			tagNS_BreakMarriageReq send;
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
		tagNS_BreakMarriageReqRes send;
		send.dwErrorCode	= dwErrorCode;
		send.dwSrcID		= pRole->GetID();
		send.dwDstID		= pRecv->dwDstID;
		SendMessage(&send, send.dwSize);
	}
	return dwErrorCode;
}
//////////////////////////////////////////////////////////////////////////
//离婚申请的反馈
//////////////////////////////////////////////////////////////////////////
DWORD PlayerSession::HandleBreakMarriageReqRes(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_BreakMarriageReqRes);

	if(GT_INVALID == pRecv->dwDstID)
	{
		return GT_INVALID;
	}

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		IMSG(_T("Cant't find the role object of the session when tackling the BreakMarriageReqRes message\r\n"));
		return GT_INVALID;
	}
	Role *pApplicant = NULL;
	DWORD dwErrorCode = pRole->ProcBreakMarriageReqRes(pApplicant, pRecv->dwDstID, pRecv->dwErrorCode);

	// 检查是否需要发反馈消息
	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}

	//两边都返信息
	tagNS_BreakMarriageReqRes send;
	send.dwSrcID = pRole->GetID();
	send.dwDstID = pRecv->dwDstID;
	send.dwErrorCode = dwErrorCode;

	pRole->SendMessage(&send, send.dwSize);
	if(P_VALID(pApplicant))
	{
		pApplicant->SendMessage(&send, send.dwSize);
	}

	if(E_Marriage_Accept == dwErrorCode)
	{	
//////////////////////////////////////////////////////////////////////////
		tagFriend* pFriRole = pApplicant->GetFriendPtr(pRole->GetID());
		if(P_VALID(pFriRole))
		{
			//称号
			pApplicant->GetTitleMgr()->SigRemoveEvent(ETE_MARRY_JOIN, GT_INVALID);
			pRole->GetTitleMgr()->SigRemoveEvent(ETE_MARRY_JOIN, GT_INVALID);
		}
//////////////////////////////////////////////////////////////////////////
		if(P_VALID(pFriRole))
		{
			pFriRole->dwFriVal = 0;
			//跟客户端同步友好度
			tagNS_UpdateFriValue sendfri;
			sendfri.dwRoleID = pRole->GetID();
			sendfri.nFriVal = 0;
			pApplicant->SendMessage(&sendfri,sendfri.dwSize);

			//友好度归零
			pFriRole = pRole->GetFriendPtr(pApplicant->GetID());
			pFriRole->dwFriVal = 0;
			//跟客户端同步友好度
			sendfri.dwRoleID = pApplicant->GetID();
			sendfri.nFriVal = 0;
			pRole->SendMessage(&sendfri,sendfri.dwSize);
		}
//////////////////////////////////////////////////////////////////////////
		// 通知客户端
		tagNS_RemoteChangeSpouse sendBigVis;
		sendBigVis.dwRoleID	= pRole->GetID();
		sendBigVis.dwLoverID = GT_INVALID;
		sendBigVis.bHaveWedding = pRole->IsRoleWedded();
		pRole->GetMap()->SendBigVisTileMsg(pRole, &sendBigVis, sendBigVis.dwSize);

		sendBigVis.dwRoleID	= pApplicant->GetID();
		sendBigVis.dwLoverID = GT_INVALID;
		sendBigVis.bHaveWedding = pApplicant->IsRoleWedded();
		pApplicant->GetMap()->SendBigVisTileMsg(pApplicant, &sendBigVis, sendBigVis.dwSize);
//////////////////////////////////////////////////////////////////////////
		//更新loverid
		pApplicant->UpdateLoverID(GT_INVALID);
		pRole->UpdateLoverID(GT_INVALID);
//////////////////////////////////////////////////////////////////////////
		//往db发消息更新友好度数据
		tagNDBC_UpdateFriVal sendDB;
		sendDB.dwRoleID = (pApplicant->GetID() < pRole->GetID()) ? pApplicant->GetID() : pRole->GetID();
		sendDB.FriendshipSave.dwFriendID = (pApplicant->GetID() > pRole->GetID()) ? pApplicant->GetID() : pRole->GetID();
		sendDB.FriendshipSave.nFriVal = 0;
		g_dbSession.Send(&sendDB, sendDB.dwSize);
//////////////////////////////////////////////////////////////////////////
		//往db发消息 记录离婚信息
		tagNDBC_RoleBreakMarriage send;
		send.dwRoleID1 = pRole->GetID();
		send.dwRoleID2 = pRecv->dwDstID;
		g_dbSession.Send(&send,send.dwSize);
//////////////////////////////////////////////////////////////////////////
		//调用脚本
		g_ScriptMgr.GetRoleScript()->OnBreakMarriage(pApplicant, pApplicant->GetID(), pRole->GetID());

	}
	return dwErrorCode;
}
//////////////////////////////////////////////////////////////////////////
//申请婚礼的请求
//////////////////////////////////////////////////////////////////////////
DWORD PlayerSession::HandleCelebrateWeddingReq(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_CelebrateWeddingReq);

	// 获取角色
	Role* pRole = GetRole();
	if(!P_VALID(pRole))
	{
		IMSG(_T("Cant't find the role object of the session when tackling the CelebrateWeddingReq message\r\n"));
		return GT_INVALID;
	}

	//
	Role *pTarget = NULL;
	Role *pOfficiatorRole = NULL;
	DWORD dwErrorCode = pRole->ProcCelebrateWeddingReq(pTarget,pOfficiatorRole,pRecv->dwOfficiatorNameCrc,pRecv->dwNpcID);

	// 检查是否需要发反馈消息
	if(GT_INVALID == dwErrorCode)
	{
		return dwErrorCode;
	}
	
	//如果一起ok 往主婚人发请求消息
	if(E_Wedding_Success == dwErrorCode)
	{	
		//这个时候将夫妻双方和主婚人set到夫妻系统管理对象中
		//并等待主婚人回复 来决定是系统自动主持 还是主婚人主持
		g_MarriageMgr.SetApplicantRoleID(pRole->GetID());
		g_MarriageMgr.SetTargetRoleID(pTarget->GetID());

		//此时并非真正开始 需要等待主婚人的返回信息
		//并在g_MarriageMgr.update中检测3个人的状态
		g_MarriageMgr.BegWedding();
		//
		if(P_VALID(pOfficiatorRole))
		{	
			g_MarriageMgr.SetOfficiatorRoleID(pOfficiatorRole->GetID());
			// 转发给主婚人
			tagNS_CelebrateWeddingReq send;
			send.dwOfficiatorNameCrc = pRecv->dwOfficiatorNameCrc; 
			pOfficiatorRole->SendMessage(&send, send.dwSize);
		}
		//系统主持婚礼
		else 
		{
			//直接返回去 等待婚礼的update信息
			tagNC_CelebrateWeddingReqRes send;
			send.dwErrorCode = E_Wedding_Success;
			pRole->SendMessage(&send, send.dwSize);
		}
	}
	//回复申请人
	else if(dwErrorCode != GT_INVALID)
	{
		// 回馈申请人
		tagNS_CelebrateWeddingReqRes send;
		send.dwErrorCode = dwErrorCode; 
		pRole->SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}

//////////////////////////////////////////////////////////////////////////
//主婚人的反馈
//////////////////////////////////////////////////////////////////////////
DWORD PlayerSession::HandleCelebrateWeddingReqRes(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NC_CelebrateWeddingReqRes);

	//获得主婚人
	Role* pOfficiator = GetRole();
	if(!P_VALID(pOfficiator))
	{
		IMSG(_T("Cant't find the role object of the session when tackling the CelebrateWeddingReqRes message\r\n"));
		return GT_INVALID;
	}

	Role *pApplicant = NULL;
	DWORD dwErrorCode = pOfficiator->ProcCelebrateWeddingReqRes(pApplicant,pRecv->dwErrorCode);

	if(dwErrorCode != GT_INVALID && P_VALID(pOfficiator))
	{
		//直接返回去　等待申请人发出update消息
		tagNC_CelebrateWeddingReqRes send;
		send.dwErrorCode = dwErrorCode;
		pApplicant->SendMessage(&send, send.dwSize);
	}
	else
	{
		//这表示结婚申请者 下线 或切换地图了 应该结束婚礼
		g_MarriageMgr.EndWedding();
	}
	return dwErrorCode;
};
/*
//////////////////////////////////////////////////////////////////////////
//当有主婚人时该消息由主婚人发送。
//当由系统主持婚礼时该消息由婚礼申请人发送。
//////////////////////////////////////////////////////////////////////////
DWORD PlayerSession::HandleUpdateWeddingStepReq(tagNetCmd* pCmd)
{
	// 接收网络消息
	MGET_MSG(pRecv, pCmd, NS_UpdateWeddingStepReq);




}
*/