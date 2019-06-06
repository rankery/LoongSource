//-----------------------------------------------------------------------------
//!\file session.cpp
//!\author Lyp
//!
//!\date 2004-07-07
//! last 2008-04-17
//!
//!\brief 服务器端会话层
//-----------------------------------------------------------------------------
#include "..\stdafx.h"
#include "session.h"

// 使用其它部件
#include "transport.h"
#include "..\console\console_gui.h"

namespace vEngine {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
CompletionSession::CompletionSession():m_Trunk(this)
{
	// 登陆回调函数
	m_fnLogIn = NULL;
	m_fnLogOut = NULL;

	m_pTransport = new CompletionTransport;
	InitializeCriticalSectionAndSpinCount(&m_LockLog, DEFAULT_SPIN_COUNT);

	m_dwIDHold = 0;

	// unit统计
	m_dwGetFreeUnitCounter = 0;
	m_dwReturnUnitCounter = 0;
	m_dwSendUnitCounter = 0;
	m_dwRecvUnitCounter = 0;
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
CompletionSession::~CompletionSession()
{
	DeleteCriticalSection(&m_LockLog);
	SAFE_DEL(m_pTransport);
}


//-----------------------------------------------------------------------------
// 初始化完成端口网络会话层
//-----------------------------------------------------------------------------
BOOL CompletionSession::Init(tagServerInitParam* pInitParam)
{
	ASSERT(m_fnLogIn);	// 外部应该先调用SetLogCallBack函数注册簦入登出回调函数

	InterlockedExchange((LONG*)&m_bDestroy, FALSE);
	m_mapClient.Clear();

	// 登记回调函数
	m_pTransport->SetLogCallback((LOGINCALLBACK)m_Trunk.sfp2(&vEngine::CompletionSession::LogInCallback),
		(LOGOUTCALLBACK)m_Trunk.sfp1(&vEngine::CompletionSession::LogOutCallback));

	// 初始化传输底层
	BOOL bResult = m_pTransport->Init(pInitParam);
	if( FALSE == bResult)
		return FALSE;

	// 注册几个调试用的控制台命令
	TObjRef<Console>()->Register(_T("cn"), m_Trunk.sfp0(&vEngine::CompletionSession::GetClientNum), 
		_T("get number of client"), 0 );

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL CompletionSession::Destroy()
{
	InterlockedExchange((LONG*)&m_bDestroy, TRUE);

	std::list<DWORD> list;
	m_mapClient.ExportAllKey(list);
	std::list<DWORD>::iterator it;
	for(it=list.begin(); it!=list.end(); ++it)
		this->Kick((*it));

	while( m_mapClient.Size() > 0 )	// 等待所有连接断开
		Sleep(10);

	if( m_pTransport )
		m_pTransport->Destroy();

	m_fnLogIn = NULL;
	m_fnLogOut = NULL;

	return TRUE;
}


//-----------------------------------------------------------------------------
// SetLogCallBack
//-----------------------------------------------------------------------------
VOID CompletionSession::SetLogCallBack(LOGINCALLBACK fnLogIn, 
											LOGOUTCALLBACK fnLogOut)
{
	// 登陆回调函数
	m_fnLogIn = fnLogIn;
	m_fnLogOut = fnLogOut;
}



//-----------------------------------------------------------------------------
// 将信息发给指定客户端,可以发送大于500字节的信息
//-----------------------------------------------------------------------------
BOOL CompletionSession::Send(DWORD dwID, LPVOID pMsg, DWORD dwSize, INT EncryptNum)
{
    ASSERT(dwSize < 1024*16);   // 超过16K就报错

	INT nCDIndex = m_mapClient.Peek(dwID);
	if( GT_INVALID == nCDIndex )
		return 0;

    INT nPackage = (dwSize + sizeof(DWORD)) / GT_MAX_PACKAGE_LEN;
    if( (DWORD)nPackage * GT_MAX_PACKAGE_LEN < dwSize + sizeof(DWORD) )
        nPackage += 1;
 
    LPBYTE pSrc = (LPBYTE)pMsg;
    DWORD dwUnsendSize = dwSize + sizeof(DWORD);
    tagUnitData* pUnit = NULL;
	BYTE byRand = (BYTE)EncryptNum;
 
    for(INT n=0; n<nPackage; n++)
    {
        pUnit = this->GetFreeUnit();
        pUnit->nCDIndex = nCDIndex;
		pUnit->byEncryptNum = byRand;
 
        // 计算unit大小
        if( dwUnsendSize > GT_MAX_PACKAGE_LEN )
            pUnit->wbuf.len = GT_MAX_PACKAGE_LEN;
        else
            pUnit->wbuf.len = dwUnsendSize;
 
        if( 0 == n ) // 第一个包因为有包长度信息,所以需要特殊处理
        {
			pUnit->bFirstUnitOfPackage = true;
            *(DWORD*)&pUnit->wbuf.buf[0] = dwSize;  // 包长度
            LPBYTE pDest = ((LPBYTE)pUnit->wbuf.buf) + sizeof(DWORD);
            memcpy(pDest, pSrc, pUnit->wbuf.len-sizeof(DWORD));
            pSrc += pUnit->wbuf.len-sizeof(DWORD);
            
        }
        else
        {
			pUnit->bFirstUnitOfPackage = false;
            memcpy(pUnit->wbuf.buf, pSrc, pUnit->wbuf.len);
            pSrc += pUnit->wbuf.len;
        }
 
        dwUnsendSize -= pUnit->wbuf.len;
        this->Send(pUnit);
	}

	return TRUE;
}






//-----------------------------------------------------------------------------
// 踢出指定ID的玩家
//-----------------------------------------------------------------------------
BOOL CompletionSession::Kick(DWORD dwClientID)
{
	// 需要将ClientID转化成SerialID
	INT nCDIndex = m_mapClient.Peek(dwClientID);
	if( GT_INVALID != nCDIndex )
	{
		DWORD dwSerialID = m_pTransport->GetSerialID(nCDIndex) ;
		m_pTransport->Kick(dwSerialID);

		// 发送一个0字节消息
		char mess[16] = {0};
		this->Send(dwClientID, mess, 0);
		return TRUE;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// 得到指定ID客户端的sock name
//-----------------------------------------------------------------------------
BOOL CompletionSession::GetClientSockName(sockaddr_in& addr_in, DWORD dwID)
{
	INT nCDIndex = m_mapClient.Peek(dwID);
	if( FALSE == GT_VALID(nCDIndex) )
		return FALSE;

	return GetUnauthClientSockName(addr_in, nCDIndex);
}


//-----------------------------------------------------------------------------
// 得到未验证客户端的sock name
//-----------------------------------------------------------------------------
BOOL CompletionSession::GetUnauthClientSockName(sockaddr_in& addr_in, INT nCDIndex)
{
	DWORD dwSerialID = m_pTransport->GetSerialID(nCDIndex);
	if( FALSE == m_pTransport->GetClientSockName(addr_in, dwSerialID) )
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------
// login callback function
//-----------------------------------------------------------------------------
UINT CompletionSession::LogInCallback(tagUnitData* pFirstUnit, tagLoginParam* param)
{
	DWORD dwID = GT_INVALID;
	if( m_bDestroy )
		return GT_INVALID;

	EnterCriticalSection(&m_LockLog);

	// 用第一个包进行身份验证
	dwID = (*m_fnLogIn)(pFirstUnit, param);	// 调用登陆回调函数
	if( dwID != GT_INVALID )
	{
		// 注意此处写法，外部如果调用send(ID,...)函数时需要等待
		// 第一个包收到并处理后才行，因为在第一个包收到之前可能m_mapClient
		// 尚未准备好
		m_mapClient.Add(dwID, pFirstUnit->nCDIndex);
	}

	LeaveCriticalSection(&m_LockLog);

	return dwID;
}

//-----------------------------------------------------------------------------
// logout callback function
//-----------------------------------------------------------------------------
UINT CompletionSession::LogOutCallback(DWORD dwID)
{
	EnterCriticalSection(&m_LockLog);

	if( m_mapClient.IsExist(dwID) )
	{
		if( m_fnLogOut )	// 必须先调用注销回调函数
			(*m_fnLogOut)(dwID);

		m_mapClient.Erase(dwID);
	}

	LeaveCriticalSection(&m_LockLog);
	return 0;
}




} // namespace wEngine {