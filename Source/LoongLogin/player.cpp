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
#include "stdafx.h"

#include "../WorldDefine/login.h"

#include "player.h"
#include "player_mgr.h"
#include "player_net_cmd_mgr.h"
#include "loongworld.h"
#include "loongworld_mgr.h"
#include "db.h"
#include "login_server.h"

//-------------------------------------------------------------------------------
// 静态变量声明
//-------------------------------------------------------------------------------
PlayerNetCmdMgr Player::m_playerNetMgr;

//-------------------------------------------------------------------------------
// 构造函数
//-------------------------------------------------------------------------------
Player::Player(DWORD dwClientID, DWORD dwCDIndex, DWORD dwIP, DWORD dwWorldID) : 
				m_Data(), m_bConnLost(FALSE), m_nVerifyCode(0), m_bProofEnd(FALSE),
				m_bProofing(FALSE), m_bNeedMibao(FALSE), m_dwKickTime(), m_bSelectWorldOk(FALSE)
{
	m_Data.dwClientID		=		dwClientID;
	m_Data.dwCDIndex		=		dwCDIndex;
	m_Data.dwAccountID		=		GT_INVALID;
	m_Data.dwFrobidMask		=		0;
	m_Data.bGuard			=		FALSE;
	m_Data.nAccOLSec		=		0;
	m_Data.dwIP				=		dwIP;
	m_Data.dwWorldNameCrc	=		dwWorldID;
	m_Data.byPrivilege		=		0;
}

//-------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------
Player::~Player()
{
   
}

//-------------------------------------------------------------------------------
// 验证返回
//-------------------------------------------------------------------------------
VOID Player::ProofReturn(tagProofResult* pResult)
{
	m_Data.dwAccountID		=	pResult->dwAccountID;
	m_Data.bGuard			=	pResult->bGuard;
	m_Data.nAccOLSec		=	pResult->nGuardAccuTime;
	m_Data.byPrivilege		=	pResult->byPrivilege;
	m_Data.dwFrobidMask		=	pResult->dwFrobidMask;

	//memcpy(m_Data.szAccountName,pResult->szAccountName,X_SHORT_NAME); 

	m_bNeedMibao			=	pResult->bNeedMibao;

	SetProofEnd();

	// 缓冲一下账号名
	sPlayerMgr.CacheAccountName(GetAccountID(), GetAccoutName());
	sPlayerMgr.CacheGuard(GetAccountID(), GetPlayerData().bGuard);
	sPlayerMgr.MapAccountName2AccountID(GetAccoutName(), GetAccountID());
}

//-------------------------------------------------------------------------------
// 注册消息处理函数
//-------------------------------------------------------------------------------
VOID Player::RegisterPlayerMsg()
{
	m_playerNetMgr.Register("NLC_SafeProof",	&HandleProof,			_T("NLC_SafeProof"));
	m_playerNetMgr.Register("NLC_MiBao",		&HandleMibao,			_T("NLC_MiBao"));
}

//-------------------------------------------------------------------------------
// 注销消息处理函数
//-------------------------------------------------------------------------------
VOID Player::UnRegisterPlayerMsg()
{
	 m_playerNetMgr.UnRegisterAll();
}

//-------------------------------------------------------------------------------
// 处理玩家网络消息函数
//-------------------------------------------------------------------------------
INT Player::HandleMessage()
{
    // 检测玩家是否断开连接
    if( m_bConnLost )
        return CON_LOST;

    // 接收消息
	DWORD	dwSize		=	0;
	INT		nUnRecved	=	0;

	LPBYTE pMsg = RecvMsg(dwSize, nUnRecved);
	if( !P_VALID(pMsg) ) return 0;

	// 处理消息
    tagNetCmd* pCmd = (tagNetCmd*)pMsg;

    NETMSGHANDLER pHandler = m_playerNetMgr.GetHandler(pCmd, dwSize);

    if( NULL != pHandler )
        (this->*pHandler)(pCmd);
    
	// 回收消息
    ReturnMsg(pMsg);

	return 0;
}

//-------------------------------------------------------------------------------
// 发送消息
//-------------------------------------------------------------------------------
VOID Player::SendMessage(LPVOID pMsg, DWORD dwSize)
{
	if( !P_VALID(pMsg) || dwSize == 0 )
		return;

	// 连接已经中断，不再发送了
	if( m_bConnLost ) return;

	SendMsg((LPBYTE)pMsg, dwSize);
}

//-------------------------------------------------------------------------------
// 接收消息
//-------------------------------------------------------------------------------
LPBYTE Player::RecvMsg(DWORD& dwSize, INT& nUnRecved)
{
	return sPlayerMgr.RecvMsg(dwSize, nUnRecved, m_Data.dwCDIndex);
}

//-------------------------------------------------------------------------------
// 归还消息
//-------------------------------------------------------------------------------
VOID Player::ReturnMsg(LPBYTE pMsg)
{
	sPlayerMgr.ReturnMsg(pMsg);
}

//-------------------------------------------------------------------------------
// 发送消息
//-------------------------------------------------------------------------------
VOID Player::SendMsg(LPBYTE pMsg, DWORD dwSize)
{
	sPlayerMgr.SendMsg(m_Data.dwCDIndex, pMsg, dwSize);
}


//-----------------------------------------------------------------------------
// 玩家认证消息
//-----------------------------------------------------------------------------
DWORD Player::HandleProof(tagNetCmd* pCmd)
{
    tagNLC_SafeProof* pRecv = (tagNLC_SafeProof*)pCmd;

	// 防止字符串溢出
	pRecv->szUserName[X_SHORT_NAME-1] = '\0';
	pRecv->szPsd[MAX_MD5_ARRAY-1] = '\0';
	pRecv->szGUID[X_LONG_NAME-1] = '\0';

	// 进行初始认证
	INT nRet = E_Success;

	// 验证玩家要登录的游戏世界是否可用
	LoongWorld* pWorld = sLoongWorldMgr.GetLoongWorld(pRecv->dwWorldNameCrc);
	if( !P_VALID(pWorld) )
	{
		nRet = E_SelectWorld_GameWorldName_Wrong;
	}

	// 查看玩家要登录的LoongWorld是否正常
	else if( pWorld->GetStatus() != EWS_Well )
	{
		nRet = E_SelectWorld_Server_Maintenance;
	}

	// 查看验证策略是否一致
	else if( pRecv->dwType != sPlayerMgr.GetProofPolicy() )
	{
		nRet = E_ProofResult_Wrong_Type;
	}

	// 查看版本号是否一致
	else if( pRecv->dwCurVersionID != sPlayerMgr.GetCurrVer() )
	{
		nRet = E_ProofResult_Wrong_Build_Number;
	}

	// 如果正在验证，或者已经验证完毕
	else if( IsProofing() || IsProofEnd() )
	{
		nRet = E_ProofResult_Proofing;
	}
	// 检查用户名合法
	else
	{
		string strUserName = pRecv->szUserName;

		if( CheckName(strUserName) )
		{
			nRet = E_ProofResult_Account_No_Match;
		}
		else
		{
			strUserName.copy(m_Data.szAccountName, X_SHORT_NAME);
#ifdef UNICODE
			MultiByteToWideChar(CP_ACP,NULL,m_Data.szAccountName,-1,m_cAccountNameT,X_SHORT_NAME);
#else
			strcpy(m_cAccountNameT,m_Data.szAccountName);
#endif
		}
	}

	// 如果有错误，则返回客户端
	if( E_Success != nRet )
	{
		tagNLS_ProofResult send;
		send.dwErrorCode = nRet;
		SendMessage(&send, send.dwSize);
	}
	// 如果没有错误，则发送帐号验证
	else
	{
		sPlayerMgr.Proof(GetClientID(), GetAccoutName(), pRecv->szPsd, pRecv->szGUID, m_Data.dwIP);
	}

	return 0;
}

DWORD Player::HandleMibao(tagNetCmd* pCmd)
{
	tagNLC_MiBao* pRecv = (tagNLC_MiBao*)pCmd;

	// 进行验证
	INT nRet = E_Success;

	if( !IsProofEnd() )
	{
		nRet = E_ProofResult_Mibao_Error;
	}
	else if( !CheckMibao(pRecv->dwMBCrc) )
	{
		nRet = E_ProofResult_Mibao_Error;
	}

	// 如果返回正确
	if( E_Success == nRet )
	{
		// 加入到游戏世界登入列表
		sPlayerMgr.RemovePlayerFromLogining(GetClientID());

		// 加入帐号
		sPlayerMgr.AddAccount(GetAccountID(), GetClientID());

		//将玩家加入到LoongWorld的玩家列表中
		sLoongWorldMgr.AddToWorld(this, GetLoginWorldNameCrc());

	}
	// 如果返回错误
	else
	{
		// 发送给客户端验证错误
		tagNLS_ProofResult send;
		send.dwErrorCode = nRet;
		SendMessage(&send, send.dwSize);
	}

	return 0;
}

//-------------------------------------------------------------------------------
// 替换掉姓名中的回车和换行,同时检查用户名是否为空
//-------------------------------------------------------------------------------
BOOL Player::CheckName(string& str)
{
	// 去掉回车
	INT nFind = 0;
	while( (nFind = str.find('\r', nFind)) != str.npos )
		str.replace(nFind, 1, "");

	// 去掉换行
	nFind = 0;
	while( (nFind = str.find('\n', nFind)) != str.npos )
		str.replace(nFind, 1, "");

	// 去掉tab
	nFind = 0;
	while( (nFind = str.find('\t', nFind)) != str.npos )
		str.replace(nFind, 1, "");

	// 去掉空格
	if((nFind = str.find(' ')) != str.npos)
	{
		str = str.substr(0,nFind);
	}

	// 检查用户名是否为空
	return str.empty();
}

//--------------------------------------------------------------------------------
// 生成验证用密保
//--------------------------------------------------------------------------------
BOOL Player::GenerateMibao(CHAR szMibao[MAX_MIBAO])
{
	// 从数据库里面查询该玩家的密报值
	CHAR szMibaoInDB[MIBAO_LEN] = {'\0'};
	if( !sBeton.GetPlayerMibao(GetAccountID(), szMibaoInDB) )
	{
		return FALSE;
	}

	CHAR szValue[MAX_MIBAO] = {'\0'};
	// 密保的格式：以逗号分隔的字符序列，8*10的矩阵
	// 例如：97,55,87,21,90,33,19,36,55,90,26,96,24,61,32,27,70,64,86,69,97,54,36,21,
	// 18,58,55,96,37,32,75,64,08,87,08,74,33,13,34,90,70,14,09,98,93,37,19,75,21,68,51,
	// 46,59,41,86,69,13,93,00,15,48,36,57,50,16,98,24,57,38,63,91,28,53,06,35,40,61,59,94,15,
	for(INT n = 0; n < 3; ++n)
	{
		INT nRand = IUTIL->Rand() % 80;		// 随机值

		INT nX = nRand / 10;	// x轴，从'A'到'H'
		INT nY = nRand % 10;	// y轴，从'0'到'9'

		// 向密保行列中写入行列号
		szMibao[n*2]	=	'A' + nX;
		szMibao[n*2+1]	=	'0' + nY;

		// 向值中写入密保卡值
		szValue[n*2]	=	szMibaoInDB[nRand*3];
		szValue[n*2+1]	=	szMibaoInDB[nRand*3+1];
	}

	// 将szValue算出CRC保存在起来
	m_Data.dwMibaoCrc = IUTIL->Crc32((LPBYTE)szValue, MAX_MIBAO);


	return TRUE;
}

//--------------------------------------------------------------------------------
// 检测密保
//--------------------------------------------------------------------------------
BOOL Player::CheckMibao(DWORD dwResultCrc)
{
	return m_Data.dwMibaoCrc == dwResultCrc;
}
//--------------------------------------------------------------------------------
// 自适应类型版本
//--------------------------------------------------------------------------------
LPCTSTR	Player::GetAccoutNameT()
{
	return m_cAccountNameT;
}