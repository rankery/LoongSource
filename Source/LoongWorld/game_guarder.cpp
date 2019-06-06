//-----------------------------------------------------------------------------
//!\file game_guarder.h
//!\author xlguo
//!
//!\date 2009-03-02
//! last 
//!
//!\brief 反外挂系统
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../ServerDefine/login_issue.h"
#include "../WorldDefine/msg_game_guarder.h"

#include "ApexProxy.h"
#include "game_guarder.h"

#include "world_session.h"
#include "player_session.h"
#include "login_session.h"
#include "TimeChecker.h"

#include "world.h"

#ifdef _DEBUG
//#define _DEBUG_GAMEGUARDER
#endif

_FUNC_S_REC		GameGuarder::m_spfRec	= NULL;
BOOL			GameGuarder::m_bEnable	= FALSE;
GameGuarder g_gameGuarder;

#define MGET_ACCSTR( buffer, nSize, nAccountID)	\
	char buffer[nSize] = {0};						\
	sprintf_s(buffer, nSize, "%d", nAccountID);

GameGuarder::GameGuarder()
{
	m_bEnable = TRUE;
	long lRtv = CHSStart(SendMsg, m_spfRec);
	/*ASSERT( !lRtv );*/
	CHSSetFunc(Kick, FLAG_KILLUSER);
}

GameGuarder::~GameGuarder()
{
	CHSEnd();
}

long GameGuarder::Login( DWORD dwAccountID, LPCSTR szAccount, DWORD dwIP )
{
	Recv('L', dwAccountID, szAccount, strlen(szAccount));

	tagUserIP ip;
	ip.uIP = dwIP;
	
	return Recv('S', dwAccountID, (const char *)(&ip), 5);
}

long GameGuarder::Logout( DWORD dwAccountID, LPCSTR szAccount )
{
	return Recv('G', dwAccountID, szAccount, strlen(szAccount));
}

long GameGuarder::Ret( DWORD dwAccountID, const char* pRet )
{

	return Recv('R', dwAccountID, pRet, 4);
}

long GameGuarder::Transport(DWORD dwAccountID, const char* pBuffer, const int nLen)
{
	return Recv('T', dwAccountID, pBuffer, nLen);
}

long GameGuarder::UserData(DWORD dwAccountID, const char* pBuffer, const int nLen)
{
	return Recv('D', dwAccountID, pBuffer, nLen);
}

VOID GameGuarder::SendKickOut(const tagNWL_KickLog* pKickLog)
{
	g_loginSession.Send((LPVOID)pKickLog, pKickLog->dwSize);
}

long GameGuarder::Recv(char cMsgId,DWORD dwAccountID,const char * pBuffer,int nLen)
{
	if (!m_bEnable)
	{
		MGET_ACCSTR(szBuffer, 64, dwAccountID);
		m_spfRec('G', (signed int)dwAccountID, szBuffer, strlen(szBuffer));
		return 0;
	}
	/*ASSERT( P_VALID(m_spfRec) );*/
	const tagUserIP* pIp = (const tagUserIP*)pBuffer;
	if (P_VALID(m_spfRec))
		m_spfRec(cMsgId, (signed int)dwAccountID, pBuffer, nLen);

#ifdef _DEBUG_GAMEGUARDER
	IMSG(_T("Apex Msg ____Recv.AccountID: %d, Len: %d, Flag: %c!\r\n"), dwAccountID, nLen, cMsgId);
#endif

	return 0;
}

long GameGuarder::SendMsg( signed int nAccountID,const char * pBuffer,int nLen )
{
	if (!m_bEnable)
		return 0;
	
	g_gameGuarder.Wait2Send(nAccountID, nLen, pBuffer);

#ifdef _DEBUG_GAMEGUARDER 
	IMSG(_T("Apex Msg Sent____.AccountID: %d, Len: %d!\r\n"), nAccountID, nLen);
#endif	

	return 0;

}

long GameGuarder::Kick(signed int nAccountID, int nAction)
{
	if (!m_bEnable)
	{
		MGET_ACCSTR(szBuffer, 64, nAccountID);
		m_spfRec('G', nAccountID, szBuffer, strlen(szBuffer));
		return 0;
	}

	UINT16	u16ErrCode	= static_cast<UINT16>((nAction >> 16) & 0x0000ffff);
	UINT16	u16SealMark = static_cast<UINT16>(nAction & 0x0000ffff);

#ifdef _DEBUG_GAMEGUARDER
	if (0 == u16SealMark)
	{
		// 封号
		IMSG(_T("Account: %d Sealed!\r\n"), nAccountID);
	}
	else if (1 == u16SealMark)
	{
		// 踢人
		IMSG(_T("Account: %d Kicked!\r\n"), nAccountID);
	}
#endif
	
	g_gameGuarder.Wait2Kick(nAccountID, g_world.GetWorldTime(), u16ErrCode, u16SealMark);

	return 0;
}

VOID GameGuarder::Update()
{
	CHECK_TIME();
	// 发送踢人log
	while(m_listKickAccount.Size() > 0)
	{
		tagNWL_KickLog* pKickLog = m_listKickAccount.PopFront();

		PlayerSession* pToKick = g_worldSession.FindSession(pKickLog->dwAccountID);

		if (P_VALID(pToKick))
		{
			g_worldSession.Kick(pToKick->GetInternalIndex());
			pToKick->SetKicked();
			SendKickOut(pKickLog);
		}

		SAFE_DEL(pKickLog);
	}

	// 发送反外挂消息
	m_LockMsg.Acquire();

	while (!m_listMsg.empty())
	{
		tagAccountMsg bind = m_listMsg.front();
		m_listMsg.pop_front();
		PlayerSession* pSession = g_worldSession.FindSession(bind.dwAccountID);

		if (P_VALID(pSession))
		{
			pSession->SendMessage(bind.pMsg, bind.pMsg->dwSize);
		}

		DeleteGuardMsg(bind.pMsg);
	}

	m_LockMsg.Release();
}

VOID GameGuarder::Wait2Kick(INT nAccountID, DWORD dwKickTime, UINT16 u16ErrCode, UINT16 u16SealMark)
{
	tagNWL_KickLog* pKickLog = new tagNWL_KickLog;
	pKickLog->dwAccountID	= nAccountID;
	pKickLog->dwKickTime	= dwKickTime;
	pKickLog->u16ErrCode	= u16ErrCode;
	pKickLog->bySeal		= !u16SealMark;			// 是否封号

	m_listKickAccount.PushBack(pKickLog);
}

VOID GameGuarder::Wait2Send( INT nAccountID, INT nLen, LPCSTR pBuffer )
{

	tagNS_GameGuarder* pSend = CreateGuardMsg(nLen);

	IFASTCODE->MemCpy(pSend->chData, pBuffer, nLen);
	pSend->chCmd = 'T';
	pSend->nLen = nLen;

	tagAccountMsg bind;
	bind.dwAccountID	= nAccountID;
	bind.pMsg			= pSend;

	m_LockMsg.Acquire();
	m_listMsg.push_back(bind);
	m_LockMsg.Release();
}

tagNS_GameGuarder* GameGuarder::CreateGuardMsg( INT nLen )
{
	DWORD dwMsgSize = sizeof(tagNS_GameGuarder) - 1 + nLen;

	BYTE* pTmp = new BYTE[dwMsgSize];
	ZeroMemory(pTmp, dwMsgSize);
	MMSGINIT(pTmp, tagNS_GameGuarder);
	tagNS_GameGuarder* pSend = reinterpret_cast<tagNS_GameGuarder*>(pTmp);
	pSend->dwSize = dwMsgSize;

	return pSend;
}

VOID GameGuarder::DeleteGuardMsg( tagNS_GameGuarder* pMsg )
{
	delete [](BYTE*)pMsg;
}