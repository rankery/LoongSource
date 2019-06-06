#include "stdafx.h"
#include "Player.h"
#include "RoleMgr.h"
#include "BuffUIList.h"
#include "TeammateStateWnd.h"
#include "RoleHeadPicIcon.h"
#include "RoleEvent.h"
#include "TeamEvent.h"
#include "TeamSys.h"
#include "MapLogicData.h"
#include "ToolTipFrame.h"
#include "WorldPickerFrame.h"
#include "CombatSysUtil.h"

TeammateStateWnd::TeammateStateWnd() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pHead( NULL ),
m_pBuffUIList( NULL ),
m_pSt_Name( NULL ),
m_pSt_Level( NULL ),
m_pPro_Hp( NULL ),
m_pPro_Mp( NULL ),
m_pSt_Captain( NULL ),
m_dwRoleID( GT_INVALID ),
m_dwLastFindRoleTime( 0 ),
m_bInView( FALSE )
{

}

TeammateStateWnd::~TeammateStateWnd()
{
	SafeDestroy();
}

BOOL TeammateStateWnd::Create( XmlElement* pEle, const DWORD dwRoleID )
{
	SafeDestroy();

	m_pGUI->ChangeXml( pEle, "teamatt\\caption\\teamface", "ClassName", "RoleHeadPicIcon");

	for( INT i = 0; i < BUFFUI_NUM; i++ )
	{
		std::stringstream streamBuffStaticName;
		streamBuffStaticName<<"teamatt\\buff"<<i+1;
		m_pGUI->ChangeXml( pEle, streamBuffStaticName.str().c_str(), "ClassName", "ActiveStatic");
	}

	static DWORD nWnd = 0;
	stringstream stream;
	stream<<"teammate_"<<nWnd++;
	pEle->SetAttribute( "Name", stream.str().c_str() );
	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), pEle );

	if( !P_VALID( m_pWnd ) )
		return FALSE;

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&TeammateStateWnd::GUIEventHandler));

	m_pHead       = (RoleHeadPicIcon*)m_pWnd->GetChild( _T("caption\\teamface") );
	m_pSt_Name    = (GUIStatic*)m_pWnd->GetChild( _T("caption\\name") );
	m_pSt_Level   = (GUIStatic*)m_pWnd->GetChild( _T("caption\\grade") );
	m_pPro_Hp     = (GUIProgress*)m_pWnd->GetChild( _T("caption\\hp") );
	m_pPro_Mp	  = (GUIProgress*)m_pWnd->GetChild( _T("caption\\mp") );
	m_pSt_Captain = (GUIStatic*)m_pWnd->GetChild( _T("caption\\captionpic") );

	m_pBuffUIList = new BuffUIList;
	for( INT i = 0; i < BUFFUI_NUM; i++ )
	{
		tstringstream streamBuffStaticName;
		tstringstream streamBuffProgressName;
		streamBuffStaticName<<_T("buff")<<i+1;
		streamBuffProgressName<<_T("buffpro")<<i+1;
		m_pBuffUIList->AddUI( (ActiveStatic*)m_pWnd->GetChild( streamBuffStaticName.str().c_str() ), 
			                (GUIProgress*)m_pWnd->GetChild( streamBuffProgressName.str().c_str() ) );
	}

	m_pSt_Captain->FlipToTop();
	m_pSt_Captain->SetTopMost( TRUE );
	m_pSt_Captain->SetInvisible( TRUE );

	m_pPro_Hp->SetValue(0);
	m_pPro_Mp->SetValue(0);

	m_dwRoleID = dwRoleID;

	// 注册游戏事件响应函数
	m_pMgr->RegisterEventHandle( _T("tagTeammateStateInitEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeammateStateInitEvent));
	m_pMgr->RegisterEventHandle( _T("tagTeamLeaderChangeEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeamLeaderChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagTeammateLevelChangeEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeammateLevelChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagTeammateNameUpdataEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeammateNameUpdataEvent));
	m_pMgr->RegisterEventHandle( _T("tagTeammateAttChangeEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeammateAttChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagRoleBuffChangeEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnRoleBuffChangeEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdataAvatarEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnUpdataAvatarEvent));
	m_pMgr->RegisterEventHandle( _T("tagUpdataAvatarDisplaySetEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnUpdataAvatarDisplaySetEvent));

	return TRUE;
}

BOOL TeammateStateWnd::SafeDestroy()
{
	SAFE_DEL( m_pBuffUIList );
	if( P_VALID( m_pWnd ) )
	{
		if( m_pHead == m_pGUI->GetMousePoint() )
			ShowTip( m_pHead, _T("") );

		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}
	m_dwRoleID = GT_INVALID;
	m_bInView = FALSE;

	m_pMgr->UnRegisterEventHandler( _T("tagTeammateStateInitEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeammateStateInitEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagTeamLeaderChangeEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeamLeaderChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagTeammateLevelChangeEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeammateLevelChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagTeammateNameUpdataEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeammateNameUpdataEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagTeammateAttChangeEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnTeammateAttChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagRoleBuffChangeEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnRoleBuffChangeEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdataAvatarEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnUpdataAvatarEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagUpdataAvatarDisplaySetEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammateStateWnd::OnUpdataAvatarDisplaySetEvent));

	return TRUE;
}

VOID TeammateStateWnd::Update()
{
	if( GT_INVALID == m_dwRoleID )
		return;
	if( P_VALID( m_pBuffUIList ) )
		m_pBuffUIList->Updata();

	if( timeGetTime() - m_dwLastFindRoleTime > 1000 )
	{
		Role* pRole = RoleMgr::Inst()->FindRole( m_dwRoleID );
		if( P_VALID( pRole ) && pRole->IsAttributeInited() )
		{
			if( !m_bInView )										// 进入视野
			{
				m_pHead->UpdataHeadPic( pRole );
				m_bInView = TRUE;
			}
		}
		else
		{
			if( m_bInView )											// 离开视野
			{
				m_pBuffUIList->ClearBuffs();
				m_bInView = FALSE;
			}
		}
		m_dwLastFindRoleTime = timeGetTime();
	}
}

VOID TeammateStateWnd::Render3D()
{
	if( P_VALID( m_pHead ) )
		m_pHead->Render3D();
}

BOOL TeammateStateWnd::GUIEventHandler(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	if( !P_VALID( pWnd ) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_MoveIntoCtrl:
		if( pWnd == m_pHead )
		{
			const tagTeammateData* pData = TeamSys::Inst()->GetTeammateData( m_dwRoleID );
			ASSERT( P_VALID( pData ) );
			const tagMapLogicData* pLogicData = MapLogicData::Inst()->FindMapLogicData( pData->dwMapID );
			ASSERT( P_VALID( pLogicData ) );
			tstring strTip = g_StrTable[_T("TipsDefaultFont")];
			strTip += pLogicData->szShowName;
			ShowTip( m_pHead, strTip.c_str() );
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		if( pWnd == m_pHead )
		{
			ShowTip( m_pHead, _T("") );
		}
		break;
	case EGUIE_Click:
		if( pWnd == m_pHead )
		{
			Role* pRole = RoleMgr::Inst()->FindRole( m_dwRoleID );
			if( P_VALID( pRole ) && pRole->IsAttributeInited() )
			{
				tagRolePickEvent evt( _T("tagRolePickEvent"), NULL );
				evt.eType = EWPT_LClick;
				evt.dwRoleID = m_dwRoleID;
				m_pMgr->SendEvent( &evt );
			}
			else
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg( g_StrTable[_T("Team_SelectTooFar")] );
			}
		}
		break;
	default:
		break;
	}
	if( P_VALID( m_pBuffUIList ) )
		m_pBuffUIList->GUIEventHandler( pEvent, pWnd, m_dwRoleID );
	return FALSE;
}

VOID TeammateStateWnd::InitState()
{
	const tagTeammateData* pData = TeamSys::Inst()->GetTeammateData( m_dwRoleID );
	if( !P_VALID( pData ) )
		return;

	if( TeamSys::Inst()->IsLeader( m_dwRoleID ) )
		m_pSt_Captain->SetInvisible( FALSE );
	else
		m_pSt_Captain->SetInvisible( TRUE );

	tstringstream stream;
	stream<<pData->nLevel;
	m_pSt_Level->SetText( stream.str().c_str() );

	m_pSt_Name->SetText( pData->strName.c_str() );

	m_pPro_Hp->SetMaxValue( (FLOAT)pData->nMaxHP );
	m_pPro_Hp->SetValue( (FLOAT)pData->nHP, true );
	m_pPro_Mp->SetMaxValue( (FLOAT)pData->nMaxMP );
	m_pPro_Mp->SetValue( (FLOAT)pData->nMP, true );

	Role* pRole = RoleMgr::Inst()->FindRole( m_dwRoleID );
	if( P_VALID( pRole ) && pRole->IsAttributeInited() )
	{
		m_pHead->UpdataHeadPic( pRole );
		UpdataBuffs( pRole );
	}
	else
	{
		tagAvatarEquip ep;
		ZeroMemory( &ep, sizeof( tagAvatarEquip ) );
		for( int i = 0; i < TEAM_DISPLAY_NUM; i++ )
		{
			ep.AvatarEquip[i].dwTypeID = pData->dwEquipTypeID[i];
		}
		m_pHead->UpdataAvatarHeadPic( pData->AvatarAtt, ep, NULL );
	}

	// 体力低于20%头像闪烁
	if( m_pPro_Hp->GetCurrentValue() / m_pPro_Hp->GetMaxValue() < 0.2f )
		m_pHead->Glint( TRUE );
	else
		m_pHead->Glint( FALSE );
}

DWORD TeammateStateWnd::OnTeammateStateInitEvent(tagTeammateStateInitEvent* pEvent)
{
	if( pEvent->dwRoleID != m_dwRoleID )
		return 0;
	InitState();

	return 0;
}

DWORD TeammateStateWnd::OnTeamLeaderChangeEvent(tagTeamLeaderChangeEvent* pEvent)
{
	m_pSt_Captain->SetInvisible( TRUE );
	if( pEvent->dwNewLeaderID == m_dwRoleID )
		m_pSt_Captain->SetInvisible( FALSE );

	return 0;
}

DWORD TeammateStateWnd::OnTeammateLevelChangeEvent(tagTeammateLevelChangeEvent* pEvent)
{
	if( pEvent->dwRoleID != m_dwRoleID )
		return 0;
	tstringstream stream;
	stream<<pEvent->nLevel;
	m_pSt_Level->SetText( stream.str().c_str() );
	m_pSt_Level->SetRefresh( TRUE );

	return 0;
}
DWORD TeammateStateWnd::OnTeammateNameUpdataEvent(tagTeammateNameUpdataEvent* pEvent)
{
	if( pEvent->dwRoleID != m_dwRoleID )
		return 0;
	m_pSt_Name->SetText( pEvent->strName.c_str() );
	m_pSt_Name->SetRefresh( TRUE );

	return 0;
}

DWORD TeammateStateWnd::OnTeammateAttChangeEvent(tagTeammateAttChangeEvent* pEvent)
{
	if( pEvent->dwRoleID != m_dwRoleID )
		return 0;

	switch( pEvent->eType )
	{
		case ERRA_MaxHP:	m_pPro_Hp->SetMaxValue( (FLOAT)pEvent->nValue );		break;
		case ERRA_HP:		m_pPro_Hp->SetValue( (FLOAT)pEvent->nValue, true );		break;
		case ERRA_MaxMP:	m_pPro_Mp->SetMaxValue( (FLOAT)pEvent->nValue );		break;
		case ERRA_MP:		m_pPro_Mp->SetValue( (FLOAT)pEvent->nValue, true );		break;
		default: break;
	}

	// 体力低于20%头像闪烁
	if( m_pPro_Hp->GetCurrentValue() / m_pPro_Hp->GetMaxValue() < 0.2f )
		m_pHead->Glint( TRUE );
	else
		m_pHead->Glint( FALSE );

	return 0;
}

DWORD TeammateStateWnd::OnRoleBuffChangeEvent(tagRoleBuffChangeEvent* pEvent)
{
	if( pEvent->dwRoleID != m_dwRoleID )
		return 0;

	Role* pRole = RoleMgr::Inst()->FindRole( m_dwRoleID );
	if( !P_VALID( pRole ) )
		return 0;

	UpdataBuffs( pRole );

	return 0;
}

DWORD TeammateStateWnd::OnUpdataAvatarEvent(tagUpdataAvatarEvent* pEvent)
{
	if( pEvent->dwRoleID != m_dwRoleID )
		return 0;
	Role* pRole = RoleMgr::Inst()->FindRole( m_dwRoleID );
	if( !P_VALID( pRole ) && pRole->IsAttributeInited() )
		return 0;

	m_pHead->UpdataHeadPic( pRole );

	return 0;
}

VOID TeammateStateWnd::UpdataBuffs( Role* pRole )
{
	ASSERT( P_VALID( pRole ) );
	m_pBuffUIList->ClearBuffs();
	tagRoleBuff* pBuff = NULL;
	TList<tagRoleBuff*>& listBuff = pRole->GetBuffList();
	listBuff.ResetIterator();
	while( listBuff.PeekNext(pBuff) )
	{
		if( !P_VALID( pBuff ) )
			continue;
		if( !m_pBuffUIList->AddBuff( pBuff ) )
			break;
	}
}

DWORD TeammateStateWnd::OnUpdataAvatarDisplaySetEvent(tagUpdataAvatarDisplaySetEvent* pEvent)
{
	if( pEvent->dwRoleID == m_dwRoleID )
	{
		Role* pRole = RoleMgr::Inst()->FindRole( pEvent->dwRoleID );
		if( P_VALID( pRole ) && pRole->IS_KIND_OF(Player) )
		{
			Player* pPlayer = (Player*)pRole;
			m_pHead->UpdataDisplaySet( pPlayer->GetDisplaySet() );
		}
	}
	return 0;
}