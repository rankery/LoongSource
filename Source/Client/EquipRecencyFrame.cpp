#include "stdafx.h"
#include "EquipRecencyFrame.h"
#include "ActiveStatic.h"
#include "Item.h"
#include "ItemEvent.h"
#include "ItemMgr.h"
#include "Container.h"
#include "ToolTipFrame.h"
#include "ToolTipCreator.h"
#include "..\WorldDefine\container_define.h"

const TCHAR* szRecenyLess = _T("data\\ui\\main\\l-new-b.bmp");
const TCHAR* szRecenyOld = _T("data\\ui\\main\\l-new-c.bmp");
const TCHAR* szRecenyBad = _T("data\\ui\\main\\l-new-a.bmp");

#define Equip_Receny_Less 80
#define Equip_Receny_Old 50
#define Equip_Receny_Bad 20

EquipRecencyFrame::EquipRecencyFrame( void ):m_Trunk(this)
{
	m_pWnd				= NULL;
	m_pAS_Recency		= NULL;
	m_eReceny			= EER_NULL;
}

EquipRecencyFrame::~EquipRecencyFrame( void )
{

}

BOOL EquipRecencyFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);

	m_pMgr->RegisterEventHandle(_T("ItemEvent_EquipChange"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&EquipRecencyFrame::OnUpdateLPItemEvent));
	m_pMgr->RegisterEventHandle(_T("ItemEvent_UpdateItem"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&EquipRecencyFrame::OnUpdateLPItemEvent));

	return bRet;
}

BOOL EquipRecencyFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
	{
		m_pGUI->AddToDestroyList(m_pWnd);
		m_pWnd = NULL;
	}

	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_EquipChange"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&EquipRecencyFrame::OnUpdateLPItemEvent));
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_UpdateItem"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&EquipRecencyFrame::OnUpdateLPItemEvent));

	return bRet;
}

BOOL EquipRecencyFrame::ReloadUI()
{
	XmlElement ele;
	tstring strPath = g_strLocalPath + _T("\\ui\\newmint.xml");
	m_pGUI->LoadXml(&ele, "VFS_System", strPath.c_str());
	m_pGUI->ChangeXml(&ele, "newmint_win\\img", "ClassName", "ActiveStatic");
	m_pWnd = m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	m_pWnd->SetInvisible(TRUE);

	m_pAS_Recency = (ActiveStatic*)m_pWnd->GetChild(_T("img"));

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&EquipRecencyFrame::EventHandler));


	return TRUE;
}

VOID EquipRecencyFrame::Update()
{
	
}

DWORD EquipRecencyFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	switch (pEvent->eEvent)
	{
	case EGUIE_MoveIntoCtrl:
		{
			if( pWnd==m_pAS_Recency && m_eReceny==EER_RecenyBad )
			{
				tstring strTip = g_StrTable[_T("TipsDefaultFont")];
				strTip += g_StrTable[_T("EEquipRecency0")];
				ShowTip( m_pAS_Recency, strTip.c_str() );
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	}
	return 0;
}

DWORD EquipRecencyFrame::OnUpdateLPItemEvent( tagUpdateLPItemEvent* pGameEvent )
{
	if( pGameEvent->strEventName==_T("ItemEvent_EquipChange") || pGameEvent->strEventName==_T("ItemEvent_UpdateItem") )
	{
		if(pGameEvent->eType!=EICT_Equip)
			return 0;
		
		EEquipPos ePos = (EEquipPos)pGameEvent->n16Index;
		if( ePos != EEP_RightHand )
			return 0;

		ItemContainer* pEquipContainer = ItemMgr::Inst()->GetEquipment();
		if( !P_VALID(pEquipContainer) )
			return 0;

		Equipment* pEquip = (Equipment*)pEquipContainer->GetValue(ePos);
		if( !P_VALID(pEquip) )
		{
			m_eReceny = EER_NULL;
			m_pWnd->SetInvisible(TRUE);
			return 0;
		}

		//判断武器崭新度
		INT nReceny = pEquip->GetEquipNewness();

		if( nReceny>=Equip_Receny_Old && nReceny<Equip_Receny_Less )
			m_eReceny = EER_RecenyLess;
		else if( nReceny>=Equip_Receny_Bad && nReceny<Equip_Receny_Old )
			m_eReceny = EER_RecenyOld;
		else if( nReceny<Equip_Receny_Bad )
			m_eReceny = EER_RecenyBad;
		else
			m_eReceny = EER_NULL;

		//根据崭新度设置显示的贴图
		switch( m_eReceny )
		{
		case EER_NULL:
			{
				m_pWnd->SetInvisible(TRUE);
			}
			break;
		case EER_RecenyLess:
			{
				m_pAS_Recency->SetPic( szRecenyLess );
				m_pWnd->SetInvisible(FALSE);
			}
			break;
		case EER_RecenyOld:
			{
				m_pAS_Recency->SetPic( szRecenyOld );
				m_pWnd->SetInvisible(FALSE);
			}
			break;
		case EER_RecenyBad:
			{
				m_pAS_Recency->SetPic( szRecenyBad );
				m_pAS_Recency->Glint(TRUE);
				m_pWnd->SetInvisible(FALSE);
			}
			break;
		}
	}
	return 0;
}
