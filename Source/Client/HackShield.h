#pragma once

long NetSendToGameServer(const char * pBuffer,int nLen);
struct tagNS_GameGuarder;
class HackShield
{
public:
	HackShield(void);
	~HackShield(void);

	static HackShield* Inst();

	void Init();

	/**
	功能	:	Game Client 提供给Apex Client 的发送函数;
			Apex Client 就用这个函数 发送数据;
	参数		pBuffer	 发送数据的缓冲区
			nLen	 发送数据的长度
	*/
	friend long NetSendToGameServer(const char * pBuffer,int nLen);

	/**
	启动ApexClient;
	*/
	int	StartApexClient();

	/**
	停止ApexClient
	*/
	int	StopApexClient();

	/**
	通知ApexClient,有数据从 GameServer 发过来
	参数	:	pBuf	接收数据的缓冲区
			nBufLen 接收数据的长度
	*/
	DWORD	NoticeApec_UserData(tagNS_GameGuarder* pMsg, DWORD pPrama);

private:
	TSFPTrunk<HackShield>		m_Trunk;

};