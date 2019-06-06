#include "stdafx.h"
#include <fstream>
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "LoongGMTool.h"
#include "common.h"
#include "AccountPage.h"
#include "mylistctrl.h"
#include "resource.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "NetSessionMgr.h"
#include "ItemProtoData.h"
#include "..\ServerDefine\msg_gm_tool_c.h"
#include "..\ServerDefine\msg_rt_errorcode.h"

#define ITEM_MAX ItemProtoData::Inst()->GetItemNum()

BEGIN_EVENT_TABLE(AccountPage, wxPanel)
	EVT_BUTTON(AccountPage_Suspend, AccountPage::OnAccountSuspendOperation)
	EVT_BUTTON(AccountPage_Unsuspend, AccountPage::OnAccountUnsuspendOperation)
	EVT_BUTTON(AccountPage_Suspend_File, AccountPage::OnAccountFileSuspendOperation)
	EVT_BUTTON(AccountPage_Unsuspend_File, AccountPage::OnAccountFileUnsuspendOperation)
	EVT_BUTTON(AccountPage_AddBlack, AccountPage::OnIPBlacklistAdd)
	EVT_BUTTON(AccountPage_DelBlack, AccountPage::OnIPBlacklistDel)
END_EVENT_TABLE()

AccountPage::AccountPage( wxBookCtrlBase *book, const wxString& label ):m_label(label), wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
																						  wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxVERTICAL);

	wxTextValidator validator(wxFILTER_NUMERIC); 

	//布局-上
	wxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("WhetherSuspend")]);
	wxSizer* sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);
	
	wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);
	//wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);
	sizerTmp->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("Account")], wxID_ANY, &m_text_account),
		0,  wxALL, CONTROL_BORDER);
	//m_text_account->SetMaxLength(X_LONG_STRING-1);
	sizerTmp->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("SectionName")], wxID_ANY, &m_text_sectionname),
		0,  wxALL, CONTROL_BORDER);
	sizerTmp->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("WorldName")], wxID_ANY, &m_text_worldname),
		0,  wxALL, CONTROL_BORDER);
	m_account_suspend = new wxButton(this, AccountPage_Suspend, g_StrTable[_T("Suspend")]);
	m_account_unsuspend = new wxButton(this, AccountPage_Unsuspend, g_StrTable[_T("Unsuspend")]);
	
	//sizerTmp->Add(m_account_suspend, 0, wxALL, CONTROL_BORDER);
	//sizerTmp->Add(m_account_unsuspend, 0, wxALL, CONTROL_BORDER);
	//sizerBox->Add(sizerTmp, 0, wxALL, CONTROL_BORDER);
	sizerTmp->Add(m_account_suspend, 0, wxALL, CONTROL_BORDER);
	sizerTmp->Add(m_account_unsuspend, 0, wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerTmp, 0, wxALL, CONTROL_BORDER);

	wxSizer* sizerTmpI = new wxBoxSizer(wxHORIZONTAL);
	//wxSizer* sizerTI = new wxStaticBoxSizer(box, wxHORIZONTAL);
	sizerTmpI->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("AccountFile")], wxID_ANY, &m_text_accountfile),
		0,  wxALL, CONTROL_BORDER);
	sizerTmpI->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("SectionName")], wxID_ANY, &m_text_sectionnameI),
		0,  wxALL, CONTROL_BORDER);
	sizerTmpI->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("WorldName")], wxID_ANY, &m_text_worldnameI),
		0,  wxALL, CONTROL_BORDER);
	m_account_suspendI = new wxButton(this, AccountPage_Suspend_File, g_StrTable[_T("Suspend")]);
	m_account_unsuspendI = new wxButton(this, AccountPage_Unsuspend_File, g_StrTable[_T("Unsuspend")]);
	
	sizerTmpI->Add(m_account_suspendI, 0, wxALL, CONTROL_BORDER);
	sizerTmpI->Add(m_account_unsuspendI, 0, wxALL, CONTROL_BORDER);
	sizerBox->Add(sizerTmpI, 0, wxALL, CONTROL_BORDER);
	sizerTop->Add(sizerBox, 1, wxGROW|wxALL, CONTROL_BORDER);

	// 布局，结果集
	m_lctrl_result = new MyListCtrl(this, AccountPage_ResultList, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	wxListItem itemCol;
	itemCol.SetText(g_StrTable[_T("OperationType")]);
	m_lctrl_result->InsertColumn(0, itemCol);

	itemCol.SetText(g_StrTable[_T("Result")]);
	itemCol.SetAlign(wxLIST_FORMAT_CENTER);
	m_lctrl_result->InsertColumn(1, itemCol);

	sizerBox->Add(m_lctrl_result, 1, wxGROW | wxALL, CONTROL_BORDER);	
	sizerAll->Add(sizerTop, 1, wxGROW|wxALL, CONTROL_BORDER);

	// 黑名单
	wxSizer* sizerBlacklist = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *boxBlacklist = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("Blacklist")]);
	wxSizer* sizerBoxI = new wxStaticBoxSizer(boxBlacklist, wxHORIZONTAL);

	wxSizer* sizerTmpBlack = new wxBoxSizer(wxHORIZONTAL);
	sizerBoxI->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("BlackIP")], wxID_ANY, &m_text_IP),
		0,  wxALL, CONTROL_BORDER);
	sizerBoxI->Add(CreateSizerWithTextAndLabel(this, g_StrTable[_T("SectionName")], wxID_ANY, &m_text_sectionname_I),
		0,  wxALL, CONTROL_BORDER);

	m_add_blacklist = new wxButton(this, AccountPage_AddBlack, g_StrTable[_T("AddBlacklist")]);
	m_del_blacklist = new wxButton(this, AccountPage_DelBlack, g_StrTable[_T("DelFromBlack")]);
	sizerTmpBlack->Add(m_add_blacklist, 0, wxALL, CONTROL_BORDER);
	sizerTmpBlack->Add(m_del_blacklist, 0, wxALL, CONTROL_BORDER);
	sizerBoxI->Add(sizerTmpBlack, 0, wxALL, CONTROL_BORDER);

	sizerBlacklist->Add(sizerBoxI, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerBlacklist, 1, wxGROW|wxALL, CONTROL_BORDER);
	SetSizer(sizerAll);
	sizerAll->Fit(this);

	sUserFrameMgrPgr->RegisterEventHandle(_T("tagAccountSuspend"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnAccountSuspendEvent));
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagIPBlacklist"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnIPBlacklistEvent));
}

void AccountPage::OnAccountSuspendOperation(wxCommandEvent& event)
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_AccountForbid")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 询问玩家是否确定这样做
	wxMessageDialog dialog( NULL, g_StrTable[_T("ConfirmSuspend")], wxT(""), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	if( dialog.ShowModal() == wxID_NO) return;

	tstring strAccountStr = m_text_account->GetValue();
	tstring strSection = m_text_sectionname->GetValue();
	tstring strWorld = m_text_worldname->GetValue();

	// 检查输入的大区名和游戏世界名是否正确
	if( _tcscmp(strWorld.c_str(), _T("")) != 0 )
	{
		if( !sDataMgrPtr->IsSectionWorldValid(strSection, strWorld))
		{
			wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
	}
	else
	{
		if( !sDataMgrPtr->IsSectionValid(strSection))
		{
			wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
	}
	
	INT nLength = sizeof(tagNGMTC_AccountForbid) + sizeof(TCHAR)*(strAccountStr.length());
	tagNGMTC_AccountForbid* pSend = (tagNGMTC_AccountForbid*) malloc(nLength);
	pSend->dwID = Crc32("NGMTC_AccountForbid");
	pSend->dwSize = nLength;
	_sntprintf( pSend->szSectionName, X_SHORT_NAME, strSection.c_str());
	_sntprintf( pSend->szWorldName, X_SHORT_NAME, strWorld.c_str());
	pSend->nAction = 1;
	_tcsncpy(pSend->szAccountStr, strAccountStr.c_str(), strAccountStr.length());
	
	pSend->szAccountStr[strAccountStr.length()] = _T('\0');

	sNetSessionMgrPtr->SendMsg( pSend, pSend->dwSize );

	if(P_VALID(pSend)) free(pSend);
}

void AccountPage::OnAccountUnsuspendOperation(wxCommandEvent& event)
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_AccountForbid")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 询问玩家是否确定这样做
	wxMessageDialog dialog( NULL, g_StrTable[_T("ConfirmUnSuspend")], wxT(""), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	if( dialog.ShowModal() == wxID_NO) return;

	tstring strAccountStr = m_text_account->GetValue();
	tstring strSection = m_text_sectionname->GetValue();
	tstring strWorld = m_text_worldname->GetValue();

	// 检查输入的大区名和游戏世界名是否正确
	if( _tcscmp(strWorld.c_str(), _T("")) != 0 )
	{
		if( !sDataMgrPtr->IsSectionWorldValid(strSection, strWorld))
		{
			wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
	}
	else
	{
		if( !sDataMgrPtr->IsSectionValid(strSection))
		{
			wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
	}

	INT nLength = sizeof(tagNGMTC_AccountForbid) + strAccountStr.length()*sizeof(TCHAR);
	tagNGMTC_AccountForbid* pSend = (tagNGMTC_AccountForbid*) malloc(nLength);
	pSend->dwID = Crc32("NGMTC_AccountForbid");
	pSend->dwSize = nLength;
	_sntprintf( pSend->szSectionName, X_SHORT_NAME, strSection.c_str());
	_sntprintf( pSend->szWorldName, X_SHORT_NAME, strWorld.c_str());
	pSend->nAction = 0;
	_sntprintf( pSend->szAccountStr, strAccountStr.length(), strAccountStr.c_str());
	pSend->szAccountStr[strAccountStr.length()] = '\0';

	sNetSessionMgrPtr->SendMsg( pSend, pSend->dwSize );
	if(P_VALID(pSend)) free(pSend);
}

DWORD AccountPage::OnAccountSuspendEvent(tagUserEvent* pGameEvent)
{
	if( pGameEvent->strEventName == _T("tagAccountSuspend") )
	{
		tagAccountSuspend* pEvt = (tagAccountSuspend*)pGameEvent;
		TCHAR szBuff[X_LONG_STRING] = {0};
		
		switch(pEvt->dwErrorCode)
		{
		case 0:
			wxMessageBox(g_StrTable[_T("SuspendOK")], wxT(""), wxOK|wxCENTRE, this);
			break;
		case E_Login_GMServer_Freed:
			_sntprintf( szBuff, X_LONG_STRING, g_StrTable[_T("AlreadyFreed")], pEvt->szAccount );
			wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
			break;
		case E_Login_GMServer_Forbidded:
			_sntprintf( szBuff, X_LONG_STRING, g_StrTable[_T("AlreadySuspend")], pEvt->szAccount );
			wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
			break;
		case E_Login_GMServer_NoAccount:
			_sntprintf( szBuff, X_LONG_STRING, g_StrTable[_T("NoAccount")], pEvt->szAccount );
			wxMessageBox(szBuff, wxT(""), wxOK|wxCENTRE, this);
			break;
		case E_RT_Error:
			wxMessageBox(g_StrTable[_T("ErrorDone")], wxT(""), wxOK|wxCENTRE, this);
			break;
		default:
			break;
		}
	}

	return 0;
}

void AccountPage::OnIPBlacklistAdd(wxCommandEvent& event)
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_Blacklist")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strIP = m_text_IP->GetValue();
	tstring strSectionName = m_text_sectionname_I->GetValue();

	// 检查输入的大区名和游戏世界名是否正确
	if( !sDataMgrPtr->IsSectionValid(strSectionName))
	{
		wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	char szIP[X_SHORT_NAME] = {0};
	WideCharToMultiByte(CP_ACP, NULL, strIP.c_str(), -1, szIP, X_SHORT_NAME, NULL, FALSE);
	DWORD dwIP = inet_addr(szIP);

	tagNGMTC_Blacklist send;
	send.dwIP = dwIP;
	send.nAction = 1;
	_sntprintf(send.szSectionName, X_SHORT_NAME, strSectionName.c_str());

	sNetSessionMgrPtr->SendMsg( &send, send.dwSize );
}

void AccountPage::OnIPBlacklistDel(wxCommandEvent& event)
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_Blacklist")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strIP = m_text_IP->GetValue();
	tstring strSectionName = m_text_sectionname_I->GetValue();

	// 检查输入的大区名和游戏世界名是否正确
	if( !sDataMgrPtr->IsSectionValid(strSectionName))
	{
		wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	char szIP[X_SHORT_NAME] = {0};
	WideCharToMultiByte(CP_ACP, NULL, strIP.c_str(), -1, szIP, X_SHORT_NAME, NULL, FALSE);
	DWORD dwIP = inet_addr(szIP);

	tagNGMTC_Blacklist send;
	send.dwIP = dwIP;
	send.nAction = 0;
	_sntprintf(send.szSectionName, X_SHORT_NAME, strSectionName.c_str());

	sNetSessionMgrPtr->SendMsg( &send, send.dwSize );
}

DWORD AccountPage::OnIPBlacklistEvent(tagUserEvent* pGameEvent)
{
	if( pGameEvent->strEventName == _T("tagIPBlacklist") )
	{
		tagIPBlacklist* pEvt = (tagIPBlacklist*)pGameEvent;
		TCHAR szBuff[X_LONG_STRING] = {0};
		switch(pEvt->dwErrorCode)
		{
		case E_RT_Success:
			wxMessageBox(g_StrTable[_T("SuspendOK")], wxT(""), wxOK|wxCENTRE, this);
			break;
		case E_RT_Error:
			wxMessageBox(g_StrTable[_T("ErrorDone")], wxT(""), wxOK|wxCENTRE, this);
			break;
		default:
			break;
		}
	}

	return 0;
}

void AccountPage::OnAccountFileSuspendOperation(wxCommandEvent& event)
{
	// 判断权限
 	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_AccountForbid")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 询问玩家是否确定这样做
	wxMessageDialog dialog( NULL, g_StrTable[_T("ConfirmFileSuspend")], wxT(""), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	if( dialog.ShowModal() == wxID_NO) return;

	// 打开文件
	//tstring strFileName =  wxT("D:\\Account.txt"); 
	//_sntprintf( strFileName.c_str(), X_SHORT_NAME, wxT("D:\\Account.txt"));
	tstring strFileName = m_text_accountfile->GetValue();
	std::wifstream infile(strFileName.c_str());
	if (!infile.good())
	{
		wxMessageBox(g_StrTable[_T("ReadFileFail")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strSection = m_text_sectionnameI->GetValue();
	tstring strWorld = m_text_worldnameI->GetValue();
	// 检查输入的大区名和游戏世界名是否正确
	if( _tcscmp(strWorld.c_str(), _T("")) != 0 )
	{
		if( !sDataMgrPtr->IsSectionWorldValid(strSection, strWorld))
		{
			wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
	}
	else
	{
		if( !sDataMgrPtr->IsSectionValid(strSection))
		{
			wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
	}

	// 读取文件，组装消息
	TCHAR szLineStr[X_HUGE_STRING*30] = {0};
	TCHAR szAccountStr[X_HUGE_STRING*30] = {0};
	INT nTmpLength = 1;
	while( infile.getline(szLineStr, X_HUGE_STRING*30) )
	{	
		nTmpLength = nTmpLength + _tcslen(szLineStr);
		if( nTmpLength > X_HUGE_STRING*30 ) break;

		_tcscat_s(szAccountStr, nTmpLength, szLineStr);
		ZeroMemory(szLineStr, X_HUGE_STRING*30);
	}

	INT nLength = sizeof(tagNGMTC_AccountForbid) + (_tcslen(szAccountStr)+1)*sizeof(TCHAR);
	tagNGMTC_AccountForbid* pSend = (tagNGMTC_AccountForbid*) malloc(nLength);
	pSend->dwID = Crc32("NGMTC_AccountForbid");
	pSend->dwSize = nLength;
	_sntprintf( pSend->szSectionName, X_SHORT_NAME, strSection.c_str());
	_sntprintf( pSend->szWorldName, X_SHORT_NAME, strWorld.c_str());
	pSend->nAction = 1;
	_sntprintf( pSend->szAccountStr, _tcslen(szAccountStr), szAccountStr);
	pSend->szAccountStr[_tcslen(szAccountStr)] = '\0';

	sNetSessionMgrPtr->SendMsg( pSend, pSend->dwSize );

	if(P_VALID(pSend)) free(pSend);
}

void AccountPage::OnAccountFileUnsuspendOperation(wxCommandEvent& event)
{
	// 判断权限
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_AccountForbid")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	// 询问玩家是否确定这样做
	wxMessageDialog dialog( NULL, g_StrTable[_T("ConfirmFileUnSuspend")], wxT(""), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);
	if( dialog.ShowModal() == wxID_NO) return;

	// 打开文件
	//tstring strFileName = wxT("D:\\Account.txt"); 
	//_sntprintf( strFileName.c_str(), X_SHORT_NAME, wxT("D:\\Account.txt"));
	tstring strFileName = m_text_accountfile->GetValue();
	std::wifstream infile(strFileName.c_str());
	if (!infile.good())
	{
		wxMessageBox(g_StrTable[_T("ReadFileFail")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strSection = m_text_sectionnameI->GetValue();
	tstring strWorld = m_text_worldnameI->GetValue();
	// 检查输入的大区名和游戏世界名是否正确
	if( _tcscmp(strWorld.c_str(), _T("")) != 0 )
	{
		if( !sDataMgrPtr->IsSectionWorldValid(strSection, strWorld))
		{
			wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
	}
	else
	{
		if( !sDataMgrPtr->IsSectionValid(strSection))
		{
			wxMessageBox(g_StrTable[_T("WorldNameError")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
	}

	// 读取文件，组装消息
	TCHAR szLineStr[X_HUGE_STRING*30] = {0};
	TCHAR szAccountStr[X_HUGE_STRING*30] = {0};
	INT nTmpLength = 1;
	while( infile.getline(szLineStr, X_HUGE_STRING*30) )
	{	
		nTmpLength = nTmpLength + _tcslen(szLineStr);
		if( nTmpLength > X_HUGE_STRING*30 ) break;

		_tcscat_s(szAccountStr, nTmpLength, szLineStr);
		ZeroMemory(szLineStr, X_HUGE_STRING*30);
	}

	INT nLength = sizeof(tagNGMTC_AccountForbid) + (_tcslen(szAccountStr)+1)*sizeof(TCHAR);
	tagNGMTC_AccountForbid* pSend = (tagNGMTC_AccountForbid*) malloc(nLength);
	pSend->dwID = Crc32("NGMTC_AccountForbid");
	pSend->dwSize = nLength;
	_sntprintf( pSend->szSectionName, X_SHORT_NAME, strSection.c_str());
	_sntprintf( pSend->szWorldName, X_SHORT_NAME, strWorld.c_str());
	pSend->nAction = 0;
	_sntprintf( pSend->szAccountStr, _tcslen(szAccountStr), szAccountStr);
	pSend->szAccountStr[_tcslen(szAccountStr)] = '\0';

	sNetSessionMgrPtr->SendMsg( pSend, pSend->dwSize );

	if(P_VALID(pSend)) free(pSend);
}