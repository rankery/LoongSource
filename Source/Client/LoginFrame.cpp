//-----------------------------------------------------------------------------
//!\file LoginFrame.cpp
//!\author Lyp xtian
//!
//!\date 2008-02-17
//! last 2009-5-31
//!
//!\brief login
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "LoginFrame.h"
#include "..\WorldDefine\login.h"
#include ".\SeleRoleMgr.h"
#include "shellapi.h"

#include "AudioSys.h"
#include "ClientMain.h"
#include "HackShield.h"
#include "ServerTime.h"
#include "GameSet.h"
#include "../WorldDefine/msg_fatigue_guarder.h"
#include <iphlpapi.h>
#include "LoadingFrame.h"
#include "SaveAccount.h"
#include "HealthyControlFrame.h"

#pragma comment(lib, "IPHlpAPI.lib")
bool g_bCommandLine = false;

//获得本机mac地址
void __fastcall GetMAC(string& strMAC)   
{
	DWORD   Err;     
	PIP_ADAPTER_INFO   pAdapterInfo,   pAdapt;   
	DWORD   AdapterInfoSize;   

	AdapterInfoSize = 0;   
	if((Err = ::GetAdaptersInfo(NULL,&AdapterInfoSize))!=0)   
	{   
		if(Err != ERROR_BUFFER_OVERFLOW)   
		{   
			IMSG(_T("GetAdaptersInfo sizing failed with error: %u\n"), Err);   
			strMAC = "";
			return;   
		}   
	}   

	if((pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, AdapterInfoSize))==NULL)   
	{   
		IMSG(_T("Memory   allocation   error\n"));   
		strMAC = "";
		return;   
	}   

	//   Get   actual   adapter   information   
	if((Err = ::GetAdaptersInfo(pAdapterInfo,  &AdapterInfoSize))!= 0)   
	{   
		IMSG(_T("GetAdaptersInfo failed with error %u\n"), Err);   
		strMAC = "";
		return;   
	}   

	pAdapt = pAdapterInfo;
	char szBuff[128] = {0};
	std::string str("");   
	if(pAdapt) 
	{
		int  i=1;   	
		for(int i=0; i<6; i++)   
		{      
			INT64 n64Value = INT64(pAdapt->Address[i]);
			std::string strTemp(_i64toa(n64Value, szBuff, 16));
			transform(strTemp.begin(), strTemp.end(), strTemp.begin(), toupper);
			str = str + strTemp;
		}   
	}
	strMAC = str;
}
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
LoginFrame::LoginFrame():m_Trunk(this)
{
	m_pWnd			= NULL;
	m_pWndKey		= NULL;
	m_pWndCaption	= NULL;
	m_pEditAccount	= NULL;
	m_pEditPassword = NULL;
	m_pButtonLogin	= NULL;
	m_pButtonQuit	= NULL;

	m_pWndWorldList	= NULL;		
	m_pBtnOKWorld	= NULL;
	m_pBtnCancelWorld = NULL;
	m_pWorldList	= NULL;
	m_pPBtn_Tab		= NULL;
	m_bTab			= TRUE;

	m_eState		= ELS_WaitInput;
	m_dwStateTime	= timeGetTime();
	m_fPingTime		= 0.0f;
	m_pAniCtrl		= NULL;
	m_pTime			= 0.0f;
	m_nPlayState	= Loading;
	m_bEnter		= FALSE;
	m_dwSentJoinGame = 0;
	m_bSelectedWorld = FALSE;
	m_bGuard		= FALSE;

	m_pWndQueue		= NULL;
	m_pStcTime		= NULL;
	m_pStcPosition	= NULL;
	m_pBtnCancelQueue = NULL;
	m_eMB			= EMBP_NULL;	
	m_bMiBao		= FALSE;	
	m_pWndSecurity	= NULL;	
	m_pStcSecuHint	= NULL;	
	m_pStcSecuNum	= NULL;	
	m_pWndSeInputBack=NULL;	

	for(int i=0; i<10; i++)	
	{		
		m_pBtnSecuInput[0] = NULL;	
	}	
	m_pStcInputHint	= NULL;
	m_pBtnSecuOK	= NULL;	
	m_pBtnSecuCancel= NULL;	
	m_nPolicy		= GT_INVALID;	
	m_pPBtnSaveAccount	= NULL;
	m_bSave			= false;
	m_hDll			= NULL;
	m_pStcSavePass	= NULL;
	m_pStcUser		= NULL;
	m_eHint			= EXLHint_User;
	m_bLoginType	= false;
	ZeroMemory( m_szEncrypt, 100 );
	ZeroMemory( m_szUserName, 20 );
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
LoginFrame::~LoginFrame()
{
	
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL LoginFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	stringstream stream;
	stream << "XClient_Login" << this;
	m_pStream = CreateObj(stream.str().c_str(), "XClient");

	m_pStream->Init(FALSE);
	m_pVarContainer = CreateObj("login_ip_list", "VarContainer");
	m_bSelectedWorld = m_pVarContainer->Load(NULL, _T("config\\lastlogin.xml"));
	if(!m_bSelectedWorld )
		m_pStcHint->SetText(g_StrTable[_T("SelectWorld_NoServerSelect")]);

	
	m_pCmdMgr->Register("NS_JoinGame", (NETMSGPROC)m_Trunk.sfp2(&LoginFrame::NetRecvJoinGame), _T("NS_JoinGame"));

	// 播放登陆音乐
	SaveAccount::Inst()->SetCurMusic(0);
	TObjRef<AudioSys>()->StopMusic();
	TCHAR szBuff[X_SHORT_NAME] = {0};
	_sntprintf( szBuff, X_SHORT_NAME, _T("music_login%d"), 0 );
	TObjRef<AudioSys>()->PlayMusic(szBuff, FALSE, SOUND_LOUD);

	LoadMap();

    // 如果有防沉迷提示，则关掉
    HealthyControlFrame* pFrame = (HealthyControlFrame*)m_pFrameMgr->GetFrame(_T("HealthyControl"));
    if (P_VALID(pFrame))
        pFrame->ShowWnd(FALSE);

	//JudegPolicy();
	return TRUE;
}


const Vector3 Lookat = Vector3(0, 120, 0);
const float CamYaw=_DegToRad(0.0f);
const float CamPitch=_DegToRad(-6.5f);
const float CamRoll=0.0f;
const float MAXCamDist=650.0f;

VOID LoginFrame::LoadMap()
{

	IFS* pFS=(IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_Map");
	//--加载场景、node 
	m_pSG				= new NullSceneGraph;
	m_EntityNode		= new EntityNode;	
	m_EntityNode->LoadRes(_T("data\\model\\building\\denglu\\denglu.FAK"));
	m_pSG->AttachDynamicNode(m_EntityNode);
	m_pAniCtrl = (VertexAnimationCtrl*)m_EntityNode->GetAniCtrl();		
	m_pResRenderable = m_EntityNode->GetResObj();

	//--设置设像机的位置
	ASSERT(m_pSG!=NULL);
	m_Camera.SetPerspective((FLOAT)Device()->GetClientWidth()/*1024*/, /*800*/(FLOAT)Device()->GetClientHeight(), 
		_DegToRad(30.0f), 10.0f, 20000.0f);
	m_Camera.Set(Lookat, CamYaw, CamPitch, CamRoll, MAXCamDist);
	/*m_Camera.SetPerspective(1024, 768, _DegToRad(45.0f), 10.0f, 20000.0f);
	m_Camera.SetLookAt(Vector3(0,0,-1100), Vector3(0,50,0));*/

	if( P_VALID(m_pSG) )
	{
		m_pSG->BindCamera(&m_Camera);
	}
	
	//--设置人物的位置为原点
	Transform trans;
	trans.Translate(Vector3(0,0,0));
	//trans.Translate(Vector3(0,-50,0));
	m_EntityNode->SetWorldMat(trans.GetMatrix());	

	tagLight light;
	light = (tagLight&)m_pSG->GetSunLight();
	light.diffuse = Color4f(1.0f, 1.5f, 1.5f, 1.5f);
	m_pSG->SetSunLight(light);
}

VOID LoginFrame::Render3D()
{
	float delTime = Kernel::Inst()->GetDeltaTime();	

	if(!P_VALID(m_pResRenderable))
		return;
	if(!m_pResRenderable->IsCreated())
		return;
	if( !P_VALID(m_pAniCtrl) )
		return;

	if( P_VALID(m_pSG) && m_nPlayState!=Loading )
		m_pSG->Render(delTime);

	if( m_nPlayState == Loading )
	{
		m_nPlayState = Preface;
		JudegPolicy();
	}
	
	if( m_nPlayState == Preface )
	{
		m_pAniCtrl->OpenTrack(_T("sta"), false);
		m_pTime = Kernel::Inst()->GetAccumTime();
		m_nPlayState = Loop;
		return;
	}

	if( m_nPlayState == Loop )
	{
		if( m_pAniCtrl->GetTrackPosition(_T("sta")) > 1.4f )
		{
			if(!m_bLoginType)
				m_pWnd->SetInvisible(FALSE);
			//JudegPolicy();
		}
	}

	if( m_nPlayState==Loop && m_pAniCtrl->IsTrackEnd(_T("sta")) )
	{
		m_pAniCtrl->OpenTrack(_T("mid"), true);
		if( !m_bLoginType )
			m_pWnd->SetInvisible(FALSE);
		m_nPlayState = Over;
		if(m_bLoginType)
			LoginGame();
		return;
	}	
	
	if( m_nPlayState==Over && m_bEnter )
	{
		m_pAniCtrl->OpenTrack(_T("end"), false);
		m_pTime = Kernel::Inst()->GetAccumTime();
			m_pWnd->SetInvisible(TRUE);
		m_nPlayState = Enter;
		return;
	}

	if( m_nPlayState==Enter && m_pAniCtrl->IsTrackEnd(_T("end")) )
	{	
		GameFrame* pFrame = m_pFrameMgr->CreateFrame(_T("Root"), _T("SelectRole"), _T("SelectRoleFrame"), 0);
		SeleRoleMgr::Inst()->Init();
		LoadingFrame* pFrame1 = (LoadingFrame*)m_pFrameMgr->CreateFrame(_T("Root"), _T("Loading"), _T("LoadingFrame"), 0);
		if( P_VALID(pFrame1) )
			pFrame1->SetLoadingType(LoadingFrame::ELT_SelectRole);

		m_pFrameMgr->AddToDestroyList(this);
	}
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL LoginFrame::Destroy()
{
	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);
	if( P_VALID(m_pWndKey) )
		m_pGUI->AddToDestroyList(m_pWndKey);
	if(	P_VALID(m_pWndWorldList) )
		m_pGUI->AddToDestroyList(m_pWndWorldList);
	if( P_VALID(m_pWndSecurity) )
		m_pGUI->AddToDestroyList(m_pWndSecurity);
	if( P_VALID(m_pWndSeInputBack) )
		m_pGUI->AddToDestroyList(m_pWndSeInputBack);

	m_pSG->DetachDynamicNode(m_EntityNode);

	if( P_VALID(m_EntityNode) )
		SAFE_DELETE(m_EntityNode);

	if( P_VALID(m_pSG) )
		SAFE_DELETE(m_pSG); 

	m_pCmdMgr->UnRegister("NS_JoinGame", (NETMSGPROC)m_Trunk.sfp2(&LoginFrame::NetRecvJoinGame));
	KillObj("login_ip_list");

	stringstream stream;
	stream << "XClient_Login" << this;

	KillObj(stream.str().c_str());

	if( P_VALID(m_hDll) )
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
	if( m_bLoginType )
		g_bCommandLine = true;

	return GameFrame::Destroy();
}



//-----------------------------------------------------------------------------
//! 事件处理
//! 
//-----------------------------------------------------------------------------
DWORD LoginFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Drag:	//移动
		{
			m_pWndKey->Align();
			m_pWndKey->SetRefresh();
		}
	case EGUIE_EditBoxEnter:
		if( pWnd == m_pEditAccount )
		{
			tstring strAccount = m_pEditAccount->GetText();
			if( !strAccount.empty() )
				m_pGUI->SetActive(m_pEditPassword);
		}

		if( pWnd == m_pEditPassword 
			&& m_pButtonLogin->IsEnable() )
		{
			tstring strPassword  = m_pEditPassword->GetText();
			switch( m_nPolicy )
			{
			case 0:
				{
					m_pClient->WaitThreadLoad();
					Connect();
					return TRUE;
				}
				break;
			case 1:
				{
					if( !strPassword.empty() )
					{
						m_pClient->WaitThreadLoad();
						Connect();
						return TRUE;
					}
				}
				break;
			}
			
		}
		break;

	case EGUIE_KeyDown:
		if( pEvent->msg.dwParam1 == VK_TAB )
		{
			if( m_pGUI->GetActive() == m_pEditAccount )
			{
				m_pGUI->SetActive(m_pEditPassword);
				m_pPBtn->SetState(EGUIBS_PopUp, FALSE);
				m_pWndKey->SetInvisible(m_pPBtn->GetState()!=EGUIBS_PushDown);
			}
			else if(m_pGUI->GetActive() == m_pEditPassword)
			{
				m_pGUI->SetActive(m_pEditAccount);
				m_pPBtn->SetState(EGUIBS_PopUp, FALSE);
				m_pWndKey->SetInvisible(m_pPBtn->GetState()!=EGUIBS_PushDown);
			}
		}
		break;

	case EGUIE_Click:
		if( pWnd == m_pButtonQuit )
		{
			m_pClient->ExitThreadLoad();
			PostQuitMessage(0);
		}
		else if( pWnd == m_pButtonLogin )//登录
		{
			tstring strPassword  = m_pEditPassword->GetText();
			switch( m_nPolicy )
			{
			case 0:
				{
					m_pClient->WaitThreadLoad();
					Connect();
					return TRUE;
				}
				break;
			case 1:
			case 3:
				{
					if( !strPassword.empty() )
					{
						m_pClient->WaitThreadLoad();
						Connect();
						return TRUE;
					}
				}
				break;
			}
		}
		else if( pWnd == m_pBtnCancelQueue )		//取消排队不用向服务器发消息
		{
			m_pWndQueue->SetInvisible(TRUE);
			m_pBtnCancelQueue->SetInvisible(TRUE);
			m_pStcPosition->SetText(_T(""));
			m_pStcTime->SetText(_T(""));
			if( !m_bLoginType )
				m_pWnd->SetInvisible(FALSE);
			m_pWndKey->SetInvisible(m_pWndKey->IsInvisible());
			m_pStcHint->SetText(g_StrTable[_T("JoinGame_InputNameOrSN")]);
			CutOffLogin();
			if( !m_bLoginType )
				m_pButtonLogin->SetEnable(true);
			switch( m_nPolicy )
			{
			case 0:
			case 1:
			case 3:
				break;
			case 2:				//迅雷
				{
					JudegPolicy();
				}
				break;
			}
		}
		else
		{
			if( tstring::npos != pWnd->GetName().find(_T("butt")) )
			{
				tstring str = m_pLastActive->GetText();
				str += ((GUIButton*)pWnd)->GetText();
				m_pLastActive->SetText(str.c_str());
				m_pGUI->SetActive(m_pLastActive);
				m_pLastActive->Deselect();
			}
		}
		break;

	case EGUIE_Active:
		if( pWnd == m_pEditAccount )
			m_pLastActive = m_pEditAccount;
		if( pWnd == m_pEditPassword )
			m_pLastActive = m_pEditPassword;
		break;

	case EGUIE_PushButtonDown:
		{
			if( pEvent->dwParam1==1 || pEvent->dwParam1==2 )
			{
				m_pWndKey->Align();
				m_pWndKey->SetInvisible(m_pPBtn->GetState()!=EGUIBS_PushDown);
				if( pEvent->dwParam1 == 1 )
					m_pWndKey->FlipToTop();
			}
			if( pWnd == m_pPBtnSaveAccount )
			{
				if( pEvent->dwParam1 == 1 )
					m_bSave = true;
				else if( pEvent->dwParam1 == 2 )
					m_bSave = false;
			}

			if( pWnd == m_pPBtn_Tab )
			{
				LetterTab();				
			}
		}
		break;
	case EGUIE_ItemClick:
		{
			ASSERT(m_pWorldList);
			ASSERT(m_pBtnOKWorld);
			int curSel = m_pWorldList->GetCurSelectedRow();
			if( GT_INVALID!=(int)m_pWorldList->GetText(curSel,0) ) //选择项是否为空
				m_pBtnOKWorld->SetEnable(true);
			else
				m_pBtnOKWorld->SetEnable(false);
		}
		break;
	}

	return FALSE;
}

DWORD LoginFrame::EventHandlerForSecurity(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnSecuOK )
			{
				m_strInputMiBao += m_pStcInputHint->GetText();
				MBPhaseHint();
			}
			else if( pWnd == m_pBtnSecuCancel )
			{
				RandSetKeyMB();
				m_pStcInputHint->SetText(_T(""));
			}
			else
			{
				if( _tcsncmp(pWnd->GetName().c_str(), _T("butt"), 4) == 0 ) 
				{
					for( int i=0; i<10; i++ )
					{
						if( pWnd == m_pBtnSecuInput[i] )
						{
							tstring strTemp = m_pStcInputHint->GetText();
							if( _tcslen(strTemp.c_str())<2 ) 
							{
								strTemp += m_pBtnSecuInput[i]->GetText();
								m_pStcInputHint->SetText(strTemp.c_str());
								break;
							}
						}
					}
				}
			}
		}
		break;
	}
	return FALSE;
}



//-----------------------------------------------------------------------------
// ReloadUI
//-----------------------------------------------------------------------------
BOOL LoginFrame::ReloadUI()
{
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("LoginMode"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring policy = szTemp;
	if( policy == _T("") )
	{
		MessageBox(NULL, _T("Don't find LoginMode !"), _T(""), MB_OK);
		return FALSE;
	}

	INT nPolicy = _ttoi(policy.c_str());

	// 创建
	tstring strPath = g_strLocalPath + _T("\\ui\\login.xml");
	tstring strPath2 = g_strLocalPath + _T("\\ui\\queue.xml");
	tstring strPath3 = g_strLocalPath + _T("\\ui\\queuebutt.xml");
	tstring strPath4 = g_strLocalPath + _T("\\ui\\security.xml");
	tstring strPath5 = g_strLocalPath + _T("\\ui\\security_putin.xml");
	XmlElement element;
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());

	//调整位置
	GUIWnd* pDesktop = m_pGUI->GetDesktop();
	std::stringstream streamX, streamY;
	streamX << (pDesktop->GetSize().x/2-110);
	streamY << (pDesktop->GetSize().y/2-50);
	m_pGUI->ChangeXml(&element, "loginwin", "LogicPos_x", streamX.str().c_str());
	m_pGUI->ChangeXml(&element, "loginwin", "LogicPos_y", streamY.str().c_str());
	switch( nPolicy )
	{
	case 2:			//迅雷特殊处理
		{
			m_pGUI->ChangeXml(&element, "loginwin\\loginpic\\name", "ReadOnly", "yes");
			m_pGUI->ChangeXml(&element, "loginwin\\loginpic\\name", "Password", "yes");
			m_pGUI->ChangeXml(&element, "loginwin\\loginpic\\code1", "ReadOnly", "yes");
			m_pGUI->ChangeXml(&element, "loginwin\\loginpic\\kbbutt", "Invisible", "yes");	
			m_pGUI->ChangeXml(&element, "loginwin\\loginpic\\kbbutt", "Invisible", "yes");	
		}
		break;
	}


	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	//m_pWnd->SetInvisible(TRUE);
	
	tstring strPath1 = g_strLocalPath + _T("\\ui\\login_key.xml");
	m_pWndKey = m_pGUI->CreateFromFile("VFS_System", strPath1.c_str());
	m_pPBtn_Tab = (GUIPushButton*)m_pWndKey->GetChild(_T("kbpic\\shiftbutt"));

	// 排队
	m_pWndQueue = m_pGUI->CreateFromFile("VFS_System", strPath2.c_str());
	m_pWndQueue->SetInvisible(TRUE);
	m_pBtnCancelQueue = (GUIButton*)m_pGUI->CreateFromFile("VFS_System", strPath3.c_str());
	m_pBtnCancelQueue->SetInvisible(TRUE);

	//密保
	std::stringstream streamX1, streamY1;
	streamX1 << (pDesktop->GetSize().x/2-130);
	streamY1 << (pDesktop->GetSize().y/2);
	m_pGUI->LoadXml(&element, "VFS_System", strPath4.c_str());
	m_pGUI->ChangeXml(&element, "security_showback", "LogicPos_x", streamX1.str().c_str());
	m_pGUI->ChangeXml(&element, "security_showback", "LogicPos_y", streamY1.str().c_str());
	m_pGUI->ChangeXml(&element, "security_showback\\num", "ClassName", "CGUIEditBox");
	m_pWndSecurity = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pWndSecurity->SetInvisible(TRUE);
	std::stringstream streamX2, streamY2;
	streamX2 << (pDesktop->GetSize().x/2+200);
	streamY2 << (pDesktop->GetSize().y/2);
	m_pGUI->LoadXml(&element, "VFS_System", strPath5.c_str());
	m_pGUI->ChangeXml(&element, "security_inputback", "LogicPos_x", streamX2.str().c_str());
	m_pGUI->ChangeXml(&element, "security_inputback", "LogicPos_y", streamY2.str().c_str());
	m_pWndSeInputBack = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pWndSeInputBack->SetInvisible(TRUE);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&LoginFrame::EventHandler));
	m_pGUI->RegisterEventHandler(m_pWndKey->GetFullName().c_str(), m_Trunk.sfp1(&LoginFrame::EventHandler));
	m_pGUI->RegisterEventHandler(m_pBtnCancelQueue->GetFullName().c_str(), m_Trunk.sfp1(&LoginFrame::EventHandler));
	m_pGUI->RegisterEventHandler(m_pWndSecurity->GetFullName().c_str(), m_Trunk.sfp1(&LoginFrame::EventHandlerForSecurity));
	m_pGUI->RegisterEventHandler(m_pWndSeInputBack->GetFullName().c_str(), m_Trunk.sfp1(&LoginFrame::EventHandlerForSecurity));

	m_pWndCaption	= (GUIStatic*)m_pWnd->GetChild(_T("loginpic"));
	m_pWndPswHint	= (GUIStatic*)m_pWndCaption->GetChild(_T("word2"));
	m_pEditAccount	= (GUIEditBox*)m_pWndCaption->GetChild(_T("name"));
	m_pEditPassword = (GUIEditBox*)m_pWndCaption->GetChild(_T("code1"));
	m_pButtonLogin	= (GUIButton*)m_pWndCaption->GetChild(_T("surebutt"));
	m_pButtonQuit	= (GUIButton*)m_pWndCaption->GetChild(_T("canclebutt"));
	m_pStcSavePass	= (GUIStatic*)m_pWndCaption->GetChild(_T("word3"));
	m_pStcUser		= (GUIStatic*)m_pWndCaption->GetChild(_T("word1"));
	switch( nPolicy )
	{
	case 2:			//迅雷特殊处理
		m_pButtonQuit->SetEnable(false);
		break;
	}
	m_pStcHint		= (GUIStatic*)m_pWndCaption->GetChild(_T("new"));
	m_pPBtn			= (GUIPushButton*)m_pWndCaption->GetChild(_T("kbbutt"));
	m_pPBtnSaveAccount = (GUIPushButton*)m_pWndCaption->GetChild(_T("word3\\choosebutt"));

	m_pStcTime		= (GUIStatic*)m_pWndQueue->GetChild(_T("time"));
	m_pStcPosition	= (GUIStatic*)m_pWndQueue->GetChild(_T("queueing"));

	m_pStcSecuHint	= (GUIStatic*)m_pWndSecurity->GetChild(_T("word1"));
	m_pStcSecuNum	= (GUIEditBox*)m_pWndSecurity->GetChild(_T("num"));
	m_pStcInputHint = (GUIStatic*)m_pWndSeInputBack->GetChild(_T("input_show"));
	m_pBtnSecuOK	= (GUIButton*)m_pWndSeInputBack->GetChild(_T("surebutt"));
	m_pBtnSecuCancel= (GUIButton*)m_pWndSeInputBack->GetChild(_T("canclebutt"));
	TCHAR szBuff[X_LONG_NAME] = {0};
	for( int i=0; i<10; i++ )
	{
		_stprintf( szBuff, _T("butt%d"), i+1 );
		m_pBtnSecuInput[i] = (GUIButton*)m_pWndSeInputBack->GetChild(szBuff);
	}

	m_pWndKey->SetInvisible(TRUE);
	this->LoadLastLoginInfo();
	this->SetKeyButtonPos();

	m_pLastActive = m_pEditAccount;
	m_pGUI->SetActive(m_pEditAccount);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 随机交换键盘按钮位置
//-----------------------------------------------------------------------------
VOID LoginFrame::SetKeyButtonPos()
{
	tstring strName;
	GUIWnd* pPanel = m_pWndKey->GetChild(_T("kbpic"));
    if (!P_VALID(pPanel))
        return;

	for(INT n=0, m=0; n<36; n++)
	{
		m = (m_pUtil->Rand() % 36);
		while( m == n ) 
			m = (m_pUtil->Rand() % 36);

		tstringstream stream1, stream2;
		stream1 << _T("butt") << n;
		stream2 << _T("butt") << m;
		GUIButton* pButton1 = (GUIButton*)pPanel->GetChild(stream1.str().c_str());
		GUIButton* pButton2 = (GUIButton*)pPanel->GetChild(stream2.str().c_str());

        if (P_VALID(pButton1) && P_VALID(pButton2))
        {
            tstring strTemp = pButton1->GetText();
            pButton1->SetText(pButton2->GetText().c_str());
            pButton2->SetText(strTemp.c_str());
        }
	}
}

void LoginFrame::RandSetKeyMB()
{
	tstring strName;
	for(INT n=0, m=0; n<10; n++)
	{
		m = (m_pUtil->Rand() % 10);
		while( m == n ) 
			m = (m_pUtil->Rand() % 10);

		tstringstream stream1, stream2;
		stream1 << _T("butt") << n+1;
		stream2 << _T("butt") << m+1;
		GUIButton* pButton1 = (GUIButton*)m_pWndSeInputBack->GetChild(stream1.str().c_str());
		GUIButton* pButton2 = (GUIButton*)m_pWndSeInputBack->GetChild(stream2.str().c_str());

		tstring strTemp = pButton1->GetText();
		pButton1->SetText(pButton2->GetText().c_str());
		pButton2->SetText(strTemp.c_str());
	}
}

void LoginFrame::SendLoginMsg()
{
	//发送nlc_proof
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("LocalGame"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring version = szTemp;

	tstring strAccount = m_pEditAccount->GetText();
	tstring strPassword = m_pEditPassword->GetText();
	this->FilterString(strAccount);
	this->FilterString(strPassword);
	string strAct = m_pUtil->UnicodeToAnsi(strAccount.c_str());

	tagNLC_SafeProof send;
	send.dwType = m_nPolicy;		
	tstring szWorldName = m_pVarContainer->GetString(_T("gamename"), _T("last_login_server"));
	send.dwWorldNameCrc = m_pUtil->Crc32(szWorldName.c_str());
	send.dwCurVersionID = m_pUtil->Crc32(version.c_str());

	switch(m_nPolicy)
	{
	case 0:					//内部使用
		{
			strncpy( send.szUserName, strAct.c_str(), X_SHORT_NAME );
			if( !strPassword.empty() )
			{
				string pass = m_pUtil->UnicodeToAnsi(strPassword.c_str());
				//密钥
				pass += "xvDvgqZq";
				BYTE digest[16];
				m_pUtil->MD5ForString( (char*)pass.c_str(), digest );
				string strPswMD5;
				m_pUtil->MD5ToString( strPswMD5, digest );
				char szBuff[X_LONG_NAME] = {0};
				strncpy( szBuff, strPswMD5.c_str(), MAX_MD5_ARRAY );
				strupr( szBuff );
				strncpy( send.szPsd, szBuff, MAX_MD5_ARRAY );
			}
		}
		break;
	case 1:					//中心激活
		{
			strncpy( send.szUserName, strAct.c_str(), X_SHORT_NAME );
			string pass = m_pUtil->UnicodeToAnsi(strPassword.c_str());
			//密钥
			pass += "xvDvgqZq";
			BYTE digest[16];
			m_pUtil->MD5ForString( (char*)pass.c_str(), digest );
			string strPswMD5;
			m_pUtil->MD5ToString( strPswMD5, digest );
			char szBuff1[X_LONG_NAME] = {0};
			strncpy( szBuff1, strPswMD5.c_str(), MAX_MD5_ARRAY );
			strupr( szBuff1 );
			strncpy( send.szPsd, szBuff1, MAX_MD5_ARRAY );

			//MAC地址+C盘的序列号
			string strMAC;
			GetMAC(strMAC);
			DWORD Volue ;
			//API获得某个特定驱动器的序列号,这个程序中是获得C盘的序列号
			::GetVolumeInformation(_T("c:\\"), NULL, 12, &Volue, NULL, NULL, NULL, 10);
			char szBuff[64];
			std::string strTemp(_itoa(Volue, szBuff, 16));
			strMAC += strTemp;
			BYTE digest1[16];
			m_pUtil->MD5ForString( (char*)strMAC.c_str(), digest1 );
			string strGUIDMD5;
			m_pUtil->MD5ToString( strGUIDMD5, digest1 );
			strncpy( send.szGUID, strGUIDMD5.c_str(), MAX_MD5_ARRAY );
		}
		break;
	case 2:					//迅雷
		{
			memcpy( send.szUserName, m_szUserName, sizeof(m_szUserName) );
			memcpy( send.szGUID, m_szEncrypt, sizeof(m_szEncrypt) );
		}
		break;
	case 3:
		{
			if( m_bLoginType )
			{
				strncpy( send.szUserName, "NathanielYu", X_SHORT_NAME );
				int nLength = strAct.length();
				strncpy( send.szGUID, strAct.c_str(), nLength );
				send.szGUID[nLength] = ' ';
				if( !strPassword.empty() && m_szPass != NULL )
				{
					char szBuff[X_SHORT_NAME] = {0};
					m_pUtil->Encrypt( m_szPass, szBuff, X_SHORT_NAME, 0 );
					strncpy( send.szGUID + nLength + 1, szBuff, X_SHORT_NAME );
				}
			}
			else
			{
				strncpy( send.szUserName, strAct.c_str(), X_SHORT_NAME );
				int nLength = strAct.length();
				strncpy( send.szGUID, strAct.c_str(), nLength );
				send.szGUID[nLength] = ' ';
				string pass = m_pUtil->UnicodeToAnsi(strPassword.c_str());
				if( !pass.empty() )
				{
					char szBuff[X_SHORT_NAME] = {0};
					m_pUtil->Encrypt( const_cast<char*>(pass.c_str()), szBuff, X_SHORT_NAME, 0 );
					strncpy( send.szGUID + nLength + 1, szBuff, X_SHORT_NAME );
				}
			}
		}
		break;
	}
    DWORD dwKeyResult;
    ProofKey(dwKeyResult, (&send));
    send.dwProofKey     = dwKeyResult;
	
	m_pStream->Send(&send, sizeof(send));
	SetState(ELS_WaitProofResult);
}


//-----------------------------------------------------------------------------
// login
//-----------------------------------------------------------------------------
VOID LoginFrame::Connect()
{
	this->SaveLastLoginInfo();
 	this->SetUIEnable(FALSE);

	if( m_pStream->IsConnected() )
	{
		this->SendLoginMsg();
		return;
	}

	if( m_pStream->IsTryingToConnect() )
		return;	// 仍然在尝试连接

	if( m_eState == ELS_Connect )	// 连接失败
	{
		SetState(ELS_WaitInput);
		this->SetUIEnable(TRUE);
		m_pStcHint->SetText(g_StrTable[_T("ConnectFail")]);
		switch( m_nPolicy )
		{
		case 2:		//迅雷
			{
				m_eHint = EXLHint_ConnectFail;
				JudegPolicy();
			}
			break;
		}
		return;
	}

	if( !m_bSelectedWorld )
		return;

	tstringstream stream;
	stream << _T("last_login_server");
	tstring strIP = m_pVarContainer->GetString(_T("ip"), stream.str().c_str());
	INT nPort = m_pVarContainer->GetDword(_T("port"), stream.str().c_str());
	
	// 尝试连接
	m_pStream->TryToConnect(m_pUtil->UnicodeToUnicode8(strIP.c_str()), nPort);
	m_pStcHint->SetText(g_StrTable[_T("WaitAuth")]);
	SetState(ELS_Connect);
}

//-----------------------------------------------------------------------------
// WaitResponse
//-----------------------------------------------------------------------------
VOID LoginFrame::WaitProofResult()
{
	//超时
	if(timeGetTime() - m_dwStateTime > 30000)
	{
		m_pStcHint->SetText(g_StrTable[_T("ConnectFail")]);
		this->SetUIEnable(TRUE);
		m_eMB = EMBP_NULL;
		CutOffLogin();
		switch( m_nPolicy )
		{
		case 2:		//迅雷
			{
				m_eHint = EXLHint_ConnectFail;
				JudegPolicy();
			}
			break;
		}
		return;
	}

	LPBYTE pMsg = NULL;
	DWORD dwMsgSize = 0;

	pMsg = m_pStream->Recv(dwMsgSize);
	if( P_VALID(pMsg) )
	{
		tagNLS_MiBao* pMiBao = (tagNLS_MiBao*)pMsg;
		if( pMiBao->dwID == m_pUtil->Crc32("NLS_MiBao") )
		{
			m_strMiBao = pMiBao->szMiBao;
			this->SetMBWndShow(TRUE);
			this->SetUIEnable(FALSE);
			m_pWnd->SetInvisible(TRUE);
			this->MBPhaseHint();
			m_pStream->FreeRecved(pMsg);
			SetState(ELS_MiBao);
			m_strInputMiBao = _T("");
			return;
		}

		tagNLS_ProofResult* pRecv = (tagNLS_ProofResult*)pMsg;
		if( pRecv->dwID == m_pUtil->Crc32("NLS_ProofResult") )
		{
			if( pRecv->dwErrorCode != E_Success )
			{
				if( m_bLoginType )
				{
					HideLoginFrame(false);
					m_bLoginType = !m_bLoginType;
				}
				ShowUserPwdInfo(pRecv->dwErrorCode);
				return;
			}
			m_dwZoneIP		= pRecv->dwIP;
			m_dwZonePort	= pRecv->dwPort;
			m_dwAccountID	= pRecv->dwAccountID;
			m_dwVerifyCode	= pRecv->dwVerifyCode;
			if (pRecv->dwErrorCode == 0)
				m_bGuard = pRecv->bGuard;
			CHAR* szIP = m_pStream->IP2String(m_dwZoneIP);
			m_pSession->Connect(m_pUtil->Unicode8ToUnicode(szIP), m_dwZonePort);		//与zone服务器建立连接
			SetState(ELS_ConnectZoneServer);	
			m_pStream->FreeRecved(pMsg);
			return;
		}
		m_pStream->FreeRecved(pMsg);
	}
}

// 显示用户名\密码错误信息
VOID LoginFrame::ShowUserPwdInfo(DWORD nInfo)
{
	switch(nInfo)
	{
	case E_ProofResult_Account_No_Match:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Account_No_Match")]);
		m_eHint = EXLHint_Account_No_Match;
		break;
	case E_ProofResult_Account_In_Use:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Account_In_Use")]);
		m_eHint = EXLHint_Account_In_Use;
		break;
	case E_ProofResult_Account_Freezed:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Account_Freezed")]);
		m_eHint = EXLHint_Account_Freezed;
		break;
	case E_ProofResult_Wrong_Type:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Wrong_Type")]);
		m_eHint = EXLHint_Wrong_Type;
		break;
	case E_ProofResult_Mibao_Error:
		{
			this->SetMBWndShow(FALSE);
			this->SetUIEnable(TRUE);
			m_pStcHint->SetText(g_StrTable[_T("ProofResult_Account_No_Match")]);
			m_eHint = EXLHint_Account_No_Match;
			m_eMB = EMBP_NULL;
		}
		break;
	case E_ProofResult_Queue:
		{
			m_pWndQueue->SetInvisible(FALSE);
			m_pBtnCancelQueue->SetInvisible(FALSE);
			m_pWnd->SetInvisible(TRUE);
			SetState(ELS_Queue);
			return;
		}
		break;
	case E_ProofResult_Wrong_Build_Number:
		{
			m_pStcHint->SetText(g_StrTable[_T("ProofResult_Wrong_Build_Number")]);
			WIN32_FIND_DATA wfd;
			HANDLE hFile;
			tstring szFindFilter = Kernel::Inst()->GetWorkPath();
			szFindFilter += _T("launcher.exe");
			hFile=FindFirstFile(szFindFilter.c_str(),&wfd);
			if(hFile!=INVALID_HANDLE_VALUE)
			{
				ShellExecute(NULL, _T("open"), szFindFilter.c_str(), NULL, NULL, SW_SHOW);
				SetState(ELS_Launcher);
				return;
			}
			else 
			{
				m_pStcHint->SetText(g_StrTable[_T("ProofResult_NoFindLauncher")]);
				m_eHint = EXLHint_NoFindLauncher;
			}
		}
		break;
	case E_ProofResult_Disabled:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Disabled")]);
		m_eHint = EXLHint_Disabled;
		break;
	case E_SelectWorld_Server_Full:
		m_pStcHint->SetText(g_StrTable[_T("SelectWorld_Server_Full")]);
		m_eHint = EXLHint_Wrong_Type;
		break;
	case E_SelectWorld_Server_Suspended:
		m_pStcHint->SetText(g_StrTable[_T("SelectWorld_Server_Suspended")]);
		m_eHint = EXLHint_Server_Suspended;
		break;
	case E_SelectWorld_Server_Maintenance:
		m_pStcHint->SetText(g_StrTable[_T("SelectWorld_Server_Maintenance")]);
		m_eHint = EXLHint_Server_Maintenance;
		break;
	case E_SelectWorld_Failed:
		m_pStcHint->SetText(g_StrTable[_T("SelectWorld_Failed")]);
		m_eHint = EXLHint_Failed;
		break;
	case E_SelectWorld_Short_Time:
		m_pStcHint->SetText(g_StrTable[_T("SelectWorld_Short_Time")]);
		m_eHint = EXLHint_Short_Time;
		break;
	case E_SelectWorld_No_Select:
		m_pStcHint->SetText(g_StrTable[_T("SelectWorld_No_Select")]);
		m_eHint = EXLHint_No_Select;
		break;
	case E_SelectWorld_GameWorldName_Wrong:
		m_pStcHint->SetText(g_StrTable[_T("SelectWorld_GameWorldName_Wrong")]);
		m_eHint = EXLHint_GameWorldName_Wrong;
		break;
	case E_SystemError:
		m_pStcHint->SetText(g_StrTable[_T("SystemError")]);
		m_eHint = EXLHint_SystemError;
		break;
	case E_ProofResult_Forbid_MemberCenter:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Forbid_MemberCenter")]);
		m_eHint = EXLHint_ProofResult_Forbid_MemberCenter;
		break;
	case E_ProofResult_Forbid_GMTool:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Forbid_GMTool")]);
		m_eHint = EXLHint_ProofResult_Forbid_GMTool;
		break;
	case E_ProofResult_Forbid_CellPhone:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Forbid_CellPhone")]);
		m_eHint = EXLHint_ProofResult_Forbid_CellPhone;
		break;
	case E_ProofResult_Forbid_MiBao:
		m_pStcHint->SetText(g_StrTable[_T("ProofResult_Forbid_MiBao")]);
		m_eHint = EXLHint_ProofResult_Forbid_MiBao;
		break;
	default:
		m_pStcHint->SetText(g_StrTable[_T("SystemError")]);
		m_eHint = EXLHint_SystemError;
		break;
	}
	this->SetUIEnable(TRUE);
	CutOffLogin();
	//迅雷平台登录时,重新调迅雷界面
	switch(m_nPolicy)
	{
	case 2:
		{
			JudegPolicy();
		}
		break;
	}
}

VOID LoginFrame::WaitQueue()
{
	LPBYTE pMsg = NULL;
	DWORD dwMsgSize = 0;

	pMsg = m_pStream->Recv(dwMsgSize);
	if( P_VALID(pMsg) )
	{
		tagNLS_Queue* pRecv = (tagNLS_Queue*)pMsg;
		if( pRecv->dwID == m_pUtil->Crc32("NLS_Queue") )
		{
			TCHAR szBuff[X_LONG_NAME] = {0};
			_stprintf(szBuff, g_StrTable[_T("Login_Queue0")], pRecv->dwPosition);
			m_pStcPosition->SetText(szBuff);

			DWORD dwTime = pRecv->dwTime;
			if( dwTime!=GT_INVALID && dwTime==QUEUE_TIME )
				_stprintf(szBuff, g_StrTable[_T("Login_Queue1")], QUEUE_TIME/60);
			else if( dwTime!=GT_INVALID && dwTime>60 )
				_stprintf(szBuff, g_StrTable[_T("Login_Queue2")], dwTime/60+1);
			else if( dwTime!=GT_INVALID && dwTime<=60 && dwTime>0)
				_stprintf(szBuff, g_StrTable[_T("Login_Queue3")], dwTime);
			else 
				_stprintf(szBuff, g_StrTable[_T("Login_Queue4")]);
			m_pStcTime->SetText(szBuff);

			if( pRecv->dwPosition==0 )
			{
				SetState(ELS_WaitProofResult);
				m_pWndQueue->SetInvisible(TRUE);
				m_pBtnCancelQueue->SetInvisible(TRUE);
				if( !m_bLoginType)
					m_pWnd->SetInvisible(FALSE);
				m_pWndKey->SetInvisible(m_pWndKey->IsInvisible());
				m_pStcPosition->SetText(_T(""));
				m_pStcTime->SetText(_T(""));
			}
		}

		m_pStream->FreeRecved(pMsg);
	}
}

// 是否连接上区域服务器
VOID LoginFrame::TryIsConnectZoneSer()
{
	if(m_pSession->IsConnect())
	{
		m_pStream->Disconnect();						//断开与login服务器的连接
		tagNC_JoinGame joinGameMsg;
		joinGameMsg.dwAccountID = m_dwAccountID;
		joinGameMsg.dwVerifyCode = m_dwVerifyCode;
		m_pSession->Send(&joinGameMsg);
		m_dwSentJoinGame = timeGetTime();
		SetState(ELS_SentJoinGame);
	}
	else if(timeGetTime() - m_dwStateTime > 10000)
	{
		//如果超时
		CutOffLogin();
		m_pStcHint->SetText(g_StrTable[_T("ConnectFail")]);
		this->SetUIEnable(TRUE);
	}
	
}

//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID LoginFrame::Update()
{
	switch(m_eState)
	{
	case ELS_WaitInput:
		break;
	case ELS_Connect:
		this->Connect();
		break;
	case ELS_WaitProofResult:
		this->WaitProofResult();
		break;
	case ELS_Queue:
		this->WaitQueue();
		break;
	case ELS_ConnectZoneServer:
		this->TryIsConnectZoneSer();
		break;
	case ELS_SentJoinGame:							//在发送join后,如果与zone服务器断开,则让玩家重新连接
		{
			if(!m_pSession->IsConnect())
				CutOffZone();
			if(timeGetTime() - m_dwSentJoinGame >= 10000)
				CutOffZone();
		}
		break;
	case ELS_JoinGame:
		break;
	case ELS_Launcher:
		{
			if(timeGetTime() - m_dwStateTime >= 1000)
			{
				m_pClient->ExitThreadLoad();
				PostQuitMessage(0);
			}
		}
		break;
	}

	if(!TObjRef<AudioSys>()->MusicIsPlaying())  
	{
		INT nMusic = SaveAccount::Inst()->GetCurMusic() ? 0 : 1;
		SaveAccount::Inst()->SetCurMusic(nMusic);
		TObjRef<AudioSys>()->StopMusic();
		TCHAR szBuff[X_SHORT_NAME] = {0};
		_sntprintf( szBuff, X_SHORT_NAME, _T("music_login%d"), nMusic );
		TObjRef<AudioSys>()->PlayMusic(szBuff, FALSE, SOUND_LOUD);
	}
}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID LoginFrame::SetState(ELoginState eState)
{
	if( m_eState == eState )
		return;

	m_eState = eState;
	m_dwStateTime = timeGetTime();

}


//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
VOID LoginFrame::SetUIEnable(BOOL bVale)
{
	if(bVale == FALSE)
	{
		m_pButtonLogin->SetEnable(false);
		m_pWndKey->SetInvisible(TRUE);
		m_pPBtn->SetState(EGUIBS_PopUp, FALSE);
		m_pPBtn->SetEnable(false);
	}
	else
	{
		m_pWndKey->SetInvisible(m_pPBtn->GetState()!=EGUIBS_PushDown);
		if( !m_bLoginType )
			m_pButtonLogin->SetEnable(true);
		m_pPBtn->SetEnable(true);
	}
}


//-----------------------------------------------------------------------------
// 处理JoinGame消息
//-----------------------------------------------------------------------------
DWORD LoginFrame::NetRecvJoinGame(tagNS_JoinGame* pMsg, DWORD pPrama)
{
	if( pMsg->dwID != m_pUtil->Crc32("NS_JoinGame") )
		return 0;
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		m_bEnter = TRUE;				//进入选人界面前,等待动画播放完毕
		TObjRef<AudioSys>()->Play2DSound(_T("login"));
		//初始化服务器时间同步
		ServerTime::Inst()->Init();
		//初始化反外挂
		HackShield::Inst()->Init();
		HackShield::Inst()->StartApexClient();
		SetState(ELS_JoinGame);
		break;
	case E_JoinGame_AccountOnline:
		m_pStcHint->SetText(g_StrTable[_T("JoinGame_AccountOnline")]);
		break;
	case E_JoinGame_VerifyCodeError:
		m_pStcHint->SetText(g_StrTable[_T("JoinGame_VerifyCodeError")]);
		break;
	}
	if(pMsg->dwErrorCode!=E_Success)
		CutOffZone();
	return 0;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID LoginFrame::CutOffLogin()
{
	m_pStream->Disconnect();
	SetState(ELS_WaitInput);
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
VOID LoginFrame::CutOffZone()
{
	SetState(ELS_WaitInput);
	m_pStcHint->SetText(_T("与服务器断开连接"));
	if( !m_bLoginType)
		m_pButtonLogin->SetEnable(true);
	m_pSession->Disconnect();
}


//-----------------------------------------------------------------------------
// 从注册表读取当前用户名和密码
//-----------------------------------------------------------------------------
VOID LoginFrame::LoadLastLoginInfo()
{
	TCHAR szTmp[512] = {0};
	m_pUtil->ReadReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_save_account"), szTmp);


	if( _tcscmp( szTmp, _T("EGUIBS_PushDown") ) == 0 )
	{
		// 填写默认的设置
		m_pUtil->ReadReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_account_name"), szTmp);
		tstring strAccount = szTmp;
		this->FilterString(strAccount);
		m_pEditAccount->SetText(strAccount.c_str());
		m_pPBtnSaveAccount->SetState(EGUIBS_PushDown, false);
	}
	

	//程序专用
#ifdef _DEBUG
	m_pUtil->ReadReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_account_name"), szTmp);
	tstring strAccount = szTmp;
	this->FilterString(strAccount);
	m_pEditAccount->SetText(strAccount.c_str());

	m_pUtil->ReadReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_password"), szTmp);
	tstring strPassword = szTmp;
	this->FilterString(strPassword);
	m_pEditPassword->SetText(strPassword.c_str());
#endif
}



//-----------------------------------------------------------------------------
// 记录当前用户名和密码到注册表
//-----------------------------------------------------------------------------
VOID LoginFrame::SaveLastLoginInfo()
{
	tstring strAccount = m_pEditAccount->GetText();
	tstring strPassword = m_pEditPassword->GetText();

	this->FilterString(strAccount);
	this->FilterString(strPassword);

	SaveAccount::Inst()->SetAccount(strAccount.c_str());

	if( m_bSave )
	{
		m_pUtil->WriteReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_account_name"), strAccount.c_str());
		m_pUtil->WriteReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_save_account"), _T("EGUIBS_PushDown"));
	}
	else
	{
		m_pUtil->WriteReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_save_account"), _T("EGUIBS_PopUp"));
	}

	//程序专用
#ifdef _DEBUG
	m_pUtil->WriteReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_account_name"), strAccount.c_str());
	m_pUtil->WriteReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("default_password"), strPassword.c_str());
#endif

	// 重新存储客户端位置，以便手动更新包等工具使用
	tstring strDir = TObjRef<VarContainer>()->GetString(_T("DIRECTORY"));
	m_pUtil->WriteReg(_T("SOFTWARE\\TENGREN\\LOONG"), _T("InstallDir"), strDir.c_str());
}


//-----------------------------------------------------------------------------
// 过滤用户名和密码中的回车换行等无意义字符
//-----------------------------------------------------------------------------
VOID LoginFrame::FilterString(tstring& str)
{
	// 去掉回车
	INT nFind = 0;
	while( (nFind = str.find(_T('\r'), nFind)) != str.npos )
		str.replace(nFind, 1, _T(""));

	// 去掉换行
	nFind = 0;
	while( (nFind = str.find(_T('\n'), nFind)) != str.npos )
		str.replace(nFind, 1, _T(""));

}

void LoginFrame::MBPhaseHint()
{
	switch( m_eMB )
	{
	case EMBP_NULL:
		{
			m_eMB = EMBP_First;
			string strHint(m_strMiBao, m_eMB, 2);
			tstring strHint1 = m_pUtil->AnsiToUnicode(strHint.c_str());
			m_pStcSecuNum->SetText(strHint1.c_str());
			m_pStcInputHint->SetText(_T(""));
			this->RandSetKeyMB();
		}
		break;
	case EMBP_First:
		{
			m_eMB = EMBP_Second;
			string strHint(m_strMiBao, m_eMB, 2);
			tstring strHint1 = m_pUtil->AnsiToUnicode(strHint.c_str());
			m_pStcSecuNum->SetText(strHint1.c_str());
			m_pStcInputHint->SetText(_T(""));
			this->RandSetKeyMB();
		}
		break;
	case EMBP_Second:
		{
			m_eMB = EMBP_Third;
			string strHint(m_strMiBao, m_eMB, 2);
			tstring strHint1 = m_pUtil->AnsiToUnicode(strHint.c_str());
			m_pStcSecuNum->SetText(strHint1.c_str());
			m_pStcInputHint->SetText(_T(""));
			this->RandSetKeyMB();
		}		
		break;
	case EMBP_Third:
		{
			// 发送用户名、密码、密保
			this->SetMBWndShow(FALSE);
			this->SetUIEnable(TRUE);
			m_pButtonLogin->SetEnable(false);
			if( !m_bLoginType )
				m_pWnd->SetInvisible(FALSE);
			string strCRC = m_pUtil->UnicodeToAnsi(m_strInputMiBao.c_str());
			tagNLC_MiBao msg;
			msg.dwMBCrc = m_pUtil->Crc32(strCRC.c_str());
			m_pStream->Send(&msg, msg.dwSize);
			SetState(ELS_WaitProofResult);
		}
		break;
	case EMBP_End:
		break;
	}
}

void LoginFrame::SetMBWndShow(BOOL bVal)
{
	m_pWndSecurity->SetInvisible(!bVal);
	m_pWndSeInputBack->SetInvisible(!bVal);
}

typedef int(*lpFun) (char*, int);
int g_iRtn = 0;
int XunLeiCallBack( char *szData, int iRet )
{
	g_iRtn = iRet;
	if( iRet == 1 )
	{
		//登录器认证已经成功，szData为100字节的字符串
		TObjRef<Client>()->WaitThreadLoad();
		LoginFrame* pFrame = (LoginFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Login"));
		
		char szEncrypt[100] = {0};
		char szUserName[20] = {0};

		memcpy( szEncrypt, szData, 100 );
		memcpy(szUserName, &szData[100], 20);
		pFrame->SetEncrypt( szEncrypt, szUserName );
		pFrame->Connect();
		return 2;
	}
	else if( iRet == 0 )
	{
		//用户进行退出操作，iRet为0，szData无效
		TObjRef<Client>()->ExitThreadLoad();
		PostQuitMessage(0);
	}

	return 2;
}

void LoginFrame::JudegPolicy()
{
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("LoginMode"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring policy = szTemp;
	if( policy == _T("") )
	{
		MessageBox(NULL, _T("Don't find LoginMode !"), _T(""), MB_OK);
		return;
	}

	m_nPolicy = _ttoi(policy.c_str());
	switch( m_nPolicy )
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		{
			
			typedef void(*xlFun)(int, DWORD, TCHAR*, lpFun);
			HWND hWnd = (HWND)TObjRef<VarContainer>()->GetDword(_T("HWND"));

			m_hDll = LoadLibrary(_T("XlLogin.dll"));
			if( !P_VALID(m_hDll) )
			{
				MessageBox(NULL, _T("Load XlLogin.dll failed!"), _T(""), MB_OK);
				return;
			}

			ShowWindow(hWnd,SW_HIDE);
			xlFun LoginFun = (xlFun)GetProcAddress(m_hDll, "LoginWnd");
			if( !P_VALID(LoginFun) )
			{
				MessageBox(NULL, _T("Can't find Login()"), NULL, MB_OK);
				FreeLibrary(m_hDll);
				m_hDll = NULL;
				return;
			}
			DWORD dwWorldID = m_pVarContainer->GetDword(_T("world_id"), _T("last_login_server"));

			TCHAR szBuff[X_SHORT_NAME] = {0};
			_sntprintf( szBuff, X_SHORT_NAME, GetXunLeiHint(m_eHint) );

			DWORD dwWindow = GameSet::Inst()->Get(EGS_Windowed);
			
			INT nMode = GetWindowMode(dwWindow);

			LoginFun(nMode, dwWorldID, szBuff, (lpFun)XunLeiCallBack);
	
			if(m_hDll != NULL)
			{
				FreeLibrary(m_hDll);
				m_hDll = NULL;
			}

			ShowWindow(hWnd,SW_NORMAL);
			if( g_iRtn == 2 )
			{
				m_pStcHint->SetText(g_StrTable[_T("ProofResult_XlFlashError")]);
				ShellExecuteXunLei();
			}
			else if( g_iRtn!=1 && g_iRtn!=0 )
			{
				m_pStcHint->SetText(g_StrTable[_T("ProofResult_XlOtherError")]);
				ShellExecuteXunLei();
			}
		}
		break;
	case 3:
		{
			m_Camera.SetLookAt(Vector3(0,0,-1450), Vector3(0,40,0));
			m_bLoginType = GetCmdLineUserAndPassword();
			if(m_bLoginType)
				m_bLoginType = g_bCommandLine ? false : true;
			if(m_bLoginType)
				HideLoginFrame(m_bLoginType);
		}
		break;
	}
}

LPCTSTR LoginFrame::GetXunLeiHint( EXLHint eType )
{
	switch(eType)
	{
	case EXLHint_User:
		return g_StrTable[_T("JoinGame_InputNameOrSN_xl")];
		break;
	case EXLHint_UserError:
		return g_StrTable[_T("ProofResult_Account_No_Match_xl")];
		break;
	case EXLHint_SendMsg:
		return g_StrTable[_T("WaitAuth")];
		break;
	case EXLHint_ConnectFail:
		return g_StrTable[_T("ConnectFail")];
		break;
	case EXLHint_Account_No_Match:
		return g_StrTable[_T("ProofResult_Account_No_Match_xl")];
		break;
	case EXLHint_Account_In_Use:
		return g_StrTable[_T("ProofResult_Account_In_Use_xl")];
		break;
	case EXLHint_Account_Freezed:
		return g_StrTable[_T("ProofResult_Account_Freezed_xl")];
		break;
	case EXLHint_Wrong_Build_Number:
		return g_StrTable[_T("ProofResult_Wrong_Build_Number")];
		break;
	case EXLHint_Disabled:
		return g_StrTable[_T("ProofResult_Disabled_xl")];
		break;
	case EXLHint_NoFindLauncher:
		return g_StrTable[_T("ProofResult_NoFindLauncher")];
		break;
	case EXLHint_Wrong_Type:
		return g_StrTable[_T("ProofResult_Wrong_Type")];
		break;
	case EXLHint_Server_Full:
		return g_StrTable[_T("SelectWorld_Server_Full")];
		break;
	case EXLHint_Server_Suspended:
		return g_StrTable[_T("SelectWorld_Server_Suspended")];
		break;
	case EXLHint_Server_Maintenance:
		return g_StrTable[_T("SelectWorld_Server_Maintenance")];
		break;
	case EXLHint_Failed:
		return g_StrTable[_T("SelectWorld_Failed")];
		break;
	case EXLHint_Short_Time:
		return g_StrTable[_T("SelectWorld_Short_Time")];
		break;
	case EXLHint_No_Select:
		return g_StrTable[_T("SelectWorld_No_Select")];
		break;
	case EXLHint_GameWorldName_Wrong:
		return g_StrTable[_T("SelectWorld_GameWorldName_Wrong")];
		break;;
	case EXLHint_ProofResult_Forbid_MemberCenter:
		return g_StrTable[_T("ProofResult_Forbid_MemberCenter_xl")];
		break;
	case EXLHint_ProofResult_Forbid_GMTool:
		return g_StrTable[_T("ProofResult_Forbid_GMTool_xl")];
		break;
	case EXLHint_ProofResult_Forbid_CellPhone:
		return g_StrTable[_T("ProofResult_Forbid_CellPhone_xl")];
		break;
	case EXLHint_ProofResult_Forbid_MiBao:
		return g_StrTable[_T("ProofResult_Forbid_MiBao_xl")];
		break;
	default:
		return _T("");
		break;
	}
	return _T("");
}

void LoginFrame::SetUserNameUI( const char* szUserName )
{
	tstring strUserName = m_pUtil->AnsiToUnicode(szUserName);
	this->FilterString(strUserName);
	m_pEditAccount->SetText(strUserName.c_str());
	m_pEditPassword->SetText(_T("123"));
}

INT LoginFrame::GetWindowMode( DWORD dwIndex )
{
	switch(dwIndex)
	{
	case 0:	// 全屏
		return 2;
		break;
	case 1:	// 最佳窗口
		return 1;
		break;
	case 2: // 标准窗口
		return 0;
		break;
	case 3:	// 假全屏
		return 2;
		break;
	default:

		break;
	}

	return 0;
}

void LoginFrame::SetEncrypt( const char* szEncry , const char* szUserName )
{
	memcpy( m_szEncrypt, szEncry, 100 );
	memcpy( m_szUserName, szUserName, 20 );
	SetUserNameUI( m_szUserName );
}

void LoginFrame::ShellExecuteXunLei()
{
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("XunLeiFAQURL"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring url = szTemp;
	if( url == _T("") )
	{
		MessageBox(NULL, _T("Don't find FAQ url!"), _T(""), MB_OK);
		return;
	}

	//打开注册网页
	ShellExecute(NULL, _T("open"), url.c_str(), NULL, NULL, SW_MAXIMIZE);
}

void LoginFrame::LetterTab()
{
	if( m_bTab )
	{
		m_bTab = FALSE;
		TCHAR szLetter;
		tstring strName;
		GUIWnd* pPanel = m_pWndKey->GetChild(_T("kbpic"));
		for(INT i=0; i<36; ++i)
		{
			tstringstream stream;
			stream << _T("butt") << i;
			GUIButton* pButton1 = (GUIButton*)pPanel->GetChild(stream.str().c_str());
			if (P_VALID(pButton1))
            {
			    tstring strTemp = pButton1->GetText();
			    szLetter = strTemp.at(0);
			    if( szLetter>='A' && szLetter<='Z' )
			    {
				    szLetter+=32;
				    strTemp = szLetter;
				    pButton1->SetText(strTemp.c_str());
			    }
            }
		}
	}
	else
	{
		m_bTab = TRUE;
		TCHAR szLetter;
		tstring strName;
		GUIWnd* pPanel = m_pWndKey->GetChild(_T("kbpic"));
		for(INT i=0; i<36; ++i)
		{
			tstringstream stream;
			stream << _T("butt") << i;
			GUIButton* pButton1 = (GUIButton*)pPanel->GetChild(stream.str().c_str());
            if (P_VALID(pButton1))
            {
			    tstring strTemp = pButton1->GetText();
			    szLetter = strTemp.at(0);
			    if( szLetter>='a' && szLetter<='z' )
			    {
				    szLetter-=32;
				    strTemp = szLetter;
				    pButton1->SetText(strTemp.c_str());
			    }
            }
		}
	}
	m_pWndKey->SetRefresh(TRUE);
}

bool LoginFrame::GetCmdLineUserAndPassword()
{
	string strCommandLine = m_pUtil->UnicodeToAnsi( GetCommandLine() );
	if( strCommandLine.empty() || strCommandLine.length() > 1024 )
		return false;
	const char *pCommandLine = strstr( strCommandLine.c_str(), ".exe" );
	if( pCommandLine == NULL )
	{
		return false;
	}
	if( !( ( pCommandLine[6] == 'T' || pCommandLine[6] == 't' ) && ( pCommandLine[7] == 'W' || pCommandLine[7] == 'w' ) ) )
	{
		return false;
	}

	char commandLine[1024] = {0};
	char userAndPas[2][64] = {0};
	strcpy( commandLine, pCommandLine + 9 );
	int nNum = 0;
	char *pPart = strtok( commandLine, " " );
	while( pPart && nNum < 2 )
	{
		strcpy( userAndPas[nNum], pPart );
		nNum++;
		pPart = strtok( NULL, " " );
	}

	if( nNum != 2 )
		return false;

	if( P_VALID(m_pEditAccount) )
		m_pEditAccount->SetText( m_pUtil->AnsiToUnicode( userAndPas[0] ) );

	if( P_VALID(m_pEditPassword) )
	{
		m_pEditPassword->SetText(_T("NathanielYu"));
		strcpy( m_szPass, userAndPas[1] );
		//m_pEditPassword->SetText( m_pUtil->AnsiToUnicode( userAndPas[1] ) );
	}
	return true;
}

void LoginFrame::HideLoginFrame( bool bHide )
{
	if( bHide && P_VALID(m_pWnd) )
	{
		m_pEditAccount->SetInvisible(bHide);
		m_pEditPassword->SetInvisible(bHide);
		m_pWndPswHint->SetInvisible(bHide);
		m_pStcSavePass->SetInvisible(bHide);
		m_pPBtn->SetInvisible(bHide);
		m_pStcUser->SetInvisible(bHide);
		m_pButtonLogin->SetEnable(!bHide);
		m_pStcHint->SetText(_T(""));
	}
	else if( !bHide && P_VALID(m_pWnd) )
	{
		m_pEditAccount->SetInvisible(bHide);
		m_pEditPassword->SetInvisible(bHide);
		m_pWndPswHint->SetInvisible(bHide);
		m_pStcSavePass->SetInvisible(bHide);
		m_pPBtn->SetInvisible(bHide);
		m_pStcUser->SetInvisible(bHide);
	}
}

void LoginFrame::LoginGame()
{
	if( 3 == m_nPolicy )
	{
		m_pClient->WaitThreadLoad();
		Connect();
	}
}