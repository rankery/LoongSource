/////////////////////////////////////////////////////////////////////////////
// Program:     RTClient
// Name:        OpenAndCloseServerPage.cpp
// Created:     2009-03-20
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdTable.h"
#include "PageAssist.h"
#include "RTServerManager.h"
#include "data_define.h"
#include "UIWords.h"
#include "RTServerManager.h"
#include "OpenAndCloseServerPage.h"
#include "..\ServerDefine\msg_gm_tool_c.h"

//控件ID
enum
{
	eSendSystemCmd = 300,
	eListSystemCmd,
	eChangeMaxOnline,
	eReloadMall,

	SelectAllServers = 200,
	UnSelectAllServers,
	SelectAllBadServers,
	UnSelectAllBadServers,

	MultiRegionOpenService = 100,
	MultiRegionCloseService,
	
	ID_MUSIC_CTRL       = 50,
};


BEGIN_EVENT_TABLE(OpenAndCloseServerPage, wxPanel)
	EVT_BUTTON(MultiRegionOpenService, OpenAndCloseServerPage::OpenMultiRegionService)
	EVT_BUTTON(MultiRegionCloseService, OpenAndCloseServerPage::CloseMultiRegionService)
	EVT_BUTTON(SelectAllServers, OpenAndCloseServerPage::SelectAllServer)
	EVT_BUTTON(UnSelectAllServers, OpenAndCloseServerPage::UnSelectAllServer)
	EVT_BUTTON(SelectAllBadServers, OpenAndCloseServerPage::SelectAllBadServer)
	EVT_BUTTON(UnSelectAllBadServers, OpenAndCloseServerPage::UnSelectAllBadServer)
	EVT_BUTTON(eSendSystemCmd, OpenAndCloseServerPage::SendSystemCmd)
	EVT_BUTTON(eChangeMaxOnline, OpenAndCloseServerPage::ChangeMaxOnline)
	EVT_BUTTON(eReloadMall, OpenAndCloseServerPage::ReloadMall)
END_EVENT_TABLE()

OpenAndCloseServerPage::OpenAndCloseServerPage(wxBookCtrlBase* book, const wxString label) : label_(label),
											wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
											wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)

{
	//Data view
	wxSizer *sizeDataView = new wxBoxSizer(wxVERTICAL);

	//正常服务器列表
	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, w_GoodServerGroup);
	wxSizer* sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);
	servers_ = new wxDataViewCtrl(this, ID_MUSIC_CTRL);
	model_ = new MyListModel;
	servers_->AssociateModel(model_);
	model_->DecRef();
	
	servers_->AppendToggleColumn( w_Select, 0, wxDATAVIEW_CELL_ACTIVATABLE, 60 );
	servers_->AppendTextColumn( w_ServerName, 1, wxDATAVIEW_CELL_EDITABLE, 80 );
	servers_->AppendTextColumn( w_Section, 2, wxDATAVIEW_CELL_EDITABLE, 80 );
	servers_->AppendTextColumn( w_GameWorld, 3, wxDATAVIEW_CELL_EDITABLE, 80 );
	servers_->AppendTextColumn( wxT("IP"), 4, wxDATAVIEW_CELL_EDITABLE, 100 );
	servers_->AppendColumn( new wxDataViewColumn(w_ServerState, new wxDataViewTextRendererAttr, 5, 100) );

	MyButtonCustomRenderer *button_custom_renderer = new MyButtonCustomRenderer(wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_LEFT);
	wxDataViewColumn *column = new wxDataViewColumn(w_Actions, button_custom_renderer, 6, 60, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
	servers_->AppendColumn(column);

	button_custom_renderer = new MyButtonCustomRenderer(wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_LEFT);
	column = new wxDataViewColumn(w_Actions, button_custom_renderer, 7, 60, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
	servers_->AppendColumn(column);

	button_custom_renderer = new MyButtonCustomRenderer(wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_LEFT);
	column = new wxDataViewColumn(w_Actions, button_custom_renderer, 8, 100, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
	servers_->AppendColumn(column);

	servers_->AppendTextColumn(w_Online, 9);
	servers_->AppendTextColumn(w_MaxOnline, 10);

	sizerBox->Add(servers_, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizeDataView->Add(sizerBox, 1, wxGROW|wxALL, CONTROL_BORDER);

	//全选、取消全选所有正常服务器
	//box = new wxStaticBox(this, wxID_ANY, wxT(""));
	wxSizer *sizerBoxx = new wxBoxSizer(wxHORIZONTAL);

	server_all_select	= new wxButton(this, SelectAllServers, w_SelectAll);
	server_all_unselect	= new wxButton(this, UnSelectAllServers, w_UnSelectAll);

	sizerBoxx->Add(server_all_select, 0, wxALL, CONTROL_BORDER);
	sizerBoxx->Add(server_all_unselect, 0, wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerBoxx, 0, wxGROW|wxALL, CONTROL_BORDER);

	// 修改最大在线人数
	wxStaticBox* boxSetMaxNum = new wxStaticBox(this, wxID_ANY, w_MaxNum);
	wxSizer* sizerSetMax = new wxStaticBoxSizer(boxSetMaxNum, wxHORIZONTAL);

	sizerSetMax->Add(CreateSizerWithTextAndLabel(this, wxT(""), wxID_ANY, &m_MaxNum), 0, wxALL | wxGROW, CONTROL_BORDER);
	m_set_MaxNum_ok = new wxButton(this, eChangeMaxOnline, w_SetMaxOK);

	sizerSetMax->Add(m_set_MaxNum_ok, 0, wxGROW|wxALL, CONTROL_BORDER);
	sizerBoxx->Add(sizerSetMax, 0, wxGROW|wxALL, CONTROL_BORDER);
	
	//更新商城
	wxStaticBox* boxReloadMall = new wxStaticBox(this, wxID_ANY, w_UpdateMall);
	wxSizer* sizerReloadMall = new wxStaticBoxSizer(boxReloadMall, wxHORIZONTAL);

	m_Reload_Mall_ok = new wxButton(this, eReloadMall, w_UpdateMallOK);

	sizerReloadMall->Add(m_Reload_Mall_ok, 0, wxGROW|wxALL, CONTROL_BORDER);
	sizerBoxx->Add(sizerReloadMall, 0, wxGROW|wxALL, CONTROL_BORDER);

	//非正常服务器列表
	box = new wxStaticBox(this, wxID_ANY, w_BadServerGroup);
	sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);

	servers_abnormal = new wxDataViewCtrl(this, ID_MUSIC_CTRL);
	model_abnormal = new MyListModel;
	servers_abnormal->AssociateModel(model_abnormal);
	model_abnormal->DecRef();

	servers_abnormal->AppendToggleColumn( w_Select, 0, wxDATAVIEW_CELL_ACTIVATABLE, 60 );
	servers_abnormal->AppendTextColumn( w_ServerName, 1, wxDATAVIEW_CELL_EDITABLE, 80 );
	servers_abnormal->AppendTextColumn( w_Section, 2, wxDATAVIEW_CELL_EDITABLE, 80 );
	servers_abnormal->AppendTextColumn( w_GameWorld, 3, wxDATAVIEW_CELL_EDITABLE, 80 );
	servers_abnormal->AppendTextColumn( wxT("IP"), 4, wxDATAVIEW_CELL_EDITABLE, 100 );
	servers_abnormal->AppendColumn( new wxDataViewColumn(w_ServerState, new wxDataViewTextRendererAttr, 5, 100) );

	MyButtonCustomRenderer *button_custom_renderer2 = new MyButtonCustomRenderer(wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_LEFT);
	wxDataViewColumn *column2 = new wxDataViewColumn(w_Actions, button_custom_renderer2, 6, 60, wxALIGN_LEFT,
		wxDATAVIEW_COL_RESIZABLE );
	servers_abnormal->AppendColumn(column2);

	button_custom_renderer2 = new MyButtonCustomRenderer(wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_LEFT);
	column2 = new wxDataViewColumn(w_Actions, button_custom_renderer2, 7, 60, wxALIGN_LEFT,
		wxDATAVIEW_COL_RESIZABLE );
	servers_abnormal->AppendColumn(column2);

	button_custom_renderer2 = new MyButtonCustomRenderer(wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_LEFT);
	column2 = new wxDataViewColumn(w_Actions, button_custom_renderer2, 8, 100, wxALIGN_LEFT,
		wxDATAVIEW_COL_RESIZABLE );
	servers_abnormal->AppendColumn(column2);

	servers_abnormal->AppendTextColumn(w_Online, 9);
	servers_abnormal->AppendTextColumn(w_MaxOnline, 10);

	sizerBox->Add(servers_abnormal, 1, wxGROW|wxALL, CONTROL_BORDER);	
	sizeDataView->Add(sizerBox, 1, wxGROW|wxALL, CONTROL_BORDER);

	//全选、取消全选所有异常服务器
	//box = new wxStaticBox(this, wxID_ANY, wxT(""));
	wxSizer *sizerBoxxx = new wxBoxSizer(wxHORIZONTAL);

	badserver_all_select	= new wxButton(this, SelectAllBadServers, w_SelectAll);
	badserver_all_unselect	= new wxButton(this, UnSelectAllBadServers, w_UnSelectAll);

	sizerBoxxx->Add(badserver_all_select, 0, wxALL, CONTROL_BORDER);
	sizerBoxxx->Add(badserver_all_unselect, 0, wxALL, CONTROL_BORDER);
	//sizerBox->Add(sizerBoxxx, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerBoxxx, 0, wxGROW|wxALL, CONTROL_BORDER);

	//按钮（多区开关服）
	box = new wxStaticBox(this, wxID_ANY, "");
	sizerBox = new wxStaticBoxSizer(box, wxHORIZONTAL);

	wxStaticBox* boxOpenCloseSome = new wxStaticBox(this, wxID_ANY, w_OpenCloseSomeServer);
	wxSizer *sizerBoxOpenCloseSome = new wxStaticBoxSizer(boxOpenCloseSome, wxHORIZONTAL);

	multi_region_open_service	= new wxButton(this, MultiRegionOpenService, w_OpenSomeServer);
	multi_region_close_service	= new wxButton(this, MultiRegionCloseService, w_CloseSomeServer);

	sizerBoxOpenCloseSome->Add(multi_region_open_service, 0, wxALL, CONTROL_BORDER);
	sizerBoxOpenCloseSome->Add(multi_region_close_service, 0, wxALL, CONTROL_BORDER);

	//按钮（执行系统指令）
	//wxStaticBox* boxSendCmd = new wxStaticBox(this, wxID_ANY, w_SetSystemCmd);
	//wxSizer* sizerBoxSendCmd = new wxStaticBoxSizer(boxSendCmd, wxHORIZONTAL);


	//sizerBoxSendCmd->Add(CreateSizerWithTextAndLabel(this, w_SystemCmd, wxID_ANY, &m_cmd), 0, wxALL, CONTROL_BORDER);
	//m_set_cmd_ok = new wxButton(this, eSendSystemCmd, w_SendCmd);

	//sizerBoxSendCmd->Add(m_set_cmd_ok, 0, wxALL, CONTROL_BORDER);

	//sizerBox->Add(sizerBoxOpenCloseSome, 0, wxGROW|wxALL, CONTROL_BORDER);
	//sizerBox->Add(sizerBoxSendCmd, 0, wxGROW|wxALL, CONTROL_BORDER);


	wxStaticBox* boxSendCmd = new wxStaticBox(this, wxID_ANY, w_SetSystemCmd);
	wxSizer* sizerBoxSendCmd = new wxStaticBoxSizer(boxSendCmd, wxHORIZONTAL);

	wxString strings[g_nSystemCmdNum];
	for( int i=0; i<g_nSystemCmdNum; i++ )
	{
		//TCHAR szBuff[X_LONG_STRING] = {0};
		//LPCTSTR lpBuff;
		//lpBuff = g_CmdTable[i];
		//strings[i] = g_CmdTable[lpBuff];
		strings[i] = g_CmdTable[i];
	}
	sizerBoxSendCmd->Add(CreateSizerWithTextAndComboBox(this, eSendSystemCmd, w_SystemCmd, &m_combobox_system_cmd, g_nSystemCmdNum, strings), 0, wxGROW | wxALL, CONTROL_BORDER);
	m_set_cmd_ok = new wxButton(this, eSendSystemCmd, w_SendCmd);
	sizerBoxSendCmd->Add(m_set_cmd_ok, 0, wxGROW | wxALL, CONTROL_BORDER);

	sizerBox->Add(sizerBoxOpenCloseSome, 0, wxGROW|wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerBoxSendCmd, 0, wxGROW|wxALL, CONTROL_BORDER);

	//控件布局
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(sizeDataView, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizer->Add(sizerBox, 0, wxALL, CONTROL_BORDER);

	SetSizerAndFit(sizer);
}

void OpenAndCloseServerPage::update_ui(PageAssist *page_assist , BOOL is_static)
{
	if (page_assist != NULL)
	{
		page_assist_ = page_assist;
		std::list<OpenAndCloseServerPageData*>& data_list = page_assist->GetOpenAndCloseServerPageDtatList();
		model_->AddData(data_list, page_assist);
		std::list<OpenAndCloseServerPageData*>& data_list_abnormal = page_assist->GetAbnormalServerPageDtatList();
		model_abnormal->AddData(data_list_abnormal, page_assist);
	}
}

//多区开服
void OpenAndCloseServerPage::OpenMultiRegionService(wxCommandEvent& event)
{
	if (!sRTServerManager.VerifyGMPrivilege(Crc32("NRTC_Start"), EP_Pri_Rt_A))
	{
		wxMessageBox(w_NoAuthOpenSome, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
		return;
	}
	
	if (wxMessageBox(w_AskForOpen, w_OpenSomeServer, wxYES_NO|wxICON_WARNING, this) != wxYES)
	{
		return;
	}

	std::vector<OpenAndCloseServerPageData*>& datas = model_->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas.begin();
	for (; it != datas.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		if (data != NULL)
		{
			//是否被选中
			if (data->is_check)
			{
				if (page_assist_ != NULL)
				{
					//发送消息
					page_assist_->SendStartServerMessage(data, FALSE);
				}
				else
				{
					LOG("PageAssist 指针为空")
				}
			}
		}
		else
		{
			LOG("OpenAndCloseServerPageData 指针为空")
		}
	}

	std::vector<OpenAndCloseServerPageData*>& datas_abnormal = model_abnormal->GetDatas();
	it = datas_abnormal.begin();
	for (; it != datas_abnormal.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		if (data != NULL)
		{
			//是否被选中
			if (data->is_check)
			{
				if (page_assist_ != NULL)
				{
					//发送消息
					page_assist_->SendStartServerMessage(data, FALSE);
				}
				else
				{
					LOG("PageAssist 指针为空")
				}
			}
		}
		else
		{
			LOG("OpenAndCloseServerPageData 指针为空")
		}
	}
}

//多区关服
void OpenAndCloseServerPage::CloseMultiRegionService(wxCommandEvent& event)
{
	if (!sRTServerManager.VerifyGMPrivilege(Crc32("NRTC_Close"), EP_Pri_Rt_A))
	{
		wxMessageBox(w_NoAuthCloseSome, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
		return;
	}

	if (wxMessageBox(w_AskForClose, w_CloseSomeServer, wxYES_NO|wxICON_WARNING, this) != wxYES)
	{
		return;
	}

	std::vector<OpenAndCloseServerPageData*>& datas = model_->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas.begin();
	for (; it != datas.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		if (data != NULL)
		{
			//是否被选中
			if (data->is_check)
			{
				if (page_assist_ != NULL)
				{
					//发送消息
					page_assist_->SendCloseServerMessage(FALSE, data, FALSE);
				}
				else
				{
					LOG("PageAssist 指针为空")
				}
			}
		}
		else
		{
			LOG("OpenAndCloseServerPageData 指针为空")
		}
	}

	std::vector<OpenAndCloseServerPageData*>& datas_abnormal = model_abnormal->GetDatas();
	it = datas_abnormal.begin();
	for (; it != datas_abnormal.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		if (data != NULL)
		{
			//是否被选中
			if (data->is_check)
			{
				if (page_assist_ != NULL)
				{
					//发送消息
					page_assist_->SendCloseServerMessage(FALSE, data, FALSE);
				}
				else
				{
					LOG("PageAssist 指针为空")
				}
			}
		}
		else
		{
			LOG("OpenAndCloseServerPageData 指针为空")
		}
	}
}

// 全选正常服务器组
void OpenAndCloseServerPage::SelectAllServer(wxCommandEvent& event)
{
	std::vector<OpenAndCloseServerPageData*>& datas = model_->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas.begin();
	for (; it != datas.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		data->is_check = TRUE;
	}

	page_assist_->UpdateOpenAndCloseServerPage(TRUE);
}

// 取消全选正常服务器组
void OpenAndCloseServerPage::UnSelectAllServer(wxCommandEvent& event)
{
	std::vector<OpenAndCloseServerPageData*>& datas = model_->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas.begin();
	for (; it != datas.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		data->is_check = FALSE;
	}

	page_assist_->UpdateOpenAndCloseServerPage(TRUE);
}

// 全选异常服务器组
void OpenAndCloseServerPage::SelectAllBadServer(wxCommandEvent& event)
{
	std::vector<OpenAndCloseServerPageData*>& datas_abnormal = model_abnormal->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas_abnormal.begin();

	it = datas_abnormal.begin();

	for (; it != datas_abnormal.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		data->is_check = TRUE;
	}
	
	page_assist_->UpdateOpenAndCloseServerPage(TRUE);
	//page_assist_->UpdateOpenAndCloseServerPageDtatList()
}

// 取消全选异常服务器组
void OpenAndCloseServerPage::UnSelectAllBadServer(wxCommandEvent& event)
{
	std::vector<OpenAndCloseServerPageData*>& datas_abnormal = model_abnormal->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas_abnormal.begin();

	it = datas_abnormal.begin();

	for (; it != datas_abnormal.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		data->is_check = FALSE;
	}

	page_assist_->UpdateOpenAndCloseServerPage(TRUE);
}

// 根据权限判定界面中显示的按钮和页面
void OpenAndCloseServerPage::ShowButtons(bool bShow_1, bool bShow_2, bool bShow_3, bool bShow_4)
{
	if (bShow_1)
	{
		multi_region_close_service->Enable();
		multi_region_open_service->Enable();
	}
	else
	{
		multi_region_close_service->Disable();
		multi_region_open_service->Disable();
	}

	if (bShow_2)
	{
		m_combobox_system_cmd->Enable();
		m_set_cmd_ok->Enable();
	}
	else
	{
		m_combobox_system_cmd->Disable();
		m_set_cmd_ok->Disable();
	}

	if (bShow_3)
	{
		m_MaxNum->Enable();
		m_set_MaxNum_ok->Enable();
	}
	else
	{
		m_MaxNum->Disable();
		m_set_MaxNum_ok->Disable();
	}
	
	if (bShow_4)
	{
		m_Reload_Mall_ok->Enable();
	}
	else
	{
		m_Reload_Mall_ok->Disable();
	}
}

// 向指定服务器发送系统命令
void OpenAndCloseServerPage::SendSystemCmd(wxCommandEvent& event)
{
	wxString strTemp = m_combobox_system_cmd->GetValue();
	wxString strCmd;
	strCmd = g_CmdTable[strTemp.wx_str()];

	if (!sRTServerManager.VerifyGMPrivilege(Crc32("NRTC_SystemCmd"), EP_Pri_Rt_A))
	{
		wxMessageBox(w_NoAuthSend, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
		return;
	}

	if (wxMessageBox(w_AskForSend, w_SendCmd, wxYES_NO|wxICON_WARNING, this) != wxYES)
	{
		return;
	}

	std::vector<OpenAndCloseServerPageData*>& datas = model_->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas.begin();
	for (; it != datas.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		if (data != NULL)
		{
			//是否被选中
			if (data->is_check)
			{
				if (page_assist_ != NULL)
				{
					//发送消息
					page_assist_->SendSystemCmdMessage(data, FALSE, strCmd);
				}
				else
				{
					LOG("PageAssist 指针为空")
				}
			}
		}
		else
		{
			LOG("OpenAndCloseServerPageData 指针为空")
		}
	}

	std::vector<OpenAndCloseServerPageData*>& datas_abnormal = model_abnormal->GetDatas();
	it = datas_abnormal.begin();
	for (; it != datas_abnormal.end(); ++it)
	{
		OpenAndCloseServerPageData* data = (*it);
		if (data != NULL)
		{
			//是否被选中
			if (data->is_check)
			{
				if (page_assist_ != NULL)
				{
					//发送消息
					page_assist_->SendSystemCmdMessage(data, FALSE, strCmd);
				}
				else
				{
					LOG("PageAssist 指针为空")
				}
			}
		}
		else
		{
			LOG("OpenAndCloseServerPageData 指针为空")
		}
	}
}

// 更改最大在线人数
void OpenAndCloseServerPage::ChangeMaxOnline(wxCommandEvent& event)
{
	wxString strMaxNum = m_MaxNum->GetValue();

	if (!sRTServerManager.VerifyGMPrivilege(Crc32("NRTC_MaxNum"), EP_Pri_Rt_C))
	{
		wxMessageBox(w_NoAuthChangeMax, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
		return;
	}

	//if (wxMessageBox(w_AskForChangeMax, w_SendCmd, wxYES_NO|wxICON_WARNING, this) != wxYES)
	//{
	//	return;
	//}

	std::vector<OpenAndCloseServerPageData*>& datas = model_->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas.begin();
	for (int i = 0; it != datas.end();)
	{
		OpenAndCloseServerPageData* data = (*it);
		if (data != NULL)
		{
			//是否被选中
			if (data->is_check)
			{
				if (page_assist_ != NULL)
				{
					if( page_assist_->SendChangeMaxMessage(data, FALSE, strMaxNum) == FALSE)
						break;
				}
				else
				{
					LOG("PageAssist 指针为空")
				}
			}
		}
		else
		{
			LOG("OpenAndCloseServerPageData 指针为空")
		}

		i++;
		it = datas.begin()+i;
	}
}

//更新商城
void OpenAndCloseServerPage::ReloadMall(wxCommandEvent& event)
{
	if (!sRTServerManager.VerifyGMPrivilege(Crc32("NRTC_UpdateMall"), EP_Pri_Rt_D))
	{
		wxMessageBox(w_NoAuthUpdataMall, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
		return;
	}

	//if (wxMessageBox(w_AskForUpdateMall, w_DoUpdate, wxYES_NO|wxICON_WARNING, this) != wxYES)
	//	return;


	std::vector<OpenAndCloseServerPageData*>& datas = model_->GetDatas();
	std::vector<OpenAndCloseServerPageData*>::iterator it = datas.begin();

	for (int i = 0; it != datas.end(); )
	{
		OpenAndCloseServerPageData* data = (*it);

		if (data != NULL)
		{
			//是否被选中
			if (data->is_check)
			{
				if (page_assist_ != NULL)
					if (page_assist_->UpdateMall(data, FALSE) == FALSE)
						break;
				else
					LOG("PageAssist 指针为空")
			}
		}
		else
			LOG("OpenAndCloseServerPageData 指针为空")
		
		i++;
		it = datas.begin()+i;
	}
}


MyListModel::MyListModel() :wxDataViewVirtualListModel( 0 )
{
	
}

MyListModel::~MyListModel()
{
}

void MyListModel::AddData(std::list<OpenAndCloseServerPageData*>& data_list, PageAssist* page_assist)
{
	if (page_assist == NULL)
	{
		LOG("PageAssist 指针为空")
		return;
	}
	data_vector_.clear();
	int i = 0;
	std::list<OpenAndCloseServerPageData*>::iterator it = data_list.begin();
	for (; it != data_list.end(); ++it)
	{
		int n = (*it)->max_online;
		data_vector_.push_back(*it);
		i++;
	}
	
	Reset(i);

	page_assist_ = page_assist;
}

void MyListModel::GetValueByRow( wxVariant &variant,
								unsigned int row, unsigned int col ) const
{
	OpenAndCloseServerPageData* page_data = (OpenAndCloseServerPageData*)data_vector_[row];
	if (page_data == NULL)
	{
		LOG("OpenAndCloseServerPageData is null...")
		return;
	}
	page_data->page_assist = page_assist_;

	if (col==0)
	{
		variant = page_data->is_check;
	}
	else if (col == 1)
	{
		if(page_data->type == EST_Login)
		{
			variant = wxT("Login");
		}
		else if (page_data->type == EST_DB)
		{
			variant = wxT("DB");
		}
		else if (page_data->type == EST_World)
		{
			variant = wxT("World");
		}
	}
	else if(col == 2)
	{
		variant =page_data->section_name;
	}
	else if(col == 3)
	{
		variant = page_data->world_name;
	}
	else if(col == 4)
	{
		variant = page_data->ip;
	}
	else if(col == 5)
	{
		if (page_data->status == EWS_Well)
		{
			variant = wxT(w_WorkNormally);
		}
		else if(page_data->status == EWS_InitNotDone)
		{
			variant = wxT(w_NoInited);
		}
		else if(page_data->status == EWS_SystemError)
		{
			variant = wxT(w_BrokenDown);
		}
		else if(page_data->status == EWS_ProofError)
		{
			variant = wxT(w_ProofError);
		}
		else if(page_data->status == EWS_DBBlock)
			variant = wxT(w_DBBlock);
	}
	else if(col == 6)
	{
		//开启
		page_data->operate_name = w_Start;
		variant = (void*)page_data;
	}
	else if(col == 7)
	{
		//关闭
		page_data->operate_name = w_Close;
		variant = (void*)page_data;
	}
	else if(col == 8)
	{
		//强制关闭
		page_data->operate_name = w_CloseForced;
		variant = (void*)page_data;
	}
	else if(col == 9)
	{
		variant = (long)page_data->online_number;
	}
	else if(col == 10)
	{
		variant = (long)page_data->max_online;
	}
}

bool MyListModel::GetAttrByRow( unsigned int row, unsigned int col,
							   wxDataViewItemAttr &attr )
{
	OpenAndCloseServerPageData* page_data = (OpenAndCloseServerPageData*)data_vector_[row];
	if (page_data == NULL)
	{
		LOG("OpenAndCloseServerPageData is null...")
			return false;
	}
	// 状态
	if (col == 5)
	{
		//Login
		if (page_data->type == EST_Login)
		{
			if(page_data->status == EWS_Well)
			{
				attr.SetColour(*wxGREEN);
			}
			else if (page_data->status == EWS_InitNotDone)
			{
				attr.SetColour(*wxLIGHT_GREY);
			}
			else if (page_data->status == EWS_SystemError)
			{
				attr.SetColour(*wxRED);
			}
			else if (page_data->status == EWS_ProofError)
			{
				attr.SetColour(*wxBLUE);
			}
		}
		//DB
		if (page_data->type == EST_DB)
		{

			if(page_data->status == EWS_Well)
			{
				attr.SetColour(*wxGREEN);
				
			}
			else if (page_data->status == EWS_InitNotDone)
			{
				attr.SetColour(*wxLIGHT_GREY);
				
			}
			else if (page_data->status == EWS_SystemError)
			{
				attr.SetColour(*wxRED);
				
			}
			else if(page_data->status == EWS_DBBlock)
			{
				attr.SetColour(wxColour(150,150,0));
			}
		}
		//World
		if (page_data->type == EST_World)
		{
			
			if(page_data->status == EWS_Well)
			{
				attr.SetColour(*wxGREEN);
			}
			else if (page_data->status == EWS_InitNotDone)
			{
				attr.SetColour(*wxLIGHT_GREY);
			}
			else if (page_data->status == EWS_SystemError)
			{
				attr.SetColour(*wxRED);
			}
		}
	}
	return true;
}

bool MyListModel::SetValueByRow( const wxVariant &variant,
								unsigned int row, unsigned int col )
{
	OpenAndCloseServerPageData* page_data = (OpenAndCloseServerPageData*)data_vector_[row];
	if (page_data == NULL)
	{
		LOG("OpenAndCloseServerPageData is null...")
			return false;
	}
	if (col == 0)
	{
		page_data->is_check = (int)variant.GetBool();
		return true;
	}
	
	return false;
}

bool MyButtonCustomRenderer::Activate(wxRect WXUNUSED(cell), wxDataViewModel *model, const wxDataViewItem & item, unsigned int col)
{
	/*if (data_ != NULL)
	{
		wxString title = data_->region_name + wxT("----") + data_->world_name;
		LoongWorldDialog dlg(title, data_);
		dlg.ShowModal();
	}*/
	if (data_ != NULL)
	{
		if (data_->page_assist != NULL)
		{
			if (name_ == w_Start)
			{
				data_->page_assist->SendStartServerMessage(data_, TRUE);
			}
			else if (name_ == w_Close)
			{
				data_->page_assist->SendCloseServerMessage(FALSE, data_, TRUE);
			}
			else if (name_ == w_CloseForced)
			{
				data_->page_assist->SendCloseServerMessage(TRUE, data_, TRUE);
			}
		}
	}
	return false;
}

bool MyButtonCustomRenderer::SetValue( const wxVariant &value )
{
	
	if(value.GetType() == wxT("void*") )
	{
		data_ = (OpenAndCloseServerPageData*)value.GetVoidPtr();
		if (data_ != NULL)
		{
			name_ = data_->operate_name;
		}
		else
		{
			LOG("OpenAndCloseServerPageData pointer is null...")
		}
		return true;
	}
	return true;
}

bool MyButtonCustomRenderer::Render(wxRect rect, wxDC *dc, int WXUNUSED(state))
{
	int width = name_.length() * 20;
	dc->SetPen( *wxGREEN_PEN );
	wxRect rect1;
	rect1.x = rect.x;
	rect1.width = width;
	rect1.y = rect.y;
	rect1.height = 18;

	wxRendererNative::Get().DrawPushButton(
		GetOwner()->GetOwner(),
		*dc,
		rect1,
		wxCONTROL_FOCUSED );
	wxColour colour = dc->GetTextForeground();
	dc->SetTextForeground(*wxBLACK);

	dc->DrawText(name_,rect1.GetX() + 10, rect1.GetY() );
	dc->SetTextForeground(colour);
	return true;
}
