#pragma once
#include "Mutex.h"
#include "Section.h"

class GMServer
{
public:
	GMServer(INT nPort, LPCTSTR szIP);

	BOOL		Init();
	BOOL		Connect();
	void		Update();

	void		SendMsg(LPVOID pMsg, DWORD dwSize);
	void		RecvMsg();

	DWORD		HandleGMServerDynInfo(tagNetCmd* pMsg, DWORD);

	void		PrintInfo(FLOAT fRate, BOOL bNewest);
	
	void		SetActive(BOOL bActive)	{ m_bActive = bActive; }
	BOOL		IsActive()	
	{ 
		if(m_bActive)
			return TRUE;
		else
			return FALSE; 
	}
	
	INT			GetSum()	{ return m_nSum; }
private:
	TSFPTrunk<GMServer>			m_Trunk;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<StreamTransport>    m_pStreamTransport;
	TObjRef<Util>				m_pUtil;
	TObjRef<Thread>				m_pThread;

	Mutex						m_lock;
	volatile BOOL				m_bTermConnect;

	TCHAR						m_szIP[X_SHORT_STRING];
	INT							m_nPort;

	BOOL						m_bActive;
	BOOL						m_bConnected;
	
	INT							m_nSum;
	TMap<INT, Section*>			m_mapSection;

	void						RegisterAllNetCommand();
	void						UnRegisterAllNetCommand();
};