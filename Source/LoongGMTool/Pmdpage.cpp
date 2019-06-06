/////////////////////////////////////////////////////////////////////////////
// Name:        PmdPage.cpp
// Created:     2009-02-04
// Info:		跑马灯公告页面
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/combobox.h"
#include "LoongGMTool.h"
#include "common.h"
#include "PmdPage.h"
#include "mylistctrl.h"
#include "resource.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "NetSessionMgr.h"
#include "..\ServerDefine\msg_gm_tool_c.h"


BEGIN_EVENT_TABLE(PmdPage, wxPanel)
	EVT_BUTTON(PmdPage_BTN, PmdPage::IssuePmdPage)
	EVT_COMBOBOX(PmdPage_Color, PmdPage::OnColorType)
END_EVENT_TABLE()

PmdPage::PmdPage(wxBookCtrlBase *book, const wxString& label) : m_label(label),wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
																							 wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxVERTICAL);

	wxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("RunHorse")]);
	wxSizer* sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);

	wxSizer* sizeRow = NULL;
	sizeRow = CreateSizerWithTextAndLabel(this, g_StrTable[_T("AfficheContent")], wxID_ANY, &m_text_pmd);
	sizerBox->Add(sizeRow, 0, wxGROW | wxALL, CONTROL_BORDER);

	wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);

	//颜色设置
	wxString strings[EColorType_End];
	for( int i=0; i<EColorType_End; i++ )
	{
		TCHAR szBuff[X_SHORT_STRING] = {0};
		_sntprintf( szBuff, X_SHORT_STRING, _T("EColorType%d"), i+1 );
		strings[i] = g_StrTable[szBuff];
	}
	sizerTmp->Add(CreateSizerWithTextAndComboBox(this, PmdPage_Color, g_StrTable[_T("Color")], &m_combobox_color_pmd, EColorType_End, strings), 0, wxGROW | wxALL, CONTROL_BORDER);
	m_btnAdd_pmd = new wxButton(this, PmdPage_BTN, g_StrTable[_T("Issue")]);
	sizerTmp->Add(m_btnAdd_pmd, 0, wxGROW | wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerTmp, 0, wxGROW | wxALL, CONTROL_BORDER);

	m_lctrlServerStatus = new MyListCtrl(this, PmdPage_ListServerStatus, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	wxListItem itemCol;
	itemCol.SetText(g_StrTable[_T("Server")]);
	m_lctrlServerStatus->InsertColumn(0, itemCol);

	itemCol.SetText(g_StrTable[_T("Execute")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(1, itemCol);

	itemCol.SetText(g_StrTable[_T("AfficheContent")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(2, itemCol);

	itemCol.SetText(g_StrTable[_T("FontColor")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(3, itemCol);

	itemCol.SetText(g_StrTable[_T("IssueTime")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrlServerStatus->InsertColumn(4, itemCol);

	m_lctrlServerStatus->SetColumnWidth( 2, 300 );
	m_lctrlServerStatus->SetColumnWidth( 4, 130 );

	sizerBox->Add(m_lctrlServerStatus, 1, wxGROW | wxALL, CONTROL_BORDER);

	sizerTop->Add(sizerBox, 1, wxGROW | wxALL, CONTROL_BORDER);

	sizerAll->Add(sizerTop, 1, wxGROW|wxALL, CONTROL_BORDER);
	SetSizer(sizerAll);
	sizerAll->Fit(this);

	//sUserFrameMgrPgr->RegisterEventHandle(_T("tagPmdPage"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnPmdPageEvent));

	//InsertServerStatus();
}

void PmdPage::InsertServerStatus()
{
	//服务器
	wxString buf;
	buf.Printf(wxT("loong-1"));
	long tmp = m_lctrlServerStatus->InsertItem(0, buf, 0);
	m_lctrlServerStatus->SetItemData(tmp, 0);
	//执行人
	buf.Printf(wxT("test"));
	m_lctrlServerStatus->SetItem(tmp, 1, buf);
	//公告内容
	buf.Printf(wxT("跑马灯公告"));
	m_lctrlServerStatus->SetItem(tmp, 2, buf);
	//字体颜色
	buf.Printf(wxT("红色"));
	m_lctrlServerStatus->SetItem(tmp, 3, buf);
	//发布时间
	buf.Printf(wxT("2009-02-19 16:00:00"));
	m_lctrlServerStatus->SetItem(tmp, 4, buf);
}

void PmdPage::IssuePmdPage( wxCommandEvent& event )
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_CastRun")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	wxMessageDialog dialog( NULL, g_StrTable[_T("IssuePmd")], wxT(""),
		wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	switch( dialog.ShowModal() )
	{
	case wxID_YES:
		{
			tstring strContent = m_text_pmd->GetValue();
			if(strContent.empty())
			{
				wxMessageBox(g_StrTable[_T("NoContent")], wxT(""), wxOK|wxCENTRE, &dialog);
				return;
			}
			EColorType eType = (EColorType)(m_combobox_color_pmd->GetSelection());

			wxCheckListBox* pCheckList = m_pMyFrame->GetCheckListBox();
			if( P_VALID(pCheckList) )
			{
				std::vector<int> vec;
				m_pMyFrame->GetCheckListBoxChecked(vec);
				if( (int)vec.size()<=0 )
				{
					wxMessageBox(g_StrTable[_T("NoSelectWorld")], wxT(""), wxOK|wxCENTRE, &dialog);
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
						/*tagNGMTC_CastRun msg;
						_sntprintf( msg.szContent, Notice_Content, strContent.c_str() );
						msg.eType = eType;
						msg.dwSectionID = section_id;
						msg.dwWorldID[0] = GT_INVALID;
						sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );*/
					}
					else
					{
						/*TCHAR byInfo[1024] = {0};
						tagNGMTC_CastRun* msg = (tagNGMTC_CastRun*)byInfo;
						msg->dwID = Crc32("NGMTC_CastRun");
						msg->dwSize = sizeof(tagNGMTC_CastRun) + sizeof(DWORD)*((int)list_world_id.size()-1);
						_sntprintf( msg->szContent, Notice_Content, strContent.c_str() );
						msg->eType = eType;
						msg->dwSectionID = section_id;
						int i=0;
						for( std::list<DWORD>::iterator it=list_world_id.begin(); it!=list_world_id.end(); it++ )
						{
							msg->dwWorldID[i] = (*it);
							i++;
						}
						sNetSessionMgrPtr->SendMsg( msg, msg->dwSize );*/
					}
				}
			}
			else
				return;			
		}
		break;
	}
}

void PmdPage::ClearTextCtrl()
{
	m_text_pmd->Clear();
}

DWORD PmdPage::OnPmdPageEvent( tagUserEvent* pGameEvent )
{
	/*if( pGameEvent->strEventName == _T("tagPmdPage") )
	{
		tagCastRun* pEvt = (tagCastRun*)pGameEvent;
		switch( pEvt->dwErrorCode )
		{
		case 0:
			break;
		}
	}*/
	return 0;
}

void PmdPage::OnColorType( wxCommandEvent& event )
{

}