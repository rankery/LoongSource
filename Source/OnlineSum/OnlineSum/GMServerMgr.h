#pragma once
#include "GMServer.h"

#define CONFIG_FILE_PATH		"../Game/server_gm_config/"
#define CONFIG_INI				"config_showsum"

class GMServer;

class GMServerMgr
{
public:
	GMServerMgr();
	~GMServerMgr();

	BOOL							Init();
	void							Destroy(){}
	void							Update();

	TMap<INT, GMServer*>			GetGMServerMap()			{ return m_mapGMServer; }
	INT								GetGMServerCount()			{ return m_nGMServerCount; }

	/*void							SendMsg(GMServer* pGMServer, LPVOID pMsg, DWORD dwSize);
	void							RecvMsg(){};*/

	void							GetGMServerDynInfo();
	FLOAT							GetRate()					{ return m_fRate; }

	DWORD							HandleGMServerDynInfo();

private:
	TSFPTrunk<GMServerMgr>			m_Trunk;
	//TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<Thread>					m_pThread;
	TObjRef<VarContainer>			m_pVar;
	TObjRef<Log>					m_pLog;
	TObjRef<Util>					m_pUtil;
	Mutex							m_lock;

	TMap<INT, GMServer*>			m_mapGMServer;
	INT								m_nGMServerCount;
	INT								m_nSumOnline;

	FLOAT							m_fRate;				// 显示倍率
	DWORD							m_dwInterval;			// 刷新间隔
};

extern GMServerMgr g_GMServerMgr;