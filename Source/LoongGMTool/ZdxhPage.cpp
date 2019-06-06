/////////////////////////////////////////////////////////////////////////////
// Name:        ZdxhPage.cpp
// Created:     2009-02-04
// Info:		自动循环公告页面
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/combobox.h"
#include "LoongGMTool.h"
#include "common.h"
#include "ZdxhPage.h"
#include "mylistctrl.h"
#include "resource.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "NetSessionMgr.h"
#include "..\ServerDefine\msg_gm_tool_c.h"


BEGIN_EVENT_TABLE(ZdxhPage, wxPanel)
	EVT_BUTTON(ZdxhPage_BTN, ZdxhPage::IssueZdxhPage)
	EVT_COMBOBOX(ZdxhPage_Color, ZdxhPage::OnColorType)
END_EVENT_TABLE()

ZdxhPage::ZdxhPage(wxBookCtrlBase *book, const wxString& label) : m_label(label),wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
																					   wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxVERTICAL);

	wxTextValidator validator(wxFILTER_NUMERIC); 

	wxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("AutoCircle")]);
	wxSizer* sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);

	sizerBox->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("AfficheTime")], wxID_ANY, &m_text_zdxh), 0, wxGROW | wxALL, CONTROL_BORDER);

	wxSizer* sizerTmp1 = new wxBoxSizer(wxHORIZONTAL);
	sizerTmp1->Add(CreateSizerWithTextAndLabels(this, g_StrTable[_T("TimeInterval")], g_StrTable[_T("Minius")], wxID_ANY, &m_text_interval, false, &validator), 
		0, wxGROW | wxALL, CONTROL_BORDER);
	sizerTmp1->Add(CreateSizerWithTextAndLabels(this, g_StrTable[_T("CircleInterval")], g_StrTable[_T("Minius")], wxID_ANY, &m_text_loop, false, &validator), 
		0, wxGROW | wxALL, CONTROL_BORDER);
	sizerTmp1->Add(CreateSizerWithTextAndLabels(this, g_StrTable[_T("CircleNum")], wxT(""), wxID_ANY, &m_text_num, false, &validator), 
		0, wxGROW | wxALL, CONTROL_BORDER);
	
	sizerBox->Add(sizerTmp1, 0, wxGROW | wxALL, CONTROL_BORDER);

	wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);

	//颜色设置
	wxString strings[EColorType_End];
	for( int i=0; i<EColorType_End; i++ )
	{
		TCHAR szBuff[X_SHORT_STRING] = {0};
		_sntprintf( szBuff, X_SHORT_STRING, _T("EColorType%d"), i+1 );
		strings[i] = g_StrTable[szBuff];
	}
	sizerTmp->Add(CreateSizerWithTextAndComboBox(this, ZdxhPage_Color, g_StrTable[_T("Color")], &m_combobox_color_zdxh, EColorType_End, strings), 0, wxGROW | wxALL, CONTROL_BORDER);
	m_btnAdd_zdxh = new wxButton(this, ZdxhPage_BTN, g_StrTable[_T("Issue")]);
	sizerTmp->Add(m_btnAdd_zdxh, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerTmp, 0, wxGROW | wxALL, CONTROL_BORDER);

	m_lctrl_zdxh = new MyListCtrl(this, ZdxhPage_List, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	wxListItem itemCol;
	itemCol.SetText(g_StrTable[_T("Server")]);
	m_lctrl_zdxh->InsertColumn(0, itemCol);

	itemCol.SetText(g_StrTable[_T("Execute")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(1, itemCol);

	itemCol.SetText(g_StrTable[_T("State")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(2, itemCol);

	itemCol.SetText(g_StrTable[_T("AfficheContent")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(3, itemCol);

	itemCol.SetText(g_StrTable[_T("FontColor")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(4, itemCol);

	itemCol.SetText(g_StrTable[_T("IssueTime")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(5, itemCol);

	itemCol.SetText(g_StrTable[_T("CircleInterval")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(6, itemCol);

	itemCol.SetText(g_StrTable[_T("TimeInterval")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(7, itemCol);

	itemCol.SetText(g_StrTable[_T("CircleNum")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_zdxh->InsertColumn(8, itemCol);

	/*m_lctrl_zdxh->SetColumnWidth( 3, 200 );
	m_lctrl_zdxh->SetColumnWidth( 5, 130 );
	m_lctrl_zdxh->SetColumnWidth( 6, 130 );*/

	sizerBox->Add(m_lctrl_zdxh, 1, wxGROW | wxALL, CONTROL_BORDER);

	sizerTop->Add(sizerBox, 1, wxGROW | wxALL, CONTROL_BORDER);

	sizerAll->Add(sizerTop, 1, wxGROW|wxALL, CONTROL_BORDER);
	SetSizer(sizerAll);
	sizerAll->Fit(this);

	//sUserFrameMgrPgr->RegisterEventHandle(_T("tagZdxhPage"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnZdxhPageEvent));

	//InsertServerStatus();
}

void ZdxhPage::InsertShowItem(tstring szWorldName, tstring szTester, tstring szContent, EColorType eColor, tagDWORDTime dwIssueTime, tstring szCirInternal, tstring szTimeInternal, tstring szCirCount)
{
	wxString buf;
	
	//服务器
	long tmp = m_lctrl_zdxh->InsertItem(0, szWorldName, 0);
	m_lctrl_zdxh->SetItemData(tmp, 0);

	//执行人
	m_lctrl_zdxh->SetItem(tmp, 1, szTester);
	
	// 状态
	m_lctrl_zdxh->SetItem(tmp, 2, wxT("成功"));

	//公告内容
	m_lctrl_zdxh->SetItem(tmp, 3, szContent);

	//字体颜色
	if (eColor == EColorType_Blue)
		m_lctrl_zdxh->SetItem(tmp, 4, wxT("蓝色"));
	else if(eColor == EColorType_Orange)
		m_lctrl_zdxh->SetItem(tmp, 4, wxT("橙色"));
	else if(eColor == EColorType_Green)
		m_lctrl_zdxh->SetItem(tmp, 4, wxT("绿色"));
	else if(eColor == EColorType_Red)
		m_lctrl_zdxh->SetItem(tmp, 4, wxT("红色"));
	else if(eColor == EColorType_Yellow)
		m_lctrl_zdxh->SetItem(tmp, 4, wxT("黄色"));
	
	//发布时间
	int nYear = dwIssueTime.year;
	int nMon = dwIssueTime.month;
	int nDate = dwIssueTime.day;
	int nHour = dwIssueTime.hour;
	int nMin = dwIssueTime.min;
	buf.Printf(wxT("200%d-%d-%d %d:%d:00"), nYear, nMon, nDate, nHour, nMin);
	m_lctrl_zdxh->SetItem(tmp, 5, buf);
	
	//一次循环时间
	m_lctrl_zdxh->SetItem(tmp, 6, szCirInternal);

	// 循环间隔时间
	m_lctrl_zdxh->SetItem(tmp, 7, szTimeInternal);

	// 循环次数
	m_lctrl_zdxh->SetItem(tmp, 8, szCirCount);
}

void ZdxhPage::ClearShowItem()
{
	//m_lctrl_zdxh->ClearAll();
	int nItemCount = m_lctrl_zdxh->GetItemCount();
	while ( nItemCount > 0)
	{
		long Index = m_lctrl_zdxh->GetTopItem();
		m_lctrl_zdxh->DeleteItem(Index);
		nItemCount = m_lctrl_zdxh->GetItemCount();
	}
}

void ZdxhPage::ZdxhOperationStatus(BOOL bStatus)
{
	long tmp = m_lctrl_zdxh->GetTopItem();
	wxString buf;
	buf.Printf(wxT("失败"));
	m_lctrl_zdxh->SetItem(tmp, 2, buf);
}

void ZdxhPage::IssueZdxhPage( wxCommandEvent& event )
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_AutoNotice")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	
	tstring strContent = m_text_zdxh->GetValue();
	if(strContent.empty())
	{
		wxMessageBox(g_StrTable[_T("NoContent")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}
	EColorType eType = (EColorType)(m_combobox_color_zdxh->GetSelection());
	tstring strTimeInter = m_text_interval->GetValue();
	tstring strCirInter = m_text_loop->GetValue();
	tstring strNum = m_text_num->GetValue();

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
				tagNGMTC_AutoNotice msg;
				_sntprintf( msg.szContent, Notice_Content, strContent.c_str() );
				msg.eType = eType;
				msg.nTimeInterval = _tstoi(strTimeInter.c_str())*60;
				msg.nCirInterval = _tstoi(strCirInter.c_str())*60;
				msg.nCirNum = _ttoi(strNum.c_str());
				msg.dwSectionID = section_id;
				msg.dwWorldID[0] = GT_INVALID;
				sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );

				// todo：在list中显示记录
				for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
				{
					tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
					InsertShowItem(szWorldName, wxT(""), strContent, eType, GetCurrentDWORDTime(), strCirInter, strTimeInter, strNum);
				}
			}
			else
			{
				TCHAR byInfo[1024] = {0};
				tagNGMTC_AutoNotice* msg = (tagNGMTC_AutoNotice*)byInfo;
				msg->dwID = Crc32("NGMTC_AutoNotice");
				msg->dwSize = sizeof(tagNGMTC_AutoNotice) + sizeof(DWORD)*((int)list_world_id.size()-1);
				_sntprintf( msg->szContent, Notice_Content, strContent.c_str() );
				msg->eType = eType;
				msg->nTimeInterval = _tstoi(strTimeInter.c_str())*60;
				msg->nCirInterval = _tstoi(strCirInter.c_str())*60;
				msg->nCirNum = _ttoi(strNum.c_str());
				msg->dwSectionID = section_id;
				int i=0;
				for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
				{
					msg->dwWorldID[i] = (*it);

					tstring szWorldName = sDataMgrPtr->GetWorldNameByWorldID(*it);
					InsertShowItem(szWorldName, wxT(""), strContent, eType, GetCurrentDWORDTime(), strCirInter, strTimeInter, strNum);

					i++;
				}
				sNetSessionMgrPtr->SendMsg( msg, msg->dwSize );
			}
		}

		ClearCtrlText();
	}
}

//DWORD ZdxhPage::OnZdxhPageEvent( tagUserEvent* pGameEvent )
//{
//	if( pGameEvent->strEventName == _T("tagZdxhPage") )
//	{
//		tagAutoNotice* pEvt = (tagAutoNotice*)pGameEvent;
//		switch( pEvt->dwErrorCode )
//		{
//		case E_GMTool_Success:
//			{
//				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
//				TCHAR szBuff[X_SHORT_NAME] = {0};
//				_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("AutoCast0")], strWorldName.c_str() );
//
//				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
//				this->ClearCtrlText();
//			}
//			break;
//		case E_GMTool_Err:
//			{
//				tstring strWorldName = sDataMgrPtr->GetWorldNameByWorldID(pEvt->dwWorldID);
//				TCHAR szBuff[X_SHORT_NAME] = {0};
//				_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("AutoCast1")], strWorldName.c_str() );
//
//				wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
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

void ZdxhPage::OnColorType( wxCommandEvent& event )
{
	
}

void ZdxhPage::ClearCtrlText()
{
	m_combobox_color_zdxh->SetSelection(-1);
	m_text_zdxh->Clear();
	m_text_interval->Clear();
	m_text_loop->Clear();
	m_text_num->Clear();
}