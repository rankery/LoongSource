#include "stdafx.h"
#include "wx/wx.h"
#include <fstream>
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "LoongGMTool.h"
#include "common.h"
#include "AutoChat.h"
#include "mylistctrl.h"
#include "resource.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "NetSessionMgr.h"
#include "ItemProtoData.h"
#include "..\ServerDefine\msg_gm_tool_c.h"
#include "..\ServerDefine\msg_rt_errorcode.h"

#define ITEM_MAX ItemProtoData::Inst()->GetItemNum()

BEGIN_EVENT_TABLE(AutoChatPage, wxPanel)
	EVT_BUTTON(AutoChat_OK_II, AutoChatPage::OnSetOperation)
	EVT_BUTTON(AutoChat_OK_I, AutoChatPage::OnUseFileOperation)
	EVT_BUTTON(AutoChat_Cancel, AutoChatPage::OnCancelOperation)
END_EVENT_TABLE()

void AutoChatPage::ClearTextCtrl()
{
	m_text_hour->Clear();
	m_text_min->Clear();
	m_text_filepath->Clear();
	m_text_notice->Clear();
}

AutoChatPage::AutoChatPage( wxBookCtrlBase *book, const wxString& label ):m_label(label), wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
																								wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxVERTICAL);
	wxTextValidator validator(wxFILTER_NUMERIC); 

	//布局-文件导入
	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("ReadFile")]);
	wxSizer* sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);

	sizerBox->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("File")], wxID_ANY, &m_text_filepath), 0,  wxGROW | wxALL, CONTROL_BORDER);
	m_setnotice_ok_I = new wxButton(this, AutoChat_OK_I, g_StrTable[_T("SetOK")]);
	sizerBox->Add(m_setnotice_ok_I, 0, wxALL, CONTROL_BORDER);
	
	//布局-输入参数
	wxStaticBox *boxI = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("InputPara")]);
	wxSizer* sizerBoxI = new wxStaticBoxSizer(boxI, wxVERTICAL);
	sizerBoxI->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("NoticeContent")], wxID_ANY, &m_text_notice), 0,  wxGROW | wxALL, CONTROL_BORDER);

	wxSizer* sizer_time = new wxBoxSizer(wxHORIZONTAL);
	sizer_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Hour")], wxID_ANY, &m_text_hour, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizer_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Minute")], wxID_ANY, &m_text_min, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBoxI->Add(sizer_time, 0, wxGROW | wxALL, CONTROL_BORDER);
	
	m_setnotice_ok_II = new wxButton(this, AutoChat_OK_II, g_StrTable[_T("SetOK")]);
	sizerBoxI->Add(m_setnotice_ok_II, 0, wxALL, CONTROL_BORDER);
	
	//布局-记录显示
	wxListItem itemCol;
	wxStaticBox *boxII = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("AutoChatStatus")]);
	wxSizer* sizerBottom = new wxStaticBoxSizer(boxII, wxVERTICAL);

	m_lctrlStatus = new MyListCtrl(this, AutoChat_List, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	itemCol.SetText(g_StrTable[_T("Server")]);
	m_lctrlStatus->InsertColumn(0, itemCol);

	itemCol.SetText(g_StrTable[_T("Execute")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlStatus->InsertColumn(1, itemCol);

	itemCol.SetText(g_StrTable[_T("State")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlStatus->InsertColumn(2, itemCol);

	itemCol.SetText(g_StrTable[_T("OpenTime")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlStatus->InsertColumn(3, itemCol);

	itemCol.SetText(g_StrTable[_T("AfficheTime")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlStatus->InsertColumn(4, itemCol);
	sizerBottom->Add(m_lctrlStatus, 1, wxGROW | wxALL, CONTROL_BORDER);

	// 布局-取消定时消息
	m_setnotice_cancel = new wxButton(this, AutoChat_Cancel, g_StrTable[_T("SetCancelAutoChat")]);

	//整体布局
	sizerAll->Add(sizerBox, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerBoxI, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerBottom, 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerAll->Add(m_setnotice_cancel, 0, wxALL, CONTROL_BORDER);

	SetSizer(sizerAll);
	//sizerAll->Fit(this);

	sUserFrameMgrPgr->RegisterEventHandle(_T("tagAutoChat"),(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnAutoChatEvent));
}

DWORD AutoChatPage::OnSetEvent(tagUserEvent* pGameEvent)
{
	return 0;
}

void AutoChatPage::OnSetOperation(wxCommandEvent& event)
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_AutoChatNotice")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strNoticeContent = m_text_notice->GetValue();
	if(strNoticeContent.empty())
	{
		wxMessageBox(g_StrTable[_T("NoContent")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 轮询服务器列表，检查已选择的
	wxCheckListBox* pCheckList = m_pMyFrame->GetCheckListBox();
	if( P_VALID(pCheckList) )
	{
		std::vector<int> vec;
		m_pMyFrame->GetCheckListBoxChecked(vec);
		if( (int)vec.size()<=0 )
		{
			wxMessageBox(g_StrTable[_T("NoSelectWorld")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
		
		// 检测到被选的服务器，开始取参数
		tstring strNoticeContent =  m_text_notice->GetValue();
		tstring strHour = m_text_hour->GetValue();
		tstring strMin = m_text_min->GetValue();
		BYTE byHour = (BYTE)_tstoi(strHour.c_str());
		BYTE byMin = (BYTE)_tstoi(strMin.c_str());
		if (!(byHour>=0 && byHour<=24 && byMin>=0 && byMin<60))
		{
			wxMessageBox(g_StrTable[_T("AutoChatNotice2")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}

		// 获得所选游戏世界ID
		std::map<DWORD, tagSWIdEx> mapIDEx;
		std::map<DWORD, tagSWIdEx>::iterator itEx;
		std::vector<int>::iterator it = vec.begin();
		for( ; it!=vec.end(); it++ )
		{
			tstring strName = pCheckList->GetString(unsigned int(*it));
			tagSWId id;
			sDataMgrPtr->GetSectionAndWorldID(strName, &id);
			if( id.section_id!=GT_INVALID && id.world_id!=GT_INVALID )
			{
				itEx = mapIDEx.find(id.section_id);
				if( itEx!=mapIDEx.end() )
				{
					itEx->second.list_world_id.push_back(id.world_id);
				}
				else
				{
					tagSWIdEx swEx;
					swEx.section_id = id.section_id;
					swEx.list_world_id.push_back(id.world_id);
					mapIDEx.insert(std::make_pair(id.section_id, swEx));
				}

			}
		}

		//--发送网络消息
		for( itEx=mapIDEx.begin(); itEx!=mapIDEx.end(); itEx++ )
		{
			DWORD section_id = itEx->first;
			std::list<DWORD>& list_world_id = itEx->second.list_world_id;
			if( sDataMgrPtr->JudgeSingleSection(section_id, list_world_id) )
			{
				tagNGMTC_AutoChatNotice notice;
				_sntprintf( notice.szNotice, X_HUGE_STRING, strNoticeContent.c_str() );
				notice.byHour = byHour;
				notice.byMinute = byMin;
				notice.dwSectionID = section_id;
				notice.dwWorldID[0] = GT_INVALID;
				sNetSessionMgrPtr->SendMsg( &notice, notice.dwSize );

				// todo：在list中显示记录
				for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
				{
					tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
					InsertShowItem(szWorldName, wxT(""), byHour, byMin, strNoticeContent);
				}
			}
			else
			{
				TCHAR byInfo[2048] = {0};
				tagNGMTC_AutoChatNotice* notice = (tagNGMTC_AutoChatNotice*)byInfo;
				notice->dwID = Crc32("NGMTC_AutoChatNotice");
				notice->dwSize = sizeof(tagNGMTC_AutoChatNotice) + sizeof(DWORD)*((int)list_world_id.size()-1);
				_sntprintf( notice->szNotice, X_HUGE_STRING, strNoticeContent.c_str() );
				notice->byHour = byHour;
				notice->byMinute = byMin;
				notice->dwSectionID = section_id;
				int i=0;
				for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
				{
					notice->dwWorldID[i] = (*it);

					tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
					InsertShowItem(szWorldName, wxT(""), byHour, byMin, strNoticeContent);

					i++;
				}
				sNetSessionMgrPtr->SendMsg( notice, notice->dwSize );
			}
		}
	}

	ClearTextCtrl();
} 

void AutoChatPage::OnUseFileOperation(wxCommandEvent& event)
{
	// 判断权限
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_AutoChatNotice")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 询问玩家是否确定这样做
	wxMessageDialog dialog( NULL, g_StrTable[_T("ConfirmFileAutoChat")], wxT(""), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	if( dialog.ShowModal() == wxID_NO) return;

	// 打开文件
	tstring strFileName = m_text_filepath->GetValue();
	std::ifstream infile(strFileName.c_str());
	if (!infile.good())
	{
		wxMessageBox(g_StrTable[_T("ReadFileFail")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 轮询服务器列表，检查已选择的
	wxCheckListBox* pCheckList = m_pMyFrame->GetCheckListBox();
	if( P_VALID(pCheckList) )
	{
		std::vector<int> vec;
		m_pMyFrame->GetCheckListBoxChecked(vec);
		if( (int)vec.size()<=0 )
		{
			wxMessageBox(g_StrTable[_T("NoSelectWorld")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}

		// 获得所选游戏世界ID
		std::map<DWORD, tagSWIdEx> mapIDEx;
		std::map<DWORD, tagSWIdEx>::iterator itEx;
		std::vector<int>::iterator it = vec.begin();
		for( ; it!=vec.end(); it++ )
		{
			tstring strName = pCheckList->GetString(unsigned int(*it));
			tagSWId id;
			sDataMgrPtr->GetSectionAndWorldID(strName, &id);
			if( id.section_id!=GT_INVALID && id.world_id!=GT_INVALID )
			{
				itEx = mapIDEx.find(id.section_id);
				if( itEx!=mapIDEx.end() )
				{
					itEx->second.list_world_id.push_back(id.world_id);
				}
				else
				{
					tagSWIdEx swEx;
					swEx.section_id = id.section_id;
					swEx.list_world_id.push_back(id.world_id);
					mapIDEx.insert(std::make_pair(id.section_id, swEx));
				}

			}
		}

		char szText[X_HUGE_STRING] = {0};
		int nCount = 0;
		BYTE byHour = 0;
		BYTE byMin = 0;
		TCHAR szContent[X_HUGE_STRING] = {0};

		while (infile.getline(&szText[0], X_HUGE_STRING))
		{
			if (nCount%2 == 0) // 取时间
			{
				char szTmp[5] = {0};
				int i = 0;
				while( !((char)szText[i] == ':'))
				{
					i++;					
				}

				// 获得时
				szText[i] = '\0';
				strncpy_s(szTmp, szText, i);
				byHour = atoi(szTmp);
				
				// 获得分	
				//TCHAR pTmp[3] = {0};
				char *pTmp = NULL; 
				pTmp = &(szText[i+1]);
				strncpy_s(szTmp, pTmp, 3);
				byMin = atoi(szTmp);
			
				nCount++;
				continue;
			}
			else // 取内容
			{
 				TCHAR szTmpContent[X_HUGE_STRING] = {0};
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)szText, -1, szTmpContent, X_HUGE_STRING);
				_tcscpy_s(szContent, X_HUGE_STRING, szTmpContent);
				nCount++;
			}

			//--发送网络消息
			for( itEx=mapIDEx.begin(); itEx!=mapIDEx.end(); itEx++ )
			{
				DWORD section_id = itEx->first;
				std::list<DWORD>& list_world_id = itEx->second.list_world_id;
				if( sDataMgrPtr->JudgeSingleSection(section_id, list_world_id) )
				{
					tagNGMTC_AutoChatNotice notice;
					_sntprintf( notice.szNotice, X_HUGE_STRING, szContent );
					notice.byHour = byHour;
					notice.byMinute = byMin;
					notice.dwSectionID = section_id;
					notice.dwWorldID[0] = GT_INVALID;
					sNetSessionMgrPtr->SendMsg( &notice, notice.dwSize );

					for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
					{
						tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
						InsertShowItem(szWorldName, wxT(""), byHour, byMin, szContent);
					}
				}
				else
				{
					TCHAR byInfo[2048] = {0};
					tagNGMTC_AutoChatNotice* notice = (tagNGMTC_AutoChatNotice*)byInfo;
					notice->dwID = Crc32("NGMTC_AutoChatNotice");
					notice->dwSize = sizeof(tagNGMTC_AutoChatNotice) + sizeof(DWORD)*((int)list_world_id.size()-1);
					_sntprintf( notice->szNotice, X_HUGE_STRING, szContent );
					notice->byHour = byHour;
					notice->byMinute = byMin;
					notice->dwSectionID = section_id;
					int i=0;
					for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
					{
						notice->dwWorldID[i] = (*it);

						tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
						InsertShowItem(szWorldName, wxT(""), byHour, byMin, szContent);

						i++;
					}
					sNetSessionMgrPtr->SendMsg( notice, notice->dwSize );
				}
			}
		}		
	}
}

void AutoChatPage::OnCancelOperation(wxCommandEvent& event)
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_CancelAutoChatNotice")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 询问玩家是否确定这样做
	wxMessageDialog dialog( NULL, g_StrTable[_T("ConfirmCancelAutoChat")], wxT(""), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	if( dialog.ShowModal() == wxID_NO) return;

	tagNGMTC_CancelAutoChatNotice send;
	sNetSessionMgrPtr->SendMsg( &send, send.dwSize );

	ClearShowItem();
	return;
}

void AutoChatPage::AutoChatOperationStatus(BOOL bStatus)
{
	long tmp = m_lctrlStatus->GetTopItem();
	wxString buf;
	buf.Printf(wxT("失败"));
	m_lctrlStatus->SetItem(tmp, 3, buf);
}

void AutoChatPage::InsertShowItem(tstring szWorldName, tstring szTester, BYTE byHour, BYTE byMinute, tstring szContent)
{
	wxString buf;

	//服务器
	long tmp = m_lctrlStatus->InsertItem(0, szWorldName, 0);
	m_lctrlStatus->SetItemData(tmp, 0);

	//执行人
	m_lctrlStatus->SetItem(tmp, 1, szTester);

	// 状态
	m_lctrlStatus->SetItem(tmp, 2, wxT("成功"));

	//发布时间
	int nHour = (int)byHour;
	int nMin = (int)byMinute;
	buf.Printf(wxT("%d:%d"), nHour, nMin);
	m_lctrlStatus->SetItem(tmp, 3, buf);

	//公告内容
	m_lctrlStatus->SetItem(tmp, 4, szContent);

}

void AutoChatPage::ClearShowItem()
{
	int nItemCount = m_lctrlStatus->GetItemCount();
	while ( nItemCount > 0)
	{
		long Index = m_lctrlStatus->GetTopItem();
		m_lctrlStatus->DeleteItem(Index);
		nItemCount = m_lctrlStatus->GetItemCount();
	}
}