#include "stdafx.h"
#include "chat_mgr.h"
#include "role.h"
#include "../ServerDefine/msg_leftmsg.h"
#include "role_mgr.h"
MsgMgr g_msgMgr;

MsgCmd* MsgCmd::CreateMsgCmd( PVOID pMsg, Role* pSender)
{
	if (!P_VALID(pMsg))
	{
		ASSERT(0);
		return NULL;
	}
	static const tagNC_RoleChat			msgChat;
	static const tagNC_RoleShowEquip	msgShowEquip;
	static const tagNC_RoleShowItem		msgShowItem;

	MsgCmd* pNewMsgCmd = NULL;

	// 根据不同Msg类型生成不同对象
	tagNetCmd* pCmd = static_cast<tagNetCmd*>(pMsg);
	if (pCmd->dwID == msgChat.dwID)
	{
		pNewMsgCmd = new ChatCmd(pCmd);
	}
	else if (pCmd->dwID == msgShowEquip.dwID)
	{
		pNewMsgCmd = new EquipCmd(pCmd);
	}
	else if (pCmd->dwID == msgShowItem.dwID)
	{
		pNewMsgCmd = new ItemCmd(pCmd);
	}

	if (P_VALID(pNewMsgCmd))
	{
		pNewMsgCmd->SetSender(pSender);
		if (E_Success != pNewMsgCmd->Parse())
		{
			ASSERT(0);
			DeleteMsgCmd(pNewMsgCmd);
		}
	}

	return pNewMsgCmd;
}

VOID MsgCmd::DeleteMsgCmd( MsgCmd* &pMsgCmd )
{
	SAFE_DEL(pMsgCmd);
}

MsgCmd::MsgCmd( tagNetCmd* pRecv )
{
	m_pSender = NULL;
	m_dwDstRoleID = GT_INVALID;
	m_eChatTag = ESCC_NULL;
	m_pSend = reinterpret_cast<tagNetCmd*>(m_byBuffer);
	m_pRecv = pRecv;
}

DWORD ChatCmd::Parse()
{
	// 初始化发送消息和接收消息指针
	tagNS_RoleChat* pSend = static_cast<tagNS_RoleChat*>(m_pSend);
	tagNC_RoleChat* pRecv = static_cast<tagNC_RoleChat*>(m_pRecv);

	DWORD dwContentSize = pRecv->dwSize - sizeof(tagNC_RoleChat) + sizeof(TCHAR);

	// 生成发送的消息
	pSend->dwID			= TObjRef<Util>()->Crc32("NS_RoleChat");
	pSend->byChannel	= (BYTE)pRecv->byChannel;
	pSend->dwDestRoleID	= pRecv->dwDestRoleID;
	pSend->dwSrcRoleID	= m_pSender->GetID(); 
	pSend->dwErrorCode	= 0;
	pSend->byAutoReply  = pRecv->byAutoReply;
	pSend->dwSize		= sizeof(tagNS_RoleChat) + dwContentSize - sizeof(TCHAR);
	IFASTCODE->MemCpy(pSend->szMsg, pRecv->szMsg, dwContentSize);

	// 基类中的公共数据
	m_eChatTag = ESendChatChannel(pSend->byChannel);
	m_dwDstRoleID = pSend->dwDestRoleID;
	m_pByContent = (BYTE*)pSend->szMsg;
	m_dwContentSize = dwContentSize;

	return E_Success;
}

DWORD EquipCmd::Parse()
{
	// 初始化发送消息和接收消息指针
	tagNS_RoleShowEquip* pSend = static_cast<tagNS_RoleShowEquip*>(m_pSend);
	tagNC_RoleShowEquip* pRecv = static_cast<tagNC_RoleShowEquip*>(m_pRecv);
	DWORD dwContentSize = sizeof(tagEquip);

	// 获得装备
	tagItem* pItem = m_pSender->GetItemMgr().GetDisplayItem(pRecv->eConType, pRecv->n64Serial);
	if (!P_VALID(pItem))
		return E_RoleShowItem_SendFailure;
	if(MIsEquipment(pItem->dwTypeID) != TRUE)
		return E_RoleShowItem_SendFailure;
	tagEquip* pEquip = (tagEquip*)pItem;

	// 生成发送的消息
	pSend->dwID			= TObjRef<Util>()->Crc32("NS_RoleShowEquip");
	pSend->byChannel	= pRecv->byChannel;
	pSend->dwDestRoleID	= pRecv->dwDestRoleID;
	pSend->dwSrcRoleID	= m_pSender->GetID(); 
	pSend->dwErrorCode	= 0;
	pSend->dwSize		= sizeof(tagNS_RoleShowEquip) + dwContentSize - sizeof(TCHAR);
	IFASTCODE->MemCpy(pSend->szEquip, pEquip, dwContentSize);

	// 基类中的公共数据
	m_eChatTag = ESendChatChannel(pSend->byChannel);
	m_dwDstRoleID = pSend->dwDestRoleID;
	m_pByContent = (BYTE*)pSend->szEquip;
	m_dwContentSize = dwContentSize;

	return E_Success;
}

DWORD ItemCmd::Parse()
{
	// 初始化发送消息和接收消息指针
	tagNS_RoleShowItem* pSend = static_cast<tagNS_RoleShowItem*>(m_pSend);
	tagNC_RoleShowItem* pRecv = static_cast<tagNC_RoleShowItem*>(m_pRecv);

	// 获得装备
	tagItem* pItem = m_pSender->GetItemMgr().GetDisplayItem(pRecv->eConType, pRecv->n64Serial);
	if (!P_VALID(pItem))
		return E_RoleShowItem_SendFailure;
	if(MIsEquipment(pItem->dwTypeID) == TRUE)
		return E_RoleShowItem_SendFailure;

	// 生成发送的消息
	pSend->dwID			= TObjRef<Util>()->Crc32("NS_RoleShowItem");
	pSend->byChannel	= pRecv->byChannel;
	pSend->dwDestRoleID	= pRecv->dwDestRoleID;
	pSend->dwSrcRoleID	= m_pSender->GetID(); 
	pSend->dwErrorCode	= 0;
	pSend->dwTypeID		= pItem->dwTypeID;
	pSend->dwSize		= sizeof(tagNS_RoleShowItem);


	// 基类中的公共数据
	m_eChatTag = ESendChatChannel(pSend->byChannel);
	m_dwDstRoleID = pSend->dwDestRoleID;
	m_pByContent = NULL;
	m_dwContentSize = 0;

	return E_Success;
}

DWORD MsgMgr::HandleChatMsg( PVOID pMsg, Role* pSender)
{
	if (!P_VALID(pSender))
		return GT_INVALID;

	DWORD dwRtv = E_Success;

	// 生成MsgCmd
	MsgCmd* pMsgCmd = MsgCmd::CreateMsgCmd(pMsg, pSender);
	if (!P_VALID(pMsgCmd))
		return GT_INVALID;

	// 发送消息
	dwRtv = m_ChannelMgr.SendMsgCmd(pMsgCmd);

	// 私聊玩家不在线，则留言
	if(E_RoleChat_Pvt_RemoteRoleLeave == dwRtv)
	{
		tagNetCmd* pLeftMsg = pMsgCmd->GetRecvMsg();
		if (P_VALID(pLeftMsg))
		{
			tagNC_RoleChat* pChatCmd = (tagNC_RoleChat*)pLeftMsg;
			pChatCmd->dwDestRoleID = pSender->GetID();

			SaveLeftMsg(pLeftMsg, pLeftMsg->dwSize, pMsgCmd->GetReceiverID());
		}
	}

	// 删除MsgCmd
	MsgCmd::DeleteMsgCmd(pMsgCmd);

	return dwRtv;
}

DWORD MsgMgr::SaveLeftMsg( tagNetCmd* pMsg, DWORD dwMsgSize, DWORD dwRoleID )
{
	BYTE byBuffer[1024] = {0};

	tagNDBC_InsertLeftMsg* pLeftMsg = reinterpret_cast<tagNDBC_InsertLeftMsg*>(byBuffer);
	pLeftMsg->data.dwRoleID = dwRoleID;
	pLeftMsg->data.dwDateTime = GetCurrentDWORDTime();
	pLeftMsg->dwID = IUTIL->Crc32("NDBC_InsertLeftMsg");
	IFASTCODE->MemCpy(pLeftMsg->data.byData, pMsg, dwMsgSize);
	pLeftMsg->dwSize = sizeof(tagNDBC_InsertLeftMsg) - sizeof(BYTE) + dwMsgSize;
	g_dbSession.Send(pLeftMsg, pLeftMsg->dwSize);

	return E_Success;
}

DWORD MsgMgr::LoadLeftMsg( DWORD dwRoleID )
{
	tagNDBC_LoadLeftMsg send;
	send.dwRoleID = dwRoleID;
	g_dbSession.Send(&send, send.dwSize);

	return E_Success;
}

DWORD MsgMgr::HandleLoadLeftMsg( PVOID pMsg )
{
	tagNDBS_LoadLeftMsg* pLoadLeftMsg = reinterpret_cast<tagNDBS_LoadLeftMsg*>(pMsg);

	// 找到人
	Role* pRole = g_roleMgr.GetRolePtrByID(pLoadLeftMsg->dwRoleID);
	if (!P_VALID(pRole))
	{
		return GT_INVALID;
	}

	// 给他发消息
	BYTE* pBegin = pLoadLeftMsg->byData ;
	BYTE* pEnd = pLoadLeftMsg->byData + pLoadLeftMsg->dwSize - sizeof(tagNDBS_LoadLeftMsg) + sizeof(BYTE);

	while(pBegin < pEnd)
	{
		tagNC_RoleChat* pRecvChat = (tagNC_RoleChat*)pBegin;
		DWORD dwSrcRoleID = pRecvChat->dwDestRoleID;
		pRecvChat->dwDestRoleID = pLoadLeftMsg->dwRoleID;

		MsgCmd* pMsgCmd = MsgCmd::CreateMsgCmd(pBegin, pRole);
		if (P_VALID(pMsgCmd))
		{
			pBegin += pMsgCmd->GetRecvMsg()->dwSize;

			tagNS_RoleChat* pSendCmd = (tagNS_RoleChat*)pMsgCmd->GetSendMsg();
			pSendCmd->dwSrcRoleID = dwSrcRoleID;

			m_ChannelMgr.SendMsgCmd(pMsgCmd);
			MsgCmd::DeleteMsgCmd(pMsgCmd);
		}
		else
		{
			break;
		}
	}

	pRole->ResetHasLeftMsg();

	tagNDBC_ClearLeftMsg send;
	send.dwRoleID = pRole->GetID();
	g_dbSession.Send(&send, send.dwSize);

	return E_Success;
}