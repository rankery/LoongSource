#include "stdafx.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "channel_mgr.h"
#include "chat_mgr.h"
#include "role.h"
#include "ps_bomb.h"
#include "role_mgr.h"
#include "map.h"
#include "guild.h"
#include "guild_mgr.h"
#include "title_mgr.h"
#include "chat_logger.h"

ChannelMgr g_channelMgr;
// struct SendFilter
// {
// 	SendFilter(ESendChatChannel eChannel)
// 		:m_eChannel(eChannel){}
// 	BOOL operator()(Unit* pUnit)
// 	{
// 		Role* pRole = dynamic_cast<Role*>(pUnit);
// 		if (!P_VALID(pRole))
// 		{
// 			return FALSE;
// 		}
// 		
// 		return TRUE;//pRole->GetChatData()->IsFit(m_eChannel);
// 	}
// 
// private:
// 	ESendChatChannel m_eChannel;
// };

DWORD ChannelMgr::SendMsgCmd(MsgCmd* pMsgCmd)
{
	MTRANS_ELSE_RET(pSender, pMsgCmd->GetSender(), Role, E_SystemError);
	MTRANS_ELSE_RET(pMap, pSender->GetMap(), Map, E_SystemError);
	MTRANS_ELSE_RET(pSend, pMsgCmd->GetSendMsg(), tagNetCmd, E_SystemError);
	MTRANS_ELSE_RET(pRecv, pMsgCmd->GetRecvMsg(), tagNetCmd, E_SystemError);
	ESendChatChannel eChannel = pMsgCmd->GetChannelType();

	//长度限制
	if ( pMsgCmd->GetContentSize() > GetContentSizeLimit(pSend, eChannel) )
		return E_RoleChat_TooLength;


	// 根据聊天频道不同分别处理
	switch( eChannel )
	{
		// 当前频道
	case ESCC_Current:
		{
			// 给客户端返回消息
			tagNC_RoleChat* pRolcChat = static_cast<tagNC_RoleChat*>(pRecv);
			if(!g_pSGuarder.OnMsg(pRolcChat->szMsg, 0))
			{
				pMap->ForEachUnitInBigVisTile(pSender, SendAction(eChannel, pSend));
				//pMap->SendBigVisTileMsg(pSender, (LPVOID)pSend, pSend->dwSize);
			}
		}
		break;
		// 私聊
	case ESCC_Pvt:
		{
			//判断玩家是否存在
			MTRANS_ELSE_RET(pDestRoleInfo, g_roleMgr.GetRoleInfo(pMsgCmd->GetReceiverID()), tagRoleInfo, E_RoleChat_Pvt_NoRoleName);
			//判断玩家是否在线
			MTRANS_ELSE_RET(pDestRole, g_roleMgr.GetRolePtrByID(pMsgCmd->GetReceiverID()), Role, E_RoleChat_Pvt_RemoteRoleLeave);
			pDestRole->SendMessage(pSend, pSend->dwSize);
			if (pSender != pDestRole)
			{
				pSender->SendMessage(pSend, pSend->dwSize);	
			}			
		}
		break;
		// 世界
	case ESCC_World:
		{
			// 判断时间间隔
			if (!pSender->TalkToWorld())
				return E_RoleChat_World_Frequently;

			// 玩家等级判断
			if (pSender->GetLevel() < WORLD_CHANNEL_ROLE_LVL)
				E_RoleChat_World_RoleNo10;

			//在世界频道喊话需要扣除 100 银
			//得到玩家身上的银 总数
			INT64 currSilver = pSender->GetCurMgr().GetBagSilver();
			if ( currSilver < WORLD_CHANNEL_DEC_SLIVER )
			{
				//银不够
				return E_RoleChat_World_NoMoney;
			}
			else
			{
				//扣除玩家背包中 100银
				pSender->GetCurMgr().DecBagSilver(WORLD_CHANNEL_DEC_SLIVER, ELCLD_CHAT_WORLD_CHANNEL);
			}
			// 给客户端返回消息
			g_roleMgr.ForEachRoleInWorld(SendAction(eChannel, pSend));
			//g_roleMgr.SendWorldMsg((LPVOID)pSend, pSend->dwSize);
		}
		break;
		//公告
	case ESCC_Affiche:
		{
			//判断发言人是不是GM权限
			if (pSender->GetSession()->IsPrivilegeEnough(1) == FALSE)
				return E_RoleChat_Not_GM;

			g_roleMgr.ForEachRoleInWorld(SendAction(eChannel, pSend));
			//g_roleMgr.SendWorldMsg((LPVOID)pSend, pSend->dwSize);
		}
		break;
		// 队伍
	case ESCC_Troop:
		{
			//根据玩家队伍ID找到相对应的队伍,给队伍所有成员发送消息
			DWORD dwTeamID = pSender->GetTeamID();
			const Team* pTeam = g_groupMgr.GetTeamPtr(dwTeamID);
			if (P_VALID(pTeam))
			{
				pTeam->ForEachRoleInTeam(SendAction(eChannel, pSend));
			}
			//g_groupMgr.SendTeamMessage(dwTeamID, (LPVOID)pSend, pSend->dwSize);
		}
		break;
		// 传音
	case ESCC_CastTalk:
		{
			TList<tagItem*> listItems;
			INT nNum = pSender->GetItemMgr().GetBagSameItemList(listItems, TYPEID_IM_CASTTALK);

			//无道具
			if (listItems.Empty())
				return E_RoleChat_CastTalk_NoIMItem;

			// 使用物品
			TList<tagItem*>::TListIterator itr = listItems.Begin();
			pSender->GetItemMgr().ItemUsedFromBag((*itr)->GetKey(), (INT16)1, (DWORD)ELCLD_CAST_TALK_IM);

			// 称号事件触发
			pSender->GetTitleMgr()->SigEvent(ETE_SHOUT, GT_INVALID, GT_INVALID);

			// 给客户端返回消息
			g_roleMgr.ForEachRoleInWorld(SendAction(eChannel, pSend));
			//g_roleMgr.SendWorldMsg((LPVOID)pSend, pSend->dwSize);
		}
		break;
		// 帮派
	case ESCC_Guild:
		{
			// 给客户端返回消息
			MTRANS_ELSE_RET(pGuild, g_guildMgr.GetGuild(pSender->GetGuildID()), Guild, E_RoleChat_Guild_NoJoin);
			pGuild->ForEachRoleInGuild(SendAction(eChannel, pSend));
			//pGuild->SendGuildMsg(pSend, pSend->dwSize);
		}
		break;
	}

	TheChatLogger.Log(*pMsgCmd);
	return E_Success;
}

DWORD ChannelMgr::GetContentSizeLimit( tagNetCmd* pNetCmd, BYTE byChannelType )
{
	static const tagNS_RoleChat			msgChat;
	static const tagNS_RoleShowEquip	msgShowEquip;
	static const tagNS_RoleShowItem		msgShowItem;

	DWORD dwSizeLimit = 1024;
	if (msgChat.dwID == pNetCmd->dwID)
	{
		switch(ESendChatChannel(byChannelType))
		{
		case ESCC_CastTalk:
			dwSizeLimit = WORLD_CHANNEL_MSG_LEN;
			break;
		case ESCC_World:
			dwSizeLimit = MAX_CHAT_LEN;
			break;
		case ESCC_Pvt:
		case ESCC_Guild:
		case ESCC_Troop:
		case ESCC_Current:
			dwSizeLimit = MAX_CHAT_LEN;
			break;
		case ESCC_Leave:
		case ESCC_Combat:
		case ESCC_Affiche:
		case ESCC_GM:
		case ESCC_System:
		default:
			break;
		}
	}
	else if (msgShowEquip.dwID == pNetCmd->dwID)
	{
		dwSizeLimit = sizeof(tagEquip);
	}
	else if (msgShowItem.dwID == pNetCmd->dwID)
	{
		dwSizeLimit = sizeof(tagItem);
	}

	return dwSizeLimit;
}

