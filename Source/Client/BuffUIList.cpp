#include "StdAfx.h"
#include "BuffUIList.h"
#include "ActiveStatic.h"
#include "..\WorldDefine\msg_combat.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "RoleMgr.h"
#include "Player.h"
#include "LocalPlayer.h"

BuffUIList::BuffUIList() :
m_nCurShowNum( 0 ),
m_dwLastUpdataTipsTime( 0 )
{

}

BuffUIList::~BuffUIList()
{
	m_vecBuffUI.clear();
}

BOOL BuffUIList::AddUI( ActiveStatic* pBuffStatic, GUIProgress* pTimeProgress )
{
	ASSERT( P_VALID( pBuffStatic ) && P_VALID( pTimeProgress ) );

	tagBuffUI bu;
	ZeroMemory( &bu.stBuff, sizeof(bu.stBuff) );
	bu.pSt_BuffPic = pBuffStatic;
	bu.pPro_BuffTime = pTimeProgress;

	bu.pSt_BuffPic->SetInvisible( TRUE );
	bu.pPro_BuffTime->SetInvisible( TRUE );

	m_vecBuffUI.push_back( bu );
	return TRUE;
}


BOOL BuffUIList::AddBuff( const tagRoleBuff* pBuff )
{
	ASSERT( P_VALID( pBuff ) );
	if( !P_VALID( pBuff->pBuffProto ) )
		return FALSE;
	if( m_vecBuffUI.size() == m_nCurShowNum )
		return FALSE;

	// 没有图标则不显示
	if( _T('\0') == pBuff->pBuffProto->szIcon[0] )
		return TRUE;

	tagBuffUI& bu = m_vecBuffUI[m_nCurShowNum];
	bu.stBuff = *pBuff;
	bu.pSt_BuffPic->SetInvisible( FALSE );
	bu.pPro_BuffTime->SetInvisible( FALSE );
	bu.pSt_BuffPic->Glint( FALSE );

	// 永久BUFF
	if( GT_INVALID == pBuff->nMaxPersistTime )
	{
		bu.pPro_BuffTime->SetMaxValue( 100 );
		bu.pPro_BuffTime->SetValue( 100, true );
	}
	else
	{
		bu.pPro_BuffTime->SetMaxValue( (FLOAT)pBuff->nMaxPersistTime );
		bu.pPro_BuffTime->SetValue( (FLOAT)pBuff->nPersistTimeLeft, true );
	}
	bu.pPro_BuffTime->SetRefresh( TRUE );

	if( pBuff->nWarpTimes > 1 )
	{
		tstringstream stream;
		stream<<pBuff->nWarpTimes;
		bu.pSt_BuffPic->SetText( stream.str().c_str() );
	}
	else
	{
		bu.pSt_BuffPic->SetText( _T("") );
	}
	
	tstring strIconFileName = _T("data\\ui\\Icon\\");
	strIconFileName += pBuff->pBuffProto->szIcon;
	strIconFileName += _T(".tga");
	bu.pSt_BuffPic->SetPic( strIconFileName );
	m_nCurShowNum++;

	// 显示TIPS
	if( m_pGUI->GetMousePoint() == bu.pSt_BuffPic )
	{
		tstring strTips = ToolTipCreator::Inst()->GetBuffTips( &bu.stBuff );
		ShowTip( bu.pSt_BuffPic, strTips.c_str() );
		m_dwLastUpdataTipsTime = Kernel::Inst()->GetAccumTimeDW();
	}

	return TRUE;
}

VOID BuffUIList::ClearBuffs()
{
	for( size_t i = 0; i < m_vecBuffUI.size(); i++ )
	{
		tagBuffUI& bu = m_vecBuffUI[i];

		// 关闭TIPS
		if( m_pGUI->GetMousePoint() == bu.pSt_BuffPic )
			ShowTip( bu.pSt_BuffPic, _T("") );

		ZeroMemory( &bu.stBuff, sizeof(bu.stBuff) );
		bu.pSt_BuffPic->SetInvisible( TRUE );
		bu.pPro_BuffTime->SetInvisible( TRUE );
		bu.pSt_BuffPic->Glint( FALSE );
	}
	m_nCurShowNum = 0;
}

VOID BuffUIList::Updata()
{
	const DWORD dwCurrTime = Kernel::Inst()->GetAccumTimeDW();

	for( INT i = 0; i < m_nCurShowNum; i++ )
	{
		tagBuffUI& bu = m_vecBuffUI[i];

		// 永久BUFF
		if( GT_INVALID == bu.stBuff.nMaxPersistTime )
			continue;

		// 1s更新一次Tips
		if( dwCurrTime - m_dwLastUpdataTipsTime > 1000 && m_pGUI->GetMousePoint() == bu.pSt_BuffPic )
		{
			tstring strTips = ToolTipCreator::Inst()->GetBuffTips( &bu.stBuff );
			ShowTip( bu.pSt_BuffPic, strTips.c_str() );
			m_dwLastUpdataTipsTime = dwCurrTime;
		}

		const DWORD dwTimeLeft = bu.stBuff.nPersistTimeLeft - (INT)(Kernel::Inst()->GetAccumTimeDW() - bu.stBuff.dwStartTime);
		if(  dwTimeLeft < 5000 )
		{
			bu.pSt_BuffPic->Glint( TRUE, 0x80 );
		}
		bu.pPro_BuffTime->SetValue( (FLOAT)dwTimeLeft, true );
		bu.pPro_BuffTime->SetRefresh( TRUE );
	}
}

BOOL BuffUIList::GUIEventHandler( tagGUIEvent* pEvent, const GUIWnd* pEventWnd, const DWORD dwRoleID )
{
	switch( pEvent->eEvent )
	{
		case EGUIE_Click:
			if( dwRoleID == RoleMgr::Inst()->GetLocalPlayer()->GetID() )
			{
				for( INT i = 0; i < m_nCurShowNum; i++ )
				{
					// 手动取消BUFF
					const tagBuffUI& bu = m_vecBuffUI[i];
					if( pEventWnd == bu.pSt_BuffPic && 
						bu.stBuff.pBuffProto->dwInterruptFlag & EBIF_Manual )
					{
						tagNC_CancelBuff cmd;
						cmd.dwBuffTypeID = m_vecBuffUI[i].stBuff.dwBuffTypeID;
						TObjRef<NetSession>()->Send( &cmd );
						break;
					}
				}
			}
			break;
		case EGUIE_MoveIntoCtrl:
			{
				for( INT i = 0; i < m_nCurShowNum; i++ )
				{
					// 显示TIPS
					const tagBuffUI& bu = m_vecBuffUI[i];
					if( pEventWnd == bu.pSt_BuffPic )
					{
						tstring strTips = ToolTipCreator::Inst()->GetBuffTips( &bu.stBuff );
						ShowTip( bu.pSt_BuffPic, strTips.c_str() );
						m_dwLastUpdataTipsTime = Kernel::Inst()->GetAccumTimeDW();
						break;
					}
				}
			}
			break;
		default:
			break;
	}

	return FALSE;
}
