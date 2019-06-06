#include "stdafx.h"
#include "ServerSelectFrame.h"
#include "shellapi.h"
#include "ClientMain.h"
#include "SerActiveData.h"
#include "ServerTime.h"
#include "HealthyControlFrame.h"
#include "SaveAccount.h"

const TCHAR* szFileName = _T("launcher\\serverinfo_back.xml");
const char* szBackName = "\\data\\ui\\Login\\l_server-diban-b.bmp";

ServerSelectFrame::ServerSelectFrame():m_Trunk(this)
{
	m_nBorough				= 0;
	m_nGameWorld			= 0;
	
	m_pWnd					= NULL;
	m_pWndCaption			= NULL;
	m_pOK					= NULL;
	m_pRegister				= NULL;
	m_pAbundance			= NULL;
	m_pExit					= NULL;
	m_pLastLogin			= NULL;
	m_pLastServer			= NULL;
	memset(m_pRecommend, 0x0, sizeof(m_pRecommend));
	memset(m_pBorough, 0x0, sizeof(m_pBorough));
	memset(m_pGameWorld, 0x0, sizeof(m_pGameWorld));
	m_pToday				= NULL;
	m_pTomorrow				= NULL;
	m_nBorough				= GT_INVALID;
	m_nGameWorld			= GT_INVALID;
	m_nRecoWorld			= GT_INVALID;
	m_pWnd_Back				= NULL;
	m_pStc_Pic				= NULL;
	for(int i=0; i<(int)m_vBrough.size(); i++)
	{
		m_vBrough[i].vGameWorld.clear();
	}
	m_vBrough.clear();
}
ServerSelectFrame::~ServerSelectFrame()
{

}

BOOL ServerSelectFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;
		
	m_pVarContainer = CreateObj("Borough", "VarContainer");
	m_pVarIPSaver	= CreateObj("ip_login", "VarContainer");
	m_pVarIPSaver->SetSaveSupport(TRUE);

	SerActiveData::Inst()->LoadFromFile();
	LoadConfigFile();
	LoadLastLogin();
	CalDate();
	return TRUE;
}

BOOL ServerSelectFrame::Destroy()
{
	GameFrame::Destroy();
	if( P_VALID(m_pWnd) )
		m_pGUI->DestroyWnd(m_pWnd);
	if( P_VALID(m_pWnd_Back) )
		m_pGUI->DestroyWnd(m_pWnd_Back);

	KillObj("Borough");
	KillObj("ip_login");
	
	return TRUE;
}

BOOL ServerSelectFrame::ReloadUI()
{
	// 背景
	tstring strPath1 = g_strLocalPath + _T("\\ui\\optseverback.xml");
	XmlElement ele;
	m_pGUI->LoadXml(&ele, "VFS_System", strPath1.c_str());
	stringstream clientX, clientY;
	GUIWnd* pDesktop = m_pGUI->GetDesktop();
	clientX << (pDesktop->GetSize().x);
	clientY << (pDesktop->GetSize().y);
	m_pGUI->ChangeXml(&ele, "os_win", "Size_x", clientX.str().c_str());
	m_pGUI->ChangeXml(&ele, "os_win", "Size_y", clientY.str().c_str());
	m_pGUI->ChangeXml(&ele, "os_win", "LogicPos_x", "0");
	m_pGUI->ChangeXml(&ele, "os_win", "LogicPos_y", "0");
	m_pGUI->ChangeXml(&ele, "os_win\\logopic", "Size_x", clientX.str().c_str());
	m_pGUI->ChangeXml(&ele, "os_win\\logopic", "Size_y", clientY.str().c_str());
	m_pGUI->ChangeXml(&ele, "os_win\\logopic", "LogicPos_x", "0");
	m_pGUI->ChangeXml(&ele, "os_win\\logopic", "LogicPos_y", "0");
	m_pWnd_Back = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pWnd_Back->SetUnClickable(TRUE);

	// 创建
	tstring strPath = g_strLocalPath + _T("\\ui\\optsever.xml");
	m_pWnd = m_pGUI->CreateFromFile("VFS_System", strPath.c_str());
	m_pWnd->FlipToTop();
	

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ServerSelectFrame::EventHandler));
	
	m_pWndCaption	= m_pWnd->GetChild(_T("os_pic"));
	m_pOK			= (GUIButton*)m_pWndCaption->GetChild(_T("login"));
	m_pOK->SetText(g_StrTable[_T("SelectServer1")]);
	m_pRegister		= (GUIButton*)m_pWndCaption->GetChild(_T("register"));
	m_pRegister->SetText(g_StrTable[_T("SelectServer2")]);
	m_pAbundance	= (GUIButton*)m_pWndCaption->GetChild(_T("abundance"));
	m_pAbundance->SetText(g_StrTable[_T("SelectServer3")]);
	m_pExit			= (GUIButton*)m_pWndCaption->GetChild(_T("exit"));
	m_pExit->SetText(g_StrTable[_T("SelectServer4")]);
	m_pToday		= (GUIListBox*)m_pWndCaption->GetChild(_T("tasklist1"));
	m_pTomorrow		= (GUIListBox*)m_pWndCaption->GetChild(_T("tasklist2"));
	m_pLastLogin = (GUIPushButton*)m_pWndCaption->GetChild(_T("burough33"));
	m_pLastServer = (GUIStatic*)m_pWndCaption->GetChild(_T("lastburough"));
	
	for(int i=0; i<RECOMMEND_GAMEWORLD; i++)
	{
		tstringstream name;
		name << _T("fra1\\Recommended") << i;
		m_pRecommend[i]	= (GUIPushButton*)m_pWndCaption->GetChild(name.str().c_str());
		if( P_VALID(m_pRecommend[i]) )
			m_pRecommend[i]->SetInvisible(TRUE);
	}

	for(int i=0; i<BOROUGH_MAX; i++)
	{
		tstringstream name;
		name << _T("fra2\\burough") << i;
		m_pBorough[i]	= (GUIPushButton*)m_pWndCaption->GetChild(name.str().c_str());
		if( P_VALID(m_pBorough[i]) )
			m_pBorough[i]->SetInvisible(TRUE);
	}

	for(int i=0; i<GAMEWORLD_MAX; i++)
	{
		tstringstream name;
		name << _T("fram3\\burough") << i;
		tstring name1 = name.str();
		m_pGameWorld[i]	= (GUIPushButton*)m_pWndCaption->GetChild(name.str().c_str());
		if( P_VALID(m_pGameWorld[i]) )
			m_pGameWorld[i]->SetInvisible(TRUE);
	}
	m_pToday->SetColWidth( 0, 70 );
	m_pToday->SetColWidth( 1, 80 );
	m_pToday->SetColWidth( 2, 100 );
	m_pTomorrow->SetColWidth( 0, 70 );
	m_pTomorrow->SetColWidth( 1, 80 );
	m_pTomorrow->SetColWidth( 2, 100 );


	return TRUE;
}

VOID ServerSelectFrame::Update()
{
	// 激活自身窗口，避免刚启动又不小心被用户切到后台
	HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));
	::ShowWindow(hWnd, SW_SHOWNORMAL);
}

BOOL ServerSelectFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pOK )
				ConfirmLogin();

			if( pWnd == m_pRegister )
				AccountRegister();

			if( pWnd == m_pAbundance )
				AccountAbundance();

			if(	pWnd == m_pExit )
				ExitGame();
		}
		break;	
	case EGUIE_PushButtonDown:
		{
			//如果是模拟发的Down事件,则不处理
			if(pEvent->dwParam1!=1)
				return false;

			if(_tcsnccmp(pWnd->GetName().c_str(), _T("Recommended"), 11) == 0)
			{
				for(int i=0; i<RECOMMEND_GAMEWORLD; i++)
				{	
					if( pWnd == m_pRecommend[i] )
					{
						m_nRecoWorld = i;
						SetWorldPushButtonState();
						m_pLastLogin->SetState(EGUIBS_PopUp, FALSE);
						break;
					}
				}
			}
			
			if(_tcsnccmp(pWnd->GetName().c_str(), _T("burough"), 7) == 0)
			{
				for(int i=0; i<BOROUGH_MAX; i++)
				{
					if( pWnd == m_pBorough[i] )
					{
						// 如果两次点的是同一个按钮,不进行处理
						if (m_nBorough != i)
						{
							m_nBorough = i;
							// 切换大区,游戏世界默认设置为无
							SetWorldPushButtonState();
							ShowWorldPushButton();
							m_pLastLogin->SetState(EGUIBS_PopUp, FALSE);
						}
					}
				}

				for(int i=0; i<GAMEWORLD_MAX; i++)
				{
					if( pWnd == m_pGameWorld[i] )
					{
						m_nGameWorld = i;
						SetRecommedPushButtonState();
						m_pLastLogin->SetState(EGUIBS_PopUp, FALSE);
					}
				}
			}

			if( pWnd == m_pLastLogin )
			{
				SetWorldPushButtonState();
				SetRecommedPushButtonState();
			}
		}
		break;
	}
	return FALSE;
}

void ServerSelectFrame::CalDate()
{
	SYSTEMTIME CurTime;
	GetLocalTime(&CurTime);
}

// 得到服务器返回的游戏世界状态颜色
DWORD ServerSelectFrame::GetWorldStatusColor(const INT nWorldStatus)
{
	INT nStatus;
	if(nWorldStatus>0 && nWorldStatus<=30)
	{
		nStatus = 0;			//优良
	}
	else if(nWorldStatus>30 && nWorldStatus<=50)
	{
		nStatus = 1;			//良好
	}
	else if(nWorldStatus>50 && nWorldStatus<=70)
	{
		nStatus = 2;			//热闹
	}
	else if(nWorldStatus>70 && nWorldStatus<=100)
	{
		nStatus = 3;			//火爆
	}
	else
		nStatus = 4;			//维护

	DWORD dwStatus;
	switch(nStatus)
	{	
	case 0:
		dwStatus =  0xFF13CF00;
		break;
	case 1:
		dwStatus =  0xFFFFDE26;
		break;
	case 2:
		dwStatus =  0xFFFF7800;
		break;
	case 3:
		dwStatus =  0xFFFF0000;
		break;
	case 4:
		dwStatus =  0xFFACACAC;
		break;
	default:
		dwStatus =  0xFFACACAC;
		break;
	}
	return dwStatus;
}

void ServerSelectFrame::ConfirmLogin()
{
	//选择了推荐的游戏世界
	if(m_nRecoWorld!=GT_INVALID)
	{
		TObjRef<StreamTransport>	Stream;
		tstringstream ip;
		ip << _T("ip last_login_server");
		CHAR* szIP = Stream->IP2String(m_vRecommend[m_nRecoWorld].dwWorldIP);
		tstring strIP = m_pUtil->Unicode8ToUnicode(szIP);
		m_pVarIPSaver->Set(strIP.c_str(), ip.str().c_str());
		tstringstream port;
		port << _T("port last_login_server");
		m_pVarIPSaver->Set(m_vRecommend[m_nRecoWorld].dwWorldPort, port.str().c_str());
		for( vector<tagBorough>::iterator it=m_vBrough.begin(); it!=m_vBrough.end(); ++it )
		{
			for( vector<tagGameWorld>::iterator it1=it->vGameWorld.begin(); it1!=it->vGameWorld.end(); ++it1 )
			{
				if( _tcscmp(m_vRecommend[m_nRecoWorld].szWorldName, it1->szWorldName)==0 )
				{
					tstringstream serName;
					serName << _T("server_name last_login_server");
					m_pVarIPSaver->Set(it->szBoroughName, serName.str().c_str());
					tstringstream serID;
					serID << _T("server_id last_login_server");
					m_pVarIPSaver->Set(it->dwBoroughID, serID.str().c_str());
					tstringstream worldid;
					worldid << _T("world_id last_login_server");
					m_pVarIPSaver->Set(it1->dwWorldID, worldid.str().c_str());
					break;
				}
			}
		}
		
		tstringstream name;
		name << _T("gamename last_login_server");
		m_pVarIPSaver->Set(m_vRecommend[m_nRecoWorld].szWorldName, name.str().c_str());
		m_pVarIPSaver->Save(_T("config\\lastlogin.xml"));
	}
	//选择了大区下的普通游戏世界
	else if(m_nBorough!=GT_INVALID && m_nGameWorld!=GT_INVALID)
	{
		TObjRef<StreamTransport>	Stream;
		tstringstream ip;
		ip << _T("ip last_login_server");
		CHAR* szIP = Stream->IP2String(m_vBrough[m_nBorough].vGameWorld[m_nGameWorld].dwWorldIP);
		tstring strIP = m_pUtil->Unicode8ToUnicode(szIP);
		m_pVarIPSaver->Set(strIP.c_str(), ip.str().c_str());
		tstringstream port;
		port << _T("port last_login_server");
		m_pVarIPSaver->Set(m_vBrough[m_nBorough].vGameWorld[m_nGameWorld].dwWorldPort, port.str().c_str());
		tstringstream serName;
		serName << _T("server_name last_login_server");
		m_pVarIPSaver->Set(m_vBrough[m_nBorough].szBoroughName, serName.str().c_str());
		tstringstream serID;
		serID << _T("server_id last_login_server");
		m_pVarIPSaver->Set(m_vBrough[m_nBorough].dwBoroughID, serID.str().c_str());
		tstringstream worldid;
		worldid << _T("world_id last_login_server");
		m_pVarIPSaver->Set(m_vBrough[m_nBorough].vGameWorld[m_nGameWorld].dwWorldID, worldid.str().c_str());
		tstringstream name;
		name << _T("gamename last_login_server");
		m_pVarIPSaver->Set(m_vBrough[m_nBorough].vGameWorld[m_nGameWorld].szWorldName, name.str().c_str());
		m_pVarIPSaver->Save(_T("config\\lastlogin.xml"));
	}

	tstring strBoroghID;
	TObjRef<VarContainer> serInfo = CreateObj("lastlogin", "VarContainer");
	const TCHAR* szFileName = _T("config\\lastlogin.xml");
	if( serInfo->Load(NULL, szFileName) )
	{
		strBoroghID = serInfo->GetString(_T("world_id"), _T("last_login_server"), _T(""));
		SaveAccount::Inst()->SetWorldID(strBoroghID.c_str());
	}
	KillObj("lastlogin");

    //防沉迷提示
    HealthyControlFrame* pHCFrame;
    //防止重复建立frame
    pHCFrame = (HealthyControlFrame*)m_pFrameMgr->GetFrame(_T("HealthyControl"));
    if (P_VALID(pHCFrame))
        m_pFrameMgr->DestroyFrame(pHCFrame);
    //创建
    pHCFrame = (HealthyControlFrame*)m_pFrameMgr->CreateFrame(_T("Root"), _T("HealthyControl"), _T("HealthyControlFrame"), 0);
    if (P_VALID(pHCFrame))
    {
        // 先关闭窗体，等收到服务器发来的通知再显示窗体
        pHCFrame->ShowWnd(FALSE);
    }
    if (!pHCFrame)
        IMSG(_T("Create HealthyControlFrame failed"));

    m_pFrameMgr->CreateFrame(_T("Root"), _T("Login"), _T("LoginFrame"), 0);

	m_pFrameMgr->AddToDestroyList(this);
}

void ServerSelectFrame::AccountRegister()
{
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("UserRegURL"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring url = szTemp;
	if( url == _T("") )
	{
		MessageBox(NULL, _T("Don't find user register url!"), _T(""), MB_OK);
		return;
	}

	//打开注册网页
	ShellExecute(NULL, _T("open"), url.c_str(), NULL, NULL, SW_MAXIMIZE);
}

void ServerSelectFrame::AccountAbundance()
{
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("AccountActURL"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring url = szTemp;
	if( url == _T("") )
	{
		MessageBox(NULL, _T("Don't find account active url!"), _T(""), MB_OK);
		return;
	}

	//打开充值网页
	ShellExecute(NULL, _T("open"), url.c_str(), NULL, NULL, SW_MAXIMIZE);
}

void ServerSelectFrame::ExitGame()
{
	m_pClient->ExitThreadLoad();
	PostQuitMessage(0);
}

bool CompareGameWorldRank(const tagGameWorld& world1, const tagGameWorld& world2)
{
	return world1.dwWorldRank < world2.dwWorldRank;
}

bool CompareBoroughRank(const tagBorough& borough1, const tagBorough& borough2)
{
	return borough1.dwBoroughRank < borough2.dwBoroughRank;
}

bool CompareRecommendRank(const tagGameWorld& world1, const tagGameWorld& world2)
{
	return world1.dwRecoRank < world2.dwRecoRank;
}

void ServerSelectFrame::LoadConfigFile()
{
	TObjRef<StreamTransport>	Stream;
	list<tstring> FieldList;
	m_pVarContainer->Load(NULL, szFileName, "name", &FieldList);
	list<tstring>::iterator iter = FieldList.begin();


	tstring strIP;
	string szIP;
	tstring strBourugh;
	DWORD dwBouroghID;
	for(; iter!=FieldList.end(); iter++)
	{
		tagGameWorld gameWorld;
		_tcscpy( gameWorld.szWorldName, m_pVarContainer->GetString(_T("name"), (*iter).c_str(), _T("")) );		//读出游戏世界名
		gameWorld.dwWorldRank = m_pVarContainer->GetDword(_T("worldrank"), (*iter).c_str(), GT_INVALID );			//读出游戏世界的编号
		gameWorld.nStatus = (INT)m_pVarContainer->GetDword(_T("state"), (*iter).c_str(), GT_INVALID );				//读出游戏世界的状态
		gameWorld.dwStatus = GetWorldStatusColor(gameWorld.nStatus);									//计算出游戏世界状态的颜色
		strIP = m_pVarContainer->GetString(_T("ip"), (*iter).c_str(), _T("") );					
		szIP = m_pUtil->UnicodeToUnicode8(strIP.c_str());
		gameWorld.dwWorldIP = Stream->StringIP2IP( (CHAR*)szIP.c_str() );								//游戏世界IP
		gameWorld.dwWorldPort = m_pVarContainer->GetDword(_T("port"), (*iter).c_str(), GT_INVALID );				//游戏世界Port
		gameWorld.byRecommend = (BYTE)m_pVarContainer->GetDword(_T("recommend"), (*iter).c_str(), GT_INVALID );		//读出游戏世界是否为推荐	
		gameWorld.byGreenFlag = (BYTE)m_pVarContainer->GetDword(_T("greenflag"), (*iter).c_str(), GT_INVALID );		//读出游戏世界的绿色区标识
		gameWorld.dwRecoRank = m_pVarContainer->GetDword(_T("recorank"), (*iter).c_str(), GT_INVALID );			//读出游戏世界推荐编号
		gameWorld.dwWorldID = m_pVarContainer->GetDword(_T("worldid"), (*iter).c_str(), GT_INVALID );				//读出游戏世界ID
		
		if(gameWorld.byRecommend>0)
			m_vRecommend.push_back(gameWorld);

		strBourugh = m_pVarContainer->GetString(_T("borogh"), (*iter).c_str(), _T("")); 
		dwBouroghID = m_pVarContainer->GetDword(_T("boroughid"), (*iter).c_str(), GT_INVALID); 
		DWORD dwBoroughRank = m_pVarContainer->GetDword(_T("boroughrank"), (*iter).c_str(), GT_INVALID );			//读出大区的编号
		
		//缓存
		bool bHave = false;
		int nBorough = 0;
		for(; nBorough<(int)m_vBrough.size(); ++nBorough)
		{
			if( _tcscmp(m_vBrough[nBorough].szBoroughName, strBourugh.c_str())==0 )
			{
				m_vBrough[nBorough].vGameWorld.push_back(gameWorld);
				bHave = true;
				break;
			}
				
		}
		if( !bHave )
		{
			tagBorough borogh;
			_tcscpy( borogh.szBoroughName, strBourugh.c_str() );
			borogh.dwBoroughID = dwBouroghID;
			borogh.dwBoroughRank = dwBoroughRank;
			m_vBrough.push_back(borogh);
			m_vBrough[nBorough].vGameWorld.push_back(gameWorld);
		}	
	}

	vector<tagBorough>::iterator bIter = m_vBrough.begin();
	for(; bIter!=m_vBrough.end(); bIter++)
	{
		bIter->dwWorldNum = (DWORD)bIter->vGameWorld.size();
		sort( bIter->vGameWorld.begin(), bIter->vGameWorld.end(), CompareGameWorldRank);
	}
	sort(m_vBrough.begin(), m_vBrough.end(), CompareBoroughRank);
	sort(m_vRecommend.begin(), m_vRecommend.end(), CompareRecommendRank);

	for(int i=0; i<(int)m_vRecommend.size(); i++)
	{
		if(i>=RECOMMEND_GAMEWORLD)
			break;
		if( P_VALID(m_pRecommend[i]) )
		{
			m_pRecommend[i]->SetInvisible(FALSE);
			m_pRecommend[i]->SetText(m_vRecommend[i].szWorldName);
			m_pRecommend[i]->SetTextColor(m_vRecommend[i].dwStatus);
			m_pRecommend[i]->SetPushTextColor(m_vRecommend[i].dwStatus);
			m_pRecommend[i]->SetMoveTextColor(m_vRecommend[i].dwStatus);
		}
	}

	for(int i=0; i<(int)m_vBrough.size(); i++)
	{
		if(i>=BOROUGH_MAX)
			break;
		if( P_VALID(m_pBorough[i]) )
		{
			m_pBorough[i]->SetInvisible(FALSE);
			m_pBorough[i]->SetText(m_vBrough[i].szBoroughName);
		}
	}

	ShowServerActive();
}

//读取上次登录过的游戏世界
void ServerSelectFrame::LoadLastLogin()
{
	const TCHAR* szFileName = _T("config\\lastlogin.xml");
	if(m_pVarIPSaver->Load(NULL, szFileName))
	{
		m_strLastWorldName	= m_pVarIPSaver->GetString(_T("gamename"), _T("last_login_server"), _T(""));
		m_strLastIP			= m_pVarIPSaver->GetString(_T("ip"), _T("last_login_server"));	
		m_dwLastPort		= m_pVarIPSaver->GetDword(_T("port"), _T("last_login_server"));	
		m_strLastServer		= m_pVarIPSaver->GetString(_T("server_name"), _T("last_login_server"), _T(""));	
		if( P_VALID(m_pLastLogin) )
		{
			m_pLastLogin->SetText(m_strLastWorldName.c_str());
			m_pLastLogin->SetInvisible(FALSE);
			m_pLastLogin->SetState(EGUIBS_PushDown, FALSE);
		}
		if( P_VALID(m_pLastServer) )
		{
			m_pLastServer->SetText(m_strLastServer.c_str());
			m_pLastServer->SetInvisible(FALSE);
		}
	}
	else
	{
		if( P_VALID(m_pLastLogin) )
			m_pLastLogin->SetInvisible(TRUE);
		if( P_VALID(m_pLastServer) )
			m_pLastServer->SetInvisible(TRUE);
	}
}

void ServerSelectFrame::SetRecommedPushButtonState()
{
	for(int i=0; i<(int)m_vRecommend.size(); i++)
	{
		if( P_VALID(m_pRecommend[i]) )
			m_pRecommend[i]->SetState(EGUIBS_PopUp, FALSE);
	}
	m_nRecoWorld = GT_INVALID;
}

void ServerSelectFrame::ShowWorldPushButton()
{
	if(m_nBorough==GT_INVALID)
		return;
	for(int i=0; i<GAMEWORLD_MAX; i++)
	{
		if( P_VALID(m_pGameWorld[i]) )
			m_pGameWorld[i]->SetInvisible(TRUE);
	}
	for(int i=0; i<(int)m_vBrough[m_nBorough].vGameWorld.size(); i++)
	{
		if( P_VALID(m_pGameWorld[i]) )
		{
			m_pGameWorld[i]->SetText(m_vBrough[m_nBorough].vGameWorld[i].szWorldName);
			m_pGameWorld[i]->SetInvisible(FALSE);
			m_pGameWorld[i]->SetTextColor(m_vBrough[m_nBorough].vGameWorld[i].dwStatus);
			m_pGameWorld[i]->SetPushTextColor(m_vBrough[m_nBorough].vGameWorld[i].dwStatus);
			m_pGameWorld[i]->SetMoveTextColor(m_vBrough[m_nBorough].vGameWorld[i].dwStatus);
		}
	}
}

//设置游戏世界按钮状态
void ServerSelectFrame::SetWorldPushButtonState()
{
	if(m_nBorough==GT_INVALID)
		return;
	for(int i=0; i<(int)m_vBrough[m_nBorough].vGameWorld.size(); i++)
	{
		if( P_VALID(m_pGameWorld[i]) )
			m_pGameWorld[i]->SetState(EGUIBS_PopUp, FALSE);
	}
	m_nGameWorld = GT_INVALID;
}

void ServerSelectFrame::ShowServerActive()
{
	vector<tagSerActiveData> actmap;
	actmap = SerActiveData::Inst()->GetServerActiveVecData();
	if( actmap.size() <= 0)
		return;

	//星期日:0
	BYTE byToday = ServerTime::Inst()->GetCurrentWeekDay();
	BYTE byTomorrow = 0;
	if( byToday == 6 )
		byTomorrow = 0;
	else
		byTomorrow = byToday+1;

	//获得当前月和日
	BYTE byCurMonth = ServerTime::Inst()->GetCurrentMonth();
	BYTE byCurDay   = ServerTime::Inst()->GetCurrentDay();
	
	TCHAR szBuff[X_SHORT_NAME] = {0};
	INT nRow = 0;
	INT nRow1 = 0;
	INT nCol = 0;
	DWORD dwColor = 0xFFFFF2A4;
	for( vector<tagSerActiveData>::iterator it=actmap.begin(); it!=actmap.end(); ++it )
	{
		nCol = 0;
		const tagSerActiveData& data  = (*it);
		
		//日过是节日活动，即按照月和日开启的
		if( Is_FastivalActivity(data.eStartMode) )
		{
			if( byCurMonth == (BYTE)data.nMonth && byCurDay == (BYTE)data.nDay )
			{
				TCHAR szMonthDay[X_SHORT_NAME] = {0};
				_stprintf( szMonthDay, g_StrTable[_T("Activity_FestivalMonthDay")], data.nMonth, data.nDay );
				
				m_pToday->SetText( nRow, nCol++, data.strActiveName.c_str(), data.dwColor );
				m_pToday->SetText( nRow, nCol++, szMonthDay, data.dwColor );
				m_pToday->SetText( nRow, nCol++, data.strContent.c_str(), data.dwColor );
				++nRow;
			}
			else if( byCurMonth == (BYTE)data.nMonth && (byCurDay+1) == data.nDay )
			{
				TCHAR szMonthDay[X_SHORT_NAME] = {0};
				_stprintf( szMonthDay, g_StrTable[_T("Activity_FestivalMonthDay")], data.nMonth, data.nDay );
				m_pTomorrow->SetText( nRow1, nCol++, data.strActiveName.c_str(), data.dwColor );
				m_pTomorrow->SetText( nRow1, nCol++, szMonthDay, data.dwColor );
				m_pTomorrow->SetText( nRow1, nCol++, data.strContent.c_str(), data.dwColor );
				++nRow1;
			}
			continue;
		}

		//今天是星期日
		if( byToday==0 )
		{
			if( data.byDate[MAX_WEEK-1] != 0 ) 
			{
				m_pToday->SetText( nRow, nCol++, data.strActiveName.c_str(), data.dwColor );
				m_pToday->SetText( nRow, nCol++, data.strTime.c_str(), data.dwColor );
				m_pToday->SetText( nRow, nCol++, data.strContent.c_str(), data.dwColor );
				++nRow;
			}
		}
		else
		{
			if( data.byDate[byToday-1] != 0 )
			{
				m_pToday->SetText( nRow, nCol++, data.strActiveName.c_str(), data.dwColor );
				m_pToday->SetText( nRow, nCol++, data.strTime.c_str(), data.dwColor );
				m_pToday->SetText( nRow, nCol++, data.strContent.c_str(), data.dwColor );
				++nRow;
			}
		}

		nCol = 0;
		//明天是星期日
		if( byTomorrow==0 )
		{
			if( data.byDate[MAX_WEEK-1] != 0 ) 
			{
				m_pTomorrow->SetText( nRow1, nCol++, data.strActiveName.c_str(), data.dwColor );
				m_pTomorrow->SetText( nRow1, nCol++, data.strTime.c_str(), data.dwColor );
				m_pTomorrow->SetText( nRow1, nCol++, data.strContent.c_str(), data.dwColor );
				++nRow1;
			}
		}
		else
		{
			if( data.byDate[byTomorrow-1] != 0 )
			{
				m_pTomorrow->SetText( nRow1, nCol++, data.strActiveName.c_str(), data.dwColor );
				m_pTomorrow->SetText( nRow1, nCol++, data.strTime.c_str(), data.dwColor );
				m_pTomorrow->SetText( nRow1, nCol++, data.strContent.c_str(), data.dwColor );
				++nRow1;
			}
		}
	}

	m_pToday->SetCurSelectedRow(GT_INVALID);
	m_pTomorrow->SetCurSelectedRow(GT_INVALID);
}