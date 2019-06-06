#include "stdafx.h"
/*

#include "chat_data.h"

BOOL ChannelTagData::Init( const DWORD* pData )
{
	for (INT i=0; i<CHANNEL_NUM; ++i)
	{
		m_ChannelMasks[i].InitState(pData[i]);
	}
	UpdateAllMask();
	return TRUE;
}

BOOL ChannelTagData::GetData( DWORD* pData ) const
{
	for (INT i=0; i<CHANNEL_NUM; ++i)
	{
		pData[i] = m_ChannelMasks[i].GetState();
	}
	return TRUE;
}

BOOL ChannelTagData::IsFit( ESendChatChannel eChatTag ) const
{
	if (eChatTag < ESCC_CastTalk && eChatTag >= ESCC_End)
	{
		ASSERT(0);
		return FALSE;
	}
	return m_AllMask.IsInStateAny(eChatTag);
}

BOOL ChannelTagData::SetChannelTag( EChatChannel eChatChannel, ESendChatChannel eChatTag, BOOL bSet )
{
	if (eChatTag < ESCC_CastTalk && eChatTag >= ESCC_End)
	{
		ASSERT(0);
		return FALSE;
	}
	if (eChatChannel < ECC_All && eChatChannel >= ECC_End)
	{
		ASSERT(0);
		return FALSE;
	}

	TagMask& channelMask = m_ChannelMasks[eChatChannel];

	if(channelMask.IsInState(eChatTag) && !bSet ||
		!channelMask.IsInState(eChatTag) && bSet)
	{
		bSet ? channelMask.SetState(eChatTag) : channelMask.UnsetState(eChatTag);
		UpdateAllMask();
		return TRUE;
	}

	return FALSE;
}

VOID ChannelTagData::UpdateAllMask()
{
	m_AllMask.InitState(0);
	for (INT i=0; i<CHANNEL_NUM; ++i)
	{
		m_AllMask.InitState(m_ChannelMasks[i].GetState() | m_AllMask.GetState());
	}
}

BOOL ChatData::Init( BOOL bhasLeftMsg, const DWORD* pMaskData )
{
	m_bHasLeftMsg = bhasLeftMsg;
	return m_TagData.Init(pMaskData);
}

BOOL ChatData::GetData( BOOL &bHasLeftMsg, DWORD* pData, DWORD &dwDataSize )
{
	bHasLeftMsg = m_bHasLeftMsg;
	m_TagData.GetData(pData);
	dwDataSize = ChannelTagData::CHANNEL_NUM * sizeof(DWORD);
	return TRUE;
}

BOOL ChatData::SetChannelTag( EChatChannel eChatChannel, ESendChatChannel eChatTag, BOOL bSet )
{
	return m_TagData.SetChannelTag(eChatChannel, eChatTag, bSet);
}

BOOL ChatData::IsFit( ESendChatChannel eChatTag ) const
{
	return m_TagData.IsFit(eChatTag);
}
*/