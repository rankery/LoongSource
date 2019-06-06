#include "StdAfx.h"
#include "RoleLevelUpPromptFrame.h"
#include "LocalPlayer.h"
#include "MapMgr.h"
#include "UIEvent.h"
#include "RoleMgr.h"
#include "CombatEvent.h"

RoleLevelUpPromptFrame::RoleLevelUpPromptFrame(void) :
m_Trunk(this),
m_pWnd(NULL),
m_pOk(NULL),
m_pStcContent(NULL)
{
}

RoleLevelUpPromptFrame::~RoleLevelUpPromptFrame(void)
{
}

BOOL RoleLevelUpPromptFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init( pMgr, pFather, dwParam ) )
		return FALSE;

	return TRUE;
}

BOOL RoleLevelUpPromptFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();

	if( P_VALID(m_pWnd) )
	{
		m_pGUI->DestroyWnd( m_pWnd );
		m_pWnd = NULL;
	}
	return bRet;
}

VOID RoleLevelUpPromptFrame::Update()
{

}

BOOL RoleLevelUpPromptFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = vEngine::g_strLocalPath + _T("\\ui\\common15.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );
	if( !P_VALID( m_pWnd ) )
		return FALSE;

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler( m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&RoleLevelUpPromptFrame::EventHandler) );

	m_pOk				= (GUIButton*)m_pWnd->GetChild( _T("comm_pic\\surebutt") );
	m_pStcContent		= (GUIStaticEx*)m_pWnd->GetChild( _T("comm_pic\\back\\word") );
	
	m_pWnd->SetInvisible( TRUE );
	return TRUE;
}


BOOL RoleLevelUpPromptFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd( pEvent->dwWndID );

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pOk )
			{
				ShowWnd( FALSE );
				m_pMgr->AddToDestroyList( this );
			}
		}
		break;
	case EGUIE_StaticExLinkClick:
		{
			if( m_vecIdLinks.size() <=0 && m_vecPosLinks.size() <= 0 )
				return TRUE;
			Navigate( pEvent );
		}
		break;
	default:
		break;
	}
	return TRUE;
}

VOID RoleLevelUpPromptFrame::Navigate( tagGUIEvent* pEvent )
{
	if( !P_VALID(pEvent) )
		return;

	LPCTSTR szLink = (LPCTSTR)( pEvent->dwParam2 );
	Vector3 pos;

	//根据ID寻路
	if( (INT)pEvent->dwParam1 < 1000 )	
	{
		INT index = pEvent->dwParam1 - 1;
		if( index > (INT)m_vecIdLinks.size()-1 )
			return;

		tagIDLink& link = m_vecIdLinks[index];
		IMSG( _T("path map: %s typeid: %d\n"), link.mapName.c_str(), link.typeID );

		//不在本地图，进行跨场景寻路
		if( link.mapName != MapMgr::Inst()->GetCurMapName() )
		{
			if( QuestMgr::Inst()->MoveToMap( link.mapName.c_str(), pos ) != TRUE )
				return;
		}
		else
		{
			if( link.typeID == 1 )//触发器或者其他没有TypeID的东西
			{
				QuestData *pData = QuestMgr::Inst()->GetData();
				ASSERT( P_VALID(pData) );
				if( !pData->GetTriggerPosition( szLink, pos ) )
					return;
			}
			else
			{
				QuestQuery *pQuery = QuestMgr::Inst()->GetQuery();
				ASSERT( P_VALID(pQuery) );
				if( !pQuery->GetNPCPosition( link.typeID, pos ) )
					return;
			}
		}
	}
	else
	{
		INT index = pEvent->dwParam1-1000;
		if( index > (INT)m_vecPosLinks.size()-1 )
			return;

		tagPosLink& link = m_vecPosLinks[index];

		//不在本地图，进行跨场景寻路
		if( link.mapName != MapMgr::Inst()->GetCurMapName() )
		{
			if( QuestMgr::Inst()->MoveToMap( link.mapName.c_str(),pos ) != TRUE )
				return;
		}
		else
		{
			pos = link.pos;
		}
	}

	LocalPlayer *pLP = RoleMgr::Inst()->GetLocalPlayer();
	tagMouseMoveEvent event( _T("tagMouseMoveEvent"), NULL );
	event.start = pLP->GetPos();
	event.end = pos;
	event.validDist = 100.0f;

	IMSG( _T("target pos: x-%f,y-%f,z-%f\n"),pos.x,pos.y,pos.z );
	m_pMgr->SendEvent( &event );
}

VOID RoleLevelUpPromptFrame::ShowWnd( BOOL bShow )
{
	m_pWnd->SetInvisible( !bShow );
}

VOID RoleLevelUpPromptFrame::LevelUpTo( DWORD dwLevel )
{
	switch( dwLevel )
	{
	case 10:
		m_strDesc = g_StrTable[_T("Role_LevelUpTo10")];
		break;
	case 20:
		m_strDesc = g_StrTable[_T("Role_LevelUpTo20")];
		break;
	default:
		{
			/*TCHAR szDesc[50] = {0};
			_stprintf( szDesc, g_StrTable[_T("Role_LvlUp")], dwLevel );
			m_strDesc = szDesc;*/
		}
		return;
	}

	m_vecIdLinks.clear();
	m_vecPosLinks.clear();
	//将描述中的有关NPC,地物，等的导航信息提取并改变现实颜色
	QuestMgr::Inst()->ParseQuestView( m_strDesc, m_vecIdLinks,m_vecPosLinks );
	m_pStcContent->SetText( m_strDesc.c_str() );
	ShowWnd( TRUE );
}