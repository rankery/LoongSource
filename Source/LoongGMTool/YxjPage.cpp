/////////////////////////////////////////////////////////////////////////////
// Name:        YxjPage.cpp
// Created:     2009-02-04
// Info:		右下角公告页面
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/combobox.h"
#include "LoongGMTool.h"
#include "common.h"
#include "YxjPage.h"
#include "mylistctrl.h"
#include "resource.h"
#include "NetSessionMgr.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "..\ServerDefine\msg_gm_tool_c.h"

BEGIN_EVENT_TABLE(YxjPage, wxPanel)
	EVT_BUTTON(YxjPage_Btn_Issue, YxjPage::IssueYxjPage)
	EVT_BUTTON(YxjPage_Btn_Cancel, YxjPage::CancelYxj)
END_EVENT_TABLE()

YxjPage::YxjPage(wxBookCtrlBase *book, const wxString& label) : m_label(label),wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
																						 wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
,m_pMyFrame(NULL)
{
	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxVERTICAL);

	wxTextValidator validator(wxFILTER_NUMERIC); 

	wxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);

	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("RightAffiche")]);
	wxSizer* sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);

	sizerBox->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Title")], wxID_ANY, &m_text_title),
		0, wxGROW | wxALL, CONTROL_BORDER);
	//sizerBox->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Connect")], wxID_ANY, &m_text_link), 0, wxGROW | wxALL, CONTROL_BORDER);

	sizerBox->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("AfficheContent")], wxID_ANY, &m_text_yxj),
		0, wxGROW | wxALL, CONTROL_BORDER);

	

	wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);
	m_btnAdd_yxj = new wxButton(this, YxjPage_Btn_Issue, g_StrTable[_T("Issue")]);
	//m_chkOnlinePrompt = new wxCheckBox(this, YxjPage_OnlinePrompt, g_StrTable[_T("LoginHint")]);	
	//sizerTmp->Add(m_chkOnlinePrompt, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerTmp->Add(m_btnAdd_yxj, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerTmp, 0, wxGROW | wxALL , CONTROL_BORDER);
	

	wxStaticBox *box_time = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("OpenTime")]);
	wxSizer* sizerBox_time= new wxStaticBoxSizer(box_time, wxHORIZONTAL);
	//wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Year")], wxID_ANY, &m_text_year, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Month")], wxID_ANY, &m_text_month, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Date")], wxID_ANY, &m_text_day, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Hour")], wxID_ANY, &m_text_hour, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Minute")], wxID_ANY, &m_text_min, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	//sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Second")], wxID_ANY, &m_text_sec, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerBox_time, 0, wxALL, CONTROL_BORDER);

	m_lctrl_yxj = new MyListCtrl(this, YxjPage_List, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	wxListItem itemCol;
	itemCol.SetText(g_StrTable[_T("Server")]);
	m_lctrl_yxj->InsertColumn(EYLC_0, itemCol);

	itemCol.SetText(g_StrTable[_T("Execute")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_yxj->InsertColumn(EYLC_1, itemCol);

	itemCol.SetText(g_StrTable[_T("State")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_yxj->InsertColumn(EYLC_2, itemCol);

	itemCol.SetText(g_StrTable[_T("Title")]);
	m_lctrl_yxj->InsertColumn(EYLC_3, itemCol);

	itemCol.SetText(g_StrTable[_T("Content")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_yxj->InsertColumn(EYLC_4, itemCol);

	itemCol.SetText(g_StrTable[_T("Time")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_yxj->InsertColumn(EYLC_5, itemCol);

	itemCol.SetText(g_StrTable[_T("LoginHint")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_yxj->InsertColumn(EYLC_6, itemCol);
	m_lctrl_yxj->SetColumnWidth(4, 150);

	sizerBox->Add(m_lctrl_yxj, 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerTop->Add(sizerBox, 1, wxGROW | wxALL, CONTROL_BORDER);

	m_btnAdd_cancelyxj = new wxButton(this, YxjPage_Btn_Cancel, g_StrTable[_T("SetCancelRightNotice")]);

	sizerAll->Add(sizerTop, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(m_btnAdd_cancelyxj, 0, wxALL, CONTROL_BORDER);

	SetSizer(sizerAll);
	sizerAll->Fit(this);

	//sUserFrameMgrPgr->RegisterEventHandle(_T("tagYxjPage"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnYxjPageEvent));

	//InsertServerStatus();
}

void YxjPage::InsertShowItem(tstring szWorldName, tstring szTester, tstring szTitle, tstring szContent, tagDWORDTime dwStartTime)
{
	wxString buf;
	
	//服务器
	long tmp = m_lctrl_yxj->InsertItem(0, szWorldName, 0);
	m_lctrl_yxj->SetItemData(tmp, 0);

	//执行人
	m_lctrl_yxj->SetItem(tmp, 1, szTester);

	//状态
	m_lctrl_yxj->SetItem(tmp, 2, wxT("成功"));

	//标题
	m_lctrl_yxj->SetItem(tmp, 3, szTitle);
	
	//内容
	m_lctrl_yxj->SetItem(tmp, 4, szContent);
	
	//开始时间
	int nYear = dwStartTime.year;
	int nMon = dwStartTime.month;
	int nDate = dwStartTime.day;
	int nHour = dwStartTime.hour;
	int nMin = dwStartTime.min;
	buf.Printf(wxT("200%d-%d-%d %d:%d:00"), nYear, nMon, nDate, nHour, nMin);
	m_lctrl_yxj->SetItem(tmp, 5, buf);
	
	//上线提示
	//buf.Printf(wxT("是"));
	//m_lctrl_yxj->SetItem(tmp, 5, buf);
}

void YxjPage::YxjOperationStatus(BOOL bStatus)
{
	long tmp = m_lctrl_yxj->GetTopItem();
	wxString buf;
	buf.Printf(wxT("失败"));
	m_lctrl_yxj->SetItem(tmp, 2, buf);
}

void YxjPage::ClearShowItem()
{
	//m_lctrl_yxj->ClearAll();
	
 	int nItemCount = m_lctrl_yxj->GetItemCount();
	while ( nItemCount > 0)
	{
		long Index = m_lctrl_yxj->GetTopItem();
		m_lctrl_yxj->DeleteItem(Index);
		nItemCount = m_lctrl_yxj->GetItemCount();
	}
}

void YxjPage::IssueYxjPage(wxCommandEvent& event)
{

	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_RightNotice")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strContent = m_text_yxj->GetValue();
	if(strContent.empty())
	{
		wxMessageBox(g_StrTable[_T("NoContent")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}


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

		//获取数据
		tstring strTitle = m_text_title->GetValue();
		
		//tstring strLink = m_text_link->GetValue();
		//bool bLoginHint = m_chkOnlinePrompt->GetValue();
		tstring strYear = m_text_year->GetValue();
		tstring strMonth = m_text_month->GetValue();
		tstring strDay = m_text_day->GetValue();
		tstring strHour = m_text_hour->GetValue();
		tstring strMin = m_text_min->GetValue();
		//tstring strSec = m_text_sec->GetValue();

		BYTE byYear = (BYTE)(_tstoi(strYear.c_str())  % 100 );
		BYTE byMonth = (BYTE)_tstoi(strMonth.c_str());
		BYTE byDay = (BYTE)_tstoi(strDay.c_str());
		BYTE byHour = (BYTE)_tstoi(strHour.c_str());
		BYTE byMin = (BYTE)_tstoi(strMin.c_str());
		//BYTE bySec  = (BYTE)_tstoi(strSec.c_str());

		tagDWORDTime time(0, byMin, byHour, byDay, byMonth, byYear);
		
		//获得所选游戏世界ID
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
				tagNGMTC_RightNotice notice;
				notice.dwNoticeTime = time;
				_sntprintf( notice.szTitle, Notice_Title, strTitle.c_str() );
				_sntprintf( notice.szContent, Notice_Content, strContent.c_str() );
				//_sntprintf( notice.szLink, Notice_Link, strLink.c_str() );
				//notice.bLoginHint = bLoginHint;
				notice.dwSectionID = section_id;
				notice.dwWorldID[0] = GT_INVALID;
				sNetSessionMgrPtr->SendMsg( &notice, notice.dwSize );

				// todo：在list中显示记录
				for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
				{
					tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
					InsertShowItem(szWorldName, wxT(""), strTitle, strContent, time);
				}
			}
			else
			{
				TCHAR byInfo[2048] = {0};
				tagNGMTC_RightNotice* notice = (tagNGMTC_RightNotice*)byInfo;
				notice->dwID = Crc32("NGMTC_RightNotice");
				notice->dwSize = sizeof(tagNGMTC_RightNotice) + sizeof(DWORD)*((int)list_world_id.size()-1);
				notice->dwNoticeTime = time;
				_sntprintf( notice->szTitle, Notice_Title, strTitle.c_str() );
				_sntprintf( notice->szContent, Notice_Content, strContent.c_str() );
				//_sntprintf( notice->szLink, Notice_Link, strLink.c_str() );
				//notice->bLoginHint = bLoginHint;
				notice->dwSectionID = section_id;
				int i=0;
				for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
				{
					notice->dwWorldID[i] = (*it);

					// todo：在list中显示记录
					tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
					InsertShowItem(szWorldName, wxT(""), strTitle, strContent, time);
					i++;
				}
				sNetSessionMgrPtr->SendMsg( notice, notice->dwSize );
			}
		}

		ClearTextCtrl();

		//--保存
		tagYxj yxj;
		yxj.dwNetCrc = 0;
		yxj.strTitle = strTitle;
		yxj.strContent = strContent;
		//yxj.strLink = strLink;
		//yxj.bLoginHint = bLoginHint;
		yxj.strYear = strYear;
		yxj.strMonth = strMonth;
		yxj.strDay = strDay;
		yxj.strHour = strHour;
		yxj.strMin = strMin;
		//yxj.strSec = strSec;
		// 保存发布的右下角公告
		m_mapYxj.insert(std::make_pair(yxj.dwNetCrc, yxj));
	}


}

void YxjPage::CancelYxj(wxCommandEvent& event)
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_CancelRightNotice")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 询问玩家是否确定这样做
	wxMessageDialog dialog( NULL, g_StrTable[_T("ConfirmCancelRightNotice")], wxT(""), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	if( dialog.ShowModal() == wxID_NO) return;

	tagNGMTC_CancelRightNotice send;
	sNetSessionMgrPtr->SendMsg( &send, send.dwSize );

	ClearShowItem();
	return;
}

void YxjPage::ClearTextCtrl()
{
	m_text_title->Clear();
	m_text_yxj->Clear();
	//m_text_link->Clear();
	//m_chkOnlinePrompt->SetValue(false);
	m_text_year->Clear();
	m_text_month->Clear();
	m_text_day->Clear();
	m_text_hour->Clear();
	m_text_min->Clear();
	//m_text_sec->Clear();
}

//DWORD YxjPage::OnYxjPageEvent(tagUserEvent* pGameEvent)
//{
//	if( pGameEvent->strEventName == _T("tagYxjPage") )
//	{
//		tagNotice* pEvt = (tagNotice*)pGameEvent;
//		switch( pEvt->dwErrorCode )
//		{
//		case E_GMTool_Success:
//			{
//				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
//				TCHAR szBuff[X_SHORT_NAME] = {0};
//				_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("YxjNotice0")], strWorldName.c_str() );
//
//				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
//				this->ClearTextCtrl();
//			}
//			break;
//		case E_GMTool_Err:
//			{
//				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
//				TCHAR szBuff[X_SHORT_NAME] = {0};
//				_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("YxjNotice1")], strWorldName.c_str() );
//
//				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
//			}		
//			break;
//		case E_GMTool_Time_Err:
//			{
//				wxMessageBox(g_StrTable[_T("IssueTimeError")], wxT(""), wxOK|wxCENTRE, this);
//			}
//			break;
//		case E_GMTool_NullContent:
//			{
//				wxMessageBox(g_StrTable[_T("NoContent")], wxT(""), wxOK|wxCENTRE, this);
//			}
//			break;
//		}
//	}
//	return 0;
//}