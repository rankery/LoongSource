#pragma once
#include "Singleton.h"

struct tagFatigueInfo;
struct tagNC_ANTIENTHRALL;
class FatigueServer;

//-------------------------------------------------------------------------
// 防沉迷Timer
//-------------------------------------------------------------------------
class FatigueTimer
{
	//accountnamecrc, fatigueinfo
	typedef TMap<DWORD, tagFatigueInfo*>	FatigueInfoMap;

public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	FatigueTimer(FatigueServer* pFatigueServer)
		:m_pUtil("Util"), m_pFatigueServer(pFatigueServer){}
	~FatigueTimer();


	//-------------------------------------------------------------------------
	// 更新
	//-------------------------------------------------------------------------
	VOID	Update();
	VOID	Reset();

	//-------------------------------------------------------------------------
	// 其他接口
	//-------------------------------------------------------------------------
	VOID	RecordOLGuardAccounts(tagNC_ANTIENTHRALL* pMsg);
	VOID	NotifyOLGuardPlayers(tagFatigueInfo* pFagitueInfo);
	VOID	NotifyLoginPlayer(LPCSTR szAccountName);

	template<typename OPERATION>
	VOID	ForEachFatigueInfo(OPERATION oper);
	FatigueInfoMap &GetFatigueInfoMap(){return m_mapAllFatigue;}

private:
	TObjRef<Util>				m_pUtil;
	TSafeMap<DWORD, LPSTR>		m_mapCrcName;		// 游戏世界名crc到游戏世界名的映射

	FatigueInfoMap				m_mapAllFatigue;	// 所有沉迷信息列表
	FatigueServer*				m_pFatigueServer;	// fatigueserver
};

template<typename OPERATION>
VOID FatigueTimer::ForEachFatigueInfo( OPERATION oper )
{
	FatigueInfoMap::TMapIterator itr = m_mapAllFatigue.Begin();
	tagFatigueInfo* pFatigueInfo = NULL;
	// 对所有防沉迷玩家
	while(m_mapAllFatigue.PeekNext(itr, pFatigueInfo))
	{
		if (!P_VALID(pFatigueInfo))
			break;
		oper(pFatigueInfo);
	}
}

//-------------------------------------------------------------------------
// 防沉迷服务器
//-------------------------------------------------------------------------
class FatigueServer : public Singleton<FatigueServer>
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	FatigueServer();
	~FatigueServer() {Destroy();}

public:
	//-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
	BOOL	Init();
	VOID	Destroy();

	VOID	SendMsg(PVOID pMsg, DWORD dwSize);

private:
	//-------------------------------------------------------------------------
	// 更新
	//-------------------------------------------------------------------------
	VOID	ThreadUpdate();

	//-------------------------------------------------------------------------
	// 连接线程
	//-------------------------------------------------------------------------
	DWORD	LoginCallBack(LPBYTE pByte, DWORD dwSize);
	DWORD	LogoutCallBack(LPBYTE pByte, DWORD dwSize);

	//-------------------------------------------------------------------------
	// 初始化配置
	//-------------------------------------------------------------------------
	BOOL	InitConfig();

	//-------------------------------------------------------------------------
	// 更新消息
	//-------------------------------------------------------------------------
	VOID	UpdateSession();


	//-------------------------------------------------------------------------
	// 消息处理函数
	//-------------------------------------------------------------------------
	DWORD	HandleOnLineAccounts(tagNetCmd* pCmd);
	DWORD	HandleQueryState( tagNetCmd* pCmd);

private:
	TSFPTrunk<FatigueServer>	m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<Thread>				m_pThread;
	TObjRef<StreamServer>		m_pSession;

	//-------------------------------------------------------------------------
	// 线程结束标志
	//-------------------------------------------------------------------------
	volatile BOOL				m_bTerminateUpdate;
	volatile BOOL				m_bTerminateConnect;

	//-------------------------------------------------------------------------
	// 连接参数
	//-------------------------------------------------------------------------
	INT							m_nPort;				// 连接中央认证服务器的端口

	FatigueTimer				m_FatigueTimer;
	DWORD						m_dwSectionID;

};

#define sFatigueServer FatigueServer::getSingleton()

