/////////////////////////////////////////////////////////////////////////////
// Name:       client_mgr.h
// Purpose:     管理所有client
// Author:      
// Modified by:
// Created:     2008-12-09
// Copyright:   (c) 2004 TENGWU Entertainment All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef CLIENT_MGR_H_
#define CLIENT_MGR_H_

#define MAX_CLIENT 500
class Player;

class ClientMgr
{
public:
	//构造函数
	ClientMgr();
	//析构函数
	~ClientMgr();

	//初始化函数
	BOOL Init(HINSTANCE hInst);
	//更新函数
	VOID Update();
	//销毁函数
	VOID Destroy();

	// 压力限制管理线程
	VOID ThreadUpdate();

	// 控制消息处理
	DWORD CreateClient(LPCTSTR szAccount, LPCTSTR szNum, LPCTSTR szWayPoint);
	DWORD LoadClient(LPCTSTR szXML, LPCTSTR szNum);
	DWORD TerminateClient();
	DWORD ChatControl(LPCTSTR szActive, LPCTSTR szChannel = _T("5"));
	DWORD MoveControl(LPCTSTR szActive);

	// 资源载入/获取
	VOID InitWayPoint(Player* pPlayer, vector<vector<Vector3>*>& vecWayPoints);
	BOOL GetRandomLoginServer(tagLoginInfo* &pLoginInfo) { return m_listLoginInfo.RandPeek(pLoginInfo); }
	BOOL GetRandomChatContent(tstring& strChat) { return m_listChat.RandPeek(strChat); }

// 界面管理部分
private:
	TSFPTrunk<ClientMgr>		m_Trunk;
	TObjRef<Thread>				m_pThread;
	TObjRef<Util>				m_pUtil;
	TObjRef<GDIRender>			m_pRender;
	TObjRef<WindowWrap>			m_pWindow;
	TObjRef<GUISystem>			m_pGUI;
	IGUIRenderNative			m_render;

// 资源和控制部分
private:
	TObjRef<VarContainer>		m_pVar;
	TObjRef<VarContainer>		m_pConfigVar;
	TObjRef<Console>			m_pConsole;
	vector<vector<Vector3>*>	m_vecWayPoints;
	TList<tagLoginInfo*>		m_listLoginInfo;	//Login服务器信息容器
	TList<tstring>				m_listChat;			//喊话内容

// 机器人管理部分
private:
	VOLATILE BOOL				m_bTerminate;		// 线程停止标志
	INT32						m_nPressLimit;
	TList<Player*>				m_listAllPlayer;
	TList<Player*>				m_listWaiting;
	DWORD						m_dwTotalRunMinute;		// 机器人活动时间
};

extern ClientMgr* g_pClientMgr;

#endif /* CLIENT_MGR_H_ */





