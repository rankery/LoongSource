#include "StdAfx.h"
#include "PageAssist.h"
#include "OpenAndCloseServerPage.h"
#include "data_define.h"
#include "RTServerManager.h"
#include "RTClient.h"
#include "UIWords.h"

PageAssist::PageAssist()
{
	user_info_ = new UserInfo;
}

PageAssist::~PageAssist()
{
	std::list<OpenAndCloseServerPageData*>::iterator it = openandcloseserver_page_data_list_.begin();
	for (; it != openandcloseserver_page_data_list_.end(); ++it)
	{
		OpenAndCloseServerPageData* data = *it;
		delete data;
		data = NULL;
	}
	if (user_info_ != NULL)
	{
		delete user_info_;
		user_info_ = NULL;
	}
}

void PageAssist::UpdateOpenAndCloseServerPage( BOOL is_static)
{
	if (OpenAndCloseServerPage_fn != NULL)
	{
		(page_->*OpenAndCloseServerPage_fn)(this, is_static);
	}
}

void PageAssist::SetUserInfo(const UserInfo *user_info)
{
	if (user_info != NULL && user_info_ != NULL)
	{
		user_info_->name = user_info->name;
		user_info_->password = user_info->password;
		user_info_->privilege = user_info->privilege;
	}
}

void PageAssist::AddOpenAndCloseServerPageDtatList(int index, EServerType type, wxString section_name,  DWORD section_id, 
												   wxString world_name,   DWORD world_id, wxString ip)
{
	OpenAndCloseServerPageData * data = new OpenAndCloseServerPageData;
	data->index = index;
	data->type = type;
	data->section_name = section_name;
	data->section_id = section_id;
	data->world_name = world_name;
	data->world_id = world_id;
	data->ip = ip;

	std::list<OpenAndCloseServerPageData*>::iterator it = page_data_list_abnormal.begin();
	for (; it!=page_data_list_abnormal.end(); it++)
	{
		OpenAndCloseServerPageData* pData = (OpenAndCloseServerPageData*)(*it);
		if (pData->type == type && pData->section_id == section_id && pData->world_id == world_id)
		{
			page_data_list_abnormal.erase(it);
			break;
		}
	}
	page_data_list_abnormal.push_back(data);
}

void PageAssist::UpdateOpenAndCloseServerPageDtatList(SectionInfo* section_info)
{
	if (section_info == NULL)
	{
		LOG("SectionInfo 指针为空")
		return;
	}
	std::map<DWORD, GameWorldInfo>& info = section_info->game_world_infos;
	std::map<DWORD, GameWorldInfo>::iterator it2;

	//取得权限
	EPrivilege ePri = (EPrivilege)(GetUserInfo()->privilege);

	//遍历正常服务器数据列表
	std::list<OpenAndCloseServerPageData*>::iterator it = openandcloseserver_page_data_list_.begin();
	for (; it != openandcloseserver_page_data_list_.end(); ++it)
	{
		OpenAndCloseServerPageData* data = *it;
		if (data  == NULL)
		{
			LOG("OpenAndCloseServerPageData 指针为空")
			continue;
		}

		if (data->section_id == section_info->id)
		{
			//大区相同
			if(data->type == EST_Login)
			{
				//更新Login状态
				int PrevState = data->status;
				data->status = section_info->login_status;
				if (data->status == EWS_InitNotDone || data->status == EWS_SystemError) // 如果状态有良好变为故障或者未初始化，则报警
				{
					if (PrevState == EWS_Well)
					{
						// TODO: Add your command handler code here
						MCI_OPEN_PARMS mciOpenParms;
						MCI_PLAY_PARMS PlayParms;
						mciOpenParms.dwCallback=0;
						mciOpenParms.lpstrElementName=_T("error.wav");
						mciOpenParms.wDeviceID=0;
						mciOpenParms.lpstrDeviceType=_T("waveaudio");
						mciOpenParms.lpstrAlias=_T(" ");
						PlayParms.dwCallback=0;
						PlayParms.dwTo=0;
						PlayParms.dwFrom=0;
						mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_TYPE|MCI_OPEN_ELEMENT,(DWORD)(LPVOID)&mciOpenParms);//打开音频设备；
						mciSendCommand(mciOpenParms.wDeviceID,MCI_PLAY,MCI_WAIT,(DWORD)(LPVOID)&PlayParms);//播放WAVE声音文件；
						mciSendCommand(mciOpenParms.wDeviceID,MCI_CLOSE,NULL,NULL);//关闭音频设备；
		
					}
				}
				if (ePri == EP_Pri_Rt_A || ePri == EP_Pri_Rt_C)
				{
					data->online_number = section_info->nPlayerOnline;
					data->max_online = section_info->nMaxPlayerNum;
				}
			}
			else if (data->type == EST_World)
			{
				//更新World状态
				it2 = info.find(data->world_id);
				if (it2 != info.end())
				{
					//找到相同world
					GameWorldInfo info = it2->second;
					//更新world状态，和在线人数和最大人数
					int PrevState = data->status;
					data->status = info.world_status;
					if (data->status == EWS_InitNotDone || data->status == EWS_SystemError) // 如果状态有良好变为故障或者未初始化，则报警
					{
						if (PrevState == EWS_Well)
						{
							MCI_OPEN_PARMS mciOpenParms;
							MCI_PLAY_PARMS PlayParms;
							mciOpenParms.dwCallback=0;
							mciOpenParms.lpstrElementName=_T("error.wav");
							mciOpenParms.wDeviceID=0;
							mciOpenParms.lpstrDeviceType=_T("waveaudio");
							mciOpenParms.lpstrAlias=_T(" ");
							PlayParms.dwCallback=0;
							PlayParms.dwTo=0;
							PlayParms.dwFrom=0;
							mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_TYPE|MCI_OPEN_ELEMENT,(DWORD)(LPVOID)&mciOpenParms);//打开音频设备；
							mciSendCommand(mciOpenParms.wDeviceID,MCI_PLAY,MCI_WAIT,(DWORD)(LPVOID)&PlayParms);//播放WAVE声音文件；
							mciSendCommand(mciOpenParms.wDeviceID,MCI_CLOSE,NULL,NULL);//关闭音频设备；
						}
					}
					if (ePri == EP_Pri_Rt_A || ePri == EP_Pri_Rt_C)
					{
						data->online_number = info.nPlayerOnline;
						data->max_online = info.nMaxPlayerNum;
					}
				}
				else
				{
					LOG("world " << data->world_name <<"没找到")
					continue;
				}
			}
			else if (data->type == EST_DB)
			{
				//更新DB状态
				it2 = info.find(data->world_id);
				if (it2 != info.end())
				{
					//找到相同world
					GameWorldInfo info = it2->second;
					//更新db状态
					int PrevState = data->status;
					data->status = info.db_status;
					if (data->status == EWS_InitNotDone || data->status == EWS_SystemError || data->status == EWS_DBBlock ) // 如果状态有良好变为故障或者未初始化，则报警
					{
						if (PrevState == EWS_Well)
						{
							MCI_OPEN_PARMS mciOpenParms;
							MCI_PLAY_PARMS PlayParms;
							mciOpenParms.dwCallback=0;
							mciOpenParms.lpstrElementName=_T("error.wav");
							mciOpenParms.wDeviceID=0;
							mciOpenParms.lpstrDeviceType=_T("waveaudio");
							mciOpenParms.lpstrAlias=_T(" ");
							PlayParms.dwCallback=0;
							PlayParms.dwTo=0;
							PlayParms.dwFrom=0;
							mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_TYPE|MCI_OPEN_ELEMENT,(DWORD)(LPVOID)&mciOpenParms);//打开音频设备；
							mciSendCommand(mciOpenParms.wDeviceID,MCI_PLAY,MCI_WAIT,(DWORD)(LPVOID)&PlayParms);//播放WAVE声音文件；
							mciSendCommand(mciOpenParms.wDeviceID,MCI_CLOSE,NULL,NULL);//关闭音频设备；
						}
					}
				}
				else
				{
					LOG("world " << data->world_name <<"没找到")
						continue;
				}
			}
			if (data->status != EWS_Well)
			{
				SwitchDatalist(data);
			}
		}
	}

	//遍历非正常服务器数据列表
	it = page_data_list_abnormal.begin();
	for (; it != page_data_list_abnormal.end(); ++it)
	{
		OpenAndCloseServerPageData* data = *it;
		if (data  == NULL)
		{
			LOG("OpenAndCloseServerPageData 指针为空")
				continue;
		}

		if (data->section_id == section_info->id)
		{
			//大区相同
			if(data->type == EST_Login)
			{
				//更新Login状态
				data->status = section_info->login_status;
				if (ePri == EP_Pri_Rt_A || ePri == EP_Pri_Rt_C)
				{
					data->online_number = section_info->nPlayerOnline;
					data->max_online = section_info->nMaxPlayerNum;
				}
			}
			else if (data->type == EST_World)
			{
				//更新World状态
				it2 = info.find(data->world_id);
				if (it2 != info.end())
				{
					//找到相同world
					GameWorldInfo info = it2->second;
					//更新world状态，和在线人数和最大人数
					data->status = info.world_status;
					if (ePri == EP_Pri_Rt_A || ePri == EP_Pri_Rt_C)
					{
						data->online_number = info.nPlayerOnline;
						data->max_online = info.nMaxPlayerNum;
					}
				}
				else
				{
					LOG("world " << data->world_name <<"没找到")
						continue;
				}
			}
			else if (data->type == EST_DB)
			{
				//更新DB状态
				it2 = info.find(data->world_id);
				if (it2 != info.end())
				{
					//找到相同world
					GameWorldInfo info = it2->second;
					//更新db状态
					data->status = info.db_status;
				}
				else
				{
					LOG("world " << data->world_name <<"没找到")
						continue;
				}
			}
			if (data->status == EWS_Well)
			{
				SwitchDatalist(data);
			}
		}
	}

	ResortDatalist();
}

void PageAssist::SendStartServerMessage(OpenAndCloseServerPageData* data, BOOL alarm)
{
	if (!data) return;

	if (data->status != EWS_InitNotDone)
		return;

	if (alarm)
	{
		wxString caption = w_ConfirmOpen;
		wxString content = wxT("");
		content << w_AskForOpenQ << data->section_name << wxT("-") << data->world_name << wxT("-");

		switch (data->type)
		{
		case EST_Login:
			content << wxT("LoginServer");
			break;

		case EST_DB:
			content << wxT("DBServer");
			break;

		case EST_World:
			content << wxT("WorldServer");
			break;
		}
		content << wxT("?");

		if (wxMessageBox(content, caption, wxYES_NO|wxICON_QUESTION) == wxYES)
		{
			sRTServerManager.SendStartServerMessage(data->section_id, data->world_id, data->type);
		}

	}
	else
	{
		sRTServerManager.SendStartServerMessage(data->section_id, data->world_id, data->type);
	}
}	

void PageAssist::SendCloseServerMessage(BOOL is_force, OpenAndCloseServerPageData* data, BOOL alarm)
{
	if (!data) return;

	if (data->status == EWS_InitNotDone)
		return;

	if (alarm)
	{
		wxString caption = w_ConfirmClose;
		wxString content = wxT("");

		content << w_AskForCloseQ;
		if (is_force)
		{
			content << w_Forced;
		}
		content << w_DoClose << data->section_name << wxT("-") << data->world_name << wxT("-");

		switch (data->type)
		{
		case EST_Login:
			content << wxT("LoginServer");
			break;

		case EST_DB:
			content << wxT("DBServer");
			break;

		case EST_World:
			content << wxT("WorldServer");
			break;
		}
		content << wxT("?");

		if (wxMessageBox(content, caption, wxYES_NO|wxICON_QUESTION) == wxYES)
		{
			sRTServerManager.SendCloseServerMessage(is_force, data->section_id, data->world_id, data->type);
		}
	}
	else
	{
		sRTServerManager.SendCloseServerMessage(is_force, data->section_id, data->world_id, data->type);
	}
}

void PageAssist::SwitchDatalist( OpenAndCloseServerPageData* server_data )
{
	temp_moving_data_list.push_back(server_data);

	bListChange = TRUE;
}

void PageAssist::ResortDatalist()
{
	if (!bListChange)
	{
		return;
	}

	std::list<OpenAndCloseServerPageData*>::iterator it = temp_moving_data_list.begin();
	for (; it != temp_moving_data_list.end(); it++)
	{
		OpenAndCloseServerPageData* data = *it;
		if (data->status == EWS_Well)
		{
			openandcloseserver_page_data_list_.push_back(data);
			page_data_list_abnormal.remove(data);
		}
		else
		{
			page_data_list_abnormal.push_back(data);
			openandcloseserver_page_data_list_.remove(data);
		}
	}

	openandcloseserver_page_data_list_.sort(CmpData());
	page_data_list_abnormal.sort(CmpData());

	temp_moving_data_list.clear();
	bListChange = FALSE;
}

void PageAssist::SendCreateGMMessage( LPCTSTR szName, LPCTSTR szPwd, EPrivilege ePrivilege )
{
	sRTServerManager.SendCreateGMMessage(szName, Crc32(szPwd), ePrivilege);
}

void PageAssist::SendModifyGMMessage( LPCTSTR szName, DWORD dwIP )
{
	sRTServerManager.SendModifyGMMessage(szName, dwIP);
}

void PageAssist::SendDeleteGMMessage( LPCTSTR szName )
{
	sRTServerManager.SendDeleteGMMessage(szName);
}

void PageAssist::SendQueryGMMessage()
{
	sRTServerManager.SendQueryGMMessage();
}

void PageAssist::SendSystemCmdMessage(OpenAndCloseServerPageData* data, BOOL alarm, LPCTSTR szCmd)
{
	if (!data) return;

	//if (data->status != EWS_Well)
	//	return;

	if (alarm)
	{
		wxString caption = w_ConfirmOpen;
		wxString content = wxT("");
		content << w_AskForSendQ << data->section_name << wxT("-") << data->world_name << wxT("-");

		switch (data->type)
		{
		case EST_Login:
			content << wxT("LoginServer");
			break;

		case EST_World:
			content << wxT("WorldServer");
			break;
		
		case EST_DB:
			content << wxT("DBServer");
			break;

		default:

			return;
		}
		content << wxT("?");

		if (wxMessageBox(content, caption, wxYES_NO|wxICON_QUESTION) == wxYES)
		{
			sRTServerManager.SendSystemCmdMessage(data->section_id, data->world_id, data->type, szCmd);
		}

	}
	else
	{
		sRTServerManager.SendSystemCmdMessage(data->section_id, data->world_id, data->type, szCmd);
	}
}

BOOL PageAssist::SendChangeMaxMessage(OpenAndCloseServerPageData *data, BOOL alarm, LPCTSTR szCmd)
{
	if (!data) return FALSE;

	if (data->status != EWS_Well)
		return FALSE;

	
	if( data->type != EST_World)
	{
		wxString caption = w_Warning4RType;
		wxString content = wxT("");
		if ( data->type == EST_Login ) 
			content << w_ErrorType << data->section_name << wxT("-Login") ;
		else if ( data->type == EST_DB )
			content << w_ErrorType << data->section_name << wxT("-") << data->world_name << wxT("-DB") ;
		wxMessageBox(content, caption, wxYES_NO|wxICON_QUESTION);

		return TRUE;
	}

	wxString caption = w_ConfirmChangeMax;
	if (wxMessageBox(w_AskForChangeMax, caption, wxYES_NO|wxICON_WARNING) != wxYES)
		return TRUE;

	if (alarm)
	{
		wxString caption = w_ConfirmChangeMax;
		wxString content = wxT("");
		content << w_AskForChangeMaxQ << data->section_name << wxT("-") << data->world_name << wxT("-");
		content << wxT("WorldServer");
		content << wxT("?");

		if (wxMessageBox(content, caption, wxYES_NO|wxICON_QUESTION) == wxYES)
		{
			sRTServerManager.SendChangeMaxMessage(data->section_id, data->world_id, szCmd);
		}

	}
	else
	{
		sRTServerManager.SendChangeMaxMessage(data->section_id, data->world_id, szCmd);
	}

	return TRUE;
}

BOOL PageAssist::UpdateMall(OpenAndCloseServerPageData* data, BOOL alarm)
{
	if (!data) return FALSE;

	if (data->status != EWS_Well)
		return FALSE;

	if( data->type != EST_World)
	{
		wxString caption = w_Warning4RType;
		wxString content = wxT("");
		if ( data->type == EST_Login ) 
			content << w_ErrorType << data->section_name << wxT("-Login") ;
		else if ( data->type == EST_DB )
			content << w_ErrorType << data->section_name << wxT("-") << data->world_name << wxT("-DB") ;
		wxMessageBox(content, caption, wxYES_NO|wxICON_QUESTION);

		return TRUE;
	}

	wxString caption = w_ConfirmUpdateMall;
	if (wxMessageBox(w_AskForUpdateMall, caption, wxYES_NO|wxICON_WARNING) != wxYES)
		return TRUE;

	if (alarm)
	{
		wxString caption = w_ConfirmUpdateMall;
		wxString content = wxT("");
		content << w_AskForUpdateMallQ << data->section_name << wxT("-") << data->world_name << wxT("-");
		content << wxT("WorldServer");
		content << wxT("?");

		if (wxMessageBox(content, caption, wxYES_NO|wxICON_QUESTION) == wxYES)
		{
			sRTServerManager.SendUpdateMallMessage(data->section_id, data->world_id);
		}

	}
	else
	{
		sRTServerManager.SendUpdateMallMessage(data->section_id, data->world_id);
	}

	return TRUE;
}