#include "stdafx.h"
#include "InsuranceFrame.h"
#include "ComposeStatic.h"
#include "CombatSysUtil.h"
#include "Item.h"
#include "ItemFrame.h"
#include "PocketFrame.h"
#include "Container.h"
#include "ItemMgr.h"
#include "ItemProtoData.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "ItemEvent.h"
#include "ItemFrame.h"
#include "..\WorldDefine\msg_compose.h"

InsuranceFrame::InsuranceFrame( void ):m_Trunk(this)
{
	m_pWnd					= NULL;
	m_pWndBackPic			= NULL;
	m_pBtnOk				= NULL;
	m_pBtnCancel			= NULL;
	m_pStcHint				= NULL;
	m_pStcInsurance			= NULL;
	m_n64SrcID				= GT_INVALID;
	m_n64DstID				= GT_INVALID;
	m_n64InsuranceID		= GT_INVALID;
	m_eType					= EISF_Null;
}

InsuranceFrame::~InsuranceFrame( void )
{

}

BOOL InsuranceFrame::Init( GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	//--注册游戏事件
	m_pMgr->RegisterEventHandle(_T("ItemEvent_Move2Insurance"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&InsuranceFrame::OnEventConsolidateAddItem));

	return TRUE;
}

BOOL InsuranceFrame::Destroy()
{
	if( P_VALID(m_pWnd) )
		m_pGUI->AddToDestroyList(m_pWnd);
	
	//--注销游戏事件
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Insurance"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&InsuranceFrame::OnEventConsolidateAddItem));

	return GameFrame::Destroy();
}

BOOL InsuranceFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\insurance.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	m_pGUI->ChangeXml(&ele, "insurance_win\\insurance_backpic\\itempic\\item", "ClassName", "ComposeStatic");
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);

	m_pWndBackPic	= m_pWnd->GetChild(_T("insurance_backpic"));
	m_pBtnOk		= (GUIButton*)m_pWndBackPic->GetChild(_T("surebutt"));
	m_pBtnCancel	= (GUIButton*)m_pWndBackPic->GetChild(_T("canclebutt"));
	m_pStcHint		= (GUIStatic*)m_pWndBackPic->GetChild(_T("word"));
	m_pStcInsurance = (ComposeStatic*)m_pWndBackPic->GetChild(_T("itempic\\item"));

	//注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&InsuranceFrame::EventHandler));

	ShowFrame(FALSE);

	return TRUE;
}	

VOID InsuranceFrame::Update()
{

}

DWORD InsuranceFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;
	switch(pEvent->eEvent)
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnOk)
			{
				this->SendMsg();
			}
			else if( pWnd == m_pBtnCancel )
			{
				this->ShowFrame(FALSE);
			}
		}
		break;
	case EOnEventSendDropGUI_Item:				//拖动释放到些frame
		{		
			if( pWnd == m_pStcInsurance )
			{
				ItemFrame* pFrame = (ItemFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("Item"));
				if(P_VALID(pFrame))
				{
					EItemStrategy eStrategy = pFrame->GetCurStrategy();
					if( eStrategy!=EIST_ItemInsurance )
						return FALSE;
				}

				DWORD dwTypeID = pEvent->dwParam3;
				if(MIsEquipment(dwTypeID))
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Insurance_Error1")]);
					return FALSE;
				}

				INT16 nPos = INT16(pEvent->dwParam2);
				Item* pItem = ItemMgr::Inst()->GetPocketItem(nPos);
				if(!P_VALID(pItem))
					return FALSE;

				const tagItemProto* pItemProto = pItem->GetProto();
				if( pItemProto->eSpecFunc!=EISF_ProtectSign)
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Insurance_Error2")]);
					return FALSE;
				}

				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
				m_pStcInsurance->SetPic(strPic);
				m_n64InsuranceID = pItem->GetItemId();
				m_pWnd->SetRefresh(TRUE);
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if( pWnd == m_pStcInsurance )
			{
				Item* pItem = ItemMgr::Inst()->GetPocketItemByID(m_n64InsuranceID);
				if( P_VALID(pItem) )
				{
					ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	default:
		break;
	}
	return TRUE;
}

DWORD InsuranceFrame::OnEventConsolidateAddItem( tagItemPosChangeExEvent* pGameEvent )
{
	if( pGameEvent->eTypeSrc != EICT_Bag )
		return 0;

	DWORD dwTypeID = pGameEvent->dwTypeIDSrc;
	if(MIsEquipment(dwTypeID))
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Insurance_Error1")]);
		return 0;
	}

	INT16 nPos = INT16(pGameEvent->n16IndexSrc);
	Item* pItem = ItemMgr::Inst()->GetPocketItem(nPos);
	if(!P_VALID(pItem))
		return 0;

	const tagItemProto* pItemProto = pItem->GetProto();
	if( pItemProto->eSpecFunc!=EISF_ProtectSign)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Insurance_Error2")]);
		return 0;
	}

	tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
	m_pStcInsurance->SetPic(strPic);
	m_n64InsuranceID = pItem->GetItemId();

	m_pWnd->SetRefresh(TRUE);
	return 0;
}

void InsuranceFrame::ShowFrame( BOOL bShow, EItemSpecFunc eType/*=EISF_Null*/, 
							   INT64 n64SrcID/*=GT_INVALID*/, INT64 n64DstID/*=GT_INVALID*/)
{
	if( P_VALID(m_pWnd) )
	{
		m_pWnd->SetInvisible(!bShow);
		if( bShow )
		{	
			m_pWnd->FlipToTop();
		}
		else
		{
			SetStrategy(EIST_ItemPocket);
			m_pStcInsurance->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
		}
	}
	m_eType			= eType;
	m_n64SrcID		= n64SrcID;
	m_n64DstID		= n64DstID;
	
}

void InsuranceFrame::SendMsg()
{
	switch( m_eType )
	{
	case EISF_Brand:
		{
			tagNC_Brand brand;
			brand.n64SrcItemID	= m_n64SrcID;
			brand.n64DstItemID	= m_n64DstID;
			brand.n64IMID		= m_n64InsuranceID;
			TObjRef<NetSession>()->Send(&brand);
			this->ShowFrame(FALSE);
		}
		break;
	case EISF_EquipAddPotValue:
		{
			tagNC_AddPot AddPot;
			AddPot.n64SrcItemID	= m_n64SrcID;
			AddPot.n64DstItemID	= m_n64DstID;
			AddPot.n64IMID		= m_n64InsuranceID;
			TObjRef<NetSession>()->Send(&AddPot);
			this->ShowFrame(FALSE);
		}
	}
}

BOOL InsuranceFrame::EscCancel()
{
	if(m_pWnd->IsInvisible())
		return FALSE;

	ShowFrame(FALSE);
	return TRUE;
}