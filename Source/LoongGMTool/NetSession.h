#pragma once

class NetSessionMgr;
class NetSession
{
public:
	NetSession(void);
	~NetSession(void);

	//初始化
	bool Init();

	//销毁
	void Destroy();

	//注册消息处理函数
	void RegiserHandleMessage();

	//接收网络消息
	void RecvMessage();

	//--
	BOOL IsConnected();
	BOOL IsTryingToConnect();
	VOID TryToConnect(LPCSTR szIP, INT nPort=0);
	VOID StopTryingConnect();
	void SendMsg(LPVOID msg, DWORD size);

	//--网络消息
	DWORD OnNGMTS_GMCreate(tagNetCmd* pMsg);
	DWORD OnNGMTS_GMModify(tagNetCmd* pMsg);
	DWORD OnNGMTS_GMDelete(tagNetCmd* pMsg);

	DWORD OnNRTS_Challenge(tagNetCmd* pMsg);
	DWORD OnNRTS_PrivilegeInfo(tagNetCmd* pMsg);
	DWORD OnNRTS_AllServerStatInfo(tagNetCmd* pMsg);
	DWORD OnNRTS_AllServerDynaInfo(tagNetCmd* pMsg);
	//DWORD OnNGMTS_Double(tagNetCmd* pMsg);
	//DWORD OnNGMTS_Notice(tagNetCmd* pMsg);
	//DWORD OnNGMTC_CastRun(tagNetCmd* pMsg);
	//DWORD OnNGMTS_AutoNotice(tagNetCmd* pMsg);
	DWORD OnNGMTS_ItemCreate(tagNetCmd* pMsg);
	DWORD OnNGMTS_ItemDelete(tagNetCmd* pMsg);
	DWORD OnNGMTS_ItemResume(tagNetCmd* pMsg);
	DWORD OnNGMTS_RoleResume(tagNetCmd* pMsg);
	DWORD OnNGMTS_ProModify(tagNetCmd* pMsg);
	DWORD OnNGMTS_TimeLimitedMsg(tagNetCmd* pMsg);
	DWORD OnNGMTS_AccountForbid(tagNetCmd* pMsg);
	DWORD OnNGMTS_IPBlacklist(tagNetCmd* pMsg);
	DWORD OnNGMTS_CancelTimeLimitedMsg(tagNetCmd* pMsg);
	DWORD OnNGMTS_CancelDouble(tagNetCmd* pMsg);
	DWORD OnNGMTS_CancelRightNotice(tagNetCmd* pMsg);

private:
	NetCommandManager<NetSession>	m_net_command_mgr;
	StreamTransport*				m_pStream;
};