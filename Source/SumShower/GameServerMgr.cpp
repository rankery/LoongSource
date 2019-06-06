
#include "StdAfx.h"
#include "SumShower.h"
#include "GameServerMgr.h"
#include "RTServer.h"
#include "PageAssist.h"
#include "../ServerDefine/msg_rt_c.h"
#include "../ServerDefine/msg_rt_s.h"
#include "../ServerDefine/msg_gm_tool_c.h"
#include "data_define.h"
#include "UIWords.h"

GameServerMgr::GameServerMgr()
{
	LOG("GameServerMgr构造")
	thread_update_terminate_ = FALSE;
	thread_update_handle_ = 0;
	rt_server_ = 0;
}

GameServerMgr::~GameServerMgr()
{
	LOG("GameServerMgr析构")
	Destroy();
}

bool GameServerMgr::Init()
{
	LOG("GameServerMgr初始化")
	//读取配置文件
	if (ReadConfigFile() == false)
	{
		LOG("读取配置文件失败")
		return false;
	}
	else
	{
		LOG("读取配置文件成功")
	}

	//RTserver列表初始化
	if (rt_server_ != NULL)
	{
		if (rt_server_->Init() == false)
		{
			LOG("RTServer初始化失败")
		}
		else
		{
			LOG("RTServer初始化成功")
		}
	}
	else
	{
		LOG("RTserver指针为空")
	}
	
	return true;
}

void GameServerMgr::Destroy()
{
	//结束Connect线程
	::InterlockedExchange((LONG*)&thread_update_terminate_, TRUE);
	DWORD result = WaitForSingleObject(thread_update_handle_, 5000);
	if (thread_update_handle_ != NULL)
	{
		CloseHandle(thread_update_handle_);
	}
	
	if (rt_server_ != NULL)
	{
		delete rt_server_;
		rt_server_ = NULL;
	}
}

void GameServerMgr::ConnectRTServer()
{
	//连接GMServer
	if (!P_VALID(rt_server_))
	{
		return;
	}
	rt_server_->SetFirstConnected(false);

	while (true)
	{
		if (rt_server_->IsConnected())
		{
			//发送第一次连接消息
			UserInfo* user_info = NULL;
			user_info = rt_server_->GetPageAssist()->GetUserInfo();
			if (user_info != NULL)
			{
				tagNRTC_Challenge send;
				wxStringToTCHAR(user_info->name,  send.szAccountName,  X_SHORT_NAME);
				TCHAR buff[X_SHORT_NAME];
				wxStringToTCHAR(user_info->password,  buff,  X_SHORT_NAME);
				send.dwPsdCrc = Crc32(buff);
				rt_server_->SendMsg(&send, send.dwSize);
				LOG("Send NRTS_Challenge...")
				rt_server_->SetFirstConnected(false);
			}
			break;
		}

		if (rt_server_->IsTryingToConnent())
			continue;

		//连接失败
		if (rt_server_->IsFirstConnected() == true)
		{
			wxBusyInfo info(w_ConnectFailed, g_pLoginDialog);
			g_pLoginDialog->Suspend();
			break;
		}

		//尝试连接
		rt_server_->Connected();
		rt_server_->SetFirstConnected(true);
	}
}

void GameServerMgr::QueryServerInfo()
{
	

}

void GameServerMgr::SetPageAssistPtr(PageAssist *page_assist)
{
	if (page_assist == NULL)
	{
		LOG("PageAssist pointer is NULL...")
		return;
	}

	//设置GameServerMgr中的PageAssist指针
	page_assist_ = page_assist;
	rt_server_->SetPageAssist(page_assist);
}

void GameServerMgr::ReadRtServer(TiXmlElement* element)
{
	rt_server_ = new RTServer;
	
	TiXmlElement *child_element = element->FirstChildElement();
	for (child_element; child_element; child_element = child_element->NextSiblingElement())
	{
		const char* key = child_element->Value();
		const char* text = child_element->GetText();
		if (key && text)
		{
			if (strcmp(key, "IP") == 0)
			{
				//IP
				rt_server_->SetIP(text);
				continue;
			}
			if (strcmp(key, "Port") == 0)
			{
				//PORT
				rt_server_->SetPort(atoi(text));
				continue;
			}
		}
	}
}
bool GameServerMgr::ReadConfigFile()
{
	TiXmlDocument doc("SumShower_config.xml");
	if (doc.LoadFile() != true)
	{
		LOG("加载配置文件失败")
		return false;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *element;
	TiXmlHandle root(0);

	element = hDoc.FirstChildElement("Config").Element();
	if (!element)
	{
		LOG("配置文件Config不存在")
		return false;
	}
	root = TiXmlHandle(element);

	//block <RTServer>
	{
		element = root.FirstChild("RTServer").Element();
		while (element)
		{
			ReadRtServer(element);
			element = element->NextSiblingElement();
		}
	}
	return true;
}

BOOL GameServerMgr::IsConnected()
{
	if (rt_server_ != NULL)
	{
		return rt_server_->IsConnected();
	}
	else
	{
		return FALSE;
	}
}

bool GameServerMgr::IsInitSuccess()
{
	if (rt_server_ != NULL)
	{
		return rt_server_->IsInitSuccess();
	}
	else
	{
		return false;
	}
}

UINT GameServerMgr::ThreadUpdate(LPVOID param)
{
	GameServerMgr *rt_server_manager = (GameServerMgr*)param;
	if (rt_server_manager == NULL)
	{
		return 0;
	}
	RTServer* rt_server = rt_server_manager->GetRtServerPtr();
	if (rt_server == NULL)
	{
		return 0;
	}
	while (true)
	{
		//检测线程是否终止
		if (rt_server_manager->thread_update_terminate_ == TRUE)
		{
			break;
		}

		if (rt_server == NULL)
		{
			break;
		}
		//已经连上
		if (rt_server->IsConnected() == TRUE)
		{
			//接收并处理消息
			rt_server->RecvMessage();
		}
		else
		{
			//断开连接，返回登录界面
			if (g_pMainFrame->IsShownOnScreen())
			{
				wxMessageBox(w_BadConnection, w_Error);
				rt_server->Disconnect();
				g_pMainFrame->GetPageAssistPtr()->ClearShowPageDtatList();
				g_pMainFrame->Hide();
				g_pLoginDialog->CenterOnScreen();
				g_pLoginDialog->Show();
			}
		}
		Sleep(100);	
	}
	return 0;
}

//void GameServerMgr::SendStartServerMessage(DWORD section_id, DWORD world_id, EServerType server_type)
//{
//	if (rt_server_ != NULL)
//	{
//		tagNRTC_Start send;
//		send.dwSectionID = section_id;
//		send.dwWorldID = world_id;
//		send.eType = server_type;
//
//		rt_server_->SendMsg(&send,  send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}
//
//void GameServerMgr::SendCloseServerMessage(BOOL is_force, DWORD section_id, DWORD world_id, EServerType server_type)
//{
//	if (rt_server_ != NULL)
//	{
//		tagNRTC_Close send;
//		send.dwSectionID = section_id;
//		send.dwWorldID = world_id;
//		send.eType = server_type;
//		send.bForce = is_force;
//		rt_server_->SendMsg(&send,  send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}
//
//void GameServerMgr::SendSystemCmdMessage(DWORD section_id, DWORD world_id, EServerType server_type, LPCTSTR szCmd)
//{
//	if (rt_server_ != NULL)
//	{
//		tagNRTC_SystemCmd send;
//		send.dwSectionID = section_id;
//		send.dwWorldID = world_id;
//		send.eType = server_type;
//		wcscpy(send.szCmd, szCmd);
//		rt_server_->SendMsg(&send,  send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}
//
//void GameServerMgr::SendChangeMaxMessage(DWORD section_id, DWORD world_id, LPCTSTR szCmd)
//{
//	if (rt_server_ != NULL)
//	{
//		tagNRTC_MaxNum send;
//		send.dwSectionID = section_id;
//		send.dwWorldID = world_id;
//		send.eType = EST_World;
//		send.nMaxNum = _wtoi(szCmd);
//		rt_server_->SendMsg(&send,  send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}
//
//void GameServerMgr::SendCreateGMMessage( LPCTSTR szName, DWORD dwPswCRC, EPrivilege ePrivilege )
//{
//	if (rt_server_ != NULL)
//	{
//		tagNGMTC_GMCreate send;
//		_tcsncpy(send.szName, szName, X_SHORT_NAME);
//		send.dwPswCrc = dwPswCRC;
//		send.ePrivilege = ePrivilege;
//		rt_server_->SendMsg(&send,  send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}
//
//void GameServerMgr::SendModifyGMMessage( LPCTSTR szName, DWORD dwIP )
//{
//	if (rt_server_ != NULL)
//	{
//		tagNRTC_BindIP send;
//		_tcsncpy(send.szAccountName, szName, X_SHORT_NAME);
//		send.dwIP = dwIP;
//		rt_server_->SendMsg(&send,  send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}
//
//void GameServerMgr::SendDeleteGMMessage( LPCTSTR szName )
//{
//	if (rt_server_ != NULL)
//	{
//		tagNGMTC_GMDelete send;
//		_tcsncpy(send.szName, szName, X_SHORT_NAME);
//		rt_server_->SendMsg(&send,  send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}
//
//void GameServerMgr::SendQueryGMMessage()
//{
//	if (rt_server_ != NULL)
//	{
//		tagNRTC_GMQuery send;
//		rt_server_->SendMsg(&send, send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}

bool GameServerMgr::VerifyGMPrivilege( DWORD dwMsgCrc, EPrivilege ePrivilege )
{
	if (rt_server_ != NULL)
	{
		return rt_server_->VerifyGMPrivilege(dwMsgCrc, ePrivilege);
	}
	
	return false;
}

DWORD GameServerMgr::IsIPValid( LPSTR szIP )
{
	if (rt_server_ != NULL)
	{
		return rt_server_->IsIPValid(szIP);
	}

	return INADDR_NONE;
}

void GameServerMgr::HandleMessages()
{
	//启动update线程
	thread_update_handle_ = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)&GameServerMgr::ThreadUpdate, this, 0, NULL);
	if (thread_update_handle_ == NULL)
	{
		CloseHandle(thread_update_handle_);
		LOG("Create update thread error!")
			thread_update_handle_ = 0;
	}
	else
	{
		LOG("Update thread start.......")
	}
}

//void GameServerMgr::SendUpdateMallMessage(DWORD dwSectionID, DWORD dwWorldID)
//{
//	if (rt_server_ != NULL)
//	{
//		tagNRTC_UpdateMall send;
//		send.dwSectionID = dwSectionID;
//		send.dwWorldID = dwWorldID;
//		send.eType = EST_World;
//		rt_server_->SendMsg(&send,  send.dwSize);
//	}
//	else
//	{
//		LOG("RTServer 指针为空")
//	}
//}