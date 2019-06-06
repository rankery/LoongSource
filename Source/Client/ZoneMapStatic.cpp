#include "StdAfx.h"
#include "ZoneMapStatic.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "MapMgr.h"
#include "Role.h"
#include "NPC.h"
#include "Player.h"
#include "QuestMgr.h"
#include "CombatEvent.h"
#include "..\WorldDefine\creature_define.h"
#include "TeamSys.h"
#include "MapLogicData.h"
#include "CreatureData.h"
#include "ToolTipFrame.h"

ZoneMapStatic::ZoneMapStatic() : 
m_dwCurShowMapID( GT_INVALID )
{
	for( int i = 0; i < EMME_Num; i++ )
	{
		m_pElementImg[i] = NULL;
		m_bElementShow[i] = FALSE;
	}
	m_bElementShow[EMME_LocalPlayer] = TRUE;
	m_bElementShow[EMME_Teammate]   = TRUE;
	m_bElementShow[EMME_AcceptQuestNPC] = TRUE;
	m_bElementShow[EMME_CompleteQuestNPC] = TRUE;
}

ZoneMapStatic::~ZoneMapStatic()
{

}

BOOL ZoneMapStatic::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	if( !MiniMapStatic::Init( pSystem, pFather, pXmlElement ) )
		return FALSE;

	return TRUE;
}

VOID ZoneMapStatic::Destroy()
{
	MiniMapStatic::Destroy();
}

VOID ZoneMapStatic::Update()
{
	if( !P_VALID( m_pColorTex ) )
		return;
	if( m_bInvisible )
		return;

	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();

	if( NULL == pLocalPlayer )
		return;

	const Vector3& vLPPos = pLocalPlayer->GetPos();
	m_ptLP = WorldToMap( Vector2( vLPPos.x, vLPPos.z ) );

	// 子窗口也需要实时渲染
	std::list<GUIWnd*>::reverse_iterator it;
	for(it=m_listChild.rbegin(); it!=m_listChild.rend(); ++it)
		(*it)->SetRefresh();

	GUIStatic::Update();
}

VOID ZoneMapStatic::Render()
{
	if( m_bInvisible )
		return GUIWnd::Render();

	if( !P_VALID( m_pColorTex ) || !P_VALID( m_pCanvas ) )
		return;

	if( !P_VALID( m_pColorTex->dwHandle ) )
		return;

	// 地图
	m_pCanvas->rc.SetLTWH( m_ptView, m_ptSize );
	m_pRender->Draw( m_pCanvas, m_pColorTex, m_dwPicColor, m_eAlphaWrite );

	if( m_dwCurShowMapID == MapMgr::Inst()->GetCurMapID() )
	{
		// 可接任务NPC
		const QuestQuery::NPCSet& setAcceptNPCs = QuestMgr::Inst()->GetQuery()->GetAcceptQuestNPCs();
		for( QuestQuery::NPCSet::const_iterator cItr = setAcceptNPCs.begin(); cItr != setAcceptNPCs.end(); cItr++ )
		{
			bool bContinue = false;

			if (QuestMgr::Inst()->GetData()->IsNPCHasQuestByCurrentFilter((*cItr)) == FALSE)
				bContinue = true;
			if (QuestMgr::Inst()->GetData()->IsFilterByPlayerLevel() == TRUE)
			{
				if (QuestMgr::Inst()->GetData()->IsNPCHasQuestByCurrentFilterByPlayerLevel((*cItr)) == FALSE)
					bContinue = true;
			}
			if (bContinue)
				continue;


			vector<Vector3> vecPos;
			QuestMgr::Inst()->GetQuery()->GetNPCPosition( (*cItr), vecPos );
			for( size_t n = 0; n < vecPos.size(); n++ )
			{
				const tagPoint ptPos = WorldToMap( Vector2( vecPos[n].x, vecPos[n].z ) );
				DrawElement( EMME_AcceptQuestNPC, ptPos );
			}
		}

		// 可交任务NPC
		const QuestQuery::NPCSet& setCompleteNPCs = QuestMgr::Inst()->GetQuery()->GetCompleteQuestNPCs();
		for( QuestQuery::NPCSet::const_iterator cItr = setCompleteNPCs.begin(); cItr != setCompleteNPCs.end(); cItr++ )
		{
			vector<Vector3> vecPos;
			QuestMgr::Inst()->GetQuery()->GetNPCPosition( (*cItr), vecPos );
			for( size_t n = 0; n < vecPos.size(); n++ )
			{
				const tagPoint ptPos = WorldToMap( Vector2( vecPos[n].x, vecPos[n].z ) );
				DrawElement( EMME_CompleteQuestNPC, ptPos );
			}
		}
	}


	// 队友
	const vector<tagTeammateData>& vecTeammates = TeamSys::Inst()->GetTeammateList();
	for( size_t i = 0; i < vecTeammates.size(); i++ )
	{
		const tagTeammateData& data = vecTeammates[i];
		if( data.dwMapID == m_dwCurShowMapID )
		{
			const tagPoint ptPos = WorldToMap( Vector2( data.fX, data.fZ ) );
			DrawElement( EMME_Teammate, ptPos );
		}
	}

	// 寻路目的点标记
	if( m_bShowDestMark && m_dwDestMarkMapID == m_dwCurShowMapID && P_VALID(m_pImgDestMark) && P_VALID( m_pImgDestMark->dwHandle ) )
	{
		BYTE byAlpha = BYTE( m_dwDestMarkColor>>24 );
		byAlpha += (BYTE)( Kernel::Inst()->GetDeltaTime() * 0xFF );
		m_dwDestMarkColor = ( byAlpha << 24 ) | ( m_dwDestMarkColor & 0x00ffffff );
		tagPoint ptDestMark = WorldToMap( m_vDestMark );
		DrawImg( m_pImgDestMark, ptDestMark, m_dwDestMarkColor );
	}


	// 本地玩家
	if( m_dwCurShowMapID == MapMgr::Inst()->GetCurMapID() )
	{
		DrawElement( EMME_LocalPlayer, m_ptLP, RoleMgr::Inst()->GetLocalPlayer()->GetYaw() + D3DX_PI );
	}

	return GUIWnd::Render();
}

BOOL ZoneMapStatic::OnInputMessage(tagGUIInputMsg* pMsg)
{
	if( !P_VALID( m_pColorTex ) )
		return GUIStatic::OnInputMessage( pMsg );

	switch( pMsg->dwType )
	{
		case WM_MOUSEMOVE:
			// 显示TIPS
			if( PointInWnd( pMsg->pt ) )
			{
				tstring strTip = g_StrTable[_T("TipsDefaultFont")];
				Vector2 vPointPos = MapToWorld( PointToMap( pMsg->pt ) );
				tstringstream stream;
				stream<<(LONG)( vPointPos.x / 50.0f )<<_T(" ")<<(LONG)( vPointPos.y / 50.0f )<<_T("\\n");
				strTip += stream.str().c_str();

				if( m_dwCurShowMapID == MapMgr::Inst()->GetCurMapID() )
				{
					// 可接任务NPC
					const QuestQuery::NPCSet& setAcceptNPCs = QuestMgr::Inst()->GetQuery()->GetAcceptQuestNPCs();
					for( QuestQuery::NPCSet::const_iterator cItr = setAcceptNPCs.begin(); cItr != setAcceptNPCs.end(); cItr++ )
					{
						vector<Vector3> vecPos;
						QuestMgr::Inst()->GetQuery()->GetNPCPosition( (*cItr), vecPos );
						for( size_t n = 0; n < vecPos.size(); n++ )
						{
							if( IsMousePoint( EMME_AcceptQuestNPC, Vector2( vecPos[n].x, vecPos[n].z ), pMsg->pt ) )
							{
								const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt( (*cItr) );
								ASSERT( P_VALID( pProto ) );
								strTip += pProto->szName;
								strTip += _T("\\n");
							}
						}
					}

					// 可交任务NPC
					const QuestQuery::NPCSet& setCompleteNPCs = QuestMgr::Inst()->GetQuery()->GetCompleteQuestNPCs();
					for( QuestQuery::NPCSet::const_iterator cItr = setCompleteNPCs.begin(); cItr != setCompleteNPCs.end(); cItr++ )
					{
						vector<Vector3> vecPos;
						QuestMgr::Inst()->GetQuery()->GetNPCPosition( (*cItr), vecPos );
						for( size_t n = 0; n < vecPos.size(); n++ )
						{
							if( IsMousePoint( EMME_CompleteQuestNPC, Vector2( vecPos[n].x, vecPos[n].z ), pMsg->pt ) )
							{
								const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt( (*cItr) );
								ASSERT( P_VALID( pProto ) );
								strTip += pProto->szName;
								strTip += _T("\\n");
							}
						}
					}
				}

				// 队友
				const vector<tagTeammateData>& vecTeammates = TeamSys::Inst()->GetTeammateList();
				for( size_t i = 0; i < vecTeammates.size(); i++ )
				{
					const tagTeammateData& data = vecTeammates[i];
					if( data.dwMapID == m_dwCurShowMapID )
					{
						Vector2 vPos = Vector2( data.fX, data.fZ );
						if( IsMousePoint( EMME_Teammate, vPos, pMsg->pt ) )
						{
							strTip += data.strName;
							strTip += _T("\\n");
						}
					}
				}
				ShowTip( pMsg->pt + tagPoint( 16, 16 ), strTip.c_str() );
			}
			else
			{
				ShowTip( NULL, _T("") );
			}
			break;
		case WM_LBUTTONDBLCLK:
			if( PointInWnd( pMsg->pt ) )
			{
				// 当前地图
				if( MapMgr::Inst()->GetCurMapID() == m_dwCurShowMapID )
				{
					tagPoint ptDestMark = PointToMap( pMsg->pt );
					Vector2 vDestMark = MapToWorld( ptDestMark );
					LPAutoMoveTo( vDestMark, 0.0f );
				}
				// 跨地图寻路
				else
				{
					const tstring& strCurMapName = MapMgr::Inst()->GetCurMapName();
					const tagMapLogicData* pDestMapLogicData = MapLogicData::Inst()->FindMapLogicData( m_dwCurShowMapID );
					ASSERT( P_VALID( pDestMapLogicData ) );
					NavMap* pNav = MapMgr::Inst()->GetNavMap();
					PlayerNavMap* pPlayerNav = pNav->GetPlayerNavMap();
					Vector3 vSwitchPos( 0.0f, 0.0f, 0.0f );
					const bool bFind = pPlayerNav->FindPathByMap( strCurMapName.c_str(), pDestMapLogicData->szName.c_str(), vSwitchPos );
					if( bFind )
						LPAutoMoveTo( vSwitchPos, 0.0f );
				}
			}
			break;
		default:
			break;
	}
	return GUIStatic::OnInputMessage( pMsg );
}

tagPoint ZoneMapStatic::PointToMap( tagPoint ptPoint )
{
	ASSERT( P_VALID( m_pColorTex ) );
	tagPoint ptControl = ptPoint - m_ptView;
	ptControl.x = LONG( ptControl.x * ( (FLOAT)m_pColorTex->ptSize.x / m_ptSize.x ) + 0.5f );
	ptControl.y = LONG( ptControl.y * ( (FLOAT)m_pColorTex->ptSize.y / m_ptSize.y ) + 0.5f );
	return ptControl;
}

tagPoint ZoneMapStatic::UISizeToMap( tagPoint ptUI )
{
	ASSERT( P_VALID( m_pColorTex ) );
	ptUI.x = (LONG)( ptUI.x * ( (FLOAT)m_pColorTex->ptSize.x / m_ptSize.x ) + 0.5f );
	ptUI.y = (LONG)( ptUI.y * ( (FLOAT)m_pColorTex->ptSize.y / m_ptSize.y ) + 0.5f );
	return ptUI;
}

VOID ZoneMapStatic::DrawElement( const INT nIndex, const tagPoint& ptPos, FLOAT fRotate )
{
	ASSERT( nIndex >= 0 && nIndex < EMME_Num );
	if( !m_bElementShow[nIndex] || !P_VALID( m_pElementImg[nIndex] ) || !P_VALID( m_pElementImg[nIndex]->dwHandle ) )
		return;

	tagGUIImage* pImg = m_pElementImg[nIndex];

	DrawImg( m_pElementImg[nIndex], ptPos, 0xFFFFFFFF, fRotate );
}

VOID ZoneMapStatic::DrawImg( const tagGUIImage* pImg, const tagPoint& ptPos, DWORD dwMulColor/* = 0xFFFFFFFF*/, FLOAT fRotate/* = 0.0f */)
{
	ASSERT( P_VALID( pImg ) && P_VALID( pImg->dwHandle ) );

	tagRect rc;
	rc.left   = LONG( ptPos.x * ( (FLOAT)m_ptSize.x / m_pColorTex->ptSize.x ) + 0.5f ) - pImg->ptSize.x / 2;
	rc.top    = LONG( ptPos.y * ( (FLOAT)m_ptSize.y / m_pColorTex->ptSize.y ) + 0.5f ) - pImg->ptSize.y / 2;
	rc.right  = rc.left + pImg->ptSize.x;
	rc.bottom = rc.top + pImg->ptSize.y;

	// 出界，不画
	if( rc.left < 0 || rc.right > m_ptSize.x || rc.top < 0 || rc.bottom > m_ptSize.y )
		return;

	tagRect rcDest = rc + m_ptView - m_pCanvas->ptOffset;
	IDraw2D::Inst()->Draw(	(const RECT*)&rcDest, 
							(const RECT*)&pImg->rc, 
							(ResTexture*)pImg->dwHandle, 
							dwMulColor, ETFilter_POINT, fRotate, IDraw2D::EAWT_NoWrite );
}
