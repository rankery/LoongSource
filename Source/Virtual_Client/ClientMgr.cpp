#include "stdafx.h"
#include "ClientMgr.h"
#include "Player.h"
#include "..\WorldDefine\login.h"
#include "..\WorldDefine\selectrole.h"

//------------------------------------------------------------------------------
// CONSTANT
//------------------------------------------------------------------------------
#define CONFIG_INI		"virtual_client_config\\vc_config.ini"
#define VC_PATH			"virtual_client_config\\"

//-----------------------------------------------------------------------------
// 全局变量
//-----------------------------------------------------------------------------
ClientMgr* g_pClientMgr;

//-------------------------------------------------------------------------------
// 构造函数
//-------------------------------------------------------------------------------
ClientMgr::ClientMgr():m_Trunk(this)
{
	m_listLoginInfo.Clear();
	m_listWaiting.Clear();
}

//-------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------
ClientMgr::~ClientMgr()
{
	Destroy();
}

//-------------------------------------------------------------------------------
// 初始化函数
//-------------------------------------------------------------------------------
BOOL ClientMgr::Init(HINSTANCE hInst)
{
	//m_pRender	= "GDIRender";
	//m_pWindow	= "WindowWrap";
	//m_pGUI		= "GUISystem";
	//m_pConsole	= "Console";

	// 界面初始化
	m_render.Init(m_pRender);
	BOOL bRet = m_pWindow->Init(&m_render, 800, 600, TRUE);
	if ( bRet != TRUE )
	{
		MessageBox(NULL, _T("Init失败"), _T("LoginServer"), MB_OK);
		return FALSE;
	}
	bRet = m_pWindow->CreateWnd( _T("Virtual_Client"), hInst );
	if ( bRet != TRUE )
	{
		MessageBox(NULL, _T("创建窗口失败"), _T("LoginServer"), MB_OK);
		return FALSE;
	}
	//m_pWindow->CreateWnd(_T("V_V"), hInst);
	if ( !m_pRender->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight()) )
	{
		MessageBox(NULL, _T("m_pRender失败"), _T("LoginServer"), MB_OK);
		return FALSE;
	}
	
	m_pGUI->Init(m_pWindow->GetWidth(), m_pWindow->GetHeight(), &m_render);
	m_pRender->SetShowFPS(FALSE);

	m_pConsole->Init(NULL, 1000, TRUE);
	m_pConsole->SetCaption(_T("(O_O)"));


	//注册函数
	m_pConsole->Register(_T("new"),		m_Trunk.sfp3(&ClientMgr::CreateClient),		_T("创建客户端"),					3);
	m_pConsole->Register(_T("load"),	m_Trunk.sfp2(&ClientMgr::LoadClient),		_T("读取客户端xml文件"),			2);
	m_pConsole->Register(_T("stop"),	m_Trunk.sfp0(&ClientMgr::TerminateClient),	_T("停止所有客户端活动线程"),		0);
	m_pConsole->Register(_T("chat"),	m_Trunk.sfp2(&ClientMgr::ChatControl),		_T("客户端开始喊话"),				2);
	m_pConsole->Register(_T("run"),		m_Trunk.sfp1(&ClientMgr::MoveControl),		_T("客户端开始移动"),				1);


	//读取配置文件: ID, name, password 
	CreateObj("ClientContainer", "VarContainer");
	m_pVar = "ClientContainer";
	m_pUtil = "Util";
	m_pThread = "Thread";

	// 读取机器人配置文件
	CreateObj("VC_Config", "VarContainer");
	m_pConfigVar = "VC_Config";
	if (!P_VALID(m_pConfigVar))
	{
		return FALSE;
	}
	m_pConfigVar->Load("VirtualFileSys",_T(CONFIG_INI));

	//读取LoginServer信息
	INT nLoginNum	= m_pConfigVar->GetInt(_T("num login_server"));
	for (int n=0; n<nLoginNum; n++)
	{
		TCHAR szTemp[X_SHORT_NAME] = {_T('\0')};
		_stprintf(szTemp, _T("login_%d"), n);

		tagLoginInfo* pLoginInfo = new tagLoginInfo;
		if (!P_VALID(pLoginInfo))
		{
			n--;
			continue;
		}
		pLoginInfo->nPort			= m_pConfigVar->GetInt(_T("port"), szTemp);
		pLoginInfo->strLoginIP		= m_pConfigVar->GetString(_T("ip"), szTemp);

		INT nWorldNum				= m_pConfigVar->GetInt(_T("world_num"), szTemp);
		pLoginInfo->listWorldName	= new TList<tstring>;
		for (int i=0; i<nWorldNum; i++)
		{
			TCHAR szTempWorld[X_SHORT_NAME] = {_T('\0')};
			_stprintf(szTempWorld, _T("world_%d_name"), i);			
			tstring strWorldName	= m_pConfigVar->GetString(szTempWorld, szTemp);
			pLoginInfo->listWorldName->PushFront(strWorldName);
		}
		//pLoginInfo->strWorldName	= m_pConfigVar->GetString(_T("world_name"), szTemp);

		m_listLoginInfo.PushFront(pLoginInfo);
	}

	//读取路点文件
	INT32 nWayPointNum = m_pConfigVar->GetInt(_T("num way_point"));
	for (INT32 i=1; i<=nWayPointNum; i++)
	{
		stringstream tss;
		tstring strMapName = m_pConfigVar->GetString(_T("map way_point"));
		
		tss<<VC_PATH<<m_pUtil->UnicodeToUnicode8(strMapName.c_str())<<"_"<<i<<".log";
		ifstream in(tss.str().c_str());
		
		vector<Vector3>* pVec3 = new vector<Vector3>;
		m_vecWayPoints.push_back(pVec3);

		string strtmp;
		while(getline(in, strtmp, '\n'))
		{
			Vector3 vec;
			size_t start =0;
			size_t end = 0;
			end =strtmp.find(' ');
			vec.x = (float)atof(strtmp.substr(start, end).c_str());
			start = end + 1;
			end = strtmp.find(' ', start);
			vec.y = (float)atof(strtmp.substr(start, end).c_str());
			start = end + 1;
			vec.z = (float)atof(strtmp.substr(start).c_str());
			pVec3->push_back(vec);
		}
	}

	// 读取喊话内容
	stringstream tssChat;
	tssChat<<VC_PATH<<"chat_content.txt";
	ifstream inChat(tssChat.str().c_str());

	string strChat;
	while (getline(inChat, strChat, '\n'))
	{
		tstring strContent(m_pUtil->AnsiToUnicode(strChat.c_str()));
		m_listChat.PushBack(strContent);
	}

	if (m_listChat.Empty())
	{
		tstring strTmp(_T("冲啊!\n"));
		m_listChat.PushBack(strTmp);
	}

	// 读取压力信息
	m_nPressLimit = m_pConfigVar->GetInt(_T("press virtual_client"));

	// 开启压力限制管理线程
	m_pThread->CreateThread(_T("PressMgr"), (vEngine::THREADPROC)m_Trunk.sfp0(&ClientMgr::ThreadUpdate), NULL);
	while( FALSE == m_pThread->IsThreadActive(_T("PressMgr")) )
	{
		Sleep(10);
		continue;
	}

	return TRUE;
}

//-------------------------------------------------------------------------------
// 销毁函数
//-------------------------------------------------------------------------------
VOID ClientMgr::Destroy()
{
	m_pWindow->Destroy();
	m_pGUI->Destroy();

	// 删除配置文件
	KillObj("ClientContainer");
	KillObj("VC_Config");

	// 结束压力监控线程
	if (TRUE == m_pThread->IsThreadActive(_T("PressMgr")))
	{
		InterlockedExchange( (LONG*)&m_bTerminate, TRUE );
		m_pThread->WaitForThreadDestroy( _T("PressMgr"), INFINITE );
	}

	//清除路点包
	vector<vector<Vector3>*>::iterator it = m_vecWayPoints.begin();
	for (; it != m_vecWayPoints.end(); it++)
	{
		SAFE_DEL(*it);
	}

	//清空Login信息
	tagLoginInfo* pLoginInfo = NULL;
	m_listLoginInfo.ResetIterator();
	while (m_listLoginInfo.PeekNext(pLoginInfo))
	{
		SAFE_DEL(pLoginInfo);
	}
	m_listLoginInfo.Clear();


	Player* pPlayer = NULL;
	pPlayer = m_listAllPlayer.PopFront();
	while (P_VALID(pPlayer))
	{
		SAFE_DEL(pPlayer);
		pPlayer = m_listAllPlayer.PopFront();
	}

	pPlayer = m_listWaiting.PopFront();
	while (P_VALID(pPlayer))
	{
		SAFE_DEL(pPlayer);
		pPlayer = m_listWaiting.PopFront();
	}
}

//-------------------------------------------------------------------------------
// 更新函数
//-------------------------------------------------------------------------------
VOID ClientMgr::Update()
{	
	DWORD dwMsg, dwParam1, dwParam2;
	static DWORD dwTimeKeeper = timeGetTime();

	while (FALSE == m_pWindow->MsgLoop())
	{
		if (FALSE == m_pWindow->IsWindowActive())
		{
			Sleep(30);
			continue;
		}
		
		while (m_pWindow->PeekWindowsMsg(dwMsg, dwParam1, dwParam2))
		{
			m_pGUI->OnWindowsMsg(dwMsg, dwParam1, dwParam2);
			if (dwMsg == WM_QUIT)
				return;
		}
		
		m_dwTotalRunMinute = timeGetTime() - dwTimeKeeper;
		INT nHour = m_dwTotalRunMinute / 3600000;
		INT nMin = (m_dwTotalRunMinute % 3600000) / 60000;
		INT nSec = ((m_dwTotalRunMinute % 3600000) % 60000) / 1000;

		// 加入要观察的实时变量
		m_pConsole->Watch(_T("sec: "),					nSec);
		m_pConsole->Watch(_T("min: "),					nMin);
		m_pConsole->Watch(_T("hour: "),					nHour);

		// 华丽的分割线
		m_pConsole->Watch(_T("---------------------"), 0);
		m_pConsole->Watch(_T("-- active --"), m_listAllPlayer.Size());
		m_pConsole->Watch(_T("-- waiting --"), m_listWaiting.Size());

		tagLoginInfo* pLogInfo = NULL;
		m_listLoginInfo.ResetIterator();
		while (m_listLoginInfo.PeekNext(pLogInfo))
		{
			m_pConsole->Watch(pLogInfo->strLoginIP.c_str(), 0);
			m_pConsole->Watch(_T("InLogin:"), pLogInfo->nInLogin);
			m_pConsole->Watch(_T("InSelect:"), pLogInfo->nInSelect);
			m_pConsole->Watch(_T("InWorld:"), pLogInfo->nInWorld);
		}
		m_pGUI->Update();

		Sleep(50);

		m_pRender->Clear(0,0,0);
		m_pGUI->Render();
		m_pRender->Render();
	}
}

VOID ClientMgr::InitWayPoint(Player *pPlayer, vector<vector<Vector3>*>& vecWayPoints)
{
	if(pPlayer == NULL)
		return;
	if (vecWayPoints.empty())
		return;
	if (pPlayer->m_wayPoint < 1 || pPlayer->m_wayPoint > (INT)vecWayPoints.size())
		return;
	if (!P_VALID(vecWayPoints[pPlayer->m_wayPoint-1]))
		return;

	pPlayer->m_vecWayPoint = *vecWayPoints[pPlayer->m_wayPoint-1];
	Vector3 vector3 = pPlayer->m_vecWayPoint[0];
	pPlayer->m_desPos = vector3;
	pPlayer->m_Start_x =  (INT)vector3.x / 50;
	pPlayer->m_Start_z = (INT)vector3.z / 50;
	pPlayer->m_it = pPlayer->m_vecWayPoint.begin();
}

// 控制台命令处理部分
DWORD ClientMgr::CreateClient(LPCTSTR szAccount, LPCTSTR szNum, LPCTSTR szWayPoint)
{
	DWORD dwAccount = _ttol(szAccount);
	DWORD dwNum = _ttol(szNum);
	//DWORD dwWayPoint = _ttol(szWayPoint);

	return 0;
}

DWORD ClientMgr::LoadClient( LPCTSTR szXML, LPCTSTR szNum )
{
	// 创建客户端机器人
	INT32 nMaxLoadNum = (INT32)_tcstol(szNum, NULL, 10);

	tstringstream tss;
	tss<<_T(VC_PATH)<<szXML;
	//tss<<_T(VC_PATH)<<_T("test.xml");
	list<tstring> listField;
	if (!m_pVar->Load(NULL, tss.str().c_str(), "id", &listField))
	{
		return GT_INVALID;
	}

	list<tstring>::iterator it;
	int index = 0;
	for (it= listField.begin(); it != listField.end(); ++it)
	{
		Player* pPlayer = new Player;
		//帐号ID
		pPlayer->m_dwID = _tcstol(it->c_str(), NULL, 10);
		//类型
		pPlayer->m_type = m_pVar->GetDword(_T("type"), it->c_str(), 0);
		//用户名
		pPlayer->m_strName = m_pVar->GetString(_T("user"), it->c_str(), _T(""));
		//密码
		pPlayer->m_strPassword = m_pVar->GetString(_T("pwd"), it->c_str(), _T(""));
		//路点文件名称
		pPlayer->m_wayPoint = m_pVar->GetInt(_T("wayPoint"), it->c_str(), 1);
		//SendTime
		pPlayer->m_SendTime = m_pVar->GetDword(_T("sendTime"), it->c_str(), 600);

		InitWayPoint(pPlayer, m_vecWayPoints);

		if (m_nPressLimit != 0 && index >= m_nPressLimit)
		{
			m_listWaiting.PushBack(pPlayer);
			index++;
			if (index >= nMaxLoadNum)
			{
				break;
			}
		}
		else if (pPlayer->Init())
		{

			m_listAllPlayer.PushBack(pPlayer);

			index++;
			if (index >= nMaxLoadNum)
			{
				break;
			}
		}
		else
		{
			SAFE_DEL(pPlayer);
		}
	}

	return E_Success;
}

DWORD ClientMgr::TerminateClient()
{
	return E_Success;
}

DWORD ClientMgr::ChatControl( LPCTSTR szActive, LPCTSTR szChannel /*= _T("5")*/ )
{
	if (m_listAllPlayer.Empty())
		return GT_INVALID;

	BOOL bChat = FALSE;
	if (_tcstol(szActive, NULL, 10) != 0)
	{
		bChat = TRUE;
	}
	BYTE byChannel = (BYTE)_tcstol(szChannel, NULL, 10);

	Player* pTmpPlayer = NULL;
	m_listAllPlayer.ResetIterator();
	while (m_listAllPlayer.PeekNext(pTmpPlayer))
	{
		if (!P_VALID(pTmpPlayer))
		{
			continue;
		}

		pTmpPlayer->SetPlayerChat(bChat, byChannel);
	}
	return E_Success;
}

DWORD ClientMgr::MoveControl( LPCTSTR szActive )
{
	if (m_listAllPlayer.Empty())
		return GT_INVALID;

	BOOL bMove = FALSE;
	if (_tcstol(szActive, NULL, 10) != 0)
	{
		bMove = TRUE;
	}

	Player* pTmpPlayer = NULL;
	m_listAllPlayer.ResetIterator();
	while (m_listAllPlayer.PeekNext(pTmpPlayer))
	{
		if (!P_VALID(pTmpPlayer))
		{
			continue;
		}

		pTmpPlayer->SetPlayerMove(bMove);
	}
	return E_Success;
}

VOID ClientMgr::ThreadUpdate()
{
	while (!m_bTerminate)
	{
		// 处理机器人登出
		Player* pPlayer = NULL;
		TList<Player*>::TListIterator iter = m_listAllPlayer.Begin();
		while (m_listAllPlayer.PeekNext(iter, pPlayer))
		{
			if (pPlayer->m_bAbandon)
			{
				m_listAllPlayer.Erase(pPlayer);

				// 补充新兵
				if (m_nPressLimit != 0 && !m_listWaiting.Empty())
				{
					Player* pNewPlayer = m_listWaiting.PopFront();
					if (pNewPlayer->Init())
					{
						m_listAllPlayer.PushBack(pNewPlayer);
					}
				}
				
				// 这个比较慢,延后处理
				SAFE_DEL(pPlayer);

				break;
			}
		}
	}
}