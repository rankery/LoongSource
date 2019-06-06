#ifndef RTSERVER_H_
#define RTSERVER_H_

#include "../ServerDefine/rt_define.h"

#define MAX_WAIT_TIME 60*10	// 单位：秒，等待来自GMserver的动态游戏服务器信息的最大时间

class StreamTransport;
class PageAssist;
class RTServer
{
public:
	RTServer();
	~RTServer();
	
	//初始化
	bool Init();
	//销毁
	void Destroy();

	//注册消息处理函数
	void RegiserHandleMessage();
	
	void RecvMessage();

	//辅助函数
	PageAssist* GetPageAssist() { return page_assist_; }
	void SetPageAssist(PageAssist* page_assist) { page_assist_ = page_assist; }

	void SetIP(const char* ip) { ip_ = ip; }
	wxString& GetIP() { return ip_; }

	void SetPort(int port) { port_ = port; }
	int GetPort() { return port_; }

	bool IsFirstConnected() { return is_first_connect_; }
	void SetFirstConnected( bool first_connect ) { is_first_connect_ = first_connect; }
	BOOL IsConnected();
	BOOL IsTryingToConnent();
	bool IsInitSuccess() { return is_init_success; }
	void Connected();
	void Disconnect();
    void SendMsg(LPVOID msg, DWORD size);

	//--当前GM权限
	void SetPrivilege( EPrivilege ePri );
	EPrivilege GetPrivilege();
	bool VerifyGMPrivilege( DWORD dwMsgCrc, EPrivilege ePrivilege = EP_Null );
	DWORD IsIPValid(LPSTR szIP);

private:
	//处理NRTS_LoginService
	DWORD HandleLoginService(tagNetCmd *command);
	//处理NRTS_Challenge
	DWORD HandleChanllenge(tagNetCmd *command);
	//处理NRTS_Close
	DWORD HandleCloseServer(tagNetCmd *command);
	//处理NRTS_Start
	DWORD HandleStartServer(tagNetCmd *command);
	//处理NRTS_AllServerStatInfo
	DWORD HandleAllServerStatInfo(tagNetCmd *command);
	//处理NRTS_AllServerDynaInfo
	DWORD HandleAllServerDynaInfo(tagNetCmd *command);
	//处理NRTS_PrivilegeInfo
	DWORD HandlePrivilegeInfo(tagNetCmd *command);
	
	//处理NGMTS_GMCreate
	DWORD HandleGMCreateResult(tagNetCmd* command);
	//处理NGMTS_GMModify
	DWORD HandleGMModifyResult(tagNetCmd* command);
	//处理NGMTS_GMDelete
	DWORD HandleGMDeleteResult(tagNetCmd* command);

private:
	//PageAssist
	PageAssist *page_assist_;
	//网络
	wxString ip_;
	int port_;
	bool is_connect_;
	bool is_first_connect_;
	bool is_init_success;
	StreamTransport* net_;
	NetCommandManager<RTServer> net_command_manager_;
	//权限表,RTServer通过发送消息，来填充这个map,以后所有关于权限的操作都以这个为准
	std::map<DWORD, DWORD> privilege_;
	tagDWORDTime dwLatestUpdateTime_; //记录最近一次收到来自GMserver的动态游戏服务器信息的时间
};



#endif /* RTSERVER_H_ */