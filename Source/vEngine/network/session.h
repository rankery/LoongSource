//-----------------------------------------------------------------------------
//!\file session.h
//!\author Lyp
//!
//!\date 2004-07-07
//! last 2008-04-17
//!
//!\brief 服务器端会话层
//-----------------------------------------------------------------------------
#pragma once
#include "transport.h"

namespace vEngine {
//-----------------------------------------------------------------------------
//!\brief 服务器端会话层, 支持3000以上连接(通过测试)
//-----------------------------------------------------------------------------
class VENGINE_API CompletionSession
{
public:
	BOOL Init(tagServerInitParam* pInitParam);
	BOOL Destroy();

	//-----------------------------------------------------------------------------
	// 网络基本操作
	//-----------------------------------------------------------------------------
	//! 设置登陆回调函数,外部需要保证两个回调函数线程安全
	//! 注意当用户登录时,会将第一个unit作为参数传给fnLogIn,fnLogIn必须返回一个ID
	//! 当用户登出时,会将上层给的ID作为参数传给fnLogOut
	VOID SetLogCallBack(LOGINCALLBACK fnLogIn, LOGOUTCALLBACK fnLogOut);

	//--------------------------------------------------
	// 优化收发
	//--------------------------------------------------
	// 将指定信息包发送到玩家(unit.nCDIndex必须指定)
	// 在send之前先用getfreeunit得到可用的unit
	// 返回此用户还有多少包未发出去
	__forceinline INT Send(tagUnitData* pUnit);
	// 从网络收unit,处理完后应该调用ReturnUnit交还unit
	// 注意得到的unit.wbuf.buf前四个字节不是真正内容(是网络包实际长度),真正消息从第5字节开始
	__forceinline tagUnitData* RecvUnitEx(DWORD& dwRecvedNum, INT nCDIndex);
	// 外部得到空闲的unit
	tagUnitData* GetFreeUnit() { ++m_dwGetFreeUnitCounter; return m_pTransport->GetFreeUnit(); }
	// 外部返还用完的unit
	VOID ReturnUnit(tagUnitData* pUnit) { ++m_dwReturnUnitCounter; m_pTransport->ReturnUnit(pUnit); }


	//--------------------------------------------------
	// 普通收发,为了保证效率,应尽量用上面的一组收发函数
	//--------------------------------------------------
	// 按照普通格式发送,内部多一次memcpy以及相应的GetFreeUnit
	BOOL Send(DWORD dwID, LPVOID pMsg, DWORD dwSize, INT EncryptNum=0);
	// 从网络收unit,处理完后应该调用ReturnUnit交还unit
	// 注意得到的unit.wbuf.buf前四个字节不是真正内容(是网络包实际长度),真正消息从第5字节开始
	__forceinline tagUnitData* RecvUnit(DWORD dwID);



	// 踢出指定玩家,返回FALSE代表找不到指定玩家
	BOOL Kick(DWORD dwClientID);

	//-----------------------------------------------------------------------------
	// 信息统计以及工具函数
	//-----------------------------------------------------------------------------
	// 得到发出wsasend但是没有收到完成通知的包数目
	__forceinline INT GetSendCast(INT nCDIndex) { return m_pTransport->GetSendCast(nCDIndex); }

	// 得到尚未发送完成的unit数目
	INT	GetUnsendUnitNum() { return m_pTransport->GetUnsendUnitNum(); } 

	// 得到连接上的客户端数目
	INT	GetClientNum() { return m_mapClient.Size(); }  
	// 检查指定ID的客户是不是存在
	BOOL IsClientExsit(DWORD dwID) { return m_mapClient.IsExist(dwID); }

	// 得到用户ID表
	VOID GetClientList(std::list<DWORD>& list) { m_mapClient.ExportAllKey(list); }

	// 得到指定ID客户端的sock name
	BOOL GetClientSockName(sockaddr_in& addr_in, DWORD dwID);
	// 得到未验证客户端的sock name
	BOOL GetUnauthClientSockName(sockaddr_in& addr_in, INT nCDIndex);
	
	// 得到服务器端口号
	INT GetPort() { return m_pTransport->GetPort(); }
	// 得到内部unit等信息
	TCHAR* GetInfo() { return m_pTransport->GetInfo(); }

	// 得到测试统计数据
	DWORD GetCounterOfGetFreeUnit()	{ return m_dwGetFreeUnitCounter; }
	DWORD GetCounterOfReturnUnit()	{ return m_dwReturnUnitCounter; }
	DWORD GetCounterOfSendUnit()	{ return m_dwSendUnitCounter; }
	DWORD GetCounterOfRecvUnit()	{ return m_dwRecvUnitCounter; }

	CompletionSession();
	~CompletionSession();

private:
	TSFPTrunk<CompletionSession>	m_Trunk;
	CompletionTransport*			m_pTransport;
	CRITICAL_SECTION				m_LockLog;
	LOGINCALLBACK					m_fnLogIn;	// 各种回调函数
	LOGOUTCALLBACK					m_fnLogOut;
	
	VOLATILE BOOL					m_bDestroy;	// 是否需要关闭
	TSafeMap<DWORD, INT>			m_mapClient;// 记录用户nCDIndex和ID情况
	DWORD							m_dwIDHold;	// ID种子

	// unit统计
	DWORD							m_dwGetFreeUnitCounter;
	DWORD							m_dwReturnUnitCounter;
	DWORD							m_dwSendUnitCounter;
	DWORD							m_dwRecvUnitCounter;


	UINT	LogInCallback(tagUnitData* pFirstUnit, tagLoginParam* param);
	UINT	LogOutCallback(DWORD dwID);
};


//-----------------------------------------------------------------------------
// 将指定信息包发送到指定ID的玩家,
// 在send之前先用getfreeunit得到可用的unit
// 返回此用户还有多少包未发出去
//-----------------------------------------------------------------------------
INT CompletionSession::Send(tagUnitData* pUnit)
{
	ASSERT( pUnit );

	// 对于一段完整的信息被切分成n个包的情况，下面这句肯定出assert
	//ASSERT( pUnit->wbuf.len == *(DWORD*)(&pUnit->wbuf.buf[0]) + sizeof(DWORD));
	++m_dwSendUnitCounter;
	return m_pTransport->SendUnit(pUnit);
}


//-----------------------------------------------------------------------------
// 从网络收unit,处理完后应该调用returnunit交还unit
//-----------------------------------------------------------------------------
tagUnitData* CompletionSession::RecvUnit(DWORD dwID)
{	
	DWORD dwTemp; 
	INT nCDIndex = m_mapClient.Peek(dwID);
	if( GT_INVALID == nCDIndex )
		return NULL;

	tagUnitData* pUnit = m_pTransport->RecvUnit(dwTemp, nCDIndex);
	if( pUnit )
		++m_dwRecvUnitCounter;

	return pUnit; 
}


//-----------------------------------------------------------------------------
// 收取网络信息
//-----------------------------------------------------------------------------
tagUnitData* CompletionSession::RecvUnitEx(DWORD& dwRecvedNum, INT nCDIndex)
{ 
	tagUnitData* pUnit = m_pTransport->RecvUnit(dwRecvedNum, nCDIndex); 
	if( pUnit )
		++m_dwRecvUnitCounter;

	return pUnit;
}



} // namespace wEngine {