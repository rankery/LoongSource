//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild_handler.h
// author: 
// actor:
// data: 2009-04-17
// last:
// brief: 帮派消息处理
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "player_session.h"
#include "role.h"
#include "role_mgr.h"
#include "guild.h"
#include "guild_mgr.h"
#include "guild_CofC.h"
#include "../WorldDefine/msg_guild.h"
//-----------------------------------------------------------------------------
// 创建帮派
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleCreateGuild(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildCreate);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = GT_INVALID;

	// NPC检查
	dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_Guild);
	if(E_Success == dwErrorCode)
	{
		INT32 nNameTCHAR = (pRecv->dwSize - FIELD_OFFSET(tagNC_GuildCreate, szGuildName)) / sizeof(TCHAR);
		dwErrorCode = g_guildMgr.CreateGuild(pRole, pRecv->szGuildName, nNameTCHAR);
	}

	// 向客户端反馈
	if(dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 解散帮派
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleDismissGuild(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildDismiss);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = GT_INVALID;

	// NPC检查
	dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_Guild);
	if(E_Success == dwErrorCode)
	{
		dwErrorCode = g_guildMgr.DismissGuild(pRole, pRole->GetGuildID());
	}

	// 向客户端反馈
	if(dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 邀请加入
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleJoinGuildReq(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildJoinReq);
	
	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 是否对自己操作
	if(pRole->GetID() == pRecv->dwDstRoleID)
	{
		return GT_INVALID;
	}

	// 得到帮派
	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// 处理
	Role *pInvitee = NULL;
	DWORD dwErrorCode = pGuild->CanInviteJoin(pRole->GetID(), pRecv->dwDstRoleID, &pInvitee);
	if(dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		ASSERT(P_VALID(pInvitee));

		tagNS_GuildJoinReq send;
		send.dwSrcRoleID	= pRole->GetID();
		send.dwGuildID		= pRole->GetGuildID();
		pInvitee->SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 被邀请玩家反馈
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleJoinGuildReqRes(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildJoinReqRes);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 得到帮派
	Guild *pGuild = g_guildMgr.GetGuild(pRecv->dwDstGuildID);
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	if(pRecv->dwErrorCode != E_Success)
	{
		// 清除该邀请帖信息
		pGuild->EraseInviteJoin(pRole->GetID());	

		// 检查邀请者是否仍在线
		Role *pInviter = g_roleMgr.GetRolePtrByID(pRecv->dwDstRoleID);
		if(P_VALID(pInviter))
		{
			// 向邀请者发消息
			tagNS_GuildJoinReqRes	send;
			send.dwErrorCode = E_Guild_Join_BeRefused;
			send.dwInviterID = pRecv->dwDstRoleID;
			send.dwInviteeID = pRole->GetID();
			pInviter->SendMessage(&send, send.dwSize);
		}

		return E_Success;
	}

	// 处理
	DWORD dwErrorCode = pGuild->InviteJoin(pRecv->dwDstRoleID, pRole->GetID());
	if(dwErrorCode != E_Success)
	{		
		// 向双方发消息
		tagNS_GuildJoinReqRes	send;
		send.dwErrorCode = dwErrorCode;
		send.dwInviterID = pRecv->dwDstRoleID;
		send.dwInviteeID = pRole->GetID();
		SendMessage(&send, send.dwSize);

		// 检查邀请者是否仍在线
		Role *pInviter = g_roleMgr.GetRolePtrByID(pRecv->dwDstRoleID);
		if(P_VALID(pInviter))
		{
			pInviter->SendMessage(&send, send.dwSize);
		}
	}
	else
	{
		// 帮派广播
		tagRoleInfo *pRoleInfo = g_roleMgr.GetRoleInfo(pRole->GetID());
		if (P_VALID(pRoleInfo))
		{
			INT32 nRoleNameCnt = _tcsclen(pRoleInfo->szRoleName);
			INT32 nMsgSz = sizeof(tagNS_GuildJoinBroad) + nRoleNameCnt * sizeof(TCHAR);

			MCREATE_MSG(pSend, nMsgSz, NS_GuildJoinBroad);
			pSend->dwRoleID		= pRole->GetID();
			pSend->dwRoleNameID	= pRole->GetNameID();

			memcpy(pSend->szRoleName, pRoleInfo->szRoleName, (nRoleNameCnt + 1) * sizeof(TCHAR));

			pGuild->SendGuildMsg(pSend, pSend->dwSize);
			MDEL_MSG(pSend);
		}

		// 同步到周围玩家
		tagGuildMember* pMember = pGuild->GetMember(pRole->GetID());
		Map*			pMap	= pRole->GetMap();
		if (P_VALID(pMember) && P_VALID(pMap))
		{
			tagNS_RemoteRoleGuildInfoChange send;
			send.dwGuildID		= pRecv->dwDstGuildID;
			send.dwRoleID		= pRole->GetID();
			send.n8GuildPos		= pMember->eGuildPos;

			pMap->SendBigVisTileMsg(pRole, &send, send.dwSize);
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 离开帮派
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleLeaveGuild(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildLeave);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 得到帮派
	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// 处理
	DWORD dwErrorCode = pGuild->LeaveGuild(pRole);
	if(dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		// 帮派广播
		tagNS_GuildLeaveBroad send;
		send.dwSrcRoleID = pRole->GetID();
		pGuild->SendGuildMsg(&send, send.dwSize);

		// 离开者
		SendMessage(&send, send.dwSize);

		// 同步到周围玩家
		Map* pMap	= pRole->GetMap();
		if (P_VALID(pMap))
		{
			tagNS_RemoteRoleGuildInfoChange send_broad;
			send_broad.dwGuildID	= GT_INVALID;
			send_broad.dwRoleID		= pRole->GetID();
			send_broad.n8GuildPos	= EGMP_Null;

			pMap->SendBigVisTileMsg(pRole, &send_broad, send_broad.dwSize);
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 从帮派中踢出
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleKickFromGuild(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildKick);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 是否对自己操作
	if(pRole->GetID() == pRecv->dwRoleID)
	{
		return GT_INVALID;
	}

	// 得到帮派
	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// 处理
	DWORD dwErrorCode = pGuild->KickMember(pRole->GetID(), pRecv->dwRoleID);
	if(dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		tagNS_GuildKickBroad send;
		send.dwDstRoleID = pRecv->dwRoleID;
		pGuild->SendGuildMsg(&send, send.dwSize);

		// 检查被踢者是否在线
		Role *pKicked = g_roleMgr.GetRolePtrByID(pRecv->dwRoleID);
		if(P_VALID(pKicked))
		{
			pKicked->SendMessage(&send, send.dwSize);

			// 同步到周围玩家
			Map* pMap	= pKicked->GetMap();
			if (P_VALID(pMap))
			{
				tagNS_RemoteRoleGuildInfoChange send_broad;
				send_broad.dwGuildID	= GT_INVALID;
				send_broad.dwRoleID		= pKicked->GetID();
				send_broad.n8GuildPos	= EGMP_Null;

				pMap->SendBigVisTileMsg(pKicked, &send_broad, send_broad.dwSize);
			}
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 移交帮主
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleTurnoverGuild(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildTurnover);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 是否对自己操作
	if(pRole->GetID() == pRecv->dwRoleID)
	{
		return GT_INVALID;
	}

	// 得到帮派
	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// 处理
	DWORD dwErrorCode = pGuild->TurnoverLeader(pRole->GetID(), pRecv->dwRoleID);
	if(dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		tagNS_GuildTurnoverBroad send;
		send.dwSrcRoleID	= pRole->GetID();
		send.dwDstRoleID	= pRecv->dwRoleID;
		pGuild->SendGuildMsg(&send, send.dwSize);

		// 同步到周围玩家(原帮主和先帮主)
		tagGuildMember* pMember = pGuild->GetMember(pRole->GetID());
		Map*			pMap	= pRole->GetMap();
		if (P_VALID(pMember) && P_VALID(pMap))
		{
			tagNS_RemoteRoleGuildInfoChange send_broad;
			send_broad.dwGuildID	= pRole->GetGuildID();
			send_broad.dwRoleID		= pRole->GetID();
			send_broad.n8GuildPos	= pMember->eGuildPos;

			pMap->SendBigVisTileMsg(pRole, &send_broad, send_broad.dwSize);
		}

		// 判断先帮主是否在线
		Role* pNewLeader = g_roleMgr.GetRolePtrByID(pRecv->dwRoleID);
		if (P_VALID(pNewLeader))
		{
			pMember = pGuild->GetMember(pNewLeader->GetID());
			pMap	= pNewLeader->GetMap();
			if (P_VALID(pMember) && P_VALID(pMap))
			{
				tagNS_RemoteRoleGuildInfoChange send_broad;
				send_broad.dwGuildID	= pNewLeader->GetGuildID();
				send_broad.dwRoleID		= pNewLeader->GetID();
				send_broad.n8GuildPos	= pMember->eGuildPos;

				pMap->SendBigVisTileMsg(pNewLeader, &send_broad, send_broad.dwSize);
			}
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 辞职
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleDemissFromGuild(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildDemiss);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 得到帮派
	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// 处理
	INT8 n8OldGuildPos = EGMP_BangZhong;
	DWORD dwErrorCode = pGuild->DemissPostion(pRole->GetID(), n8OldGuildPos);
	if(dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		tagNS_GuildDemissBroad send;
		send.dwRoleID		= pRole->GetID();
		send.n8OldGuildPos	= n8OldGuildPos;
		pGuild->SendGuildMsg(&send, send.dwSize);

		// 同步到周围玩家
		Map* pMap	= pRole->GetMap();
		if (P_VALID(pMap))
		{
			tagNS_RemoteRoleGuildInfoChange send_broad;
			send_broad.dwGuildID	= pRole->GetGuildID();
			send_broad.dwRoleID		= pRole->GetID();
			send_broad.n8GuildPos	= EGMP_BangZhong;

			pMap->SendBigVisTileMsg(pRole, &send_broad, send_broad.dwSize);
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 任命职位
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleAppointForGuild(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildAppoint);

	// 获取角色
	Role *pAppointor = GetRole();
	if(!P_VALID(pAppointor))
	{
		return GT_INVALID;
	}

	// 是否对自己操作
	if(pAppointor->GetID() == pRecv->dwRoleID)
	{
		return GT_INVALID;
	}

	// 得到帮派
	Guild *pGuild = g_guildMgr.GetGuild(pAppointor->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// 处理
	DWORD dwErrorCode = pGuild->Appoint(pAppointor->GetID(), pRecv->dwRoleID, pRecv->ePos);
	if(dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		tagNS_GuildAppointBroad send;
		send.dwSrcRoleID	= pAppointor->GetID();
		send.dwDstRoleID	= pRecv->dwRoleID;
		send.ePos			= pRecv->ePos;
		pGuild->SendGuildMsg(&send, send.dwSize);

		// 同步到周围玩家
		Role* pDstRole = g_roleMgr.GetRolePtrByID(pRecv->dwRoleID);
		if( P_VALID(pDstRole) && P_VALID(pDstRole->GetMap()) )
		{
			tagNS_RemoteRoleGuildInfoChange send_broad;
			send_broad.dwGuildID	= pDstRole->GetGuildID();
			send_broad.dwRoleID		= pRecv->dwRoleID;
			send_broad.n8GuildPos	= pRecv->ePos;

			pDstRole->GetMap()->SendBigVisTileMsg(pDstRole, &send_broad, send_broad.dwSize);
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 修改帮派宗旨
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleChangeGuildTenet(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildTenetChange);

	// 获取角色
	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 得到帮派
	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// 处理
	INT32 nTenetCnt = (pRecv->dwSize - FIELD_OFFSET(tagNC_GuildTenetChange, szTenet)) / sizeof(TCHAR);
	DWORD dwErrorCode = pGuild->ModifyTenet(pRole->GetID(), pRecv->szTenet, nTenetCnt);
	if(dwErrorCode != E_Success)
	{
		// 判断是否需要返回
		if (dwErrorCode != E_Return_NotNeedFeed)
		{
			g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
		}
	}
	else
	{
		INT32 nMsgSz = sizeof(tagNS_GuildTenetChangeBroad) + nTenetCnt * sizeof(TCHAR);
		MCREATE_MSG(pSend, nMsgSz, NS_GuildTenetChangeBroad);
		pSend->dwRoleID	= pRole->GetID();
		memcpy(pSend->szTenet, pRecv->szTenet, (nTenetCnt + 1) * sizeof(TCHAR));
		pSend->szTenet[nTenetCnt] = _T('\0');
		pGuild->SendGuildMsg(pSend, pSend->dwSize);
		MDEL_MSG(pSend);
	}

	return dwErrorCode;
}


//********************** 获取帮派相关信息消息处理 **********************//

//-----------------------------------------------------------------------------
// 获取帮派所有成员
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetGuildMembers(tagNetCmd* pCmd)
{
	//MGET_MSG(p, pCmd, NC_GuildGetAllMembers);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	if(!pRole->IsInGuild())
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		// 执行到此处，说明帮派管理有问题 -- 帮派里没有成员时才可以删除帮派
		ASSERT(0);
		return GT_INVALID;
	}

	pGuild->SendAllMembers2Client(pRole);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获取帮派指定成员扩展信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetGuildMemberEx(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_GuildGetMemberEx);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	if(!pRole->IsInGuild())
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		// 执行到此处，说明帮派管理有问题 -- 帮派里没有成员时才可以删除帮派
		ASSERT(0);
		return GT_INVALID;
	}

	return pGuild->SendSpecMemberEx2Client(pRole, p->dwRoleID);
}

//-----------------------------------------------------------------------------
// 刷新帮派指定成员信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleRefreshGuildMember(tagNetCmd* pCmd)
{
	MGET_MSG(p, pCmd, NC_GuildRefMember);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	if(!pRole->IsInGuild())
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		// 执行到此处，说明帮派管理有问题 -- 帮派里没有成员时才可以删除帮派
		ASSERT(0);
		return GT_INVALID;
	}

	return pGuild->SendSpecMember2Client(pRole, p->dwRoleID);
}

//-----------------------------------------------------------------------------
// 获取帮派名称
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetGuildName(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetGuildName);

	Guild *pGuild = g_guildMgr.GetGuild(pRecv->dwGuildID);
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	return pGuild->SendGuildName2Client(this);
}

//-----------------------------------------------------------------------------
// 获取帮派宗旨
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetGuildTenet(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetGuildTenet);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	return pGuild->SendGuildTenet2Client(this);
}

//-----------------------------------------------------------------------------
// 获取帮派仓库中物品信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetGuildWareItems(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetGuildWare);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// NPC检查
	DWORD dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_GuildWare);
	if(E_Success != dwErrorCode)
	{
		return GT_INVALID;
	}

	// 创建足够大的缓冲区
	MCREATE_MSG(pSend, TEMP_GUILD_BUFF_SIZE, NS_GetGuildWare);
	pSend->dwLastUpdateTime = pRecv->dwLastUpdateTime;

	INT32 nItemSize = 0;
	pSend->dwErrorCode = pGuild->GetGuildWarehouse().GetGuildWareInfo(pSend->byData, pSend->nItemNum, pSend->dwLastUpdateTime, pSend->nSzGuildWare, nItemSize);

	// 重新设置消息大小
	pSend->dwSize = sizeof(tagNS_GetGuildWare) + nItemSize;

	// 发送给客户端
	SendMessage(pSend, pSend->dwSize);

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取帮派仓库操作权限列表
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetGuildWarePriList(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetGuildWarePriList);

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// NPC检查
	DWORD dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_GuildWare);
	if(E_Success == dwErrorCode)
	{
		dwErrorCode = pGuild->GetGuildWarehouse().SendGuildWarePriList2Client(pRole);
	}

	if (GT_VALID(dwErrorCode) && (E_Success != dwErrorCode))
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}

	return GT_INVALID;
}

//-----------------------------------------------------------------------------
// 变更指定成员的帮派仓库操作权限
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGuildWarePrivilege(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildWarePri);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// NPC检查
	DWORD dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_GuildWare);
	if(E_Success == dwErrorCode)
	{
		dwErrorCode = pGuild->SetGuildWarePrivilege(pRole, pRecv->dwRoleID, pRecv->bCanUse);
	}
	
	if (!GT_VALID(dwErrorCode))
	{
		return GT_INVALID;
	}

	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		// 通知操作者更新列表
		tagNS_GuildWarePri send;
		send.dwRoleID	= pRecv->dwRoleID;
		send.bCanUse	= pRecv->bCanUse;
		SendMessage(&send, send.dwSize);

		// 通知被操作者权限变更
		Role* pMember = g_roleMgr.GetRolePtrByID(pRecv->dwRoleID);
		if (P_VALID(pMember))
		{
			pMember->SendMessage(&send, send.dwSize);
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取帮派设施升级信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetGuildFacilitiesInfo(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetGuildUpgradeInfo);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// NPC检查
	DWORD dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_GuildMgr);
	if(E_Success == dwErrorCode)
	{
		dwErrorCode = pGuild->SendFacilitiesInfo2Client(pRole);
	}

	if (GT_VALID(dwErrorCode) && (E_Success != dwErrorCode))
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 玩家上缴帮派设施升级所需物品
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleHandInItems(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_HandInItems);

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	// NPC检查
	DWORD dwErrorCode = pRole->CheckFuncNPC(pRecv->dwNPCID, EFNPCT_GuildMgr);
	if(E_Success == dwErrorCode)
	{
		dwErrorCode = pGuild->GetGuildFacilities().HandInItems(pRole, pRecv->eType);
	}

	if (GT_VALID(dwErrorCode))
	{
		if (E_Success != dwErrorCode)
		{
			g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
		}
		else
		{
			// 返回变化后的设施信息
			tagNS_UpdateFacilitiesInfo send;
			pGuild->GetGuildFacilities().GetGuildFacilitiesInfo(&send.sFacilitiesInfo, pRecv->eType);
			pRole->SendMessage(&send, send.dwSize);
		}
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 发布帮派事务
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleSpreadGuildAffair(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SpreadGuildAffair);
	if (!GT_VALID(pRecv->dwBuffID))
	{
		return GT_INVALID;
	}

	Role *pRole = GetRole();
	if(!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pGuild->GetGuildAffair().SpreadGuildAffair(pRole, pRecv->dwBuffID);

	if (GT_VALID(dwErrorCode) && (dwErrorCode != E_Success))
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 取得帮派技能信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetGuildSkillInfo(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild *pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if(!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	MCREATE_MSG(pSend, TEMP_GUILD_BUFF_SIZE, NS_GetGuildSkillInfo);
	DWORD dwErrorCode = pGuild->GetGuildSkill().GetGuildSkillInfo(pSend->dwCurSkillID, pSend->n16Progress,
		pSend->nSkillNum, pSend->dwSkillInfo);

	// 重新计算消息大小
	if (dwErrorCode == E_Success)
	{
		pSend->dwSize = sizeof(tagNS_GetGuildSkillInfo) + (pSend->nSkillNum - 1) * sizeof(DWORD);
		SendMessage(pSend, pSend->dwSize);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 上缴帮派技能典籍
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleUpgradeGuildSkill(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GuildSkillUpgrade);
	if (!GT_VALID(pRecv->n64ItemSerial))
	{
		return GT_INVALID;
	}

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	tagNS_GuildSkillUpgrade send;
	DWORD dwErrorCode = pGuild->GetGuildSkill().SkillUpgradeByHandIn(pRole, pRecv->n64ItemSerial, send.dwSkillID, send.n16Progress);
	if (E_Success == dwErrorCode)
	{
		SendMessage(&send, send.dwSize);
	}
	else if (GT_VALID(dwErrorCode))
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 学习帮派技能
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleLearnGuildSkill(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_LearnGuildSkill);
	if (!GT_VALID(pRecv->dwSkillID))
	{
		return GT_INVALID;
	}

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	tagNS_LearnGuildSkill send;
	BOOL bLearnError = FALSE;
	INT nLevel = 0;
	DWORD dwErrorCode = pGuild->GetGuildSkill().LearnGuildSkill(pRole, pRecv->dwSkillID, bLearnError, nLevel);

	if (GT_VALID(dwErrorCode))
	{
		if (bLearnError)
		{
			send.dwErrorCode	= dwErrorCode;
			send.dwSkillID		= pRecv->dwSkillID * 100 + nLevel;
			SendMessage(&send, send.dwSize);
		}
		else if (dwErrorCode != E_Success)
		{
			g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
		}
	}
	
	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 设置当前研究帮派技能
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleSetResearchSkill(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SetResearchSkill);

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	tagNS_SetResearchSkill send;
	INT nLevel = 0;
	DWORD dwErrorCode = pGuild->GetGuildSkill().SetCurUpgradeGuildSkill(pRole->GetID(), pRecv->dwSkillID, nLevel, send.n16Progress);
	if (!GT_VALID(dwErrorCode))
	{
		return GT_INVALID;
	}

	if (dwErrorCode != E_Success)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		if (GT_VALID(pRecv->dwSkillID))
		{
			send.dwSkillID	= pRecv->dwSkillID * 100 + nLevel;
		}
		pGuild->SendGuildMsg(&send, send.dwSize);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 同步帮派基本信息
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleSyncGuildInfo(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	tagNS_SyncGuildInfo send;
	send.sGuildInfo = (tagGuildBase)pGuild->GetGuildAtt();

	SendMessage(&send, send.dwSize);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获取售卖商会信息	-- 地图线程
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetCofCInfo(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetCofCInfo);

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	GuildCofC* pCofC = pMap->GetCofC(pRecv->dwNPCID);
	if (!P_VALID(pCofC))
	{
		// 不需要通知客户端
		return GT_INVALID;
	}

	return pCofC->SendCommerceGoodsInfo2Role(pRole);
}

//-----------------------------------------------------------------------------
// 关闭商会界面		-- 地图线程
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleCloseCofC(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_CloseCofC);

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	Map* pMap = pRole->GetMap();
	if (!P_VALID(pMap))
	{
		return GT_INVALID;
	}

	GuildCofC* pCofC = pMap->GetCofC(pRecv->dwNPCID);
	if (!P_VALID(pCofC))
	{
		return GT_INVALID;
	}

	// 移出观察者列表
	pCofC->RemoveObserver(pRole->GetID());

	return E_Success;
}

//-----------------------------------------------------------------------------
// 购买商会商货		-- 地图线程
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleBuyCofCGoods(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_BuyCofCGoods);

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pGuild->GetGuildCommerce().BuyGoods(pRole, pRecv->dwNPCID, pRecv->dwGoodID, pRecv->byBuyNum);
	if ((E_Success != dwErrorCode) && GT_VALID(dwErrorCode))
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 出售商货给商会	-- 地图线程
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleSellCofCGoods(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SellCofCGoods);

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pGuild->GetGuildCommerce().SellGoods(pRole, pRecv->dwNPCID, pRecv->dwGoodID, pRecv->bySellNum);
	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取跑商容器信息	-- 地图线程
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetCommodityInfo(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_GetCommodityInfo);

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	INT nMsgSize = sizeof(tagNS_GetCommodityInfo) + sizeof(tagCommerceGoodInfo) * (MAX_COMMODITY_CAPACITY - 1);
	MCREATE_MSG(pSend, nMsgSize, NS_GetCommodityInfo);
	// 读取商货信息
	DWORD dwErrorCode = pGuild->GetGuildCommerce().GetCommodityGoodInfo(pRole, pSend->sGoodInfo, pSend->nGoodNum, pSend->nCurTael, pSend->nLevel);

	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		// 调整消息大小
		pSend->dwSize = sizeof(tagNS_GetCommodityInfo) + sizeof(tagCommerceGoodInfo) * (pSend->nGoodNum - 1);
		SendMessage(pSend, pSend->dwSize);
	}

	MDEL_MSG(pSend);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 获取跑商初始信息	-- 地图线程
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetTaelInfo(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	tagNS_GetTaelInfo send;
	tagTaelInfo sTaelInfo;
	DWORD dwErrorCode = pGuild->GetGuildCommerce().GetCommerceBeginningInfo(pRole->GetID(), send.nLevel, sTaelInfo);
	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}

//-----------------------------------------------------------------------------
// 获取帮派跑商排行	-- 地图线程
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleGetCommerceRank(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	INT nMsgSize = sizeof(tagNS_GetCommerceRank) + sizeof(tagCommerceRank) * (MAX_COMMERCE_RANK_INFO - 1);
	MCREATE_MSG(pSend, nMsgSize, NS_GetCommerceRank);
	// 读取排行信息
	DWORD dwErrorCode = pGuild->GetGuildCommerce().GetCommerceRankInfo(pSend->sRankInfo, pSend->nRankNum, pSend->bCommend);

	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		// 调整消息大小
		pSend->dwSize = sizeof(tagNS_GetCommerceRank) + sizeof(tagCommerceRank) * (pSend->nRankNum - 1);
		SendMessage(pSend, pSend->dwSize);
	}

	MDEL_MSG(pSend);

	return E_Success;
}

//-----------------------------------------------------------------------------
// 接收跑商
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleAcceptCommerce(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pGuild->GetGuildCommerce().AcceptCommerce(pRole);
	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		INT nLevel;
		tagTaelInfo sTaelInfo;
		tagNS_AcceptCommerce send;
		dwErrorCode = pGuild->GetGuildCommerce().GetCommerceBeginningInfo(pRole->GetID(), nLevel, sTaelInfo);
		if (dwErrorCode == E_Success)
		{
			send.nBeginningTael = sTaelInfo.nBeginningTael;
			SendMessage(&send, send.dwSize);
		}
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 上缴商银
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleCompleteCommerce(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	INT32 nFund = 0;
	DWORD dwErrorCode = pGuild->GetGuildCommerce().CompleteCommerce(pRole, nFund);
	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		tagNS_CompleteCommerce send;
		send.dwRoleID	= pRole->GetID();
		send.nFund		= nFund;
		pGuild->SendGuildMsg(&send, send.dwSize);
	}

	return E_Success;
}

//-----------------------------------------------------------------------------
// 放弃跑商
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleAbandonCommerce(tagNetCmd* pCmd)
{
	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pGuild->GetGuildCommerce().AbandonCommerce(pRole->GetID());
	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	
	return E_Success;
}

//-----------------------------------------------------------------------------
// 设置跑商嘉奖状态
//-----------------------------------------------------------------------------
DWORD PlayerSession::HandleSwitchCommendation(tagNetCmd* pCmd)
{
	MGET_MSG(pRecv, pCmd, NC_SwitchCommendation);

	Role* pRole = GetRole();
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 取得帮派信息
	Guild* pGuild = g_guildMgr.GetGuild(pRole->GetGuildID());
	if (!P_VALID(pGuild))
	{
		return GT_INVALID;
	}

	DWORD dwErrorCode = pGuild->GetGuildCommerce().SwitchCommendation(pRole->GetID(), pRecv->bSwitchON);
	if (E_Success != dwErrorCode)
	{
		g_guildMgr.SendGuildProcFailed2Client(this, dwErrorCode);
	}
	else
	{
		tagNS_SwitchCommendation send;
		send.dwRoleID	= pRole->GetID();
		send.bSwitchON	= pRecv->bSwitchON;
		pGuild->SendGuildMsg(&send, send.dwSize);
	}

	return E_Success;
}