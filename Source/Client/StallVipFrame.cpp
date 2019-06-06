#include "stdafx.h"
#include "StallVipFrame.h"
#include "StallMgr.h"
#include "PlayerNameTab.h"
#include "CurrencyMgr.h"
#include "..\WorldDefine\stall_define.h"
#include "..\WorldDefine\msg_stall.h"


StallVipFrame::StallVipFrame( void ):m_Trunk(this)
{
	m_pWnd					= NULL;
	m_pBtnRent				= NULL;
	m_pBtnCheck				= NULL;
	m_pBtnCancel			= NULL;
	m_pLVipShop				= NULL;
	m_nCurSelectedRow		= GT_INVALID;
}

StallVipFrame::~StallVipFrame( void )
{

}

BOOL StallVipFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	m_pCmdMgr->Register("NS_ApplyVIPStall",			(NETMSGPROC)m_Trunk.sfp2(&StallVipFrame::OnNS_ApplyVIPStall),	_T("NS_ApplyVIPStall"));
	m_pCmdMgr->Register("NS_SpecVIPStallGet",		(NETMSGPROC)m_Trunk.sfp2(&StallVipFrame::OnNS_SpecVIPStallGet),	_T("NS_SpecVIPStallGet"));


	m_pMgr->RegisterEventHandle(_T("AllVipStallInfoEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StallVipFrame::OnAllVipStallInfoEvent));
	m_pMgr->RegisterEventHandle(_T("RentVipStall_Event"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StallVipFrame::OnRentVipStallEvent));

	return bRet;
}

BOOL StallVipFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	m_pCmdMgr->UnRegister("NS_ApplyVIPStall",		(NETMSGPROC)m_Trunk.sfp2(&StallVipFrame::OnNS_ApplyVIPStall));
	m_pCmdMgr->UnRegister("NS_SpecVIPStallGet",		(NETMSGPROC)m_Trunk.sfp2(&StallVipFrame::OnNS_SpecVIPStallGet));


	m_pMgr->UnRegisterEventHandler(_T("AllVipStallInfoEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallVipFrame::OnAllVipStallInfoEvent));
	m_pMgr->UnRegisterEventHandler(_T("RentVipStall_Event"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallVipFrame::OnRentVipStallEvent));

	if( P_VALID(GetObj("VipStallError_MsgBox")) )
		KillObj("VipStallError_MsgBox");
	if( P_VALID(GetObj("RentVipStall_MsgBox")) )
		KillObj("RentVipStall_MsgBox");

	if (P_VALID(m_pWnd))
	{
		m_pGUI->AddToDestroyList(m_pWnd);
	}

	return bRet;
}

BOOL StallVipFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\vipstore.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), (VUIEVENTHANDLE)m_Trunk.sfp1(&StallVipFrame::EventHandler));
	

	m_pBtnRent = (GUIButton*)m_pWnd->GetChild(_T("vipshop_pic\\rentbutt"));
	m_pBtnCheck = (GUIButton*)m_pWnd->GetChild(_T("vipshop_pic\\checkbutt"));
	m_pBtnCancel = (GUIButton*)m_pWnd->GetChild(_T("vipshop_pic\\closebutt"));
	m_pLVipShop	= (GUIListBox*)m_pWnd->GetChild(_T("vipshop_pic\\listbackpic\\shoplist"));
	m_pLVipShop->SetColWidth(0, 30);
	m_pLVipShop->SetColWidth(1, 250);
	m_pLVipShop->SetColWidth(2, 50);

	return TRUE;
}

BOOL StallVipFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	tagGameEvent evt(_T("OpenVipStall"), NULL);
	m_pMgr->SendEvent(&evt);
	return TRUE;
}

DWORD StallVipFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return 0;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnRent )
			{
				//自己已经有了vip摊位,不可再申请
				if( StallMgr::Inst()->IsSelfVipStall() )
				{
					if( P_VALID(GetObj("VipStallError_MsgBox")) )
						KillObj("VipStallError_MsgBox");
					CreateObj("VipStallError_MsgBox", "MsgBoxEx");

					TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error1")], 
						_T("VipStallError_Event"), MsgBoxFlag(MBF_OK), TRUE);
					return 0;
				}

				//判断该堆位是否可租
				if( !StallMgr::Inst()->IsCanRentVipStall((BYTE)m_nCurSelectedRow) )
				{
					if( P_VALID(GetObj("VipStallError_MsgBox")) )
						KillObj("VipStallError_MsgBox");
					CreateObj("VipStallError_MsgBox", "MsgBoxEx");

					TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error2")], 
						_T("VipStallError_Event"), MsgBoxFlag(MBF_OK), TRUE);
					return 0;
				}

				tagVIPStall* pStall = NULL;
				pStall = StallMgr::Inst()->FinVipStallInfo((BYTE)m_nCurSelectedRow);
				if( P_VALID(pStall) )
				{
					TCHAR szBuff[X_LONG_NAME] = {0};
					_sntprintf( szBuff, X_LONG_NAME, g_StrTable[_T("VipStall_4")], pStall->nRent );
					if( P_VALID(GetObj("RentVipStall_MsgBox")) )
						KillObj("RentVipStall_MsgBox");
					CreateObj("RentVipStall_MsgBox", "MsgBox");

					TObjRef<MsgBox>("RentVipStall_MsgBox")->Init(_T(""), szBuff, 
						_T("RentVipStall_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
				}
			}
			else if( pWnd == m_pBtnCheck )
			{
				tagVIPStall* pStall = NULL;
				pStall = StallMgr::Inst()->FinVipStallInfo((BYTE)m_nCurSelectedRow);
				if( P_VALID(pStall) )
				{
					tagVipStallEvent evt(_T("tagVipStallEvent"), NULL);
					evt.dwRoleID = pStall->dwOwnerID;
					evt.byIndex = pStall->byStallIndex;
					m_pMgr->SendEvent(&evt);
				}
			}
			else if( pWnd == m_pBtnCancel )
			{
				tagGameEvent evt(_T("OpenVipStall"), NULL);
				m_pMgr->SendEvent(&evt);
			}
		}
		break;
	case EGUIE_ItemClick:
		{
			if( pWnd == m_pLVipShop )
			{
				m_nCurSelectedRow = pEvent->dwParam1;
				if( StallMgr::Inst()->IsCanCheckVipStall((BYTE)m_nCurSelectedRow) )
					m_pBtnCheck->SetEnable(true);
				else
					m_pBtnCheck->SetEnable(false);

				if( StallMgr::Inst()->IsCanRentVipStall((BYTE)m_nCurSelectedRow) )
					m_pBtnRent->SetEnable(true);
				else
					m_pBtnRent->SetEnable(false);

				m_pWnd->SetRefresh(TRUE);
			}
		}
		break;
	default:
		break;
	}
	return 1;
}

DWORD StallVipFrame::OnAllVipStallInfoEvent( tagGameEvent* pEventGame )
{
	if( !P_VALID(m_pLVipShop) )
		return 0;

	map<BYTE, tagVIPStall> vip;
	map<BYTE, tagVIPStall>::iterator it;
	vip = StallMgr::Inst()->GetVipStallInfo();
	INT row = 0;
	for( it=vip.begin(); it!=vip.end(); ++it )
	{
		tagVIPStall* pStall = &it->second;
		if( P_VALID(pStall) )
		{
			switch(pStall->eStallStatus)
			{
			case EVSS_Open:
				{
					TCHAR szBuff[X_SHORT_NAME] = {0};
					_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pStall->byStallIndex+1 );
					m_pLVipShop->SetText( row, 0, szBuff );
					tstring strName = PlayerNameTab::Inst()->FindNameByID(pStall->dwOwnerID);
					if( P_VALID(pStall->szStallTitle) && _tcslen(pStall->szStallTitle)>0 )
						m_pLVipShop->SetText( row, 1, pStall->szStallTitle );
					else if( !strName.empty() )
						m_pLVipShop->SetText( row, 1, strName.c_str() );
					m_pLVipShop->SetText( row, 2, g_StrTable[_T("VipStall_0")], 0xFF51DC3E );
				}
				break;
			case EVSS_Close:
				{
					TCHAR szBuff[X_SHORT_NAME] = {0};
					_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pStall->byStallIndex+1 );
					m_pLVipShop->SetText( row, 0, szBuff );
					tstring strName = PlayerNameTab::Inst()->FindNameByID(pStall->dwOwnerID);
					if( !strName.empty() )
						m_pLVipShop->SetText( row, 1, strName.c_str() );
					else 
						m_pLVipShop->SetText( row, 1, pStall->szStallTitle );
					m_pLVipShop->SetText( row, 2, g_StrTable[_T("VipStall_1")], 0xFFFF4200 );
				}
				break;
			case EVSS_ForHire:
				{
					TCHAR szBuff[X_SHORT_NAME] = {0};
					_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pStall->byStallIndex+1 );
					m_pLVipShop->SetText( row, 0, szBuff );
					_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("VipStall_3")], pStall->nRent );
					m_pLVipShop->SetText( row, 1, szBuff );
					m_pLVipShop->SetText( row, 2, g_StrTable[_T("VipStall_2")], 0xFF63D5FF );
				}
				break;
			default:
				break;
			}
			++row;
		}
	}
	tagGUIEvent evt(m_pLVipShop, EGUIE_ItemClick);
	evt.dwParam1 = 0;
	m_pLVipShop->SendEvent(&evt);
	m_pWnd->SetRefresh(TRUE);
	return 0;
}

DWORD StallVipFrame::OnRentVipStallEvent( tagMsgBoxEvent* pEventGame )
{
	if( pEventGame->eResult==MBF_OK )
	{
		tagVIPStall* pStall = NULL;
		pStall = StallMgr::Inst()->FinVipStallInfo((BYTE)m_nCurSelectedRow);
		if( P_VALID(pStall) )
		{
			//判断玩家身上的元宝
			if( pStall->nRent > CurrencyMgr::Inst()->GetBagYuanbao() )
			{
				if( P_VALID(GetObj("VipStallError_MsgBox")) )
					KillObj("VipStallError_MsgBox");
				CreateObj("VipStallError_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error3")], 
					_T("VipStallError_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
				return 0;
			}

			tagNC_ApplyVIPStall msg;
			msg.byIndex = (BYTE)m_nCurSelectedRow;
			msg.nRent = pStall->nRent;
			m_pSession->Send(&msg);
		}
	}
	return 0;
}

DWORD StallVipFrame::OnNS_ApplyVIPStall( tagNS_ApplyVIPStall* pMsg, DWORD dwParam)
{
	switch( pMsg->dwErrorCode )
	{
	case E_Success:
		{			
			if( P_VALID(GetObj("VipStallError_MsgBox")) )
				KillObj("VipStallError_MsgBox");
			CreateObj("VipStallError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_5")], 
				_T("VipStallError_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
		}
		break;
	case E_Stall_VIP_Own_Another:
		{
			if( P_VALID(GetObj("VipStallError_MsgBox")) )
				KillObj("VipStallError_MsgBox");
			CreateObj("VipStallError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error1")], 
				_T("VipStallError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Stall_VIP_Info_NotExist:
		{
			if( P_VALID(GetObj("VipStallError_MsgBox")) )
				KillObj("VipStallError_MsgBox");
			CreateObj("VipStallError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error5")], 
				_T("VipStallError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Stall_VIP_Already_Rent:
		{
			if( P_VALID(GetObj("VipStallError_MsgBox")) )
				KillObj("VipStallError_MsgBox");
			CreateObj("VipStallError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error2")], 
				_T("VipStallError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_Stall_VIP_Rent_Error:
		{
			if( P_VALID(GetObj("VipStallError_MsgBox")) )
				KillObj("VipStallError_MsgBox");
			CreateObj("VipStallError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error6")], 
				_T("VipStallError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		}
		break;
	case E_BagYuanBao_NotEnough:
		{
			if( P_VALID(GetObj("VipStallError_MsgBox")) )
				KillObj("VipStallError_MsgBox");
			CreateObj("VipStallError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error3")], 
				_T("VipStallError_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
		}
		break;
	default:
		{
			if( P_VALID(GetObj("VipStallError_MsgBox")) )
				KillObj("VipStallError_MsgBox");
			CreateObj("VipStallError_MsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("VipStallError_MsgBox")->Init(_T(""), g_StrTable[_T("SystemError")], 
				_T("VipStallError_Event"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE);
		}
		break;
	}

	return 0;
}

DWORD StallVipFrame::OnNS_SpecVIPStallGet( tagNS_SpecVIPStallGet* pMsg, DWORD dwParam)
{
	switch( pMsg->dwErrorCode )
	{
	case E_Success:
		break;
	default:
		break;
	}
	return 0;
}