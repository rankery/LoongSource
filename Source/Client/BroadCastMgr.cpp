#include "stdafx.h"
#include "BroadCastMgr.h"
#include "QuestScript.h"
#include "GameScriptMgr.h"
#include "ChatFrame.h"
#include "PlayerNameTab.h"
#include "..\WorldDefine\msg_script.h"
#include "..\WorldDefine\ScriptMsgInfo.h"
#include "GuildEvent.h"
#include "GuildMgr.h"

namespace
{
	tstring& replace_all(tstring& str,const tstring& old_value,const tstring& new_value)
	{  
		while( true )
		{
			tstring::size_type pos(0);
			if( (pos=str.find(old_value)) != tstring::npos )
				str.replace(pos,old_value.length(),new_value);
			else
				break;
		}  
		return str;
	}
}

BroadCastMgr::BroadCastMgr( void ):m_Trunk(this)
, m_bAllRoleFinded(true)
, m_bAllGuildFinded(true)
, m_dwLastParseTime(GT_INVALID)
, m_dwCurTime(GT_INVALID)
{

}

BroadCastMgr::~BroadCastMgr( void )
{

}


BroadCastMgr* BroadCastMgr::Inst()
{
	static BroadCastMgr broadcastMgr;
	return &broadcastMgr;
}

void BroadCastMgr::Init()
{
	m_pUtil			=  TObjRef<Util>();
	m_pCmdMgr		= TObjRef<NetCmdMgr>();
	m_pSession		= TObjRef<NetSession>();
	m_pFrameMgr		= TObjRef<GameFrameMgr>();

	//--注册游戏事件
	m_pFrameMgr->RegisterEventHandle(_T("tagRoleGetNameEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&BroadCastMgr::OnRoleGetNameEvent));
	m_pFrameMgr->RegisterEventHandle(_T("GuildEvent_NameGet"), (FRAMEEVENTPROC)m_Trunk.sfp1(&BroadCastMgr::OnGuildGetNameEvent));

	m_bAllRoleFinded = true;
	m_bAllGuildFinded = true;
}

void BroadCastMgr::Destroy()
{
	//--注销游戏事件
	m_pFrameMgr->UnRegisterEventHandler(_T("tagRoleGetNameEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&BroadCastMgr::OnRoleGetNameEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("GuildEvent_NameGet"), (FRAMEEVENTPROC)m_Trunk.sfp1(&BroadCastMgr::OnGuildGetNameEvent));

	m_msgCache.clear();
	m_roleCache.clear();
	m_guildCache.clear();
}

void BroadCastMgr::Update()
{
	if (m_msgCache.empty())
		return;

	if (m_bAllRoleFinded && m_bAllGuildFinded)
	{
		MsgCache::iterator msgIter = m_msgCache.begin();
		tstring &strMsg = msgIter->strContent;
		m_dwLastParseTime = timeGetTime();	// 记录解析发生的时间
		bool bRet = ParseBroadCast(strMsg);
		if (bRet)
		{
			if (msgIter->eType == EMCT_Broadcast)
			{
				PushBroadCast(strMsg.c_str());
			}
			else if (msgIter->eType == EMCT_Chat)
			{
				PushChatInfo(strMsg.c_str(), msgIter->eChannel);
			}
			m_msgCache.erase(msgIter);
		}
	}
	m_dwCurTime = timeGetTime();
	// 如果在指定超时时间都没发生过解析，清空一条广播
	if (m_dwCurTime - m_dwLastParseTime >= BROADCAST_OVERTIME_SET)
	{
		if (!m_msgCache.empty())
		{
			MsgCache::iterator msgIter = m_msgCache.begin();
			m_msgCache.erase(msgIter);
		}
		// 重置为true，给下一条广播获得处理的机会
		m_bAllRoleFinded = true;
		m_bAllGuildFinded = true;
	}

}

void BroadCastMgr::BroadCast( LPCTSTR szContent, BOOL bHasRoleOrGuild )
{
	if( bHasRoleOrGuild == TRUE )
	{
		tagMsgCache cache;
		cache.eType = EMCT_Broadcast;
		cache.strContent = szContent;
		m_msgCache.push_back( cache );
	}	
	else
	{
		this->PushBroadCast(szContent);
	}
}

void BroadCastMgr::ToChatChannel(LPCTSTR szContent, ESendChatChannel eChannel, BOOL bHasRoleOrGuid)
{
	if (bHasRoleOrGuid == TRUE)
	{
		tagMsgCache cache;
		cache.eType = EMCT_Chat;
		cache.eChannel = eChannel;
		cache.strContent = szContent;
		m_msgCache.push_back( cache );
	}
	else
	{
		this->PushChatInfo(szContent, eChannel);
	}
}

bool BroadCastMgr::ParseBroadCast( tstring& strContext )
{
	
	UINT			nIndex = 0;
	bool			bRet = true;
	DWORD			dwRoleID = 0;
	DWORD			dwGuildID = 0;
	UINT			paramIndex = 0;

	while( nIndex < strContext.size() )
	{
		if( strContext[nIndex]==_T('<') && strContext[nIndex+1]==_T('R') )
		{
			tstring::size_type flagEndPos = strContext.find_first_of(_T('>'), nIndex);
			if( flagEndPos == tstring::npos || flagEndPos - nIndex <= 1 )
			{
				bRet = false;
				break;			//语法错误
			}

			//取出RoleID
			tstring str = strContext.substr( nIndex+3, flagEndPos-(nIndex+3));
			dwRoleID = _tcstoul( str.c_str() , NULL, 10);
			if( IS_PLAYER(dwRoleID) )
			{

				tstring strName = PlayerNameTab::Inst()->FindNameByID(dwRoleID);
				if( !strName.empty() )
				{
					strContext.replace( nIndex, flagEndPos-nIndex+1, strName.c_str() );
					nIndex += strName.length();
				}
				else
				{
					bRet = false;
					m_roleCache.push_back(make_pair(false,dwRoleID));
					m_bAllRoleFinded = false;
					nIndex += (flagEndPos-nIndex+1);
				}
			}
		}
		else if (strContext[nIndex]==_T('<') && strContext[nIndex+1]==_T('G'))
		{
			tstring::size_type flagEndPos = strContext.find_first_of(_T('>'), nIndex);
			if (flagEndPos == tstring::npos || flagEndPos - nIndex <= 1)
			{
				bRet = false;
				break;		// 语法错误
			}
			// 取出GuildID
			tstring str = strContext.substr(nIndex+3, flagEndPos-(nIndex+3));
			dwGuildID = _tcstoul(str.c_str(), NULL, 10);
			tstring strGuildName = GuildMgr::Inst()->GetGuildName(dwGuildID);

			if (!strGuildName.empty())
			{
				strContext.replace(nIndex, flagEndPos - nIndex + 1, strGuildName.c_str());
				nIndex += strGuildName.length();
			}
			else
			{
				bRet = false;
				m_guildCache.push_back(make_pair(false, dwGuildID));
				m_bAllGuildFinded = false;
				nIndex += (flagEndPos-nIndex+1);
			}
		}

		else
			++nIndex;
	}

	return bRet;
}

DWORD BroadCastMgr::OnRoleGetNameEvent( tagRoleGetNameEvent* pGameEvent )
{
	if (m_roleCache.empty())
		return 0;

	RoleCache::iterator roleIter;
	bool bAllFinded = true;
	for( roleIter=m_roleCache.begin(); roleIter!=m_roleCache.end(); ++roleIter )
	{
		if( !(roleIter->first) )
		{
			if( roleIter->second == pGameEvent->dwRoleID && pGameEvent->strRoleName != _T(""))
				roleIter->first = true;
			else
				bAllFinded = false;
		}
	}

	m_bAllRoleFinded = bAllFinded;
	if (m_bAllRoleFinded)
	{
		m_roleCache.clear();
	}

	return 0;
}

void BroadCastMgr::PushBroadCast( LPCTSTR szContent )
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		pFrame->PushScrollText(szContent);
		pFrame->PushInfo(szContent, ESCC_Affiche);
	}
}


void BroadCastMgr::PushChatInfo(LPCTSTR szContent, ESendChatChannel eChannel)
{
	ChatFrame* pFrame = (ChatFrame*)m_pFrameMgr->GetFrame(_T("Chat"));
	if( P_VALID(pFrame) )
	{
		pFrame->PushInfo(szContent, eChannel);
	}
}

DWORD BroadCastMgr::OnGuildGetNameEvent(tagGuildNameGetEvent *pGameEvent)
{
	if (m_guildCache.empty())
		return 0;

	bool bAllFinded = true;

	for (GuildCache::iterator iter = m_guildCache.begin(); iter != m_guildCache.end(); ++iter)
	{
		if (!(iter->first))
		{
			if (iter->second == pGameEvent->dwGuildID && pGameEvent->strName != _T(""))
				iter->first = true;
			else
				bAllFinded = false;
		}
	}
	m_bAllGuildFinded = bAllFinded;
	if (m_bAllGuildFinded)
	{
		m_guildCache.clear();
	}

	return 0;
}

