#include "stdafx.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "LoongGMTool.h"
#include "common.h"
#include "GmMgrPage.h"
#include "mylistctrl.h"
#include "resource.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "NetSessionMgr.h"
#include "..\ServerDefine\msg_gm_tool_c.h"

BEGIN_EVENT_TABLE(GmMgrPage, wxPanel)
	EVT_BUTTON(GmMgrPage_Create, GmMgrPage::OnGMCreateOperation)
	EVT_BUTTON(GmMgrPage_Modify, GmMgrPage::OnGMModifyOperation)
	EVT_BUTTON(GmMgrPage_Delete, GmMgrPage::OnGMDeleteOperation)
END_EVENT_TABLE()

GmMgrPage::GmMgrPage( wxBookCtrlBase *book, const wxString& label ): m_label(label), wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
																					wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{

	//颜色设置
	wxString strings[EP_Pri_GM_S+1];
	for( int i=0; i<EP_Pri_GM_S+1; i++ )
	{
		TCHAR szBuff[X_SHORT_STRING] = {0};
		_sntprintf( szBuff, X_SHORT_STRING, _T("EPrivilege%d"), i );
		strings[i] = g_StrTable[szBuff];
	}

	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxVERTICAL);

	//布局-上
	wxSizer* sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("GMCreate")]);
	wxSizer* sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);

	wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);
	sizerBox->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Account")], wxID_ANY, &m_text_account),
		0,  wxALL, CONTROL_BORDER);
	sizerBox->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("PassWord")], wxID_ANY, &m_text_psw, true),
		0,  wxALL, CONTROL_BORDER);
	m_btn_create = new wxButton(this, GmMgrPage_Create, g_StrTable[_T("GMCreate")]);
	sizerTmp->Add(CreateSizerWithTextAndComboBox(this, PmdPage_Color, g_StrTable[_T("GMPrivilege")], &m_combox_Pr_cre, EP_Pri_GM_S+1, strings), 0, /*wxGROW |*/ wxALL, CONTROL_BORDER);
	sizerTmp->Add(m_btn_create, 0, wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerTmp, 0, wxGROW | wxALL, CONTROL_BORDER);

	//布局-中
	wxSizer* sizerMin = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *boxMin = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("GMModify")]);
	wxSizer* sizerBoxMin = new wxStaticBoxSizer(boxMin, wxVERTICAL);
	wxSizer* sizerTmpMin = new wxBoxSizer(wxHORIZONTAL);
	sizerBoxMin->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Account")], wxID_ANY, &m_text_account_modify),
		0,  wxALL, CONTROL_BORDER);
	m_btn_modify = new wxButton(this, GmMgrPage_Modify, g_StrTable[_T("GMModify")]);
	sizerTmpMin->Add(CreateSizerWithTextAndComboBox(this, PmdPage_Color, g_StrTable[_T("GMPrivilege")], &m_combox_Pr_modify, EP_Pri_GM_S+1, strings), 0, /*wxGROW |*/ wxALL, CONTROL_BORDER);
	sizerTmpMin->Add(m_btn_modify, 0, wxALL, CONTROL_BORDER);
	sizerBoxMin->Add(sizerTmpMin, 0, wxGROW | wxALL, CONTROL_BORDER);

	//布局-下
	wxSizer* sizerBom = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *boxBom = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("GMDelete")]);
	wxSizer* sizerBoxBom = new wxStaticBoxSizer(boxBom, wxVERTICAL);
	wxSizer* sizerTmpBom = new wxBoxSizer(wxHORIZONTAL);
	sizerTmpBom->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Account")], wxID_ANY, &m_text_account_del),
		0,  wxALL, CONTROL_BORDER);
	m_btn_del = new wxButton(this, GmMgrPage_Delete, g_StrTable[_T("GMDelete")]);
	sizerTmpBom->Add(m_btn_del, 0, wxALL, CONTROL_BORDER);
	sizerBoxBom->Add(sizerTmpBom, 0, wxGROW | wxALL, CONTROL_BORDER);	
	
	sizerTop->Add(sizerBox, 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerMin->Add(sizerBoxMin, 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerBom->Add(sizerBoxBom, 1, wxGROW | wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerTop, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerMin, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerBom, 1, wxGROW|wxALL, CONTROL_BORDER);
	SetSizer(sizerAll);
	sizerAll->Fit(this);

	sUserFrameMgrPgr->RegisterEventHandle(_T("tagGMCreate"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnGmMgrPageEvent));
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagGMModify"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnGmMgrPageEvent));
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagGMDelete"), (FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnGmMgrPageEvent));
}

void GmMgrPage::OnGMCreateOperation( wxCommandEvent& event )
{

	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_GMCreate")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strName = m_text_account->GetValue();
	tstring strPsw = m_text_psw->GetValue();
	EPrivilege ePri = (EPrivilege)(m_combox_Pr_cre->GetSelection());
	tagNGMTC_GMCreate msg;
	_sntprintf( msg.szName, X_LONG_NAME, strName.c_str() );
	msg.dwPswCrc = Crc32(strPsw.c_str());
	msg.ePrivilege = ePri;
	sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );
}

void GmMgrPage::OnGMModifyOperation( wxCommandEvent& event )
{
	
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_GMModify")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strName = m_text_account_modify->GetValue();
	EPrivilege ePri = (EPrivilege)(m_combox_Pr_modify->GetSelection());
	tagNGMTC_GMModify msg;
	_sntprintf( msg.szName, X_LONG_NAME, strName.c_str() );
	msg.ePrivilege = ePri;
	sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );
}

void GmMgrPage::OnGMDeleteOperation( wxCommandEvent& event )
{
	
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_GMDelete")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strName = m_text_account_del->GetValue();
	tagNGMTC_GMDelete msg;
	_sntprintf( msg.szName, X_LONG_NAME, strName.c_str() );
	sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );
}

DWORD GmMgrPage::OnGmMgrPageEvent( tagUserEvent* pGameEvent )
{
	if( pGameEvent->strEventName == _T("tagGMCreate") )
	{
		tagGMCreate* pEvt = (tagGMCreate*)pGameEvent;
		switch(pEvt->dwErrorCode)
		{
		case E_GMTool_Success:
			{
				TCHAR szBuff[X_SHORT_STRING] = {0};
				_sntprintf( szBuff, X_SHORT_STRING, _T("GMCreateMsg%u"), pEvt->dwErrorCode );
				wxMessageBox(g_StrTable[szBuff], wxT(""), wxOK|wxCENTRE, this);
				m_text_account->Clear();
				m_text_psw->Clear();
				m_combox_Pr_cre->SetSelection(GT_INVALID);
			}
			break;
		case E_GMTool_GM_AlreadyExist:
			{
				wxMessageBox(g_StrTable[_T("GMCreateMsg_NameIs")], wxT(""), wxOK|wxCENTRE, this);
				m_text_account->Clear();
				m_text_psw->Clear();
				m_combox_Pr_cre->SetSelection(GT_INVALID);
			}
			break;
		case E_GMTool_Err:
			{
				wxMessageBox(g_StrTable[_T("GMCreateMsg_Error")], wxT(""), wxOK|wxCENTRE, this);
				m_text_account->Clear();
				m_text_psw->Clear();
				m_combox_Pr_cre->SetSelection(GT_INVALID);
			}
			break;
		}
	}
	else if( pGameEvent->strEventName == _T("tagGMModify") )
	{
		tagGMModify* pEvt = (tagGMModify*)pGameEvent;
		switch(pEvt->dwErrorCode)
		{
		case E_GMTool_Success:
			{
				TCHAR szBuff[X_SHORT_STRING] = {0};
				_sntprintf( szBuff, X_SHORT_STRING, _T("GMModifyMsg%u"), pEvt->dwErrorCode );
				wxMessageBox(g_StrTable[szBuff], wxT(""), wxOK|wxCENTRE, this);
				m_text_account_modify->Clear();
				m_combox_Pr_modify->SetSelection(GT_INVALID);
			}
			break;
		case E_GMTool_GM_NotExist:
			{
				wxMessageBox(g_StrTable[_T("GMNotExist")], wxT(""), wxOK|wxCENTRE, this);
				m_text_account_modify->Clear();
				m_combox_Pr_modify->SetSelection(GT_INVALID);
			}
			break;
		case E_GMTool_Err:
			{
				wxMessageBox(g_StrTable[_T("GMModifyMsg_Error")], wxT(""), wxOK|wxCENTRE, this);
				m_text_account_modify->Clear();
				m_combox_Pr_modify->SetSelection(GT_INVALID);
			}
			break;
		}
	}
	else if( pGameEvent->strEventName == _T("tagGMDelete") )
	{
		tagGMDelete* pEvt = (tagGMDelete*)pGameEvent;
		switch(pEvt->dwErrorCode)
		{
		case E_GMTool_Success:
			{
				TCHAR szBuff[X_SHORT_STRING] = {0};
				_sntprintf( szBuff, X_SHORT_STRING, _T("GMDeleteMsg%u"), pEvt->dwErrorCode );
				wxMessageBox(g_StrTable[szBuff], wxT(""), wxOK|wxCENTRE, this);
				m_text_account_del->Clear();
			}
			break;
		case E_GMTool_GM_NotExist:
			{
				wxMessageBox(g_StrTable[_T("GMNotExist")], wxT(""), wxOK|wxCENTRE, this);
				m_text_account_modify->Clear();
				m_combox_Pr_modify->SetSelection(GT_INVALID);
			}
			break;
		case E_GMTool_Err:
			{
				wxMessageBox(g_StrTable[_T("GMDeleteMsg_Error")], wxT(""), wxOK|wxCENTRE, this);
				m_text_account_modify->Clear();
				m_combox_Pr_modify->SetSelection(GT_INVALID);
			}
			break;
		}
	}
	return 0;
}