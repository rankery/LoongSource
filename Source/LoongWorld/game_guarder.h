//-----------------------------------------------------------------------------
//!\file game_guarder.h
//!\author xlguo
//!
//!\date 2009-03-02
//! last 
//!
//!\brief 反外挂
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "mutex.h"

//-----------------------------------------------------------------------------
// ip转换
//-----------------------------------------------------------------------------
#pragma pack(push,1)
struct tagUserIP 
{
	char			chUserIPFlag;
	unsigned int	uIP;

#define ClientIpFlag  0x01
	tagUserIP(unsigned int ip = 0):chUserIPFlag(ClientIpFlag),uIP(ip){}
};
#pragma pack(pop)

//-----------------------------------------------------------------------------
// 反外挂客户端消息
//-----------------------------------------------------------------------------
struct tagNS_GameGuarder;

//-----------------------------------------------------------------------------
// 账号和消息的封装
//-----------------------------------------------------------------------------
struct tagAccountMsg
{
	DWORD				dwAccountID;
	tagNS_GameGuarder*	pMsg;
};

//-----------------------------------------------------------------------------
// 发送给Login的踢人消息
//-----------------------------------------------------------------------------
struct tagNWL_KickLog;

//-----------------------------------------------------------------------------
// 反外挂包装
//-----------------------------------------------------------------------------
class GameGuarder
{
public:
	GameGuarder();
	~GameGuarder();

	//-----------------------------------------------------------------------------
	// 开关
	//-----------------------------------------------------------------------------
	VOID		Switch(BOOL bOn){	if (bOn != m_bEnable)InterlockedExchange((LPLONG)&m_bEnable, bOn);	}

	//-----------------------------------------------------------------------------
	// 主线程UPDATE
	//-----------------------------------------------------------------------------
	VOID		Update();

public:
	//-----------------------------------------------------------------------------
	// 反外挂流程
	//-----------------------------------------------------------------------------
	long		Login(DWORD dwAccountID, LPCSTR szAccount, DWORD dwIP);
	long		Logout(DWORD dwAccountID, LPCSTR szAccount);
	long		Ret(DWORD dwAccountID, const char* pBuffer);
	long		Transport(DWORD dwAccountID, const char* pBuffer, const int nLen);
	long		UserData(DWORD dwAccountID, const char* pBuffer, const int nLen);

private:
	//-----------------------------------------------------------------------------
	// 内部方法
	//-----------------------------------------------------------------------------
	long		Recv(char cMsgId,DWORD dwAccountID,const char * pBuffer,int nLen);
	
	//-----------------------------------------------------------------------------
	// 保留到主线程执行
	//-----------------------------------------------------------------------------
	VOID		Wait2Send( INT nAccountID, INT nLen, LPCSTR pBuffer );
	VOID		Wait2Kick(INT nAccountID, DWORD dwKickTime, UINT16 u16ErrCode, UINT16 u16SealMark);
	
	//-----------------------------------------------------------------------------
	// 主线程执行
	//-----------------------------------------------------------------------------
	static long	SendMsg(signed int nAccountID,const char * pBuffer,int nLen);
	static VOID SendKickOut(const tagNWL_KickLog* pKickLog);

	//-----------------------------------------------------------------------------
	// 踢人回调
	//-----------------------------------------------------------------------------
	static long	Kick(signed int nAccountID,int nAction);
	
	//-----------------------------------------------------------------------------
	// 创建删除消息
	//-----------------------------------------------------------------------------
	static tagNS_GameGuarder*	CreateGuardMsg( INT nLen );
	static VOID	DeleteGuardMsg( tagNS_GameGuarder* pMsg );

private:
	static long (*m_spfRec) (char cMsgId,signed int nId,const char * pBuffer,int nLen);
	static BOOL		m_bEnable;

private:
	TSafeList<tagNWL_KickLog*>	m_listKickAccount;

	std::list<tagAccountMsg>	m_listMsg;
	Mutex						m_LockMsg;
};

extern GameGuarder g_gameGuarder;
