#pragma once
#include "unit.h"
#include "role.h"
#include "../WorldDefine/chat_define.h"

class MsgCmd;

struct SendAction
{
	SendAction(ESendChatChannel eChannel, tagNetCmd* pSend)
		:m_pSend(pSend), m_eChannel(eChannel){ASSERT(P_VALID(pSend));}
	VOID operator()(Unit* pUnit)
	{
		if (!P_VALID(m_pSend))
		{
			return ;
		}

		Role* pRole = dynamic_cast<Role*>(pUnit);
		if (P_VALID(pRole)/* && pRole->GetChatData()->IsFit(m_eChannel)*/)
		{
			pRole->SendMessage(m_pSend, m_pSend->dwSize);
		}

		return ;
	}

private:
	tagNetCmd*		m_pSend;
	ESendChatChannel m_eChannel;
};

class ChannelMgr
{
public:
	DWORD	GetContentSizeLimit(tagNetCmd* pNetCmd, BYTE byChannelType);
	DWORD	SendMsgCmd(MsgCmd* pMsgCmd);
};
