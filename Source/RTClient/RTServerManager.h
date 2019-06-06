//管理RTServer,单件

#ifndef RTSERVERMANAGER_H_
#define RTSERVERMANAGER_H_

#include "Singleton.h"
#include "../ServerDefine/rt_define.h"

class RTServer;
class PageAssist;

class RTServerManager : public Singleton<RTServerManager>
{
public:
	RTServerManager();
	~RTServerManager();

	//初始化
	bool Init();

	//读取配置文件
	bool ReadConfigFile();

	//销毁
	void Destroy();

	//连接RTServer
	void ConnectRTServer();

	//启动消息处理线程
	void HandleMessages();

	//请求服务器信息
	void QueryServerInfo();

	//设置PageAssist指针
	void SetPageAssistPtr(PageAssist *page_assist);

	//是否已经连上RTServer
	BOOL IsConnected();

	//是否已经初始化完毕
	bool IsInitSuccess();

	RTServer* GetRtServerPtr() { return rt_server_; }

	void SendStartServerMessage(DWORD section_id, DWORD world_id, EServerType server_type);
	void SendCloseServerMessage(BOOL is_force, DWORD section_id, DWORD world_id, EServerType server_type);
	void SendSystemCmdMessage(DWORD section_id, DWORD world_id, EServerType server_type, LPCTSTR szCmd);
	void SendChangeMaxMessage(DWORD section_id, DWORD world_id, LPCTSTR szCmd);
	void SendUpdateMallMessage(DWORD section_id, DWORD world_id);

	//GM管理
	void SendCreateGMMessage(LPCTSTR szName, DWORD dwPswCRC, EPrivilege ePrivilege);
	void SendModifyGMMessage(LPCTSTR szName, DWORD dwIP);
	void SendDeleteGMMessage(LPCTSTR szName);
	void SendQueryGMMessage();

	//GM权限判断
	bool VerifyGMPrivilege( DWORD dwMsgCrc, EPrivilege ePrivilege = EP_Null );

	//IP合法性判断
	DWORD IsIPValid(LPSTR szIP);

	//线程控制
	void TerminateMsgHandleThread() { ::InterlockedExchange((LONG*)&thread_update_terminate_, TRUE); }
	HANDLE			thread_update_handle_;
	volatile BOOL	thread_update_terminate_;

private:
	//读取配置文件辅助函数
	void RTServerManager::ReadRtServer(TiXmlElement* element);

	//更新线程
	static UINT ThreadUpdate(LPVOID param);

	//所有RTServer的列表
	RTServer* rt_server_;

	//PageAssist
	PageAssist *page_assist_;
};

//单件声明
#define sRTServerManager RTServerManager::getSingleton()

#endif /* RTSERVERMANAGER_H_ */