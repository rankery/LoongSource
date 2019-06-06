#include "stdafx.h"
#include "GuildUpgradeFrame.h"
#include "GuildDefine.h"
#include "GuildMgr.h"
#include "..\WorldDefine\msg_guild.h"
#include "CombatSys.h"
#include "ActiveStatic.h"
#include "ItemProtoData.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "Container.h"
#include "ItemMgr.h"
#include "ItemEvent.h"
#include "ItemFrame.h"

GuildUpgradeFrame::GuildUpgradeFrame() : 
m_Trunk( this ),
m_pWnd( NULL ),
m_pBn_Close( NULL ),
m_pSt_LevelCaption( NULL ),
m_pSt_Level( NULL ),
m_pPro_Progress( NULL ),
m_pSt_Fund( NULL ),
m_pSt_Material( NULL ),
m_pSt_Tips( NULL ),
m_pBn_HandInItems( NULL ),
m_eCurSelType( EFT_Null ),
m_bWaitingForHandInRet( false )
{
	ZeroMemory( m_pBn_Tab, sizeof(m_pBn_Tab) );
	ZeroMemory( m_pSt_Item_Icon, sizeof(m_pSt_Item_Icon) );
	ZeroMemory( m_pSt_Item_Name, sizeof(m_pSt_Item_Name) );
	ZeroMemory( m_pSt_Item_Num, sizeof(m_pSt_Item_Num) );
	ZeroMemory( m_nPutInedItemNum, sizeof(m_nPutInedItemNum) );
	ZeroMemory( m_bLevelFull, sizeof(m_bLevelFull) );
}

GuildUpgradeFrame::~GuildUpgradeFrame()
{
}

BOOL GuildUpgradeFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if (!GameFrame::Init(pMgr, pFather, dwParam))
		return FALSE;

	m_pCmdMgr->Register("NS_GetGuildUpgradeInfo",	(NETMSGPROC)m_Trunk.sfp2(&GuildUpgradeFrame::OnNS_GetGuildUpgradeInfo),	_T("NS_GuildUpgradeFrame"));
	m_pCmdMgr->Register("NS_UpdateFacilitiesInfo",	(NETMSGPROC)m_Trunk.sfp2(&GuildUpgradeFrame::OnNS_UpdateFacilitiesInfo),_T("NS_UpdateFacilitiesInfo"));
	m_pCmdMgr->Register("NS_GuildProcFailed",		(NETMSGPROC)m_Trunk.sfp2(&GuildUpgradeFrame::OnNS_GuildProcFailed),		_T("NS_GuildProcFailed"));

	m_pMgr->RegisterEventHandle( _T("ItemEvent_UpdateItem"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildUpgradeFrame::OnUpdateItemEvent));
	m_pMgr->RegisterEventHandle( _T("Close_GuildUpgrade"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildUpgradeFrame::OnClose_GuildUpgrade));
	m_pMgr->RegisterEventHandle( _T("ItemEvent_Move2GuildUpgrade"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildUpgradeFrame::OnItemEvent_Move2GuildUpgrade));
	

	// 请求设施升级信息
	CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame( _T("CombatSys") );
	if( !P_VALID( pCombatSys ) || GT_INVALID == pCombatSys->GetCurTargetID() )
		return FALSE;

	tagNC_GetGuildUpgradeInfo cmd;
	cmd.dwNPCID = pCombatSys->GetCurTargetID();
	m_pSession->Send( &cmd );

	SetStrategy( EIST_GuildUpgrade );

	return TRUE;
}

BOOL GuildUpgradeFrame::Destroy()
{
	m_pCmdMgr->UnRegister("NS_GetGuildUpgradeInfo",		(NETMSGPROC)m_Trunk.sfp2(&GuildUpgradeFrame::OnNS_GetGuildUpgradeInfo));
	m_pCmdMgr->UnRegister("NS_UpdateFacilitiesInfo",	(NETMSGPROC)m_Trunk.sfp2(&GuildUpgradeFrame::OnNS_UpdateFacilitiesInfo));
	m_pCmdMgr->UnRegister("NS_GuildProcFailed",			(NETMSGPROC)m_Trunk.sfp2(&GuildUpgradeFrame::OnNS_GuildProcFailed));

	m_pMgr->UnRegisterEventHandler( _T("ItemEvent_UpdateItem"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildUpgradeFrame::OnUpdateItemEvent));
	m_pMgr->UnRegisterEventHandler( _T("Close_GuildUpgrade"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildUpgradeFrame::OnClose_GuildUpgrade));
	m_pMgr->UnRegisterEventHandler( _T("ItemEvent_Move2GuildUpgrade"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&GuildUpgradeFrame::OnItemEvent_Move2GuildUpgrade));

	GameFrame::Destroy();

	SafeDestroyUI();

	ZeroMemory( m_nPutInedItemNum, sizeof(m_nPutInedItemNum) );

	SetStrategy( EIST_ItemPocket, EIST_GuildUpgrade );

	return TRUE;
}

BOOL GuildUpgradeFrame::ReloadUI()
{
	SafeDestroyUI();

	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\guild_upgrade.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	for( int i = 0; i < MAX_UPGRADE_NEED_ITEM_TYPE; i++ )
	{
		stringstream stream;
		stream<<"guildupgrade_win\\guildupgrade\\pageback_pic\\pageback_pic\\itemback"<<i<<"\\item"<<i;
		m_pGUI->ChangeXml( &element, stream.str().c_str(), "ClassName", "ActiveStatic");
	}
	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&GuildUpgradeFrame::GUIEventHandler));

	m_pBn_Close				= (GUIButton*)m_pWnd->GetChild( _T("guildupgrade\\closebutt") );
	m_pBn_Tab[EFT_Guild]	= (GUIPushButton*)m_pWnd->GetChild( _T("guildupgrade\\pagefra\\guild") );
	m_pBn_Tab[EFT_Fund]		= (GUIPushButton*)m_pWnd->GetChild( _T("guildupgrade\\pagefra\\cashbox") );
	m_pBn_Tab[EFT_Material]	= (GUIPushButton*)m_pWnd->GetChild( _T("guildupgrade\\pagefra\\materialsbank") );
	m_pBn_Tab[EFT_Academy]	= (GUIPushButton*)m_pWnd->GetChild( _T("guildupgrade\\pagefra\\college") );
	m_pSt_LevelCaption		= (GUIStatic*)m_pWnd->GetChild( _T("guildupgrade\\pageback_pic\\word0") );
	m_pSt_Level				= (GUIStatic*)m_pWnd->GetChild( _T("guildupgrade\\pageback_pic\\grade") );
	m_pPro_Progress			= (GUIProgress*)m_pWnd->GetChild( _T("guildupgrade\\pageback_pic\\progressbarback\\progressbar") );
	m_pSt_Fund				= (GUIStatic*)m_pWnd->GetChild( _T("guildupgrade\\pageback_pic\\num2") );
	m_pSt_Material			= (GUIStatic*)m_pWnd->GetChild( _T("guildupgrade\\pageback_pic\\num3") );
	m_pSt_Tips				= (GUIStatic*)m_pWnd->GetChild( _T("guildupgrade\\pageback_pic\\tips") );
	m_pBn_HandInItems		= (GUIButton*)m_pWnd->GetChild( _T("guildupgrade\\new") );

	for( int i = 0; i < MAX_UPGRADE_NEED_ITEM_TYPE; i++ )
	{
		tstringstream streamIcon;
		streamIcon<<_T("guildupgrade\\pageback_pic\\pageback_pic\\itemback")<<i<<_T("\\item")<<i;
		m_pSt_Item_Icon[i] = (ActiveStatic*)m_pWnd->GetChild( streamIcon.str().c_str() );
		m_pSt_Item_Icon[i]->SetInvisible( TRUE );

		tstringstream streamName;
		streamName<<_T("guildupgrade\\pageback_pic\\pageback_pic\\itemname")<<i;
		m_pSt_Item_Name[i] = (GUIStatic*)m_pWnd->GetChild( streamName.str().c_str() );

		tstringstream streamNum;
		streamNum<<_T("guildupgrade\\pageback_pic\\pageback_pic\\itemnum")<<i;
		m_pSt_Item_Num[i] = (GUIStatic*)m_pWnd->GetChild( streamNum.str().c_str() );
	}

	m_eCurSelType = EFT_Guild;
	m_pBn_Tab[m_eCurSelType]->SetState( EGUIBS_PushDown );

	return TRUE;
}

BOOL GuildUpgradeFrame::EscCancel()
{
	m_pMgr->AddToDestroyList( this );
	return TRUE;
}

BOOL GuildUpgradeFrame::GUIEventHandler(tagGUIEvent *pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pBn_Close )
		{
			m_pMgr->AddToDestroyList( this );
		}
		else if( pWnd == m_pBn_HandInItems )
		{
			HandInItems();
		}
		break;
	case EGUIE_PushButtonDown:
		for( int i = 0; i < MAX_GUILD_FACILITIES_TYPE; i++ )
		{
			if( pWnd == m_pBn_Tab[i] )
			{
				m_eCurSelType = (EFacilitiesType)i;
				UpdataFacilitiesInfoUI( m_eCurSelType );
				break;
			}
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			for( int i = 0; i < MAX_UPGRADE_NEED_ITEM_TYPE; i++ )
			{
				if( pWnd == m_pSt_Item_Icon[i] )
				{
					ShowTip( pWnd, ToolTipCreator::Inst()->GetItemTips( m_sFacilitiesInfo[m_eCurSelType].dwItemID[i] ).c_str() );
					break;
				}
			}
		}
		break;
	case EOnEventSendDropGUI_Item:
		{
			for( int i = 0; i < MAX_UPGRADE_NEED_ITEM_TYPE; i++ )
			{
				if( pWnd == m_pSt_Item_Icon[i] &&
					GT_INVALID != m_sFacilitiesInfo[m_eCurSelType].dwItemID[i] &&
					pEvent->dwParam3 == m_sFacilitiesInfo[m_eCurSelType].dwItemID[i] )
				{
					ItemContainer* Pocket = ItemMgr::Inst()->GetPocket();
					const int nItemNum = Pocket->GetItemQuantity( pEvent->dwParam3 );
					SetItemNum( m_eCurSelType, i, nItemNum );
					break;
				}
			}
		}
		break;
	}

	return FALSE;
}

VOID GuildUpgradeFrame::SafeDestroyUI()
{
	if( P_VALID( m_pWnd ) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}
}

DWORD GuildUpgradeFrame::OnNS_GetGuildUpgradeInfo( tagNS_GetGuildUpgradeInfo* pCmd, DWORD )
{
	memcpy( m_sFacilitiesInfo, pCmd->sFacilitiesInfo, sizeof(pCmd->sFacilitiesInfo) );
	for( int i = 0; i < MAX_GUILD_FACILITIES_TYPE; i++ )
		CheckLevelFull( (EFacilitiesType)i );

	ZeroMemory( m_nPutInedItemNum, sizeof(m_nPutInedItemNum) );
	UpdataFacilitiesInfoUI( m_eCurSelType );

	return 0;
}

DWORD GuildUpgradeFrame::OnNS_UpdateFacilitiesInfo( tagNS_UpdateFacilitiesInfo* pCmd, DWORD )
{
	ASSERT( pCmd->sFacilitiesInfo.eType >= 0 && pCmd->sFacilitiesInfo.eType < MAX_GUILD_FACILITIES_TYPE );

	for( int i = 0; i < MAX_GUILD_FACILITIES_TYPE; i++ )
	{
		if( m_sFacilitiesInfo[i].eType == pCmd->sFacilitiesInfo.eType )
		{
			memcpy( &m_sFacilitiesInfo[i], &pCmd->sFacilitiesInfo, sizeof(pCmd->sFacilitiesInfo) );
			CheckLevelFull( (EFacilitiesType)i );
			break;
		}
	}
	ZeroMemory( m_nPutInedItemNum[pCmd->sFacilitiesInfo.eType], sizeof(m_nPutInedItemNum[pCmd->sFacilitiesInfo.eType]) );
	if( m_eCurSelType == pCmd->sFacilitiesInfo.eType )
		UpdataFacilitiesInfoUI( m_eCurSelType );

	m_bWaitingForHandInRet = false;
	return 0;
}

DWORD GuildUpgradeFrame::OnNS_GuildProcFailed( tagNS_GuildProcFailed* pCmd, DWORD )
{
	if( m_bWaitingForHandInRet )
	{
		m_pBn_HandInItems->SetEnable( true );
		m_bWaitingForHandInRet = false;
	}
		
	return 0;
}

DWORD GuildUpgradeFrame::OnUpdateItemEvent( tagUpdateLPItemEvent* pEvent )
{
	if( EICT_Bag != pEvent->eType )
		return 0;

	for( int nType = 0; nType < MAX_GUILD_FACILITIES_TYPE; nType++ )
	{
		for( int nItem = 0; nItem < MAX_UPGRADE_NEED_ITEM_TYPE; nItem++ )
		{
			if( m_nPutInedItemNum[nType][nItem] > 0 && pEvent->dwTypeID == m_sFacilitiesInfo[nType].dwItemID[nItem] )
			{
				ItemContainer* Pocket = ItemMgr::Inst()->GetPocket();
				const int nItemNum = Pocket->GetItemQuantity( pEvent->dwTypeID );
				SetItemNum( (EFacilitiesType)nType, nItem, nItemNum );
			}
		}
	}
	return 0;
}

DWORD GuildUpgradeFrame::OnClose_GuildUpgrade( tagGameEvent* pEvent )
{
	m_pMgr->AddToDestroyList( this );
	return 0;
}

DWORD GuildUpgradeFrame::OnItemEvent_Move2GuildUpgrade( tagItemPosChangeExEvent* pEvent )
{
	if( !P_VALID( pEvent->dwTypeIDSrc ) )
		return 0;

	for( int i = 0; i < MAX_UPGRADE_NEED_ITEM_TYPE; i++ )
	{
		if( GT_INVALID != m_sFacilitiesInfo[m_eCurSelType].dwItemID[i] &&
			pEvent->dwTypeIDSrc == m_sFacilitiesInfo[m_eCurSelType].dwItemID[i] )
		{
			ItemContainer* Pocket = ItemMgr::Inst()->GetPocket();
			const int nItemNum = Pocket->GetItemQuantity( pEvent->dwTypeIDSrc );
			SetItemNum( m_eCurSelType, i, nItemNum );
			break;
		}
	}

	return 0;
}

VOID GuildUpgradeFrame::UpdataFacilitiesInfoUI( const EFacilitiesType eType )
{
	if( !P_VALID(m_pWnd) )
		return;
	if( eType < 0 || eType >= MAX_GUILD_FACILITIES_TYPE )
		return;

	// 找到升级设施信息
	tagGuildFacilitiesInfo* pInfo = NULL;
	for( int i = 0; i < MAX_GUILD_FACILITIES_TYPE; i++ )
	{
		if( eType == m_sFacilitiesInfo[i].eType )
			pInfo = &m_sFacilitiesInfo[i];
	}
	if( NULL == pInfo )
		return;

	// 所需物品
	for( int i = 0; i < MAX_UPGRADE_NEED_ITEM_TYPE; i++ )
	{
		tstring strIconPath = _T("data\\ui\\Icon\\");
		const tagItemDisplayInfo* pDisplay = ItemProtoData::Inst()->FindItemDisplay( pInfo->dwItemID[i] );
		if( P_VALID(pDisplay) )
		{
			strIconPath += pDisplay->szMaleIcon;
			strIconPath += _T(".tga");
			m_pSt_Item_Icon[i]->SetInvisible( FALSE );
			m_pSt_Item_Icon[i]->SetPic( strIconPath );

			m_pSt_Item_Name[i]->SetText( pDisplay->szName );
		}
		else
		{
			m_pSt_Item_Name[i]->SetText( _T("") );
			m_pSt_Item_Icon[i]->SetInvisible( TRUE );
		}
		SetItemNum( eType, i, m_nPutInedItemNum[eType][i] );
	}

	// 所需资金
	{
		tstringstream stream;
		stream<<pInfo->nNeedFund;
		m_pSt_Fund->SetText( stream.str().c_str() );
	}
	// 所需资财
	{
		tstringstream stream;
		stream<<pInfo->nMaterial;
		m_pSt_Material->SetText( stream.str().c_str() );
	}
	// 等级标签
	{
		tstringstream stream;
		stream<<_T("Guild_Facilitie")<<eType;
		TCHAR szBuff[X_SHORT_NAME] = { 0 };
		_sntprintf( szBuff, X_SHORT_NAME, g_StrTable[_T("GuildUpgrade_Level")], g_StrTable[stream.str().c_str()] );
		m_pSt_LevelCaption->SetText( szBuff );
	}
	// 等级
	{
		tstringstream stream;
		stream<<pInfo->byLevel;
		m_pSt_Level->SetText( stream.str().c_str() );
	}
	// 说明
	{
		tstringstream stream;
		stream<<_T("GuildUpgrade_Tips")<<eType;
		m_pSt_Tips->SetText( g_StrTable[stream.str().c_str()] );
	}
	// 进度
	m_pPro_Progress->SetMaxValue( pInfo->nFulfill );
	m_pPro_Progress->SetValue( pInfo->nProgress, true );

	m_pWnd->SetRefresh();
}

VOID GuildUpgradeFrame::SetItemNum( const EFacilitiesType eType, const int nItemIndex, const int nItemNum )
{
	ASSERT( eType >= 0 && eType < MAX_GUILD_FACILITIES_TYPE );
	if( m_bLevelFull[eType] && eType == m_eCurSelType )
	{
		m_pBn_HandInItems->SetEnable( false );
		m_pSt_Item_Num[nItemIndex]->SetText( _T("") );
		m_pWnd->SetRefresh();
		return;
	}

	ASSERT( nItemIndex >= 0 && nItemIndex < MAX_UPGRADE_NEED_ITEM_TYPE );
	ASSERT( nItemNum >= 0 );

	const int nMaxNum = m_sFacilitiesInfo[eType].nNeedNum[nItemIndex];
	int& nPutInedItemNum = m_nPutInedItemNum[eType][nItemIndex];

	// 更新数量
	nPutInedItemNum = nItemNum;
	if( nPutInedItemNum > nMaxNum )
		nPutInedItemNum = nMaxNum;
	if( nPutInedItemNum < 0 )
		nPutInedItemNum = 0;

	// 更新界面
	if( eType == m_eCurSelType )
	{
		// 界面文字
		tstringstream stream;
		stream<<nPutInedItemNum<<_T('/')<<nMaxNum;
		m_pSt_Item_Num[nItemIndex]->SetText( stream.str().c_str() );

		// 图标
		m_pSt_Item_Icon[nItemIndex]->SetColorGray( 0 == nPutInedItemNum );

		// 检查物品是否放满
		bool bFullFilled = true;
		for( int i = 0; i < MAX_UPGRADE_NEED_ITEM_TYPE; i++ )
		{
			if( m_nPutInedItemNum[eType][i] != m_sFacilitiesInfo[eType].nNeedNum[i] )
			{
				bFullFilled = false;
				break;
			}
		}
		m_pBn_HandInItems->SetEnable( bFullFilled );
		m_pWnd->SetRefresh();
	}
}

VOID GuildUpgradeFrame::HandInItems()
{
	// 检查帮派状态
	if( GuildMgr::Inst()->IsInSpecState( EGDSS_Shortage ) ||
		GuildMgr::Inst()->IsInSpecState( EGDSS_Distress ) ||
		GuildMgr::Inst()->IsInSpecState( EGDSS_Chaos ) ||
		GuildMgr::Inst()->IsInSpecState( EGDSS_Warfare ) )
	{
		GuildMgr::Inst()->ShowGuildErrorMsg(E_Guild_State_Limit);
		return;
	}

	CombatSys* pCombatSys = (CombatSys*)m_pMgr->GetFrame( _T("CombatSys") );
	ASSERT( P_VALID( pCombatSys ) && GT_INVALID != pCombatSys->GetCurTargetID() );

	tagNC_HandInItems cmd;
	cmd.dwNPCID = pCombatSys->GetCurTargetID();
	cmd.eType = m_eCurSelType;
	m_pSession->Send( &cmd );

	m_bWaitingForHandInRet = true;
	m_pBn_HandInItems->SetEnable( false );
}

VOID GuildUpgradeFrame::CheckLevelFull( const EFacilitiesType eType )
{
	m_bLevelFull[eType] = true;
	for( int i = 0; i < MAX_UPGRADE_NEED_ITEM_TYPE; i++ )
	{
		if( GT_INVALID != m_sFacilitiesInfo[eType].dwItemID[i] )
		{
			m_bLevelFull[eType] = false;
			break;
		}
	}
}