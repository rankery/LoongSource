//-----------------------------------------------------------------------------
//!\file player.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 玩家类
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/login.h"

#include "Mutex.h"
#include "player_net_cmd_mgr.h"

class PlayerNetCmdMgr;

//-----------------------------------------------------------------------------
// 玩家类
//-----------------------------------------------------------------------------
class Player  
{
public:
	//-------------------------------------------------------------------------
	// 构造函数
	//-------------------------------------------------------------------------
	Player(DWORD dwClientID, DWORD dwCDIndex, DWORD dwIP, DWORD dwWorldID);
	~Player();

	//-------------------------------------------------------------------------
	// 各种Get
	//-------------------------------------------------------------------------
	DWORD			GetAccountID()				{ return m_Data.dwAccountID; }
	LPCSTR			GetAccoutName()				{ return m_Data.szAccountName; }
	LPCTSTR			GetAccoutNameT();			
	DWORD			GetClientID()				{ return m_Data.dwClientID; }
	DWORD			GetCDIndex()				{ return m_Data.dwCDIndex; }
	BYTE			GetPrivilege()				{ return m_Data.byPrivilege; }
	DWORD			GetLoginWorldNameCrc()		{ return m_Data.dwWorldNameCrc; }
	INT				GetVerifyCode()				{ return m_nVerifyCode; }
	tagPlayerData&	GetPlayerData()				{ return m_Data; }

	DWORD			GetPreLoginTime()			{ return m_dwPreLoginTime;}
	DWORD			GetPreLoginIP()				{ return m_dwPreLoginIP;}


	BOOL			GetForbidMask()				{ return m_Data.dwFrobidMask; }
	BOOL			IsGuard()					{ return m_Data.bGuard; }

	//-------------------------------------------------------------------------
	// 玩家信息相关
	//-------------------------------------------------------------------------
	VOID			ProofReturn(tagProofResult* pResult);

	//-------------------------------------------------------------------------
	// 连接相关
	//-------------------------------------------------------------------------
	VOID			SetConnLost()				{ InterlockedExchange((LPLONG)(&m_bConnLost), TRUE); }
	BOOL			IsConnLost()				{ return m_bConnLost; }

	//-------------------------------------------------------------------------
	// 网络消息相关
	//-------------------------------------------------------------------------
	static VOID		RegisterPlayerMsg();
	static VOID		UnRegisterPlayerMsg();

	INT				HandleMessage();
	VOID			SendMessage(LPVOID pMsg, DWORD dwSize);
	
	//-------------------------------------------------------------------------
	// 验证相关
	//-------------------------------------------------------------------------
	VOID			SetPreLoginTime(tagDWORDTime dwTime)	{ m_dwPreLoginTime = dwTime; }
	VOID			SetPreLoginIP(DWORD dwIP)		{ m_dwPreLoginIP = dwIP; }


	VOID			SetKickTime(tagDWORDTime dwTime)	{ m_dwKickTime = dwTime; }
	tagDWORDTime	GetKickTime()						{ return m_dwKickTime; }

	BOOL			IsNeedMibao()						{ return m_bNeedMibao; }

	BOOL			IsProofing()						{ return m_bProofing; }
	BOOL			IsProofEnd()						{ return m_bProofEnd; }
	VOID			SetProofing()						{ m_bProofing = TRUE; m_bProofEnd = FALSE; }
	VOID			SetProofEnd()						{ m_bProofEnd = TRUE; m_bProofing = FALSE; }

	BOOL			IsSelectWorldOk()					{ return m_bSelectWorldOk; }
	VOID			SetSelectWorldOK()					{ m_bSelectWorldOk = TRUE; }

	BOOL			GenerateMibao(CHAR szMibao[MAX_MIBAO]);
	BOOL			CheckMibao(DWORD dwResultCrc);


private:
	//-----------------------------------------------------------------------
	// 消息处理函数
	//-----------------------------------------------------------------------
	DWORD			HandleProof(tagNetCmd* pCmd);
	DWORD			HandleMibao(tagNetCmd* pCmd);

	//-----------------------------------------------------------------------
	// 消息相关
	//-----------------------------------------------------------------------
	LPBYTE			RecvMsg(DWORD& dwSize, INT& nUnRecved);
	VOID			ReturnMsg(LPBYTE pMsg);
	VOID            SendMsg(LPBYTE pMsg, DWORD dwSize);

	//-----------------------------------------------------------------------
	// 辅助函数
	//-----------------------------------------------------------------------
	BOOL			CheckName(string& str);

private:
	static PlayerNetCmdMgr		m_playerNetMgr;			// 消息管理器

private:
	tagPlayerData				m_Data;					// 玩家数据

	volatile BOOL				m_bConnLost;			// 连接是否已经断开

	INT							m_nVerifyCode;			// 验证码
	BOOL						m_bNeedMibao;			// 是否需要密保验证

	BOOL						m_bProofing;			// 是否正在进行验证
	BOOL						m_bProofEnd;			// 是否已经验证完毕
	BOOL						m_bSelectWorldOk;		// 是否已经选择游戏世界成功

	tagDWORDTime				m_dwKickTime;			// 将要被踢掉的倒计时

	tagDWORDTime				m_dwPreLoginTime;		//上次登录时间
	DWORD						m_dwPreLoginIP;			//上次登录ip

	TCHAR						m_cAccountNameT[X_SHORT_NAME]; //自适应类型版的用户名
};





