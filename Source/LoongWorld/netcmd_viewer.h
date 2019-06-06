#pragma once

#define BUF_SIZE (1024*10)

//class tagNetCmd;
class NetCmdViewer
{
	typedef std::vector<DWORD> AccountList;
	NetCmdViewer();
	NetCmdViewer(const NetCmdViewer&);
	NetCmdViewer& operator=(const NetCmdViewer&);
	AccountList*				m_pAccountList;
	TSFPTrunk<NetCmdViewer>		m_Trunk;
	MsgQueueTS*					m_pMsgQueue;
	TObjRef<Thread>				m_pThread;
	BOOL						m_bTerminated;
	BYTE						m_pBuf[BUF_SIZE];
	int							m_nIndex;
	DWORD						m_dwChatCmdID;
	DWORD						m_dwEquipCmdID;
	DWORD						m_dwItemCmdID;
	DWORD						m_dwFileHandle;
	TObjRef<DiscIOMgr>			m_pDiscIOMgr;
	TCHAR						m_szDefaultName[MAX_PATH];
	std::vector<AccountList*>	m_History;
protected:
	UINT ThreadWriteLog();
	void FlushToDisk(); //none thread safe
	virtual BOOL CreateFile();
	void RegisterCmd();
	DWORD LogNetCmd(LPCTSTR strAccountID);
	DWORD UnLogNetCmd(LPCTSTR strAccountID);
public:	
	static NetCmdViewer& getInstance();
	void Log(DWORD accountID, DWORD roleID, const tagNetCmd& msgCmd);
	BOOL Init();
	void Destroy();
};

#define TheNetCmdViewer (NetCmdViewer::getInstance())