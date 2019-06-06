/////////////////////////////////////////////////////////////////////////////
// Program:     RTClient
// Name:        ShowPage.cpp
// Created:     2009-03-20
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdTable.h"
#include "PageAssist.h"
#include "GameServerMgr.h"
#include "data_define.h"
#include "UIWords.h"
#include "ShowPage.h"
#include "SumShower.h"
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


BEGIN_EVENT_TABLE(ShowPage, wxPanel)
	//EVT_BUTTON(MultiRegionOpenService, ShowPage::OpenMultiRegionService)
	//EVT_BUTTON(MultiRegionCloseService, ShowPage::CloseMultiRegionService)
	//EVT_BUTTON(SelectAllServers, ShowPage::SelectAllServer)
	//EVT_BUTTON(UnSelectAllServers, ShowPage::UnSelectAllServer)
	//EVT_BUTTON(SelectAllBadServers, ShowPage::SelectAllBadServer)
	//EVT_BUTTON(UnSelectAllBadServers, ShowPage::UnSelectAllBadServer)
	//EVT_BUTTON(eSendSystemCmd, ShowPage::SendSystemCmd)
	//EVT_BUTTON(eChangeMaxOnline, ShowPage::ChangeMaxOnline)
	//EVT_BUTTON(eReloadMall, ShowPage::ReloadMall)
END_EVENT_TABLE()

ShowPage::ShowPage(wxBookCtrlBase* book, const wxString label) : label_(label), dwSum_(0),
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
	
	//servers_->AppendToggleColumn( w_Select, 0, wxDATAVIEW_CELL_ACTIVATABLE, 60 );
	//servers_->AppendTextColumn( w_ServerType, 1, wxDATAVIEW_CELL_EDITABLE, 80 );
	servers_->AppendTextColumn( w_Section, 2, wxDATAVIEW_CELL_ACTIVATABLE, 80 );
	servers_->AppendTextColumn( w_GameWorld, 3, wxDATAVIEW_CELL_ACTIVATABLE, 80 );
	//servers_->AppendTextColumn( wxT("IP"), 4, wxDATAVIEW_CELL_EDITABLE, 100 );
	//servers_->AppendColumn( new wxDataViewColumn(w_ServerState, new wxDataViewTextRendererAttr, 5, 100) );

	servers_->AppendTextColumn(w_Online, 9);
	//servers_->AppendTextColumn(w_Sum, 10);

	sizerBox->Add(servers_, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizeDataView->Add(sizerBox, 1, wxGROW|wxALL, CONTROL_BORDER);

	//控件布局
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(sizeDataView, 1, wxGROW|wxALL, CONTROL_BORDER);
	//sizer->Add(sizerBox, 0, wxALL, CONTROL_BORDER);

	SetSizerAndFit(sizer);
}

//void ShowPage::InsertSum(DWORD dwSum, PageAssist *page_assist)
//{
//	ShowPageData tagSumItem;
//	tagSumItem.online_number = dwSum;
//
//	model_->AddData(&tagSumItem,page_assist);
//}

void ShowPage::update_ui(PageAssist *page_assist , BOOL is_static)
{
	if (page_assist != NULL)
	{
		page_assist_ = page_assist;
		std::list<ShowPageData*>& data_list = page_assist->GetShowPageDtatList();
		model_->AddData(data_list, page_assist, dwSum_);
		/*std::list<ShowPageData*>& data_list_abnormal = page_assist->GetAbnormalServerPageDtatList();
		model_abnormal->AddData(data_list_abnormal, page_assist);*/
		g_pMainFrame->update_ui();
	}
}

////多区开服
//void ShowPage::OpenMultiRegionService(wxCommandEvent& event)
//{
//	if (!sGameServerMgr.VerifyGMPrivilege(Crc32("NRTC_Start"), EP_Pri_Rt_A))
//	{
//		wxMessageBox(w_NoAuthOpenSome, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
//		return;
//	}
//	
//	if (wxMessageBox(w_AskForOpen, w_OpenSomeServer, wxYES_NO|wxICON_WARNING, this) != wxYES)
//	{
//		return;
//	}
//
//	std::vector<ShowPageData*>& datas = model_->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas.begin();
//	for (; it != datas.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		if (data != NULL)
//		{
//			//是否被选中
//			if (data->is_check)
//			{
//				if (page_assist_ != NULL)
//				{
//					//发送消息
//					page_assist_->SendStartServerMessage(data, FALSE);
//				}
//				else
//				{
//					LOG("PageAssist 指针为空")
//				}
//			}
//		}
//		else
//		{
//			LOG("ShowPageData 指针为空")
//		}
//	}
//
//	std::vector<ShowPageData*>& datas_abnormal = model_abnormal->GetDatas();
//	it = datas_abnormal.begin();
//	for (; it != datas_abnormal.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		if (data != NULL)
//		{
//			//是否被选中
//			if (data->is_check)
//			{
//				if (page_assist_ != NULL)
//				{
//					//发送消息
//					page_assist_->SendStartServerMessage(data, FALSE);
//				}
//				else
//				{
//					LOG("PageAssist 指针为空")
//				}
//			}
//		}
//		else
//		{
//			LOG("ShowPageData 指针为空")
//		}
//	}
//}
//
////多区关服
//void ShowPage::CloseMultiRegionService(wxCommandEvent& event)
//{
//	if (!sGameServerMgr.VerifyGMPrivilege(Crc32("NRTC_Close"), EP_Pri_Rt_A))
//	{
//		wxMessageBox(w_NoAuthCloseSome, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
//		return;
//	}
//
//	if (wxMessageBox(w_AskForClose, w_CloseSomeServer, wxYES_NO|wxICON_WARNING, this) != wxYES)
//	{
//		return;
//	}
//
//	std::vector<ShowPageData*>& datas = model_->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas.begin();
//	for (; it != datas.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		if (data != NULL)
//		{
//			//是否被选中
//			if (data->is_check)
//			{
//				if (page_assist_ != NULL)
//				{
//					//发送消息
//					page_assist_->SendCloseServerMessage(FALSE, data, FALSE);
//				}
//				else
//				{
//					LOG("PageAssist 指针为空")
//				}
//			}
//		}
//		else
//		{
//			LOG("ShowPageData 指针为空")
//		}
//	}
//
//	std::vector<ShowPageData*>& datas_abnormal = model_abnormal->GetDatas();
//	it = datas_abnormal.begin();
//	for (; it != datas_abnormal.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		if (data != NULL)
//		{
//			//是否被选中
//			if (data->is_check)
//			{
//				if (page_assist_ != NULL)
//				{
//					//发送消息
//					page_assist_->SendCloseServerMessage(FALSE, data, FALSE);
//				}
//				else
//				{
//					LOG("PageAssist 指针为空")
//				}
//			}
//		}
//		else
//		{
//			LOG("ShowPageData 指针为空")
//		}
//	}
//}
//
//// 全选正常服务器组
//void ShowPage::SelectAllServer(wxCommandEvent& event)
//{
//	std::vector<ShowPageData*>& datas = model_->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas.begin();
//	for (; it != datas.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		data->is_check = TRUE;
//	}
//
//	page_assist_->UpdateShowPage(TRUE);
//}
//
//// 取消全选正常服务器组
//void ShowPage::UnSelectAllServer(wxCommandEvent& event)
//{
//	std::vector<ShowPageData*>& datas = model_->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas.begin();
//	for (; it != datas.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		data->is_check = FALSE;
//	}
//
//	page_assist_->UpdateShowPage(TRUE);
//}
//
//// 全选异常服务器组
//void ShowPage::SelectAllBadServer(wxCommandEvent& event)
//{
//	std::vector<ShowPageData*>& datas_abnormal = model_abnormal->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas_abnormal.begin();
//
//	it = datas_abnormal.begin();
//
//	for (; it != datas_abnormal.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		data->is_check = TRUE;
//	}
//	
//	page_assist_->UpdateShowPage(TRUE);
//	//page_assist_->UpdateShowPageDtatList()
//}
//
//// 取消全选异常服务器组
//void ShowPage::UnSelectAllBadServer(wxCommandEvent& event)
//{
//	std::vector<ShowPageData*>& datas_abnormal = model_abnormal->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas_abnormal.begin();
//
//	it = datas_abnormal.begin();
//
//	for (; it != datas_abnormal.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		data->is_check = FALSE;
//	}
//
//	page_assist_->UpdateShowPage(TRUE);
//}
//
//// 根据权限判定界面中显示的按钮和页面
//void ShowPage::ShowButtons(bool bShow_1, bool bShow_2, bool bShow_3, bool bShow_4)
//{
//	if (bShow_1)
//	{
//		multi_region_close_service->Enable();
//		multi_region_open_service->Enable();
//	}
//	else
//	{
//		multi_region_close_service->Disable();
//		multi_region_open_service->Disable();
//	}
//
//	if (bShow_2)
//	{
//		m_combobox_system_cmd->Enable();
//		m_set_cmd_ok->Enable();
//	}
//	else
//	{
//		m_combobox_system_cmd->Disable();
//		m_set_cmd_ok->Disable();
//	}
//
//	if (bShow_3)
//	{
//		m_MaxNum->Enable();
//		m_set_MaxNum_ok->Enable();
//	}
//	else
//	{
//		m_MaxNum->Disable();
//		m_set_MaxNum_ok->Disable();
//	}
//	
//	if (bShow_4)
//	{
//		m_Reload_Mall_ok->Enable();
//	}
//	else
//	{
//		m_Reload_Mall_ok->Disable();
//	}
//}
//
//// 向指定服务器发送系统命令
//void ShowPage::SendSystemCmd(wxCommandEvent& event)
//{
//	wxString strTemp = m_combobox_system_cmd->GetValue();
//	wxString strCmd;
//	strCmd = g_CmdTable[strTemp.wx_str()];
//
//	if (!sGameServerMgr.VerifyGMPrivilege(Crc32("NRTC_SystemCmd"), EP_Pri_Rt_A))
//	{
//		wxMessageBox(w_NoAuthSend, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
//		return;
//	}
//
//	if (wxMessageBox(w_AskForSend, w_SendCmd, wxYES_NO|wxICON_WARNING, this) != wxYES)
//	{
//		return;
//	}
//
//	std::vector<ShowPageData*>& datas = model_->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas.begin();
//	for (; it != datas.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		if (data != NULL)
//		{
//			//是否被选中
//			if (data->is_check)
//			{
//				if (page_assist_ != NULL)
//				{
//					//发送消息
//					page_assist_->SendSystemCmdMessage(data, FALSE, strCmd);
//				}
//				else
//				{
//					LOG("PageAssist 指针为空")
//				}
//			}
//		}
//		else
//		{
//			LOG("ShowPageData 指针为空")
//		}
//	}
//
//	std::vector<ShowPageData*>& datas_abnormal = model_abnormal->GetDatas();
//	it = datas_abnormal.begin();
//	for (; it != datas_abnormal.end(); ++it)
//	{
//		ShowPageData* data = (*it);
//		if (data != NULL)
//		{
//			//是否被选中
//			if (data->is_check)
//			{
//				if (page_assist_ != NULL)
//				{
//					//发送消息
//					page_assist_->SendSystemCmdMessage(data, FALSE, strCmd);
//				}
//				else
//				{
//					LOG("PageAssist 指针为空")
//				}
//			}
//		}
//		else
//		{
//			LOG("ShowPageData 指针为空")
//		}
//	}
//}
//
//// 更改最大在线人数
//void ShowPage::ChangeMaxOnline(wxCommandEvent& event)
//{
//	wxString strMaxNum = m_MaxNum->GetValue();
//
//	if (!sGameServerMgr.VerifyGMPrivilege(Crc32("NRTC_MaxNum"), EP_Pri_Rt_C))
//	{
//		wxMessageBox(w_NoAuthChangeMax, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
//		return;
//	}
//
//	//if (wxMessageBox(w_AskForChangeMax, w_SendCmd, wxYES_NO|wxICON_WARNING, this) != wxYES)
//	//{
//	//	return;
//	//}
//
//	std::vector<ShowPageData*>& datas = model_->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas.begin();
//	for (int i = 0; it != datas.end();)
//	{
//		ShowPageData* data = (*it);
//		if (data != NULL)
//		{
//			//是否被选中
//			if (data->is_check)
//			{
//				if (page_assist_ != NULL)
//				{
//					if( page_assist_->SendChangeMaxMessage(data, FALSE, strMaxNum) == FALSE)
//						break;
//				}
//				else
//				{
//					LOG("PageAssist 指针为空")
//				}
//			}
//		}
//		else
//		{
//			LOG("ShowPageData 指针为空")
//		}
//
//		i++;
//		it = datas.begin()+i;
//	}
//}
//
////更新商城
//void ShowPage::ReloadMall(wxCommandEvent& event)
//{
//	if (!sGameServerMgr.VerifyGMPrivilege(Crc32("NRTC_UpdateMall"), EP_Pri_Rt_D))
//	{
//		wxMessageBox(w_NoAuthUpdataMall, w_Attention, wxOK|wxCENTRE|wxICON_ERROR, this);
//		return;
//	}
//
//	//if (wxMessageBox(w_AskForUpdateMall, w_DoUpdate, wxYES_NO|wxICON_WARNING, this) != wxYES)
//	//	return;
//
//
//	std::vector<ShowPageData*>& datas = model_->GetDatas();
//	std::vector<ShowPageData*>::iterator it = datas.begin();
//
//	for (int i = 0; it != datas.end(); )
//	{
//		ShowPageData* data = (*it);
//
//		if (data != NULL)
//		{
//			//是否被选中
//			if (data->is_check)
//			{
//				if (page_assist_ != NULL)
//					if (page_assist_->UpdateMall(data, FALSE) == FALSE)
//						break;
//				else
//					LOG("PageAssist 指针为空")
//			}
//		}
//		else
//			LOG("ShowPageData 指针为空")
//		
//		i++;
//		it = datas.begin()+i;
//	}
//}
//

MyListModel::MyListModel() :wxDataViewVirtualListModel( 0 )
{
	
}

MyListModel::~MyListModel()
{
}

void MyListModel::AddData(std::list<ShowPageData*>& data_list, PageAssist* page_assist, DWORD &dwSum)
{
	if (page_assist == NULL)
	{
		LOG("PageAssist 指针为空")
		return;
	}
	data_vector_.clear();
	dwSum = 0;
	int i = 0;
	std::list<ShowPageData*>::iterator it = data_list.begin();
	for (; it != data_list.end(); ++it)
	{
		//int n = (*it)->max_online;
		if( (*it)->type == EST_World )
		{
			//if(i==0) (*it)->max_online = 
			dwSum += (*it)->online_number;
			data_vector_.push_back(*it);
			i++;
		}	
	}
	
	//std::vector<ShowPageData*>::iterator it_ = data_vector_.begin();
	//if (it_ == data_vector_.end()) return;
	//(*it_)->max_online = dwSum;
	
	Reset(i);

	page_assist_ = page_assist;
}

void MyListModel::GetValueByRow( wxVariant &variant,
								unsigned int row, unsigned int col ) const
{
	ShowPageData* page_data = (ShowPageData*)data_vector_[row];
	if (page_data == NULL)
	{
		LOG("ShowPageData is null...")
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
	ShowPageData* page_data = (ShowPageData*)data_vector_[row];
	if (page_data == NULL)
	{
		LOG("ShowPageData is null...")
			return false;
	}
	// 状态
	//if (col == 5)
	//{
	//	//Login
	//	if (page_data->type == EST_Login)
	//	{
	//		if(page_data->status == EWS_Well)
	//		{
	//			attr.SetColour(*wxGREEN);
	//		}
	//		else if (page_data->status == EWS_InitNotDone)
	//		{
	//			attr.SetColour(*wxLIGHT_GREY);
	//		}
	//		else if (page_data->status == EWS_SystemError)
	//		{
	//			attr.SetColour(*wxRED);
	//		}
	//		else if (page_data->status == EWS_ProofError)
	//		{
	//			attr.SetColour(*wxBLUE);
	//		}
	//	}
	//	//DB
	//	if (page_data->type == EST_DB)
	//	{
	//		if(page_data->status == EWS_Well)
	//		{
	//			attr.SetColour(*wxGREEN);
	//		}
	//		else if (page_data->status == EWS_InitNotDone)
	//		{
	//			attr.SetColour(*wxLIGHT_GREY);
	//		}
	//		else if (page_data->status == EWS_SystemError)
	//		{
	//			attr.SetColour(*wxRED);
	//		}
	//	}
	//	//World
	//	if (page_data->type == EST_World)
	//	{
	//		if(page_data->status == EWS_Well)
	//		{
	//			attr.SetColour(*wxGREEN);
	//		}
	//		else if (page_data->status == EWS_InitNotDone)
	//		{
	//			attr.SetColour(*wxLIGHT_GREY);
	//		}
	//		else if (page_data->status == EWS_SystemError)
	//		{
	//			attr.SetColour(*wxRED);
	//		}
	//	}
	//}

	if (col == 6)
	{
		//Login
		if (page_data->online_number == EST_Login)
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
		}
		//World
		if (page_data->type == EST_World)
		{
			if(page_data->online_number >= 2000 && page_data->online_number < 3000 )
			{
				attr.SetColour(*wxGREEN);
			}
			else if (page_data->online_number >= 1000 && page_data->online_number < 2000)
			{
				attr.SetColour(*wxBLUE);
			}
			else if (page_data->online_number >= 3000)
			{
				attr.SetColour(*wxRED);
			}
			else
			{
				attr.SetColour(*wxBLACK);
			}
		}
	}
	return true;
}

bool MyListModel::SetValueByRow( const wxVariant &variant,
								unsigned int row, unsigned int col )
{
	ShowPageData* page_data = (ShowPageData*)data_vector_[row];
	if (page_data == NULL)
	{
		LOG("ShowPageData is null...")
			return false;
	}
	if (col == 0)
	{
		page_data->is_check = (int)variant.GetBool();
		return true;
	}
	
	return false;
}

//bool MyButtonCustomRenderer::Activate(wxRect WXUNUSED(cell), wxDataViewModel *model, const wxDataViewItem & item, unsigned int col)
//{
//	/*if (data_ != NULL)
//	{
//		wxString title = data_->region_name + wxT("----") + data_->world_name;
//		LoongWorldDialog dlg(title, data_);
//		dlg.ShowModal();
//	}*/
//	if (data_ != NULL)
//	{
//		if (data_->page_assist != NULL)
//		{
//			if (name_ == w_Start)
//			{
//				data_->page_assist->SendStartServerMessage(data_, TRUE);
//			}
//			else if (name_ == w_Close)
//			{
//				data_->page_assist->SendCloseServerMessage(FALSE, data_, TRUE);
//			}
//			else if (name_ == w_CloseForced)
//			{
//				data_->page_assist->SendCloseServerMessage(TRUE, data_, TRUE);
//			}
//		}
//	}
//	return false;
//}
//
//bool MyButtonCustomRenderer::SetValue( const wxVariant &value )
//{
//	
//	if(value.GetType() == wxT("void*") )
//	{
//		data_ = (ShowPageData*)value.GetVoidPtr();
//		if (data_ != NULL)
//		{
//			name_ = data_->operate_name;
//		}
//		else
//		{
//			LOG("ShowPageData pointer is null...")
//		}
//		return true;
//	}
//	return true;
//}
//
//bool MyButtonCustomRenderer::Render(wxRect rect, wxDC *dc, int WXUNUSED(state))
//{
//	int width = name_.length() * 20;
//	dc->SetPen( *wxGREEN_PEN );
//	wxRect rect1;
//	rect1.x = rect.x;
//	rect1.width = width;
//	rect1.y = rect.y;
//	rect1.height = 18;
//
//	wxRendererNative::Get().DrawPushButton(
//		GetOwner()->GetOwner(),
//		*dc,
//		rect1,
//		wxCONTROL_FOCUSED );
//	wxColour colour = dc->GetTextForeground();
//	dc->SetTextForeground(*wxBLACK);
//
//	dc->DrawText(name_,rect1.GetX() + 10, rect1.GetY() );
//	dc->SetTextForeground(colour);
//	return true;
//}
