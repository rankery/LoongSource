#include "stdafx.h"
#include "CursorMgr.h"
#include "resource.h"

CursorMgr::CursorMgr():
m_Trunk( this ),
m_eActiveCursor( ECAN_Null ),
m_eIdleCursor( ECIN_Default ),
m_eCursorDisplay( ECDT_Num ),
m_dwParam( GT_INVALID )
{
	for( int i = 0; i < ECDT_Num; i++ )
	{
		m_hCursor[i] = NULL;
	}
}

CursorMgr::~CursorMgr()
{
	
}

static CursorMgr g_cursormgr;

CursorMgr* CursorMgr::Inst()
{
	return &g_cursormgr;
}

void CursorMgr::Init(HINSTANCE hInst)
{
	for( int i = 0; i < ECDT_Num; i++ )
	{
		m_hCursor[i] = ::LoadCursor( hInst, MAKEINTRESOURCE( IDC_CURSOR1 + i ) );
		ASSERT( NULL != m_hCursor[i] );
	}

	Change2IdleState();
	m_ptCursor.Set( 0, 0 );
	TObjRef<GameFrameMgr>()->RegisterEventHandle(_T("Event_CursorActive"), (FRAMEEVENTPROC)m_Trunk.sfp1(&CursorMgr::Active));
}

void CursorMgr::Destroy()
{
	Change2IdleState();

	TObjRef<GameFrameMgr>()->UnRegisterEventHandler(_T("Event_CursorActive"), (FRAMEEVENTPROC)m_Trunk.sfp1(&CursorMgr::Active));

	for( int i = 0; i < ECDT_Num; i++ )
	{
		DeleteObject( m_hCursor[i] );
	}
}

void CursorMgr::Change2IdleState()
{
	m_eCursorState = ECS_Idle;
	m_eActiveCursor = ECAN_Null;
	m_dwParam = GT_INVALID;
	m_eIdleCursor = ECIN_Default;
}

void CursorMgr::Active( tagActiveCursorEvent* pEvent )
{
	// 当光标为空闲状态才接收激活事件
	if( ECS_Idle != m_eCursorState )
		return;
	if( ECAN_Null == pEvent->eCursor )
		return;

	m_eCursorState = ECS_Active;
	m_eActiveCursor = pEvent->eCursor;
	m_dwParam = pEvent->dwParam;
}

void CursorMgr::OnWindowsMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2)
{
	// 按下鼠标右键瞬间
	if(WM_RBUTTONUP == dwMsg && ECAN_Null != m_eActiveCursor)
		Change2IdleState();

	const ECursorDisplayType dt = GetCursorDisplay();
	if( WM_SETCURSOR == dwMsg || dt != m_eCursorDisplay )
	{
		::SetCursor( m_hCursor[dt] );
		m_eCursorDisplay = dt;
	}

	if( WM_MOUSEMOVE == dwMsg )
		m_ptCursor.Set( LOWORD(dwParam2), HIWORD(dwParam2) );
}

void CursorMgr::SetIdleCursor( const ECursorIdleName eIn )
{
	m_eIdleCursor = eIn;
}

ECursorDisplayType CursorMgr::GetCursorDisplay()
{
	if( ECS_Idle == m_eCursorState )
	{
		switch( m_eIdleCursor )
		{
		case ECIN_Attack:			return ECDT_Attack;
		case ECIN_NPCTalk:			return ECDT_NPCTalk;
		case ECIN_NPCShop:			return ECDT_NPCShop;
		case ECIN_PickGroundItem:	return ECDT_PickGroundItem;
		case ECIN_Gather:			return ECDT_Gather;
		case ECIN_Hyperlink:		return ECDT_Hyperlink;
		case ECIN_DigBao:			return ECDT_DigBao;
		default:                    return ECDT_Default;
		}
	}
	else if( ECS_Active == m_eCursorState )
	{
		switch( m_eActiveCursor )
		{
		case ECAN_Identify: return ECDT_Identify;
		case ECAN_Gem:     
		case ECAN_Brand:    
		case ECAN_Longfu:
		case ECAN_Chisel:
		case ECAN_Grind:
		case ECAN_AddPot:
		case ECAN_BeGoldStone:
		case ECAN_Key:		return ECDT_UseSpecilItem;
        case ECAN_Dye:      return ECDT_UseSpecilItem;
		default:            return ECDT_Default;
		}
	}
	else
	{
		return ECDT_Default;
	}

}