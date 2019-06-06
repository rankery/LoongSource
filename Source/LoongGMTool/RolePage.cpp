#include "stdafx.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"
#include "wx/panel.h"
#include "wx/bookctrl.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "LoongGMTool.h"
#include "common.h"
#include "RolePage.h"
#include "mylistctrl.h"
#include "resource.h"
#include "UserFrameMgr.h"
#include "DataMgr.h"
#include "NetSessionMgr.h"
#include "ItemProtoData.h"
#include "..\ServerDefine\msg_gm_tool_c.h"
#define ITEM_MAX ItemProtoData::Inst()->GetItemNum()

BEGIN_EVENT_TABLE(RolePage, wxPanel)
	EVT_BUTTON(RolePage_OkResume, RolePage::OnRoleResumeOperation)
	EVT_BUTTON(RolePage_OkModiyPro, RolePage::OnRoleProModifyOperation)
	EVT_BUTTON(RolePage_OkItemPro, RolePage::OnRoleItemProOperation)
	EVT_BUTTON(RolePage_OkItemDel, RolePage::OnRoleItemDelOperation)
	EVT_BUTTON(RolePage_OKItemResume, RolePage::OnRoleItemResOperation)
	EVT_BUTTON(RolePage_OkClearBaibao, RolePage::OnRoleClearBaibaoOperation)
END_EVENT_TABLE()

RolePage::RolePage( wxBookCtrlBase *book, const wxString& label ):m_label(label), wxPanel(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
																						  wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	//巨大的构造函数
	wxSizer* sizerAll = new wxBoxSizer(wxVERTICAL);

	wxTextValidator validator(wxFILTER_NUMERIC); 

	//布局-上
	wxSizer* sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("RoleResume")]);
	wxSizer* sizerBox = new wxStaticBoxSizer(box, wxVERTICAL);

	wxSizer* sizerTmp = new wxBoxSizer(wxHORIZONTAL);
	wxControl *control1 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("RoleOldName")]);
	wxControl *control2 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("RoleNewName")]);
	m_text_delrole = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	m_text_rolename = new wxTextCtrl(this, wxID_ANY);
	sizerTmp->Add(control1, 0, wxALL, CONTROL_BORDER);
	sizerTmp->Add(m_text_delrole, 0, wxALL, CONTROL_BORDER);
	sizerTmp->Add(control2, 0, wxALL, CONTROL_BORDER);
	sizerTmp->Add(m_text_rolename, 0, wxALL, CONTROL_BORDER);
	m_btn_okresume = new wxButton(this, RolePage_OkResume, g_StrTable[_T("OkResume")]);
	sizerBox->Add(sizerTmp, 0, wxALL, CONTROL_BORDER);
	sizerBox->Add(m_btn_okresume, 0, wxALL, CONTROL_BORDER);
	sizerTop->Add(sizerBox, 1, wxGROW|wxALL, CONTROL_BORDER);


	//布局-中
	wxSizer* sizerMin = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *boxMin = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("ProModify")]);
	wxSizer* sizerBoxMin = new wxStaticBoxSizer(boxMin, wxVERTICAL);
	wxSizer* sizerTmp1 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sizerTmp2 = new wxBoxSizer(wxHORIZONTAL);

	wxControl *control3 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("RoleName")]);
	m_text_prorole =  new wxTextCtrl(this, wxID_ANY);
	wxControl *control4 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("Level")]);
	m_text_level =  new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	wxControl *control5 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("Exp")]);
	m_text_exp = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	wxControl *control6 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("SkillNum")]);
	m_text_skillnum = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	wxControl *control7 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("Money")]);
	m_text_money = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	wxControl *control8 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("Yuanbao")]);
	m_text_yuanbao = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	sizerTmp1->Add(control3, 0, wxALL, CONTROL_BORDER);
	sizerTmp1->Add(m_text_prorole, 0, wxALL, CONTROL_BORDER);
	sizerTmp1->Add(control4, 0, wxALL, CONTROL_BORDER);
	sizerTmp1->Add(m_text_level, 0, wxALL, CONTROL_BORDER);
	sizerTmp1->Add(control5, 0, wxALL, CONTROL_BORDER);
	sizerTmp1->Add(m_text_exp, 0, wxALL, CONTROL_BORDER);
	sizerTmp1->Add(control6, 0, wxALL, CONTROL_BORDER);
	sizerTmp1->Add(m_text_skillnum, 0, wxALL, CONTROL_BORDER);
	sizerTmp2->Add(control7, 0, wxALL, CONTROL_BORDER);
	sizerTmp2->Add(m_text_money, 0, wxALL, CONTROL_BORDER);
	sizerTmp2->Add(control8, 0, wxALL, CONTROL_BORDER);
	sizerTmp2->Add(m_text_yuanbao, 0, wxALL, CONTROL_BORDER);
	m_btn_okmodipro = new wxButton(this, RolePage_OkModiyPro, g_StrTable[_T("OkModify")]);
	sizerBoxMin->Add(sizerTmp1, 0, wxALL, CONTROL_BORDER);
	sizerBoxMin->Add(sizerTmp2, 0, wxALL, CONTROL_BORDER);	
	sizerBoxMin->Add(m_btn_okmodipro, 0, wxALL, CONTROL_BORDER);
	sizerMin->Add(sizerBoxMin, 1,  wxGROW|wxALL, CONTROL_BORDER);

	//布局-下
	wxSizer* sizerBom = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *boxBom = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("ItemProduce")]);
	wxSizer* sizerBoxBom = new wxStaticBoxSizer(boxBom, wxVERTICAL);
	wxSizer* sizerTmp3 = new wxBoxSizer(wxHORIZONTAL);
	wxControl *contro9 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("RoleName")]);
	m_text_itemrole =  new wxTextCtrl(this, wxID_ANY);

	//设置物品
	int nItemNum = ItemProtoData::Inst()->GetItemNum();
	std::map<DWORD, tagItem2GM>& item = ItemProtoData::Inst()->GetItem();
	std::map<DWORD, tagItem2GM>::iterator it = item.begin();
	unsigned int i=0;

	wxString strins[10000];
	for( ; it!=item.end() && i<10000; ++it )
	{
		strins[i] = it->second.szName;
		++i;
	}
	m_combox_item = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, nItemNum, strins);

	wxControl *contro10 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("ItemProduce1")]);
	m_text_num =  new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	wxControl *contro11 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("ItemQuality")]);
	m_text_quality =  new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	m_btn_okitem = new wxButton(this, RolePage_OkItemPro, g_StrTable[_T("OkItemProduce")]);
	sizerTmp3->Add(contro9, 0, wxALL, CONTROL_BORDER);
	sizerTmp3->Add(m_text_itemrole, 0, wxALL, CONTROL_BORDER);
	sizerTmp3->Add(m_combox_item, 0, wxALL, CONTROL_BORDER);
	sizerTmp3->Add(contro10, 0, wxALL, CONTROL_BORDER);
	sizerTmp3->Add(m_text_num, 0, wxALL, CONTROL_BORDER);
	sizerTmp3->Add(contro11, 0, wxALL, CONTROL_BORDER);
	sizerTmp3->Add(m_text_quality, 0, wxALL, CONTROL_BORDER);
	sizerBoxBom->Add(sizerTmp3, 0, wxALL, CONTROL_BORDER);
	sizerBoxBom->Add(m_btn_okitem, 0, wxALL, CONTROL_BORDER);
	sizerBom->Add(sizerBoxBom, 1, wxGROW|wxALL, CONTROL_BORDER);

	//布局-道具删除
	wxSizer* sizerBom1 = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *boxBom1 = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("ItemDelete")]);
	wxSizer* sizerBoxBom1 = new wxStaticBoxSizer(boxBom1, wxVERTICAL);
	wxSizer* sizerTmp4 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sizerTmp5 = new wxBoxSizer(wxHORIZONTAL);

	wxControl *contro1Del1 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("RoleName")]);
	m_text_itemownername =  new wxTextCtrl(this, wxID_ANY, wxT(""));
	wxControl *contro1Del2 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("ItemGlobalID")]);
	//m_text_itemdelid =  new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	m_text_Item64IDStr =new wxTextCtrl(this, wxID_ANY, wxT(""),  wxDefaultPosition, wxSize(500,18), 0, validator);
	m_btn_okitemdel = new wxButton(this, RolePage_OkItemDel, g_StrTable[_T("OkItemDelete")]);
	
	sizerTmp4->Add(contro1Del1, 0, wxALL, CONTROL_BORDER);
	sizerTmp4->Add(m_text_itemownername, 0, wxALL, CONTROL_BORDER);
	sizerTmp4->Add(contro1Del2, 0, wxALL, CONTROL_BORDER);
	//sizerTmp4->Add(m_text_itemdelid, 0, wxALL, CONTROL_BORDER);
	sizerTmp4->Add(m_text_Item64IDStr, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerTmp4->Add(m_btn_okitemdel, 0, wxALL, CONTROL_BORDER);

	m_btn_clearbaibao = new wxButton(this, RolePage_OkClearBaibao, g_StrTable[_T("ClearBaibao")]);
	wxControl *controlDel3 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("RoleName")]);
	m_text_baibaoownername =  new wxTextCtrl(this, wxID_ANY, wxT(""));

	sizerTmp5->Add(controlDel3, 0, wxALL, CONTROL_BORDER);
	sizerTmp5->Add(m_text_baibaoownername, 0, wxALL, CONTROL_BORDER);
	sizerTmp5->Add(m_btn_clearbaibao, 0, wxALL, CONTROL_BORDER);

	sizerBoxBom1->Add(sizerTmp4, 0, wxALL, CONTROL_BORDER);
	sizerBoxBom1->Add(sizerTmp5, 0, wxALL, CONTROL_BORDER);
	//sizerBoxBom1->Add(m_btn_okitemdel, 0, wxALL, CONTROL_BORDER);
	sizerBom1->Add(sizerBoxBom1, 1, wxGROW|wxALL, CONTROL_BORDER);

	//布局-道具恢复
	wxSizer* sizerItemResume = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBox *boxItemResume = new wxStaticBox(this, wxID_ANY, g_StrTable[_T("ItemResume")]);
	wxSizer* sizerBoxItemResume = new wxStaticBoxSizer(boxItemResume, wxHORIZONTAL);

	wxSizer* sizerTmpItemResume = new wxBoxSizer(wxHORIZONTAL);
	wxControl *controlItemResume1 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("ItemSerialNum")]);
	wxControl *controlItemResume2 = new wxStaticText(this, wxID_ANY, g_StrTable[_T("TargrtRoleName")]);
	//m_text_resumeitemid = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, validator);
	m_text_resumeitemidstr = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(500,18), 0, validator);
	m_text_targetrolename = new wxTextCtrl(this, wxID_ANY);
	sizerTmpItemResume->Add(controlItemResume1, 0, wxALL, CONTROL_BORDER);
	//sizerTmpItemResume->Add(m_text_resumeitemid, 0, wxALL, CONTROL_BORDER);
	sizerTmpItemResume->Add(m_text_resumeitemidstr, 1, wxALL | wxGROW, CONTROL_BORDER);
	sizerTmpItemResume->Add(controlItemResume2, 0, wxALL, CONTROL_BORDER);
	sizerTmpItemResume->Add(m_text_targetrolename, 0, wxALL, CONTROL_BORDER);
	m_btn_okitemresume = new wxButton(this, RolePage_OKItemResume, g_StrTable[_T("OkItemResume")]);
	sizerTmpItemResume->Add(m_btn_okitemresume, 0, wxALL, CONTROL_BORDER);
	sizerBoxItemResume->Add(sizerTmpItemResume, 0, wxALL, CONTROL_BORDER);
	//sizerBoxItemResume->Add(m_btn_okitemresume, 0, wxALL, CONTROL_BORDER);
	sizerItemResume->Add(sizerBoxItemResume, 1, wxGROW|wxALL, CONTROL_BORDER);

	sizerAll->Add(sizerTop, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerMin, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerBom, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerBom1, 1, wxGROW|wxALL, CONTROL_BORDER);
	sizerAll->Add(sizerItemResume, 1, wxGROW|wxALL, CONTROL_BORDER);
	SetSizer(sizerAll);
	sizerAll->Fit(this);

	sUserFrameMgrPgr->RegisterEventHandle(_T("tagRoleResume"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnRolePageEvent));
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagRoleProModify"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnRolePageEvent));
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagItemCreate"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnRolePageEvent));
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagItemDelete"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnRolePageEvent));
	sUserFrameMgrPgr->RegisterEventHandle(_T("tagItemResume"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&UserFrame::OnRolePageEvent));
}

void RolePage::OnRoleResumeOperation( wxCommandEvent& event )
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_RoleResume")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}
	tstring strOldName = m_text_delrole->GetValue();
	tstring strNewName = m_text_rolename->GetValue();

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

		if( (int)vec.size()>1 )
		{
			wxMessageBox(g_StrTable[_T("MorethanWorld")], wxT(""), wxOK|wxCENTRE, this);
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
				tagNGMTC_RoleResume msg;
				msg.nOldID = _tstoi(strOldName.c_str());
				_sntprintf( msg.szNewName, X_SHORT_NAME, strNewName.c_str() );
				msg.dwSectionID = id.section_id;
				msg.dwWorldID = id.world_id;
				sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );
			}
		}
	}	
}

void RolePage::OnRoleProModifyOperation( wxCommandEvent& event )
{

	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_ProModify")) )
	{
		wxMessageBox(g_StrTable[_T("GMPrivilegeForbit")], wxT(""), wxOK|wxCENTRE, this);
		return;
	}

	tstring strRoleName = m_text_prorole->GetValue();

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

		if( (int)vec.size()>1 )
		{
			wxMessageBox(g_StrTable[_T("MorethanWorld")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}

		tstring strLel = m_text_level->GetValue().c_str();
		tstring strSkill = m_text_skillnum->GetValue().c_str();
		tstring strExp = m_text_exp->GetValue().c_str();
		tstring strMoney = m_text_money->GetValue().c_str();
		tstring strYuan = m_text_yuanbao->GetValue().c_str();

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
				tagNGMTC_ProModify msg;
				_sntprintf( msg.szName, X_SHORT_NAME, strRoleName.c_str() );
				if(strLel.empty())
					msg.nLel = GT_INVALID;
				else
					msg.nLel = _tstoi( strLel.c_str() );
				if(strSkill.empty())
					msg.nSkill = GT_INVALID;
				else
					msg.nSkill = _tstoi( strSkill.c_str() );
				if(strExp.empty())
					msg.n64Exp = GT_INVALID;
				else
					msg.n64Exp = _tstol( strExp.c_str() );
				if(strMoney.empty())
					msg.n64Money = GT_INVALID;
				else
					msg.n64Money = _tstol( strMoney.c_str() );
				if(strYuan.empty())
					msg.nYuanbao = GT_INVALID;
				else
					msg.nYuanbao = _tstoi( strYuan.c_str() );
				msg.dwSectionID = id.section_id;
				msg.dwWorldID = id.world_id;
				sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );
			}
		}
	}		
}

DWORD RolePage::OnRolePageEvent( tagUserEvent* pGameEvent )
{
	if( pGameEvent->strEventName == _T("tagRoleResume") )
	{
		tagRoleResume* pEvt = (tagRoleResume*)pGameEvent;
		switch( pEvt->dwErrorCode )
		{
		case E_GMTool_Success:
			{
				wxMessageBox(g_StrTable[_T("RoleResumeMsg0")], wxT(""), wxOK|wxCENTRE, this);
				m_text_delrole->Clear();
				m_text_rolename->Clear();
			}
			break;
		case E_GMTool_ResumeRole_NameAlreadyExist:
			{
				wxMessageBox(g_StrTable[_T("RoleResumeMsg1")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_ResumeRole_RoleNotExist:
			{
				wxMessageBox(g_StrTable[_T("RoleResumeMsg2")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_Err:
		default:
			{
				wxMessageBox(g_StrTable[_T("RoleResumeMsg3")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		}
	}
	else if( pGameEvent->strEventName == _T("tagRoleProModify") )
	{
		tagRoleProModify* pEvt = (tagRoleProModify*)pGameEvent;
		switch( pEvt->dwErrorCode )
		{
		case E_GMTool_Success:
			{
				TCHAR szBuff[X_SHORT_STRING] = {0};
				_sntprintf( szBuff, X_SHORT_STRING, _T("RoleProModifyMsg%u"), pEvt->dwErrorCode );
				wxMessageBox(g_StrTable[szBuff], wxT(""), wxOK|wxCENTRE, this);
				m_text_prorole->Clear();
				m_text_level->Clear();
				m_text_exp->Clear();
				m_text_skillnum->Clear();
				m_text_money->Clear();
				m_text_yuanbao->Clear();
			}
			break;
		case E_GMTool_ModifyRole_RoleNotExist:
			{
				wxMessageBox(g_StrTable[_T("RoleProModifyMsg1")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_Err:
			{
				wxMessageBox(g_StrTable[_T("RoleProModifyMsg2")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		
		}
	}
	else if( pGameEvent->strEventName == _T("tagItemCreate") )
	{
		tagItemCreate* pEvt = (tagItemCreate*)pGameEvent;
		switch ( pEvt->dwErrorCode )
		{
		case E_GMTool_Success:
			{
				wxMessageBox(g_StrTable[_T("ItemCreate0")], wxT(""), wxOK|wxCENTRE, this);
				m_text_itemrole->Clear();
				m_text_num->Clear();
				m_text_quality->Clear();
				m_combox_item->SetValue(wxT(""));
			}
			break;
		case E_GMTool_Err:
			{
				wxMessageBox(g_StrTable[_T("ItemCreate1")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_RoleNotExist:
			{
				wxMessageBox(g_StrTable[_T("RoleNoExist")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_CreateItem_EquipNumErr:
			{
				wxMessageBox(g_StrTable[_T("EquipNumErr")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_DeleteItem_ItemNotExist:
			{
				wxMessageBox(g_StrTable[_T("ItemNotExist")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		}
	}
	else if( pGameEvent->strEventName == _T("tagItemDelete") )
	{	
		tagItemDelete* pEvt = (tagItemDelete*)pGameEvent;
		switch( pEvt->dwErrorCode )
		{
		case E_GMTool_Success:
			{
				wxMessageBox(g_StrTable[_T("ItemDelete0")], wxT(""), wxOK|wxCENTRE, this);
				m_text_itemownername->Clear();
				//m_text_itemdelid->Clear();
				m_text_Item64IDStr->Clear();
			}
			break;
		case E_GMTool_Err:
			{
				wxMessageBox(g_StrTable[_T("ItemDelete1")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_RoleNotExist:
			{
				wxMessageBox(g_StrTable[_T("RoleNoExist")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_DeleteItem_ItemNotExist:
			{
				wxMessageBox(g_StrTable[_T("ItemNotExist")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		}
	}
	else if( pGameEvent->strEventName == _T("tagItemResume") )
	{	
		tagItemResume* pEvt = (tagItemResume*)pGameEvent;
		switch( pEvt->dwErrorCode )
		{
		case E_GMTool_Success:
			{
				wxMessageBox(g_StrTable[_T("ItemResume0")], wxT(""), wxOK|wxCENTRE, this);
				//m_text_resumeitemid->Clear();
				m_text_resumeitemidstr->Clear();
				m_text_targetrolename->Clear();
			}
			break;
		case E_GMTool_Err:
			{
				wxMessageBox(g_StrTable[_T("ItemResume1")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_RoleNotExist:
			{
				wxMessageBox(g_StrTable[_T("RoleNoExist")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_ResumeItemNotExit:
			{
				wxMessageBox(g_StrTable[_T("ItemResume1")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		case E_GMTool_ResumeItemNotEquip:
			{
				wxMessageBox(g_StrTable[_T("ItemResume2")], wxT(""), wxOK|wxCENTRE, this);
			}
			break;
		}
	}

	return 0;
}

void RolePage::OnRoleItemProOperation( wxCommandEvent& event )
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_ItemCreate")) )
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

		if( (int)vec.size()>1 )
		{
			wxMessageBox(g_StrTable[_T("MorethanWorld")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}
		tstring strItemName = m_combox_item->GetValue();
		const tagItem2GM* pItem = ItemProtoData::Inst()->FindItemProto(strItemName);
		if( !P_VALID(pItem) )
		{
			wxMessageBox(g_StrTable[_T("NoSelectItem")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}

		tstring strRoleName = m_text_itemrole->GetValue();		
		tstring strItemNum = m_text_num->GetValue();
		tstring strItemQuality = m_text_quality->GetValue();		

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
				tagNGMTC_ItemCreate msg;
				_sntprintf( msg.szName, X_SHORT_NAME, strRoleName.c_str() );
				msg.dwItemTypeID = pItem->dwTypeID;
				msg.nItemNum = _tstoi(strItemNum.c_str());
				msg.byQuality = (BYTE)_tstoi(strItemQuality.c_str());
				msg.dwSectionID = id.section_id;
				msg.dwWorldID = id.world_id;
				sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );
			}
		}

	}
}

void RolePage::OnRoleItemDelOperation( wxCommandEvent& event )
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_ItemDelete")) )
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

		if( (int)vec.size()>1 )
		{
			wxMessageBox(g_StrTable[_T("MorethanWorld")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}

		tstring strRoleName = m_text_itemownername->GetValue();
		//tstring strID = m_text_itemdelid->GetValue();
		tstring strItem64IDStr = m_text_Item64IDStr->GetValue();

		//获得所选游戏世界ID
		std::map<DWORD, tagSWIdEx> mapIDEx;
		std::map<DWORD, tagSWIdEx>::iterator itEx;
		std::vector<int>::iterator it = vec.begin();
		for( ; it!=vec.end(); it++ )
		{
			tstring strName = pCheckList->GetString(unsigned int(*it));
			int nItemIDStringLen = strItem64IDStr.length();

			tagSWId id;
			sDataMgrPtr->GetSectionAndWorldID(strName, &id);
			
			if( id.section_id!=GT_INVALID && id.world_id!=GT_INVALID )
			{
				int nMsgSize = 0;
				nMsgSize = sizeof(tagNGMTC_ItemDelete) - sizeof(TCHAR) + sizeof(TCHAR) * ( nItemIDStringLen + 1 );
				tagNGMTC_ItemDelete* pMsg = (tagNGMTC_ItemDelete*)malloc(nMsgSize);
				
				pMsg->dwID = Crc32("NGMTC_ItemDelete");
				pMsg->dwSize = nMsgSize;
				_sntprintf( pMsg->szName, X_SHORT_NAME, strRoleName.c_str() );
				_sntprintf( pMsg->sz64IDString, nItemIDStringLen * sizeof(TCHAR), strItem64IDStr.c_str() );
				pMsg->dwSectionID = id.section_id;
				pMsg->dwWorldID = id.world_id;
				sNetSessionMgrPtr->SendMsg( pMsg, nMsgSize );

				if (P_VALID(pMsg))
				{
					free(pMsg);
				}

			}
		}
	}

}

void RolePage::OnRoleItemResOperation( wxCommandEvent& event )
{
	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_ItemResume")) )
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

		if( (int)vec.size()>1 )
		{
			wxMessageBox(g_StrTable[_T("MorethanWorld")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}

		tstring strRoleName = m_text_targetrolename->GetValue();
		//tstring strItemID = m_text_resumeitemid->GetValue();
		tstring strItemIDStr = m_text_resumeitemidstr->GetValue();

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
				int nMsgSize = sizeof(tagNGMTC_ItemResume) - sizeof(TCHAR) + sizeof(TCHAR)*(strItemIDStr.length()+1);
				tagNGMTC_ItemResume* pMsg = (tagNGMTC_ItemResume*)malloc(nMsgSize);

				pMsg->dwID = Crc32("NGMTC_ItemResume");
				pMsg->dwSize = nMsgSize;
				_sntprintf( pMsg->szTargetRole, X_SHORT_NAME, strRoleName.c_str() );
				//msg.n64SerialNum = _tstoi64(strItemID.c_str());
				_sntprintf( pMsg->sz64IDString, strItemIDStr.length()*sizeof(TCHAR), strItemIDStr.c_str());
				pMsg->dwSectionID = id.section_id;
				pMsg->dwWorldID = id.world_id;
				sNetSessionMgrPtr->SendMsg( pMsg, pMsg->dwSize );
			
				if (P_VALID(pMsg))
				{
					free(pMsg);
				}
			}
		}
	}

}

void RolePage::OnRoleClearBaibaoOperation(wxCommandEvent& event)
{

	if( !sNetSessionMgrPtr->VerifyGMPrivilege(Crc32("NGMTC_ClearBaibao")) )
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

		if( (int)vec.size()>1 )
		{
			wxMessageBox(g_StrTable[_T("MorethanWorld")], wxT(""), wxOK|wxCENTRE, this);
			return;
		}

		tstring strRoleName = m_text_baibaoownername->GetValue();

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
				tagNGMTC_ClearBaibao msg;
				_sntprintf( msg.szOwnerName, X_SHORT_NAME, strRoleName.c_str() );
				msg.dwSectionID = id.section_id;
				msg.dwWorldID = id.world_id;
				sNetSessionMgrPtr->SendMsg( &msg, msg.dwSize );
			}
		}
	}
}