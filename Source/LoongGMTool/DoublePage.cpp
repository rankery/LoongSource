/////////////////////////////////////////////////////////////////////////////
// Name:        DoublePage.cpp
// Created:     2009-02-04
// Info:		双倍页面
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "LoongGMTool.h"
#include "common.h"
#include "DoublePage.h"
#include "mylistctrl.h"
#include "resource.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "NetSessionMgr.h"
#include "..\ServerDefine\msg_gm_tool_c.h"

BEGIN_EVENT_TABLE(DoublePage, wxPanel)
	EVT_BUTTON(DoublePage_Ok, DoublePage::OnDoubleOperation)
	EVT_BUTTON(DoublePage_Cancel, DoublePage::OnCancelDoubleOperation)
END_EVENT_TABLE()

DoublePage::DoublePage(wxBookCtrlBase *book, const wxString& label) : m_label(label), wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
													   wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxVERTICAL);

	wxTextValidator validator(wxFILTER_NUMERIC); 

	//布局-上
	wxSizer* sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *box3 = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("Param")]);
	wxSizer* sizerTopLeft = new wxStaticBoxSizer(box3, wxVERTICAL);
	
	//双倍类型设置
	wxString strings[EDoubleType_End];
	for( int i=0; i<EDoubleType_End; i++ )
	{
		TCHAR szBuff[X_SHORT_STRING] = {0};
		_sntprintf( szBuff, X_SHORT_STRING, _T("EDoubleType%d"), i+1 );
		strings[i] = g_StrTable[szBuff];
	}
	sizerTopLeft->Add(CreateSizerWithTextAndComboBox(this, DoublePage_Type, g_StrTable[_T("DoubleType")], &m_comboBox, EDoubleType_End, strings), 1, wxGROW|wxALL, CONTROL_BORDER);

	wxSizer *sizeTmp = new wxBoxSizer(wxHORIZONTAL);

	wxSizer *sizeRow = NULL;
	sizeRow = CreateSizerWithTextAndLabels(this, g_StrTable[_T("LastTime")], g_StrTable[_T("Minius")], wxID_ANY, &m_textLastTime, false, &validator);
	sizeTmp->Add(sizeRow, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizeRow = CreateSizerWithTextAndLabels(this, g_StrTable[_T("Ratio")], wxT("%"), wxID_ANY, &m_textRate, false, &validator);
	sizeTmp->Add(sizeRow, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerTopLeft->Add(sizeTmp, 0, wxGROW | wxALL, CONTROL_BORDER);

	//wxStaticBox *box_time = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("DoubleOpenTime")]);
	//wxSizer* sizerBox_time= new wxStaticBoxSizer(box3, wxHORIZONTAL);
	wxSizer *sizerBox_time = new wxBoxSizer(wxHORIZONTAL);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Year")], wxID_ANY, &m_text_year, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Month")], wxID_ANY, &m_text_month, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Date")], wxID_ANY, &m_text_day, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Hour")], wxID_ANY, &m_text_hour, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox_time->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Minute")], wxID_ANY, &m_text_min, false, &validator), 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerTopLeft->Add(sizerBox_time, 0, wxALL, CONTROL_BORDER);

	wxSizer *sizeTmp1 = new wxBoxSizer(wxHORIZONTAL);
	m_btnOk = new wxButton(this, DoublePage_Ok, g_StrTable[_T("OK")]);
	m_bCancel = new wxButton(this, DoublePage_Cancel, g_StrTable[_T("CancelDouble")]);
	sizeTmp1->Add(m_btnOk, 0, wxALL, CONTROL_BORDER);
	//sizeTmp1->Add(m_bCancel, 0, wxALL, CONTROL_BORDER);
	sizerTopLeft->Add(sizeTmp1, 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerTop->Add(sizerTopLeft, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerTop, 0, wxGROW | wxALL, CONTROL_BORDER);

	//布局-下
	wxListItem itemCol;
	wxStaticBox *box5 = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("DoubleState")]);
	wxSizer* sizerBottom = new wxStaticBoxSizer(box5, wxVERTICAL);

	m_lctrlServerStatus = new MyListCtrl(this, DoublePage_ListServerStatus, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	itemCol.SetText(g_StrTable[_T("Server")]);
	m_lctrlServerStatus->InsertColumn(0, itemCol);

	itemCol.SetText(g_StrTable[_T("Execute")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(1, itemCol);

	itemCol.SetText(g_StrTable[_T("Type")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(2, itemCol);

	itemCol.SetText(g_StrTable[_T("State")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(3, itemCol);

	itemCol.SetText(g_StrTable[_T("Ratio")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(4, itemCol);

	itemCol.SetText(g_StrTable[_T("OpenTime")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(5, itemCol);

	itemCol.SetText(g_StrTable[_T("OverTime")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(6, itemCol);

	m_lctrlServerStatus->SetColumnWidth(5, 150);
	m_lctrlServerStatus->SetColumnWidth(6, 150);

	sizerBottom->Add(m_lctrlServerStatus, 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerBottom, 1, wxGROW | wxALL, CONTROL_BORDER);
	
	sizerAll->Add(m_bCancel, 0, wxALL, CONTROL_BORDER);

	SetSizer(sizerAll);
	sizerAll->Fit(this);

	//sUserFrameMgrPgr->RegisterEventHandle(_T("tagDoublePage"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnDoublePageEvent));
}

//void DoublePage::InsertIPData()
//{
//	wxString buf;
//	buf.Printf(wxT("172.17.1.138"));
//	long tmp = m_lctrlIp->InsertItem(0, buf, 0);
//	m_lctrlIp->SetItemData(tmp, 0);
//	buf.Printf(wxT("2009-02-19"));
//	m_lctrlIp->SetItem(tmp, 1, buf);
//}

void DoublePage::InsertShowItem(tstring szWorldName, tstring szTester, int nType, DWORD dwRate, tagDWORDTime dwStartTime, tagDWORDTime dwEndTime)
{
	//服务器	
	long tmp = m_lctrlServerStatus->InsertItem(0, szWorldName, 0);
	m_lctrlServerStatus->SetItemData(tmp, 0);

	//执行人
	m_lctrlServerStatus->SetItem(tmp, 1, szTester);
	
	//类别
	wxString buf;
	if( EDoubleType_Exp==nType ) buf.Printf(wxT("经验"));
	else if ( EDoubleType_Item==nType ) buf.Printf(wxT("掉落"));
	m_lctrlServerStatus->SetItem(tmp, 2, buf);
	
	//状态
	buf.Printf(wxT("成功"));
	m_lctrlServerStatus->SetItem(tmp, 3, buf);
	
	//比例
	wxString strTmpRate;
	strTmpRate.Printf(wxT("%d"), dwRate);
	m_lctrlServerStatus->SetItem(tmp, 4, strTmpRate);
	
	//开始时间
	int nYear = dwStartTime.year;
	int nMon = dwStartTime.month;
	int nDate = dwStartTime.day;
	int nHour = dwStartTime.hour;
	int nMin = dwStartTime.min;
	buf.Printf(wxT("200%d-%d-%d %d:%d:00"), nYear, nMon, nDate, nHour, nMin);
	m_lctrlServerStatus->SetItem(tmp, 5, buf);
	
	//结束时间
	nYear = dwEndTime.year;
	nMon = dwEndTime.month;
	nDate = dwEndTime.day;
	nHour = dwEndTime.hour;
	nMin = dwEndTime.min;
	buf.Printf(wxT("200%d-%d-%d %d:%d:00"), nYear, nMon, nDate, nHour, nMin);
	m_lctrlServerStatus->SetItem(tmp, 6, buf);
}

void DoublePage::DoubleOperationStatus(BOOL bStatus)
{
	long tmp = m_lctrlServerStatus->GetTopItem();
	wxString buf;
	buf.Printf(wxT("失败"));
	m_lctrlServerStatus->SetItem(tmp, 3, buf);
}

void DoublePage::ClearShowItem()
{
	//m_lctrlServerStatus->ClearAll();
	int nItemCount = m_lctrlServerStatus->GetItemCount();
	while ( nItemCount > 0)
	{
		long Index = m_lctrlServerStatus->GetTopItem();
		m_lctrlServerStatus->DeleteItem(Index);
		nItemCount = m_lctrlServerStatus->GetItemCount();
	}
}

void DoublePage::OnDoubleOperation( wxCommandEvent& event )
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_Double")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
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

		EDoubleType eType = (EDoubleType)(m_comboBox->GetSelection());
		tstring strLasttime = m_textLastTime->GetValue();
		tstring strRatio = m_textRate->GetValue();
		tstring strYear = m_text_year->GetValue();
		tstring strMonth = m_text_month->GetValue();
		tstring strDay = m_text_day->GetValue();
		tstring strHour = m_text_hour->GetValue();
		tstring strMin = m_text_min->GetValue();
		BYTE byYear = (BYTE) (_tstoi(strYear.c_str()) % 100 );
		BYTE byMonth = (BYTE)_tstoi(strMonth.c_str());
		BYTE byDay = (BYTE)_tstoi(strDay.c_str());
		BYTE byHour = (BYTE)_tstoi(strHour.c_str());
		BYTE byMin = (BYTE)_tstoi(strMin.c_str());

		tagDWORDTime opentime(0, byMin, byHour, byDay, byMonth, byYear);

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
				tagNGMTC_Double msg;
				msg.eDType = eType;
				msg.dwOpenTime = opentime;
				msg.dwLastTime = _tstoi(strLasttime.c_str())*60;
				msg.dwRatio = _tstoi(strRatio.c_str());
				msg.dwSectionID = section_id;
				msg.dwWorldID[0] = GT_INVALID;
				sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );

				// todo：在list中显示记录
				for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
				{
					tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
					InsertShowItem(szWorldName, wxT(""), int(eType), _tstoi(strRatio.c_str()), tagDWORDTime(opentime), (tagDWORDTime)(IncreaseTime(opentime, _tstoi(strLasttime.c_str())*60)));
				}
			}
			else
			{
				TCHAR byInfo[1024] = {0};
				tagNGMTC_Double* msg = (tagNGMTC_Double*)byInfo;
				msg->dwID = Crc32("NGMTC_Double");
				msg->dwSize = sizeof(tagNGMTC_Double) + sizeof(DWORD)*((int)list_world_id.size()-1);
				msg->dwOpenTime = opentime;
				msg->dwLastTime = _tstoi(strLasttime.c_str())*60;
				msg->dwRatio = _tstoi(strRatio.c_str());				
				msg->dwSectionID = section_id;
				int i=0;
				for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
				{
					msg->dwWorldID[i] = (*it);

					// todo：在list中显示记录
					tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
					InsertShowItem(szWorldName, wxT(""), int(eType), _tstoi(strRatio.c_str()), tagDWORDTime(opentime), (tagDWORDTime)(IncreaseTime(opentime, _tstoi(strLasttime.c_str())*60)));

					i++;
				}
				sNetSessionMgrPtr->SendMsg( msg, msg->dwSize );
			}
		}

		ClearCtrlText();
	}
	else 
		return;
}

void DoublePage::OnCancelDoubleOperation(wxCommandEvent& event)
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_Double")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	//wxMessageBox(g_StrTable[_T("ConfirmCancelDouble")], wxT(""), wxOK|wxCENTRE, this);
	wxMessageDialog dialog( NULL, g_StrTable[_T("ConfirmCancelDouble")], wxT(""),
		wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	if( dialog.ShowModal() == wxID_NO) return;

	tagNGMTC_CancelDoubleMsg send;
	sNetSessionMgrPtr->SendMsg( &send, send.dwSize );
	
	ClearShowItem();
	return;

	//wxCheckListBox* pCheckList = m_pMyFrame->GetCheckListBox();
	//if( P_VALID(pCheckList) )
	//{
	//	std::vector<int> vec;
	//	m_pMyFrame->GetCheckListBoxChecked(vec);
	//	if( (int)vec.size()<=0 )
	//	{
	//		wxMessageBox(g_StrTable[_T("NoSelectWorld")], wxT(""), wxOK|wxCENTRE, this);
	//		return;
	//	}

	//	//获得所选游戏世界ID
	//	std::map<DWORD, tagSWIdEx> mapIDEx;
	//	std::map<DWORD, tagSWIdEx>::iterator itEx;
	//	std::vector<int>::iterator it = vec.begin();
	//	for( ; it!=vec.end(); it++ )
	//	{
	//		tstring strName = pCheckList->GetString(unsigned int(*it));
	//		tagSWId id;
	//		sDataMgrPtr->GetSectionAndWorldID(strName, &id);
	//		if( id.section_id!=GT_INVALID && id.world_id!=GT_INVALID )
	//		{
	//			itEx = mapIDEx.find(id.section_id);
	//			if( itEx!=mapIDEx.end() )
	//			{
	//				itEx->second.list_world_id.push_back(id.world_id);
	//			}
	//			else
	//			{
	//				tagSWIdEx swEx;
	//				swEx.section_id = id.section_id;
	//				swEx.list_world_id.push_back(id.world_id);
	//				mapIDEx.insert(std::make_pair(id.section_id, swEx));
	//			}
	//		}
	//	}

	//	//--发送网络消息
	//	for( itEx=mapIDEx.begin(); itEx!=mapIDEx.end(); itEx++ )
	//	{
	//		DWORD section_id = itEx->first;
	//		std::list<DWORD>& list_world_id = itEx->second.list_world_id;
	//		if( sDataMgrPtr->JudgeSingleSection(section_id, list_world_id) )
	//		{
	//			tagNGMTC_CancelDoubleMsg msg;
	//			msg.dwSectionID = section_id;
	//			msg.dwWorldID[0] = GT_INVALID;
	//			sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );
	//		}
	//		else
	//		{
	//			TCHAR byInfo[1024] = {0};
	//			tagNGMTC_CancelDoubleMsg* msg = (tagNGMTC_CancelDoubleMsg*)byInfo;
	//			msg->dwID = Crc32("NGMTC_CancelDouble");
	//			msg->dwSize = sizeof(tagNGMTC_CancelDoubleMsg) + sizeof(DWORD)*((int)list_world_id.size()-1);			
	//			msg->dwSectionID = section_id;
	//			int i=0;
	//			for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
	//			{
	//				msg->dwWorldID[i] = (*it);
	//				i++;
	//			}
	//			sNetSessionMgrPtr->SendMsg( msg, msg->dwSize );
	//		}
	//	}

	//	ClearCtrlText();
	//}
	//else
	//	return;
}

//DWORD DoublePage::OnDoublePageEvent( tagUserEvent* pGameEvent )
//{
//	if( pGameEvent->strEventName == _T("tagDoublePage") )
//	{
//		tagDouble* pEvt = (tagDouble*)pGameEvent;
//		switch( pEvt->dwErrorCode )
//		{
//		case E_GMTool_Success:
//			{
//				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
//				TCHAR szBuff[X_SHORT_NAME] = {0};
//				_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("OpenDouble0")], strWorldName.c_str() );
//				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
//				this->ClearCtrlText();
//			}
//			break;
//		case E_GMTool_Err:
//			{
//				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
//				TCHAR szBuff[X_SHORT_NAME] = {0};
//				_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("OpenDouble1")], strWorldName.c_str() );
//				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
//			}
//			break;
//		case E_GMTool_Time_Err:
//			{
//				wxMessageBox(g_StrTable[_T("IssueTimeError")], wxT(""), wxOK|wxCENTRE, this);
//			}
//			break;
//		}
//	}
//	return 0;
//}
//
void DoublePage::ClearCtrlText()
{
	m_textRate->Clear();
	m_textLastTime->Clear();
	m_comboBox->SetSelection(-1);
	m_text_year->Clear();
	m_text_month->Clear();
	m_text_day->Clear();
	m_text_hour->Clear();
	m_text_min->Clear();
}

