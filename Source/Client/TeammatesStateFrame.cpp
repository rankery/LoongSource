#include "stdafx.h"
#include "TeammatesStateFrame.h"
#include "TeamSys.h"
#include "TeamEvent.h"
#include "TeammateStateWnd.h"

TeammatesStateFrame::TeammatesStateFrame() : 
m_Trunk( this )
{
}

TeammatesStateFrame::~TeammatesStateFrame()
{
}

BOOL TeammatesStateFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("tagAddTeammateEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&TeammatesStateFrame::OnAddTeammateEvent));
	m_pMgr->RegisterEventHandle( _T("tagDelTeammateEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&TeammatesStateFrame::OnDelTeammateEvent));
	m_pMgr->RegisterEventHandle( _T("tagLeaveTeamEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammatesStateFrame::OnLeaveTeamEvent));

	return TRUE;
}

BOOL TeammatesStateFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("tagAddTeammateEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammatesStateFrame::OnAddTeammateEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagDelTeammateEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammatesStateFrame::OnDelTeammateEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagLeaveTeamEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TeammatesStateFrame::OnLeaveTeamEvent));

	GameFrame::Destroy();

	DeleteAllWnd();

	return TRUE;
}

TeammateStateWnd* TeammatesStateFrame::NewWnd( DWORD dwRoleID, int nIndex )
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\teamatt.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return NULL;

	if( m_ptWndSize.IsEmpty() )
	{
		element.Attribute( "LogicPos_x", (int*)&m_ptWndPosStart.x );
		element.Attribute( "LogicPos_y", (int*)&m_ptWndPosStart.y );
		element.Attribute( "Size_x",     (int*)&m_ptWndSize.x );
		element.Attribute( "Size_y",     (int*)&m_ptWndSize.y );
	}

	TeammateStateWnd* pWnd = new TeammateStateWnd;
	element.SetAttribute( "LogicPos_y", m_ptWndPosStart.y + ( m_ptWndSize.y + WND_OFFSET_Y ) * nIndex );

	if( !pWnd->Create( &element, dwRoleID ) )
	{
		SAFE_DEL( pWnd );
		return NULL;
	}
	return pWnd; 
}

VOID TeammatesStateFrame::Update()
{
	for( size_t i = 0; i < m_vecWnd.size(); i++ )
	{
		TeammateStateWnd* pWnd = m_vecWnd[i];
		if( P_VALID( pWnd ) )
			pWnd->Update();
	}

	GameFrame::Update();
}

VOID TeammatesStateFrame::Render3D()
{
	for( size_t i = 0; i < m_vecWnd.size(); i++ )
	{
		TeammateStateWnd* pWnd = m_vecWnd[i];
		if( P_VALID( pWnd ) )
			pWnd->Render3D();
	}
}

VOID TeammatesStateFrame::DeleteAllWnd()
{
	for( size_t i = 0; i < m_vecWnd.size(); i++ )
	{
		TeammateStateWnd* pWnd = m_vecWnd[i];
		if( P_VALID( pWnd ) )
		{
			pWnd->SafeDestroy();
			SAFE_DEL( pWnd );
		}
	}
	m_vecWnd.clear();
}

DWORD TeammatesStateFrame::OnAddTeammateEvent(tagAddTeammateEvent* pEvent)
{
	TeammateStateWnd* pWnd = NewWnd( pEvent->dwRoleID, m_vecWnd.size() );
	ASSERT( P_VALID( pWnd ) );
	m_vecWnd.push_back( pWnd );

	return 0;
}

DWORD TeammatesStateFrame::OnDelTeammateEvent(tagDelTeammateEvent* pEvent)
{
	DeleteAllWnd();
	const vector<tagTeammateData>& vecTeammates = TeamSys::Inst()->GetTeammateList();
	m_vecWnd.resize( vecTeammates.size() );

	for( size_t i = 0; i < vecTeammates.size(); i++ )
	{
		TeammateStateWnd* pWnd = NewWnd( vecTeammates[i].dwRoleID, i );
		ASSERT( P_VALID( pWnd ) );
		pWnd->InitState();
		m_vecWnd[i] = pWnd;
	}
	return 0;
}

DWORD TeammatesStateFrame::OnLeaveTeamEvent(tagLeaveTeamEvent* pEvent)
{
	DeleteAllWnd();
	return 0;
}