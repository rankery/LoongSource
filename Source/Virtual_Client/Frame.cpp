
#include "stdafx.h"
#include "Frame.h"
#include "Player.h"
#include "ClientMgr.h"
#include "..\WorldDefine\login.h"
#include "..\WorldDefine\selectrole.h"
#include "..\WorldDefine\chat.h"
#include "..\WorldDefine\action.h"
#include "..\WorldDefine\msg_gm_cmd.h"
#include "..\WorldDefine\msg_map.h"
#include <iphlpapi.h>

#pragma comment(lib, "IPHlpAPI.lib")

#define ILOG m_pPlayer->GetLog()->Write

//--------------------------------静态函数部分-----------------------------------

//获得本机mac地址
static void __fastcall GetMAC(string& strMAC)   
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

//--------------------------------虚基类部分-----------------------------------
FrameBase::FrameBase()
{

}

FrameBase::~FrameBase()
{

}

//--------------------------------Login部分------------------------------------

//-----------------------------------------------------------------------------
// 构造析构
//-----------------------------------------------------------------------------
LoginFrame::LoginFrame( Player* pPlayer ) :
m_Trunk(this)
{
	m_pPlayer		= pPlayer;
	m_eFrameType	= FrameMgr::EF_LOGIN;
	m_eState		= ELS_WaitRetry;
	m_dwStateTime	= timeGetTime();
	m_dwSentJoinGame = 0;

	stringstream stream_login;
	stream_login << "Client_Login_" << pPlayer->m_dwID;
	m_pLoginSession = stream_login.str().c_str();
	stringstream stream_world;
	stream_world << "Client_World_" << pPlayer->m_dwID;
	m_pWorldSession = stream_world.str().c_str();
	stringstream stream_player;
	stream_player << "Player_" << pPlayer->m_dwID;
	m_pCmdMgr = stream_player.str().c_str();
	stringstream stream_loginIP;
	stream_loginIP << "Client_LoginIP_" << pPlayer->m_dwID;
	m_pUtil = stream_loginIP.str().c_str();
}

LoginFrame::~LoginFrame()
{
	Destroy();
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
VOID LoginFrame::Init()
{
	m_pVarContainer = "VC_Config";

	m_pLoginSession->Init(FALSE);

	m_pPlayer->Acquire();
	m_pCmdMgr->Register("NS_JoinGame", (NETMSGPROC)m_Trunk.sfp2(&LoginFrame::NetRecvJoinGame), _T("NS_JoinGame"));
	m_pPlayer->Release();
	SetState(ELS_WaitRetry);
}

//-----------------------------------------------------------------------------
// 事件处理
//-----------------------------------------------------------------------------
VOID LoginFrame::Update()
{
	switch(m_eState)
	{
	case ELS_WaitRetry:
		if(timeGetTime() - m_dwStateTime > 3000)
		{
			Connect();
			SetState(ELS_Connect);
		}
		break;
	case ELS_Connect:
		this->Connect();
		break;
	case ELS_WaitResponse:
		this->WaitResponse();
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
			if(!m_pWorldSession->IsConnected())
				CutOffZone();
			if(timeGetTime() - m_dwSentJoinGame >= 10000)
				CutOffZone();
		}
		break;
	case ELS_JoinGame:
		break;

	case ELS_Abandon:
		m_pPlayer->m_bAbandon = TRUE;
		break;
	}

}

//-----------------------------------------------------------------------------
// 释放资源
//-----------------------------------------------------------------------------
VOID LoginFrame::Destroy()
{
	m_pPlayer->Acquire();
	m_pCmdMgr->UnRegister("NS_JoinGame", (NETMSGPROC)m_Trunk.sfp2(&LoginFrame::NetRecvJoinGame));
	m_pPlayer->Release();

	m_pLoginSession->Destroy();
}

//-----------------------------------------------------------------------------
// Login
//-----------------------------------------------------------------------------
VOID LoginFrame::Connect()
{
	if( m_pLoginSession->IsConnected() )
	{
		//tagNLC_Challenge send;
		//m_pLoginSession->Send(&send, sizeof(send));
		//SetState(ELS_WaitResponse);	// 进入等待相应状态
		//IMSG( _T("send tagNLC_Challenge\r\n"));
		//DWORD dwRandCrc = m_pUtil->Crc32(m_pPlayer->m_strPassword.c_str());
		//IMSG( _T("Recv tagNLS_Response. \n"));

		tagNLC_SafeProof send;

		// 取得WorldName的CRC
		tstring tmpWorldName;
		if (!m_pPlayer->m_pLoginInfo->listWorldName->RandPeek(tmpWorldName))
		{
			CutOffLogin();
			return;
		}
		send.dwWorldNameCrc = m_pUtil->Crc32(tmpWorldName.c_str());

		string strName = m_pUtil->UnicodeToAnsi(m_pPlayer->m_strName.c_str());
		strncpy(send.szUserName, strName.c_str(), X_SHORT_NAME);
		send.dwCurVersionID = m_pUtil->Crc32(m_pPlayer->m_strVersion.c_str());
		send.dwType = m_pVarContainer->GetInt(_T("policy virtual_client"));; //服务器验证策略

		// 计算密码的MD5
		string strPass = m_pUtil->UnicodeToAnsi(m_pPlayer->m_strPassword.c_str());
		if (!strPass.empty())
		{
			//密钥
			strPass += "xvDvgqZq";
			BYTE digest[16];
			m_pUtil->MD5ForString( (char*)strPass.c_str(), digest );
			string strPswMD5;
			m_pUtil->MD5ToString( strPswMD5, digest );
			strncpy( send.szPsd, strPswMD5.c_str(), MAX_MD5_ARRAY );
		}

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

		DWORD dwKeyResult;
		ProofKey(dwKeyResult, (&send));
		send.dwProofKey     = dwKeyResult;

		m_pLoginSession->Send(&send, sizeof(send));
		//IMSG( _T("Send NLC_Proof. \n"));
		SetState(ELS_WaitProofResult);
		return;
	}

	if( m_pLoginSession->IsTryingToConnect() )
		return;	// 仍然在尝试连接

	if( m_eState == ELS_Connect )	// 连接失败
	{
		SetState(ELS_WaitRetry);
		return;
	}

	if (m_pPlayer->m_type != EPT_Center)
	{
		// 重新获得大区信息
		if (!g_pClientMgr->GetRandomLoginServer(m_pPlayer->m_pLoginInfo))
			return;
	}

	if (!P_VALID(m_pPlayer->m_pLoginInfo))
		return;

	// 尝试连接
	LPCSTR szLoginIP = m_pUtil->UnicodeToUnicode8(m_pPlayer->m_pLoginInfo->strLoginIP.c_str());
	m_pLoginSession->TryToConnect(szLoginIP, m_pPlayer->m_pLoginInfo->nPort);
	SetState(ELS_Connect);
}

//-----------------------------------------------------------------------------
// WaitResponse
//-----------------------------------------------------------------------------
VOID LoginFrame::WaitResponse()
{
	//超时
	if(timeGetTime() - m_dwStateTime > 10000)
	{
		CutOffLogin();
		return;
	}

	//LPBYTE pMsg = NULL;
	//DWORD dwMsgSize = 0;	
	//pMsg = m_pLoginSession->Recv(dwMsgSize);
	//if( P_VALID(pMsg) )
	//{
	//	tagNLS_Response* pRecv = (tagNLS_Response*)pMsg;
	//	if( pRecv->dwID == m_pUtil->Crc32("NLS_Response") )
	//	{
	//		DWORD dwRandCrc = m_pUtil->Crc32(m_pPlayer->m_strPassword.c_str());
	//		tstringstream streamPassword;
	//		streamPassword << pRecv->szRandomString;
	//		streamPassword << dwRandCrc;
	//		IMSG( _T("Recv tagNLS_Response. \n"));

	//		tagNLC_Proof send;
	//		send.dwPsdCrc = m_pUtil->Crc32(streamPassword.str().c_str());
	//		tstring tmpWorldName;
	//		if (!m_pPlayer->m_pLoginInfo->listWorldName->RandPeek(tmpWorldName))
	//		{
	//			CutOffLogin();
	//			return;
	//		}
	//		send.dwWorldNameCrc = m_pUtil->Crc32(tmpWorldName.c_str());
	//		_tcsncpy(send.szUserName, m_pPlayer->m_strName.c_str(), X_SHORT_NAME);
	//		send.dwCurVersionID = m_pUtil->Crc32(m_pPlayer->m_strVersion.c_str());

	//		m_pLoginSession->Send(&send, sizeof(send));
	//		SetState(ELS_WaitProofResult);
	//		IMSG( _T("Send NLC_Proof. \n"));
	//	}

	//	m_pLoginSession->FreeRecved(pMsg);
	//}
}

VOID LoginFrame::WaitProofResult()
{
	//超时
	if(timeGetTime() - m_dwStateTime > 10000)
	{
		CutOffLogin();
		return;
	}

	LPBYTE pMsg = NULL;
	DWORD dwMsgSize = 0;

	pMsg = m_pLoginSession->Recv(dwMsgSize);
	if( P_VALID(pMsg) )
	{
		tagNLS_ProofResult* pRecv = (tagNLS_ProofResult*)pMsg;
		if( pRecv->dwID == m_pUtil->Crc32("NLS_ProofResult") )
		{
			if( pRecv->dwErrorCode != E_Success )
			{
				IMSG( _T("%s:WaitProofResult ErrorCode =%d  \r\n"), m_pPlayer->m_strName.c_str(), pRecv->dwErrorCode );
				ShowUserPwdInfo(pRecv->dwErrorCode);
				return;
			}
			m_dwZoneIP		= pRecv->dwIP;
			m_dwZonePort	= pRecv->dwPort;
			m_dwAccountID	= pRecv->dwAccountID;
			m_dwVerifyCode	= pRecv->dwVerifyCode;

			switch (m_pPlayer->m_type)
			{
			case EPT_World:
				{
					CHAR* szIP = m_pLoginSession->IP2String(m_dwZoneIP);
					m_pWorldSession->TryToConnect(szIP, m_dwZonePort);	//与zone服务器建立连接
					SetState(ELS_ConnectZoneServer);
					//IMSG( _T("Connect to World!\n"));
				}
				break;

			case EPT_Login:
			case EPT_Center:
			default:
				CutOffLogin();
				break;
			}
		}

		m_pLoginSession->FreeRecved(pMsg);
	}
}

VOID LoginFrame::WaitQueue()
{
	LPBYTE pMsg = NULL;
	DWORD dwMsgSize = 0;

	pMsg = m_pLoginSession->Recv(dwMsgSize);
	if( P_VALID(pMsg) )
	{
		tagNLS_Queue* pRecv = (tagNLS_Queue*)pMsg;
		if( pRecv->dwID == m_pUtil->Crc32("NLS_Queue") )
		{
			if( pRecv->dwPosition==0 )
			{
				SetState(ELS_WaitProofResult);
			}
		}

		m_pLoginSession->FreeRecved(pMsg);
	}
}

//-----------------------------------------------------------------------------
// 连接上区域服务器
//-----------------------------------------------------------------------------
VOID LoginFrame::TryIsConnectZoneSer()
{
	if(m_pWorldSession->IsConnected())
	{
		m_pLoginSession->Disconnect();						//断开与login服务器的连接
		tagNC_JoinGame joinGameMsg;
		joinGameMsg.dwAccountID = m_dwAccountID;
		joinGameMsg.dwVerifyCode = m_dwVerifyCode;
		m_pWorldSession->Send(&joinGameMsg, joinGameMsg.dwSize);
		m_dwSentJoinGame = timeGetTime();
		SetState(ELS_SentJoinGame);
		//IMSG( _T("Send NC_JoinGame.\n"));
	}
	else if(timeGetTime() - m_dwStateTime > 10000)
	{
		//如果超时
		CutOffLogin();
	}
}

//-----------------------------------------------------------------------------
// 显示用户名\密码错误信息
//-----------------------------------------------------------------------------
VOID LoginFrame::ShowUserPwdInfo( DWORD nInfo )
{
	switch(nInfo)
	{
	case E_ProofResult_Queue:
		{
			SetState(ELS_Queue);
			return;
		}
		break;

	default:
		break;
	}
	CutOffLogin();
}

//-----------------------------------------------------------------------------
// 中断连接
//-----------------------------------------------------------------------------
VOID LoginFrame::CutOffZone()
{
	SetState(ELS_WaitRetry);
	m_pWorldSession->Disconnect();
	//IMSG( _T("Disconnect from World!\n"));
}

VOID LoginFrame::CutOffLogin()
{
	m_pLoginSession->Disconnect();
	switch (m_pPlayer->m_type)
	{
	case EPT_Center:
		SetState(ELS_Abandon);
		break;

	default:
		SetState(ELS_WaitRetry);
		break;
	}
	//IMSG( _T("Disconnect from Login!\n"));
}

//-----------------------------------------------------------------------------
// 内部状态变更
//-----------------------------------------------------------------------------
VOID LoginFrame::SetState( ELoginState eState )
{
	if( m_eState == eState )
		return;

	m_eState = eState;
	m_dwStateTime = timeGetTime();
}

//-----------------------------------------------------------------------------
// 处理JoinGame消息
//-----------------------------------------------------------------------------
void LoginFrame::NetRecvJoinGame( tagNS_JoinGame* pMsg, LPVOID pPrama )
{
	if( pMsg->dwID != m_pUtil->Crc32("NS_JoinGame") )
		return;

	//IMSG( _T("Recv NS_JoinGame.\n"));
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		//!! 进入选人界面
		if (m_pPlayer->ChangeFrame(FrameMgr::EF_SELECT))
			SetState(ELS_JoinGame);
		else
			CutOffZone();
		break;

	default:
		break;
	}
	if(pMsg->dwErrorCode!=E_Success)
		CutOffZone();
}

//-----------------------------RoleSelect部分----------------------------------

//-----------------------------------------------------------------------------
// 构造析构
//-----------------------------------------------------------------------------
SelectRoleFrame::SelectRoleFrame( Player* pPlayer ) :
m_Trunk(this)
{
	m_pPlayer		= pPlayer;
	m_eFrameType	= FrameMgr::EF_SELECT;
	m_nSelectRoleID = GT_INVALID;
	m_eCurState		= ESRS_EnumRole;

	stringstream stream_world;
	stream_world << "Client_World_" << pPlayer->m_dwID;
	m_pWorldSession = stream_world.str().c_str();
	stringstream stream_player;
	stream_player << "Player_" << pPlayer->m_dwID;
	m_pCmdMgr = stream_player.str().c_str();
}

SelectRoleFrame::~SelectRoleFrame()
{
	Destroy();
}

//-----------------------------------------------------------------------------
// 初始化
//-----------------------------------------------------------------------------
VOID SelectRoleFrame::Init()
{
	m_pPlayer->Acquire();
	m_pCmdMgr->Register("NS_EnumRole",			(NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvEnumRole),			_T("NS_EnumRole"));
	m_pCmdMgr->Register("NS_CreateRole",		(NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvCreateRole),			_T("NS_CreateRole"));
	m_pCmdMgr->Register("NS_SelectRole",		(NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvSelectRole),			_T("NS_SelectRole"));
	m_pCmdMgr->Register("NS_SynchronismTime",	(NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvSynchronismTime),		_T("NS_SynchronismTime"));
	m_pPlayer->Release();
}

//-----------------------------------------------------------------------------
// 处理消息
//-----------------------------------------------------------------------------
VOID SelectRoleFrame::Update()
{
	if (!m_pWorldSession->IsConnected())
	{
		ReturnGame();
		return;
	}

	switch(m_eCurState)
	{
	case ESRS_EnumRole:
		{
			if(m_pWorldSession->IsConnected())
			{
				tagNC_EnumRole enumRoleMsg;
				m_pWorldSession->Send(&enumRoleMsg, enumRoleMsg.dwSize);
				SetCurState(ESRS_SentEnumRole);
				//IMSG( _T("Send NC_EnumRole.\n"));
			}
			else
			{
				SetCurState(ESRS_Back);
				return;
			}
		}
		break;

	case ESRS_SentEnumRole:
	case ESRS_SentSelectRole:
	case ESRS_SentCreateRole:
		break;

	case ESRS_RecvEnumRole:
	case ESRS_RecvSelectRole:
		{
			if( timeGetTime()-m_dwStateTime >= 3000 )
			{
				SetCurState(ESRS_EnumRole);
			}
		}
		break;

	case ESRS_SelectedRole:
		{
			SendEnterGameMsg();
		}
		break;

	case ESRS_Back:
		{
			if(timeGetTime() - m_dwStateTime >= 2000)
			{
				ReturnGame();
			}
		}
		break;

	default:
		break;
	}

}

//-----------------------------------------------------------------------------
// 释放资源
//-----------------------------------------------------------------------------
VOID SelectRoleFrame::Destroy()
{
	//--注销网络消息
	m_pPlayer->Acquire();
	m_pCmdMgr->UnRegister("NS_EnumRole",		(NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvEnumRole));
	m_pCmdMgr->UnRegister("NS_SelectRole",		(NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvSelectRole));
	m_pCmdMgr->UnRegister("NS_CreateRole",		(NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvCreateRole));
	m_pCmdMgr->UnRegister("NS_SynchronismTime", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvSynchronismTime));
	m_pPlayer->Release();
}

//-----------------------------------------------------------------------------
// 切换到WorldFrame
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::EnterGame( DWORD dwLocalRoleID )
{
	// 跳转到WorldFrame
	m_pPlayer->ChangeFrame(FrameMgr::EF_WORLD);
	return TRUE;
}

//-----------------------------------------------------------------------------
// 发送选择角色消息
//-----------------------------------------------------------------------------
VOID SelectRoleFrame::SendEnterGameMsg()
{
	if(m_pWorldSession->IsConnected())
	{
		tagNC_SelectRole seleRoleMsg;
		seleRoleMsg.dwRoleID = m_nSelectRoleID;
		m_pWorldSession->Send(&seleRoleMsg, seleRoleMsg.dwSize);
		SetCurState(ESRS_SentSelectRole);
		//IMSG(_T("Send NC_SelectRole.\n"));
	}

}

//-----------------------------------------------------------------------------
// 返回到LoginFrame
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::ReturnGame()
{
	m_pWorldSession->Disconnect();
	if (m_pPlayer->ChangeFrame(FrameMgr::EF_LOGIN))
		return TRUE;
	else
		return FALSE;
}

//-----------------------------------------------------------------------------
// 创建新角色
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::CreateRole()
{
	// 发送创建角色信息
	tagAvatarAtt avatarAtt;
	avatarAtt.bySex = m_pUtil->Rand() % 2;
	tagNC_CreateRole send;
	tstring playername;
	if (m_pPlayer->m_strName.length() > 7)
	{
		playername = m_pPlayer->m_strName.substr(m_pPlayer->m_strName.length()-7);
	}
	else
	{
		playername = m_pPlayer->m_strName;
	}
	_tcsncpy(send.szRoleName, playername.c_str(), X_SHORT_NAME);
	send.eStatus = (EPreChapStatus)0;
	send.AvatarAtt = avatarAtt;
	m_pWorldSession->Send(&send, send.dwSize);
	//IMSG(_T("Send NC_CreateRole.\n"));
	return TRUE;
}

//-----------------------------------------------------------------------------
// 接收玩家的角色信息
//-----------------------------------------------------------------------------
void SelectRoleFrame::NetRecvEnumRole( tagNS_EnumRole* pMsg, LPVOID pPrama )
{
	if(pMsg->dwErrorCode == E_Success)
	{	
		tagSimRole* pRole = (tagSimRole *)pMsg->bySimpleInfo;
		for(int i=0; i<pMsg->nNum; i++)
		{
			// 角色简易信息
			tstring name = pRole[i].szRoleName;
			tstring playername;
			if (m_pPlayer->m_strName.length() > 7)
			{
				playername = m_pPlayer->m_strName.substr(m_pPlayer->m_strName.length()-7);
			}
			else
			{
				playername = m_pPlayer->m_strName;
			}
			
			if (playername == name)
			{
				//角色已经存在,则发送SelectRole
				m_nSelectRoleID = pRole[i].dwRoleID;
				SetCurState(ESRS_SelectedRole);
				//IMSG(_T("Role Is Exist.\n"));
				return;
			}
		}

		// 若没有指定角色则创建一个
		CreateRole();
	}
	else
	{
		SetCurState(ESRS_RecvEnumRole);
		IMSG(_T("%s:Recv NS_RnumRole Error %d\n"), m_pPlayer->m_strName.c_str(), pMsg->dwErrorCode);
	}
}

//-----------------------------------------------------------------------------
// 接收玩家的选择角色信息
//-----------------------------------------------------------------------------
void SelectRoleFrame::NetRecvSelectRole( tagNS_SelectRole* pMsg, LPVOID pPrama )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			EnterGame(pMsg->dwRoleID);			
		}
		break;

	default:
		break;
	}
	if(pMsg->dwErrorCode!=E_Success)
	{
		SetCurState(ESRS_RecvSelectRole);
		IMSG(_T("%s:Recv NS_SelectRole Error %d\n"), m_pPlayer->m_strName.c_str(), pMsg->dwErrorCode);
	}
}

//-----------------------------------------------------------------------------
// 接收创建角色信息
//-----------------------------------------------------------------------------
void SelectRoleFrame::NetRecvCreateRole( tagNS_CreateRole* pMsg, LPVOID pPrama )
{
	 //IMSG(_T("Recv NS_CreateRole.\n"));

	 if (pMsg->dwErrorCode == E_Success)
	 {
		 tagSimRole* pRole = (tagSimRole *)pMsg->bySimRoleInfo;
		 m_nSelectRoleID = pRole->dwRoleID;
		 SetCurState(ESRS_SelectedRole);
		 return;
	 }

	 ReturnGame();
}

//-----------------------------------------------------------------------------
// 接收时间同步信息
//-----------------------------------------------------------------------------
void SelectRoleFrame::NetRecvSynchronismTime( tagNS_SynchronismTime* pMsg, LPVOID pPrama )
{
}

//-----------------------------LoongWorld部分----------------------------------

//-----------------------------------------------------------------------------
// 构造析构
//-----------------------------------------------------------------------------

WorldFrame::WorldFrame( Player* pPlayer ) :
m_Trunk(this)
{
	m_pPlayer		= pPlayer;
	m_eFrameType	= FrameMgr::EF_WORLD;
	m_curState		= EWState_GotoNewMap;
	stringstream stream_world;
	stream_world << "Client_World_" << pPlayer->m_dwID;
	m_pWorldSession = stream_world.str().c_str();
	stringstream stream_player;
	stream_player << "Player_" << pPlayer->m_dwID;
	m_pCmdMgr = stream_player.str().c_str();

	m_dwNextChatTime = m_dwNextMoveTime = timeGetTime();
	m_dwLogoutTime = 0;
}

WorldFrame::~WorldFrame()
{
	Destroy();
}

VOID WorldFrame::Init()
{
	m_pVarContainer = "VC_Config";
	m_dwOnlineTime = m_pVarContainer->GetInt(_T("online_time player"));; //服务器验证策略
	m_dwOnlineTime *= 1000;

	m_pPlayer->Acquire();
	RegisterALLRecvCmd();
	m_pPlayer->Release();
}

VOID WorldFrame::Update()
{
	if (!m_pWorldSession->IsConnected())
	{
		ReturnGame();
		return;
	}

	switch(m_curState)
	{
	case EWState_GotoNewMap:
		{
			//发送GM
			tstringstream tss;
			tss << _T("goto ") << m_pPlayer->m_strMapName << _T(" ") << m_pPlayer->m_Start_x <<_T(" ") << m_pPlayer->m_Start_z;
			tagNC_GMCommand send;
			lstrcpyn(send.szCommand, tss.str().c_str(), MAX_GM_COMMAND_LEN-1);
			m_pWorldSession->Send(&send, send.dwSize);
		}
		break;
	case EWState_Playing:
		// 计时下线
		if (m_dwOnlineTime != 0 && timeGetTime() - m_dwLogoutTime > m_dwOnlineTime)
		{
			ReturnGame();
			break;
		}

		// 发送chat和move消息
		SendChatMsg();
		SendMoveMsg();
		break;
	}
}

VOID WorldFrame::Destroy()
{
	m_pPlayer->Acquire();
	UnRegisterALLRecvCmd();
	m_pPlayer->Release();
}

void WorldFrame::SwitchToState( EWorldState state )
{
	m_curState = state;
	switch(m_curState)
	{	
	case EWState_GotoNewMap:
		break;
	case EWState_Playing:
		m_dwLogoutTime = timeGetTime();
		break;
	}

}

void WorldFrame::NetRecvGotoMap( tagNS_GotoNewMap* pMsg, LPVOID pPrama )
{
	tagNS_GotoNewMap* pRecv = (tagNS_GotoNewMap*)pMsg;
	m_pPlayer->m_srcPos.x = pRecv->pos.x;
	m_pPlayer->m_srcPos.y = pRecv->pos.y;
	m_pPlayer->m_srcPos.z = pRecv->pos.z;
	SwitchToState(EWState_Playing);
}

void WorldFrame::NetRecvMoveFailed( tagNS_MoveFailed* pMsg, LPVOID pPrama )
{
	SwitchToState(EWState_GotoNewMap);
}

void WorldFrame::NetRecvRoleChat(tagNS_RoleChat* pMsg, LPVOID pPrama)
{
	tstringstream tss;

	switch (pMsg->dwErrorCode)
	{
	case E_RoleChat_World_RoleNo10:		//玩家等级不足10级
		tss << _T("exp 10000");
		break;

	case E_RoleChat_World_NoMoney:		//金钱不足
		tss << _T("silver 1000");
		break;

	case E_RoleChat_CastTalk_NoIMItem:	//无道具
		tss << _T("item ") << pMsg->dwSrcRoleID << _T(" 100");
		break;

	case E_Success:
	default:
		return;
	}

	//发送GM
	tagNC_GMCommand send;
	lstrcpyn(send.szCommand, tss.str().c_str(), MAX_GM_COMMAND_LEN-1);
	m_pWorldSession->Send(&send, send.dwSize);
}

void WorldFrame::NetRecvDoNothing( tagNetCmd* pMsg, LPVOID pPrama )
{
	return;
}

void WorldFrame::Disconnection()
{
	m_pWorldSession->Disconnect();
	// !!返回LoginFrame
	m_pPlayer->ChangeFrame(FrameMgr::EF_LOGIN);
}

//-----------------------------------------------------------------------------
// 注册/注销所有World发送的信息
//-----------------------------------------------------------------------------
void WorldFrame::RegisterALLRecvCmd()
{
	// 属性和状态
	m_pCmdMgr->Register("NS_SetState",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SetState"));
	m_pCmdMgr->Register("NS_UnSetState",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UnSetState"));
	m_pCmdMgr->Register("NS_SetRoleState",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SetRoleState"));
	m_pCmdMgr->Register("NS_UnSetRoleState",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UnSetRoleState"));
	m_pCmdMgr->Register("NS_GetRoleInitState_Att",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleInitState_Att"));
	m_pCmdMgr->Register("NS_GetRoleInitState_Skill",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleInitState_Skill"));
	m_pCmdMgr->Register("NS_GetRoleInitState_CompleteQuest",		(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleInitState_CompleteQuest"));
	m_pCmdMgr->Register("NS_GetRoleInitState_IncompleteQuest",		(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleInitState_IncompleteQuest"));
	m_pCmdMgr->Register("NS_GetRoleInitState_Item",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleInitState_Item"));
	m_pCmdMgr->Register("NS_GetRoleInitState_Suit",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleInitState_Suit"));
	m_pCmdMgr->Register("NS_GetRoleInitState_ItemCDTime",			(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleInitState_ItemCDTime"));
	m_pCmdMgr->Register("NS_GetRoleInitState_Money",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleInitState_Money"));
	m_pCmdMgr->Register("NS_GetRemoteRoleState",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRemoteRoleState"));
	m_pCmdMgr->Register("NS_GetRemoteCreatureState",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRemoteCreatureState"));
	m_pCmdMgr->Register("NS_RoleAttChangeSingle",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleAttChangeSingle"));
	m_pCmdMgr->Register("NS_RoleAttChangeMutiple",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleAttChangeMutiple"));
	m_pCmdMgr->Register("NS_RemoteAttChangeSingle",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RemoteAttChangeSingle"));
	m_pCmdMgr->Register("NS_RemoteAttChangeMutiple",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RemoteAttChangeMutiple"));
	m_pCmdMgr->Register("NS_RoleExpChange",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleExpChange"));
	m_pCmdMgr->Register("NS_RoleLevelChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleLevelChange"));
	m_pCmdMgr->Register("NS_RoleBidAttPoint",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleBidAttPoint"));
	m_pCmdMgr->Register("NS_RoleClearAttPoint",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleClearAttPoint"));
	m_pCmdMgr->Register("NS_RoleAttPointAddChange",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleAttPointAddChange"));
	m_pCmdMgr->Register("NS_SendFriendBlackList",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SendFriendBlackList"));
	m_pCmdMgr->Register("NS_RemoveRemote",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RemoveRemote"));

	// 名字和ID
	m_pCmdMgr->Register("NS_RoleGetID",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleGetID"));
	m_pCmdMgr->Register("NS_RoleGetName",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleGetName"));
	m_pCmdMgr->Register("NS_GetNamebyNameID",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetNamebyNameID"));

	// 移动
	m_pCmdMgr->Register("NS_SyncWalk",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SyncWalk"));
	m_pCmdMgr->Register("NS_SyncJump",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SyncJump"));
	m_pCmdMgr->Register("NS_SyncDrop",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SyncDrop"));
	m_pCmdMgr->Register("NS_SyncVDrop",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SyncVDrop"));
	m_pCmdMgr->Register("NS_SyncSlide",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SyncSlide"));
	m_pCmdMgr->Register("NS_SyncStand",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SyncStand"));
	m_pCmdMgr->Register("NS_MoveFailed",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvMoveFailed),	_T("NS_MoveFailed"));
	m_pCmdMgr->Register("NS_HitFly",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_HitFly"));
	m_pCmdMgr->Register("NS_MoveSpeedChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_MoveSpeedChange"));

	// 聊天
	m_pCmdMgr->Register("NS_RoleChat",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvRoleChat),		_T("NS_RoleChat"));

	// 战斗
	m_pCmdMgr->Register("NS_Skill",									(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Skill"));
	m_pCmdMgr->Register("NS_SkillInterrupt",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SkillInterrupt"));
	m_pCmdMgr->Register("NS_UseItem",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UseItem"));
	m_pCmdMgr->Register("NS_UseItemInterrupt",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UseItemInterrupt"));
	m_pCmdMgr->Register("NS_HitTarget",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_HitTarget"));
	m_pCmdMgr->Register("NS_RoleHPChange",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleHPChange"));
	m_pCmdMgr->Register("NS_RoleDead",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleDead"));
	m_pCmdMgr->Register("NS_RoleRevive",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleRevive"));
	m_pCmdMgr->Register("NS_RoleReviveNotify",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleReviveNotify"));
	m_pCmdMgr->Register("NS_AddRoleBuff",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_AddRoleBuff"));
	m_pCmdMgr->Register("NS_RemoveRoleBuff",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RemoveRoleBuff"));
	m_pCmdMgr->Register("NS_UpdateRoleBuff",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UpdateRoleBuff"));
	m_pCmdMgr->Register("NS_StopAction",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StopAction"));
	m_pCmdMgr->Register("NS_RoleStyleAction",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleStyleAction"));
	m_pCmdMgr->Register("NS_MonsterEnterCombat",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_MonsterEnterCombat"));

	// 生产
	m_pCmdMgr->Register("NS_Produce",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Produce"));
	m_pCmdMgr->Register("NS_Decomposition",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Decomposition"));
	m_pCmdMgr->Register("NS_ConsolidatePosy",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ConsolidatePosy"));
	m_pCmdMgr->Register("NS_ConsolidateEngrave",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ConsolidateEngrave"));
	m_pCmdMgr->Register("NS_ConsolidateQuench",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ConsolidateQuench"));
	m_pCmdMgr->Register("NS_Inlay",									(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Inlay"));
	m_pCmdMgr->Register("NS_Brand",									(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Brand"));
	m_pCmdMgr->Register("NS_LoongSoul",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_LoongSoul"));
	m_pCmdMgr->Register("NS_Chisel",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Chisel"));
	m_pCmdMgr->Register("NS_DyeFashion",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_DyeFashion"));

	// 金钱和元宝
	m_pCmdMgr->Register("NS_BagSilver",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_BagSilver"));
	m_pCmdMgr->Register("NS_WareSilver",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_WareSilver"));
	m_pCmdMgr->Register("NS_BagYuanBao",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_BagYuanBao"));
	m_pCmdMgr->Register("NS_BaiBaoYuanBao",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_BaiBaoYuanBao"));

	// 交易
	m_pCmdMgr->Register("NS_ExchangeReq",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeReq"));
	m_pCmdMgr->Register("NS_ExchangeReqRes",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeReqRes"));
	m_pCmdMgr->Register("NS_ExchangeAdd_2Src",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeAdd_2Src"));
	m_pCmdMgr->Register("NS_ExchangeAdd_2Dst",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeAdd_2Dst"));
	m_pCmdMgr->Register("NS_ExchangeDec",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeDec"));
	m_pCmdMgr->Register("NS_ExchangeMoney",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeMoney"));
	m_pCmdMgr->Register("NS_ExchangeLock",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeLock"));
	m_pCmdMgr->Register("NS_ExchangeCancel",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeCancel"));
	m_pCmdMgr->Register("NS_ExchangeFinish",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ExchangeFinish"));

	// 职能NPC
	m_pCmdMgr->Register("NS_Dak",									(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Dak"));
	m_pCmdMgr->Register("NS_WareExtend",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_WareExtend"));
	m_pCmdMgr->Register("NS_BagExtend",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_BagExtend"));
	m_pCmdMgr->Register("NS_SaveSilver",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SaveSilver"));
	m_pCmdMgr->Register("NS_GetSilver",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetSilver"));
	m_pCmdMgr->Register("NS_SaveYuanBao",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SaveYuanBao"));
	m_pCmdMgr->Register("NS_GetYuanBao",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetYuanBao"));
	m_pCmdMgr->Register("NS_Abrase",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Abrase"));

	// 组队
	m_pCmdMgr->Register("NS_InviteToLeader",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_InviteToLeader"));
	m_pCmdMgr->Register("NS_InviteJoinTeam",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_InviteJoinTeam"));
	m_pCmdMgr->Register("NS_InviteReply",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_InviteReply"));
	m_pCmdMgr->Register("NS_KickMember",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_KickMember"));
	m_pCmdMgr->Register("NS_LeaveTeam",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_LeaveTeam"));
	m_pCmdMgr->Register("NS_SetPickMode",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SetPickMode"));
	m_pCmdMgr->Register("NS_ChangeLeader",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ChangeLeader"));
	m_pCmdMgr->Register("NS_RoleStateToTeam",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleStateToTeam"));
	m_pCmdMgr->Register("NS_RolePosToTeam",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RolePosToTeam"));

	// 物品和装备
	m_pCmdMgr->Register("NS_Equip",									(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Equip"));
	m_pCmdMgr->Register("NS_Unequip",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_Unequip"));
	m_pCmdMgr->Register("NS_SwapWeapon",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SwapWeapon"));
	m_pCmdMgr->Register("NS_AvatarEquipChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_AvatarEquipChange"));
	m_pCmdMgr->Register("NS_IdentifyEquip",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_IdentifyEquip"));
	m_pCmdMgr->Register("NS_EquipChange",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_EquipChange"));
	m_pCmdMgr->Register("NS_SuitEffect",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SuitEffect"));
	m_pCmdMgr->Register("NS_SuitNum",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SuitNum"));
	m_pCmdMgr->Register("NS_ItemPosChange",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ItemPosChange"));
	m_pCmdMgr->Register("NS_ItemPosChangeEx",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ItemPosChangeEx"));
	m_pCmdMgr->Register("NS_ItemAdd",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ItemAdd"));
	m_pCmdMgr->Register("NS_NewItemAdd",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_NewItemAdd"));
	m_pCmdMgr->Register("NS_NewEquipAdd",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_NewEquipAdd"));
	m_pCmdMgr->Register("NS_ItemRemove",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ItemRemove"));
	m_pCmdMgr->Register("NS_ItemCDUpdate",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ItemCDUpdate"));

	// 掉落
	m_pCmdMgr->Register("NS_SyncGroundItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SyncGroundItem"));
	m_pCmdMgr->Register("NS_RoleGroundItemDisappear",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleGroundItemDisappear"));
	m_pCmdMgr->Register("NS_PutDownItem",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_PutDownItem"));
	m_pCmdMgr->Register("NS_RolePutDownItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RolePutDownItem"));
	m_pCmdMgr->Register("NS_RolePickUpItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RolePickUpItem"));
	m_pCmdMgr->Register("NS_GetGroundItemInfo",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetGroundItemInfo"));

	// 地图
	m_pCmdMgr->Register("NS_EnterInstance",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_EnterInstance"));
	m_pCmdMgr->Register("NS_GotoNewMap",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvGotoMap),		_T("NS_GotoNewMap"));
	m_pCmdMgr->Register("NS_InstanceNofity",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_InstanceNofity"));
	m_pCmdMgr->Register("NS_InstanceAgree",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_InstanceAgree"));
	m_pCmdMgr->Register("NS_InstanceComplete",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_InstanceComplete"));
	m_pCmdMgr->Register("NS_SyncInstanceTime",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SyncInstanceTime"));
	m_pCmdMgr->Register("NS_BindRebornMap",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_BindRebornMap"));

	// PK
	m_pCmdMgr->Register("NS_SafeGuard",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SafeGuard"));
	m_pCmdMgr->Register("NS_PK",									(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_PK"));
	m_pCmdMgr->Register("NS_RolePKStateChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RolePKStateChange"));

	// 商店
	m_pCmdMgr->Register("NS_GetShopItems",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetShopItems"));
	m_pCmdMgr->Register("NS_GetShopEquips",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetShopEquips"));
	m_pCmdMgr->Register("NS_BuyShopItem",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_BuyShopItem"));
	m_pCmdMgr->Register("NS_BuyShopEquip",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_BuyShopEquip"));
	m_pCmdMgr->Register("NS_FeedbackFromShop",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_FeedbackFromShop"));

	// 展示物品和装备
	m_pCmdMgr->Register("NS_RoleShowEquip",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleShowEquip"));
	m_pCmdMgr->Register("NS_RoleShowItem",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleShowItem"));

	// 社会关系
	m_pCmdMgr->Register("NS_LoginToFriend",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_LoginToFriend"));
	m_pCmdMgr->Register("NS_LogoutToFriend",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_LogoutToFriend"));
	m_pCmdMgr->Register("NS_RoleMakeFriend",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleMakeFriend"));
	m_pCmdMgr->Register("NS_RoleCancelFriend",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleCancelFriend"));
	m_pCmdMgr->Register("NS_UpdateFriGroup",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UpdateFriGroup"));
	m_pCmdMgr->Register("NS_MoveToBlackList",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_MoveToBlackList"));
	m_pCmdMgr->Register("NS_DeleteBlackList",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_DeleteBlackList"));
	m_pCmdMgr->Register("NS_SendGiftToFri",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SendGiftToFri"));
	m_pCmdMgr->Register("NS_SendGiftToSender",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SendGiftToSender"));
	m_pCmdMgr->Register("NS_SendGiftBroadcast",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SendGiftBroadcast"));
	m_pCmdMgr->Register("NS_SendBlackList",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SendBlackList"));
	m_pCmdMgr->Register("NS_UpdateFriState",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UpdateFriState"));
	m_pCmdMgr->Register("NS_MakeFriNotice",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_MakeFriNotice"));
	m_pCmdMgr->Register("NS_UpdateFriValue",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UpdateFriValue"));

	// 摆摊
	m_pCmdMgr->Register("NS_StallStart",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallStart"));
	m_pCmdMgr->Register("NS_StallSetGoods",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallSetGoods"));
	m_pCmdMgr->Register("NS_StallUnsetGoods",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallUnsetGoods"));
	m_pCmdMgr->Register("NS_StallSetTitle",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallSetTitle"));
	m_pCmdMgr->Register("NS_StallSetAdText",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallSetAdText"));
	m_pCmdMgr->Register("NS_StallSetAdFlag",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallSetAdFlag"));
	m_pCmdMgr->Register("NS_StallSetFinish",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallSetFinish"));
	m_pCmdMgr->Register("NS_StallClose",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallClose"));
	m_pCmdMgr->Register("NS_StallBuyRefresh",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallBuyRefresh"));
	m_pCmdMgr->Register("NS_StallSetRefresh",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallSetRefresh"));
	m_pCmdMgr->Register("NS_StallUnsetRefresh",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallUnsetRefresh"));
	m_pCmdMgr->Register("NS_StallGet",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallGet"));
	m_pCmdMgr->Register("NS_StallGetTitle",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallGetTitle"));
	m_pCmdMgr->Register("NS_StallBuy",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallBuy"));
	m_pCmdMgr->Register("NS_StallGetSpec",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_StallGetSpec"));

	// 天资及技能
	m_pCmdMgr->Register("NS_LearnSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_LearnSkill"));
	m_pCmdMgr->Register("NS_AddSkill",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_AddSkill"));
	m_pCmdMgr->Register("NS_LevelUpSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_LevelUpSkill"));
	m_pCmdMgr->Register("NS_UpdateSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UpdateSkill"));
	m_pCmdMgr->Register("NS_UpdateSkillCoolDown",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UpdateSkillCoolDown"));
	m_pCmdMgr->Register("NS_ForgetSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ForgetSkill"));
	m_pCmdMgr->Register("NS_ClearTalent",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ClearTalent"));
	m_pCmdMgr->Register("NS_RemoveSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RemoveSkill"));
	m_pCmdMgr->Register("NS_AddTalent",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_AddTalent"));
	m_pCmdMgr->Register("NS_RemoveTalent",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RemoveTalent"));
	m_pCmdMgr->Register("NS_UpdateTalent",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_UpdateTalent"));

	// 任务
	m_pCmdMgr->Register("NS_AcceptQuest",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_AcceptQuest"));
	m_pCmdMgr->Register("NS_AddQuest",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_AddQuest"));
	m_pCmdMgr->Register("NS_CompleteQuest",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_CompleteQuest"));
	m_pCmdMgr->Register("NS_DeleteQuest",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_DeleteQuest"));
	m_pCmdMgr->Register("NS_QuestUpdateKillCreature",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_QuestUpdateKillCreature"));
	m_pCmdMgr->Register("NS_QuestUpdateItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_QuestUpdateItem"));
	m_pCmdMgr->Register("NS_QuestUpdateNPCTalk",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_QuestUpdateNPCTalk"));
	m_pCmdMgr->Register("NS_QuestUpdateTrigger",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_QuestUpdateTrigger"));

	// 声望
	m_pCmdMgr->Register("NS_GetReputation",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetReputation"));

	// 反外挂
	m_pCmdMgr->Register("NS_GameGuarder",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GameGuarder"));

	// 名人堂
	m_pCmdMgr->Register("NS_GetFameHallRoles",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetFameHallRoles"));
	m_pCmdMgr->Register("NS_GetReputeTop",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetReputeTop"));
	m_pCmdMgr->Register("NS_GetActClanTreasure",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetActClanTreasure"));
	m_pCmdMgr->Register("NS_ActiveTreasure",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ActiveTreasure"));
	m_pCmdMgr->Register("NS_NewActivedTreasure",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_NewActivedTreasure"));
	m_pCmdMgr->Register("NS_TreasureActCountChange",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_TreasureActCountChange"));
	m_pCmdMgr->Register("NS_BecomeFame",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_BecomeFame"));

	// 角色名贴
	m_pCmdMgr->Register("NS_GetRoleVCard",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleVCard"));
	m_pCmdMgr->Register("NS_SetRoleVCard",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SetRoleVCard"));
	m_pCmdMgr->Register("NS_GetRoleHeadPicURL",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleHeadPicURL"));

	// 称号
	m_pCmdMgr->Register("NS_ActiveRoleTitle",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_ActiveRoleTitle"));
	m_pCmdMgr->Register("NS_GetRoleTitles",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetRoleTitles"));
	m_pCmdMgr->Register("NS_NewTitles",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_NewTitles"));
	m_pCmdMgr->Register("NS_RoleTitleChangeBroadcast",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_RoleTitleChangeBroadcast"));

	// 帮派
	m_pCmdMgr->Register("NS_GuildCreateBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildCreateBroad"));
	m_pCmdMgr->Register("NS_GuildDismissBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildDismissBroad"));
	m_pCmdMgr->Register("NS_GuildJoinReq",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildJoinReq"));
	m_pCmdMgr->Register("NS_GuildJoinReqRes",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildJoinReqRes"));
	m_pCmdMgr->Register("NS_GuildJoinBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildJoinBroad"));
	m_pCmdMgr->Register("NS_GuildLeaveBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildLeaveBroad"));
	m_pCmdMgr->Register("NS_GuildKickBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildKickBroad"));
	m_pCmdMgr->Register("NS_GuildTurnoverBroad",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildTurnoverBroad"));
	m_pCmdMgr->Register("NS_GuildDemissBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildDemissBroad"));
	m_pCmdMgr->Register("NS_GuildAppointBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildAppointBroad"));
	m_pCmdMgr->Register("NS_GuildTenetChangeBroad",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildTenetChangeBroad"));
	m_pCmdMgr->Register("NS_GuildProcFailed",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildProcFailed"));
	m_pCmdMgr->Register("NS_GuildGetAllMembers",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildGetAllMembers"));
	m_pCmdMgr->Register("NS_GuildGetMemberEx",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildGetMemberEx"));
	m_pCmdMgr->Register("NS_GuildRefMember",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GuildRefMember"));
	m_pCmdMgr->Register("NS_GetGuildName",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetGuildName"));
	m_pCmdMgr->Register("NS_GetGuildTenet",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_GetGuildTenet"));

	// 百宝袋
	m_pCmdMgr->Register("NS_InitBaiBaoRecord",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_InitBaiBaoRecord"));
	m_pCmdMgr->Register("NS_SingleBaiBaoRecord",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing),	_T("NS_SingleBaiBaoRecord"));
}

void WorldFrame::UnRegisterALLRecvCmd()
{
	// 属性和状态
	m_pCmdMgr->UnRegister("NS_SetState",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UnSetState",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SetRoleState",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UnSetRoleState",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Att",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Skill",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_CompleteQuest",		(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_IncompleteQuest",	(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Item",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Suit",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_ItemCDTime",			(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleInitState_Money",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRemoteRoleState",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRemoteCreatureState",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleAttChangeSingle",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleAttChangeMutiple",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RemoteAttChangeSingle",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RemoteAttChangeMutiple",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleExpChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleLevelChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleBidAttPoint",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleClearAttPoint",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleAttPointAddChange",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SendFriendBlackList",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RemoveRemote",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 名字和ID
	m_pCmdMgr->UnRegister("NS_RoleGetID",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleGetName",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetNamebyNameID",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 移动
	m_pCmdMgr->UnRegister("NS_SyncWalk",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SyncJump",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SyncDrop",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SyncVDrop",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SyncSlide",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SyncStand",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_MoveFailed",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvMoveFailed));
	m_pCmdMgr->UnRegister("NS_HitFly",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_MoveSpeedChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 聊天
	m_pCmdMgr->UnRegister("NS_RoleChat",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvRoleChat));

	// 战斗
	m_pCmdMgr->UnRegister("NS_Skill",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SkillInterrupt",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UseItem",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UseItemInterrupt",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_HitTarget",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleHPChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleDead",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleRevive",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleReviveNotify",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_AddRoleBuff",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RemoveRoleBuff",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UpdateRoleBuff",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StopAction",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleStyleAction",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_MonsterEnterCombat",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 生产
	m_pCmdMgr->UnRegister("NS_Produce",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_Decomposition",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ConsolidatePosy",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ConsolidateEngrave",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ConsolidateQuench",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_Inlay",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_Brand",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_LoongSoul",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_Chisel",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_DyeFashion",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 金钱和元宝
	m_pCmdMgr->UnRegister("NS_BagSilver",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_WareSilver",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_BagYuanBao",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_BaiBaoYuanBao",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 交易
	m_pCmdMgr->UnRegister("NS_ExchangeReq",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ExchangeReqRes",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ExchangeAdd_2Src",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ExchangeAdd_2Dst",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ExchangeDec",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ExchangeMoney",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ExchangeLock",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ExchangeCancel",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ExchangeFinish",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 职能NPC
	m_pCmdMgr->UnRegister("NS_Dak",									(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_WareExtend",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_BagExtend",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SaveSilver",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetSilver",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SaveYuanBao",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetYuanBao",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_Abrase",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 组队
	m_pCmdMgr->UnRegister("NS_InviteToLeader",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_InviteJoinTeam",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_InviteReply",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_KickMember",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_LeaveTeam",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SetPickMode",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ChangeLeader",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleStateToTeam",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RolePosToTeam",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 物品和装备
	m_pCmdMgr->UnRegister("NS_Equip",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_Unequip",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SwapWeapon",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_AvatarEquipChange",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_IdentifyEquip",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_EquipChange",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SuitEffect",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SuitNum",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ItemPosChange",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ItemPosChangeEx",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ItemAdd",								(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_NewItemAdd",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_NewEquipAdd",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ItemRemove",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ItemCDUpdate",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 掉落
	m_pCmdMgr->UnRegister("NS_SyncGroundItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleGroundItemDisappear",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_PutDownItem",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RolePutDownItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RolePickUpItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetGroundItemInfo",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 地图
	m_pCmdMgr->UnRegister("NS_EnterInstance",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GotoNewMap",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvGotoMap));
	m_pCmdMgr->UnRegister("NS_InstanceNofity",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_InstanceAgree",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_InstanceComplete",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SyncInstanceTime",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_BindRebornMap",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// PK
	m_pCmdMgr->UnRegister("NS_SafeGuard",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_PK",									(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RolePKStateChange",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 商店
	m_pCmdMgr->UnRegister("NS_GetShopItems",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetShopEquips",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_BuyShopItem",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_BuyShopEquip",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_FeedbackFromShop",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 展示物品和装备
	m_pCmdMgr->UnRegister("NS_RoleShowEquip",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleShowItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 社会关系
	m_pCmdMgr->UnRegister("NS_LoginToFriend",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_LogoutToFriend",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleMakeFriend",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleCancelFriend",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UpdateFriGroup",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_MoveToBlackList",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_DeleteBlackList",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SendGiftToFri",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SendGiftToSender",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SendGiftBroadcast",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SendBlackList",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UpdateFriState",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_MakeFriNotice",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UpdateFriValue",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 摆摊
	m_pCmdMgr->UnRegister("NS_StallStart",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallSetGoods",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallUnsetGoods",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallSetTitle",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallSetAdText",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallSetAdFlag",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallSetFinish",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallClose",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallBuyRefresh",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallSetRefresh",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallUnsetRefresh",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallGet",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallGetTitle",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallBuy",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_StallGetSpec",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 天资及技能
	m_pCmdMgr->UnRegister("NS_LearnSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_AddSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_LevelUpSkill",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UpdateSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UpdateSkillCoolDown",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ForgetSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ClearTalent",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RemoveSkill",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_AddTalent",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RemoveTalent",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_UpdateTalent",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 任务
	m_pCmdMgr->UnRegister("NS_AcceptQuest",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_AddQuest",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_CompleteQuest",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_DeleteQuest",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_QuestUpdateKillCreature",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_QuestUpdateItem",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_QuestUpdateNPCTalk",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_QuestUpdateTrigger",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 声望
	m_pCmdMgr->UnRegister("NS_GetReputation",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 反外挂
	m_pCmdMgr->UnRegister("NS_GameGuarder",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 名人堂
	m_pCmdMgr->UnRegister("NS_GetFameHallRoles",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetReputeTop",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetActClanTreasure",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_ActiveTreasure",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_NewActivedTreasure",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_TreasureActCountChange",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_BecomeFame",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 角色名贴
	m_pCmdMgr->UnRegister("NS_GetRoleVCard",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SetRoleVCard",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleHeadPicURL",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 称号
	m_pCmdMgr->UnRegister("NS_ActiveRoleTitle",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetRoleTitles",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_NewTitles",							(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_RoleTitleChangeBroadcast",			(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 帮派
	m_pCmdMgr->UnRegister("NS_GuildCreateBroad",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildDismissBroad",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildJoinReq",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildJoinReqRes",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildJoinBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildLeaveBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildKickBroad",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildTurnoverBroad",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildDemissBroad",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildAppointBroad",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildTenetChangeBroad",				(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildProcFailed",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildGetAllMembers",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildGetMemberEx",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GuildRefMember",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetGuildName",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_GetGuildTenet",						(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));

	// 百宝袋
	m_pCmdMgr->UnRegister("NS_InitBaiBaoRecord",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
	m_pCmdMgr->UnRegister("NS_SingleBaiBaoRecord",					(NETMSGPROC)m_Trunk.sfp2(&WorldFrame::NetRecvDoNothing));
}

//-----------------------------------------------------------------------------
// 发送聊天消息
//-----------------------------------------------------------------------------
void WorldFrame::SendChatMsg()
{
	if (m_pPlayer->GetChatStatus() == TRUE)
	{
		if (timeGetTime() > m_dwNextChatTime)
		{
			// 发送聊天消息
			TCHAR mess[256] = {0};
			tagNC_RoleChat* pSend = (tagNC_RoleChat*)mess;
			pSend->dwID			= m_pUtil->Crc32("NC_RoleChat");
			pSend->dwDestRoleID	= 0;							//对谁说 
			pSend->byChannel	= m_pPlayer->GetChatChannel();	//喊话频道

			// 说话内容
			tstring tmpChat;
			if (!g_pClientMgr->GetRandomChatContent(tmpChat))
			{
				tmpChat = _T("冲啊!\n");
			}
			_tcsncpy(pSend->szMsg, tmpChat.c_str(), MAX_CHAT_LEN);

			// 重新计算消息大小
			pSend->dwSize = sizeof(tagNC_RoleChat) +(_tcslen(pSend->szMsg))*sizeof(TCHAR);

			m_pWorldSession->Send(pSend, pSend->dwSize);
			//IMSG(tss.str().c_str());

			// 计算下一次聊天时间
			m_dwNextChatTime += (m_pPlayer->m_SendTime + m_pUtil->Rand()%1000);
		}
	}
}

//-----------------------------------------------------------------------------
// 发送移动消息
//-----------------------------------------------------------------------------
void WorldFrame::SendMoveMsg()
{
	if (m_pPlayer->GetMoveStatus() == TRUE)
	{
		if (timeGetTime() > m_dwNextMoveTime)
		{
			// 取得移动目的地
			m_pPlayer->GetNextWayPoint();

			// 发送移动消息
			tagNC_MouseWalk send;
			send.srcPos = m_pPlayer->m_srcPos;
			send.dstPos = m_pPlayer->m_desPos;
			send.bSwim	= false;

			m_pWorldSession->Send(&send, send.dwSize);

			// 计算下一次移动时间
			m_dwNextMoveTime += (m_pPlayer->m_SendTime*3 + m_pUtil->Rand()%2000);
		}
	}
}

//-----------------------------------------------------------------------------
// 下线
//-----------------------------------------------------------------------------
BOOL WorldFrame::ReturnGame()
{
	m_pWorldSession->Disconnect();
	if (m_pPlayer->ChangeFrame(FrameMgr::EF_LOGIN))
		return TRUE;
	else
		return FALSE;
}