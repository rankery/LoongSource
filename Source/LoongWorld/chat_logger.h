#pragma once

#define BUF_SIZE (1024*4)

class MsgCmd;
class ChatLogger
{
	ChatLogger();
	ChatLogger(const ChatLogger&);
	ChatLogger& operator=(const ChatLogger&);
	BOOL						m_bOn;
	TSFPTrunk<ChatLogger>		m_Trunk;
	MsgQueueTS*					m_pMsgQueue;
	TObjRef<Thread>				m_pThread;
	BOOL						m_bTerminated;
	TCHAR						m_pBuf[BUF_SIZE];
	int							m_nIndex;
	TObjRef<Log>				m_Logger;
	DWORD						m_dwChatCmdID;
	DWORD						m_dwEquipCmdID;
	DWORD						m_dwItemCmdID;
protected:
	UINT ThreadWriteLog();
	void FlushToDisk(); //none thread safe
	LPTSTR Format(const MsgCmd& msgCmd);
public:	
	static ChatLogger& getInstance();
	void Log(const MsgCmd& msgCmd);
	BOOL Init();
	void Destroy();
};

#define TheChatLogger (ChatLogger::getInstance())