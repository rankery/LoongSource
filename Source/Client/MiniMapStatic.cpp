#include "StdAfx.h"
#include "MiniMapStatic.h"
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
#include "ToolTipFrame.h"

MiniMapStatic::MiniMapStatic() :
m_pColorTex( NULL ),
m_pAlphaTex( NULL ),
m_fZoom( 1.0f ),
m_nRadius( 0 ),
m_pImgDestMark( NULL ),
m_bShowDestMark( FALSE ),
m_dwDestMarkColor( 0xFFFFFFFF ),
m_dwDestMarkMapID( GT_INVALID )
{
	for( int i = 0; i < EMME_Num; i++ )
	{
		m_pElementImg[i] = NULL;
		m_bElementShow[i] = FALSE;
	}
	m_bElementShow[EMME_LocalPlayer]		= TRUE;
	m_bElementShow[EMME_AcceptQuestNPC]		= TRUE;
	m_bElementShow[EMME_CompleteQuestNPC]	= TRUE;
	m_bElementShow[EMME_Teammate]			= TRUE;
}

MiniMapStatic::~MiniMapStatic()
{

}

BOOL MiniMapStatic::Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement)
{
	if( !GUIStatic::Init( pSystem, pFather, pXmlElement ) )
		return FALSE;

	return TRUE;
}

VOID MiniMapStatic::Destroy()
{
	for( int i = 0; i < EMME_Num; i++ )
	{
		if( P_VALID( m_pElementImg[i] ) )
		{
			m_pRender->DestroyImage( m_pElementImg[i] );
			m_pElementImg[i] = NULL;
		}
	}

	if( P_VALID( m_pAlphaTex ) )
	{
		m_pRender->DestroyImage( m_pAlphaTex );
		m_pAlphaTex = NULL;
	}
	
	if( P_VALID( m_pColorTex ) )
	{
		m_pRender->DestroyImage( m_pColorTex );
		m_pColorTex = NULL;
	}
	
	GUIStatic::Destroy();
}

VOID MiniMapStatic::Update()
{
	if( !P_VALID( m_pColorTex ) )
		return;
	if( m_bInvisible )
		return;

	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();

	if( NULL == pLocalPlayer )
		return;

	const Vector3& vLPPos = pLocalPlayer->GetPos();
	m_ptMapCenter = WorldToMap( Vector2( vLPPos.x, vLPPos.z ) );

	// 边缘检测
	const INT nRectSize = (INT)( m_nRadius * m_fZoom + 0.5f );
	if( m_ptMapCenter.x - nRectSize < 0 )
		m_ptMapCenter.x = nRectSize;
	if( m_ptMapCenter.x + nRectSize > m_pColorTex->ptSize.x )
		m_ptMapCenter.x = m_pColorTex->ptSize.x - nRectSize;
	if( m_ptMapCenter.y - nRectSize < 0 )
		m_ptMapCenter.y = nRectSize;
	if( m_ptMapCenter.y + nRectSize > m_pColorTex->ptSize.y )
		m_ptMapCenter.y = m_pColorTex->ptSize.y - nRectSize;

	// 计算地图显示范围
	tagPoint ptViewMapSize( (LONG)( m_ptSize.x * m_fZoom + 0.5f ), 
		                    (LONG)( m_ptSize.y * m_fZoom + 0.5f ) );
	FLOAT fX = (FLOAT)m_ptCenterClient.x / m_ptSize.x;
	FLOAT fY = (FLOAT)m_ptCenterClient.y / m_ptSize.y;

	m_pColorTex->rc.left   = m_ptMapCenter.x - (LONG)( ptViewMapSize.x * fX + 0.5f );
	m_pColorTex->rc.top    = m_ptMapCenter.y - (LONG)( ptViewMapSize.y * fY + 0.5f );
	m_pColorTex->rc.right  = m_pColorTex->rc.left + ptViewMapSize.x;
	m_pColorTex->rc.bottom = m_pColorTex->rc.top  + ptViewMapSize.y;

	GUIStatic::Update();
}

VOID MiniMapStatic::Render()
{
	if( m_bInvisible )
		return GUIWnd::Render();

	if( !P_VALID( m_pColorTex ) || !P_VALID( m_pAlphaTex ) || !P_VALID( m_pPic ) || !P_VALID( m_pCanvas ) )
		return;

	if( !P_VALID( m_pColorTex->dwHandle ) || !P_VALID( m_pAlphaTex->dwHandle ) || !P_VALID( m_pPic->dwHandle ) )
		return;

	m_pCanvas->rc.SetLTWH( m_ptView, m_ptSize );

	tagRect rc = m_pCanvas->rc - m_pCanvas->ptOffset;
	IDraw2D::Inst()->Draw( (RECT*)&rc, 
		                   (const RECT*)&m_pColorTex->rc, 
		                   (const RECT*)&m_pAlphaTex->rc, 
		                   (ResTexture*)(m_pColorTex->dwHandle), 
		                   (ResTexture*)(m_pAlphaTex->dwHandle), ETFilter_LINEAR );

	const map<DWORD,Role*>& mapRole = RoleMgr::Inst()->GetRoleMap();
	for( map<DWORD,Role*>::const_iterator itr = mapRole.begin(); itr != mapRole.end(); itr++ )
	{
		if( TeamSys::Inst()->IsTeammate( itr->first ) )
			continue;

		Role* pRole = itr->second;
		if( NULL == pRole || !pRole->IsAttributeInited() )
			continue;
		const EMiniMapElement eEle = GetElementType( pRole );
		if( EMME_Num == eEle || EMME_LocalPlayer == eEle )
			continue;
		// 如果是被过滤的NPC不画
		if (EMME_AcceptQuestNPC == eEle)
		{
			bool bContinue = false;

			if (QuestMgr::Inst()->GetData()->IsNPCHasQuestByCurrentFilter(((NPC*)pRole)->GetTypeID()) == FALSE)
				bContinue = true;
			if (QuestMgr::Inst()->GetData()->IsFilterByPlayerLevel() == TRUE)
			{
				if (QuestMgr::Inst()->GetData()->IsNPCHasQuestByCurrentFilterByPlayerLevel(((NPC*)pRole)->GetTypeID()) == FALSE)
					bContinue = true;
			}
			if (bContinue)
				continue;


		}


		const Vector3 vPos = pRole->GetPos();
		tagPoint ptRole = WorldToMap( Vector2(vPos.x, vPos.z) );
		DrawElement( eEle, ptRole );
	}

	// 队友
	const vector<tagTeammateData>& vecTeammates = TeamSys::Inst()->GetTeammateList();
	for( size_t i = 0; i < vecTeammates.size(); i++ )
	{
		const tagTeammateData& data = vecTeammates[i];
		if( data.dwMapID == MapMgr::Inst()->GetCurMapID() )
		{
			const tagPoint ptPos = WorldToMap( Vector2( data.fX, data.fZ ) );
			DrawElement( EMME_Teammate, ptPos );
		}
	}

	// 寻路目的点标记
	if( m_bShowDestMark && m_dwDestMarkMapID == MapMgr::Inst()->GetCurMapID() && P_VALID(m_pImgDestMark) && P_VALID( m_pImgDestMark->dwHandle ) )
	{
		BYTE byAlpha = BYTE( m_dwDestMarkColor>>24 );
		byAlpha += (BYTE)( Kernel::Inst()->GetDeltaTime() * 0xFF );
		m_dwDestMarkColor = ( byAlpha << 24 ) | ( m_dwDestMarkColor & 0x00ffffff );
		tagPoint ptDestMark = WorldToMap( m_vDestMark );
		DrawImg( m_pImgDestMark, ptDestMark, m_dwDestMarkColor );
	}

	// 最后画本地玩家
	DrawLocalPlayer();

	m_pRender->Draw( m_pCanvas, m_pPic, m_dwPicColor, m_eAlphaWrite );

	return GUIWnd::Render();
}


BOOL MiniMapStatic::OnInputMessage(tagGUIInputMsg* pMsg)
{
	if( !P_VALID( m_pColorTex ) )
		return GUIStatic::OnInputMessage( pMsg );

	switch( pMsg->dwType )
	{
	case WM_KEYDOWN:
		break;
	case WM_MOUSEMOVE:
		// 显示TIPS
		if( PointInMap( pMsg->pt ) )
		{
			tstring strTip = g_StrTable[_T("TipsDefaultFont")];
			Vector2 vPointPos = MapToWorld( PointToMap( pMsg->pt ) );
			tstringstream stream;
			stream<<(LONG)( vPointPos.x / 50.0f )<<_T(" ")<<(LONG)( vPointPos.y / 50.0f )<<_T("\\n");
			strTip += stream.str().c_str();
			const map<DWORD,Role*>& mapRole = RoleMgr::Inst()->GetRoleMap();
			for( map<DWORD,Role*>::const_iterator itr = mapRole.begin(); itr != mapRole.end(); itr++ )
			{
				if( TeamSys::Inst()->IsTeammate( itr->first ) )
					continue;

				Role* pRole = itr->second;
				if( NULL == pRole || !pRole->IsAttributeInited() )
					continue;

				const EMiniMapElement eEle = GetElementType( pRole );
				if( EMME_Num == eEle || EMME_LocalPlayer == eEle )
					continue;

				Vector3 vPos = pRole->GetPos();
				if( IsMousePoint( eEle, Vector2( vPos.x, vPos.z ), pMsg->pt ) )
				{
					strTip += pRole->GetRoleName();
					strTip += _T("\\n");
				}
			}

			// 队友
			const vector<tagTeammateData>& vecTeammates = TeamSys::Inst()->GetTeammateList();
			for( size_t i = 0; i < vecTeammates.size(); i++ )
			{
				const tagTeammateData& data = vecTeammates[i];
				if( data.dwMapID == MapMgr::Inst()->GetCurMapID() )
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
		if( PointInMap( pMsg->pt ) )
		{
			tagPoint ptDestMark = PointToMap( pMsg->pt );
			Vector2 vDestMark = MapToWorld( ptDestMark );
			LPAutoMoveTo( vDestMark, 0.0f );
		}
		break;
	default:
		break;
	}
	return GUIStatic::OnInputMessage( pMsg );
}

VOID MiniMapStatic::SetInvisible(BOOL b)
{
	if( b && m_pSystem->GetMousePoint() == this )
		ShowTip( NULL, _T("") );

	GUIStatic::SetInvisible( b );
}

BOOL MiniMapStatic::PointInMap( tagPoint& ptView )
{
	if( !GUIStatic::PointInWnd( ptView, false ) )
		return FALSE;

	// 是否在圆内
	tagPoint ptCenterView = m_ptView + m_ptCenterClient;
	LONG lSqX = ( ptView.x - ptCenterView.x ) * ( ptView.x - ptCenterView.x );
	LONG lSqY = ( ptView.y - ptCenterView.y ) * ( ptView.y - ptCenterView.y );

	if( lSqX + lSqY > m_nRadius * m_nRadius )
		return FALSE;

	return TRUE;
}

BOOL MiniMapStatic::SetAlphaTex( const tstring strFilename )
{
	if( strFilename.empty() )
		return FALSE;

	if( P_VALID( m_pAlphaTex ) )
		m_pRender->DestroyImage( m_pAlphaTex );

	m_pAlphaTex = m_pRender->CreateImage( strFilename, tagRect() );

	if( !P_VALID( m_pAlphaTex ) )
		return FALSE;

	if( !P_VALID( m_pAlphaTex->dwHandle ) )
	{
		m_pRender->DestroyImage( m_pAlphaTex );
		m_pAlphaTex = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL MiniMapStatic::SetColorTex( const tstring strFilename )
{
	if( strFilename.empty() )
		return FALSE;

	if( P_VALID( m_pColorTex ) )
		m_pRender->DestroyImage( m_pColorTex );

	m_pColorTex = m_pRender->CreateImage( strFilename, tagRect() );

	if( !P_VALID( m_pColorTex ) )
		return FALSE;

	if( !P_VALID( m_pColorTex->dwHandle ) )
	{
		m_pRender->DestroyImage( m_pColorTex );
		m_pColorTex = NULL;
		return FALSE;
	}

	Update();
	SetRefresh( TRUE );
	return TRUE;
}

VOID MiniMapStatic::SetMapRect( const tagRect& rc )
{
	m_nRadius = ( rc.right - rc.left ) / 2;
	m_ptCenterClient.x = ( rc.right + rc.left ) / 2 - m_ptClient.x;
	m_ptCenterClient.y = ( rc.bottom + rc.top ) / 2 - m_ptClient.y;
}

BOOL MiniMapStatic::SetElementImg( const INT nIndex, tstring strFilename )
{
	ASSERT( nIndex >= 0 && nIndex < EMME_Num );

	if( strFilename.empty() )
		return FALSE;

	if( P_VALID( m_pElementImg[nIndex] ) )
		m_pRender->DestroyImage( m_pElementImg[nIndex] );

	m_pElementImg[nIndex] = m_pRender->CreateImage( strFilename, tagRect() );

	if( !P_VALID( m_pElementImg[nIndex] ) )
		return FALSE;

	if( !P_VALID( m_pElementImg[nIndex]->dwHandle ) )
	{
		m_pRender->DestroyImage( m_pElementImg[nIndex] );
		m_pElementImg[nIndex] = NULL;
		return FALSE;
	}

	return TRUE;
}

VOID MiniMapStatic::SetElementShow( const INT nIndex, BOOL bShow )
{
	ASSERT( nIndex >= 0 && nIndex < EMME_Num );
	m_bElementShow[nIndex] = bShow;
}

BOOL MiniMapStatic::GetElementShow( const INT nIndex ) const
{
	ASSERT( nIndex >= 0 && nIndex < EMME_Num );
	return m_bElementShow[nIndex];
}

VOID MiniMapStatic::SetZoom( FLOAT fZoom )
{
	if( fabsf( fZoom - m_fZoom ) > FLOAT_EPSILON )
		SetRefresh( TRUE );

	m_fZoom = fZoom;
	
}

tagPoint MiniMapStatic::WorldToMap( const Vector2& vPoint )
{
	ASSERT( P_VALID( m_pColorTex ) );
	
	tagPoint ptPoint( 0, 0 );
	ptPoint.x = LONG( ( vPoint.x - m_MapData.vWorldPos.x ) / m_MapData.vWorldSize.x * m_pColorTex->ptSize.x + 0.5f );
	ptPoint.y = LONG( ( vPoint.y - m_MapData.vWorldPos.y ) / m_MapData.vWorldSize.y * m_pColorTex->ptSize.y + 0.5f );
	// 地图图片的左下角为世界起点
	ptPoint.y = m_pColorTex->ptSize.y - ptPoint.y;
	return ptPoint;
}

Vector2 MiniMapStatic::MapToWorld( const tagPoint& ptPoint )
{
	ASSERT( P_VALID( m_pColorTex ) );

	Vector2 vPoint( 0.0f, 0.0f );
	vPoint.x = (FLOAT)ptPoint.x / m_pColorTex->ptSize.x * m_MapData.vWorldSize.x + m_MapData.vWorldPos.x;
	vPoint.y = (FLOAT)ptPoint.y / m_pColorTex->ptSize.y * m_MapData.vWorldSize.y;
	// 地图图片的左下角为世界起点
	vPoint.y = m_MapData.vWorldSize.y - vPoint.y + m_MapData.vWorldPos.y;
	return vPoint;
}

tagPoint MiniMapStatic::PointToMap( tagPoint ptPointView )
{
	ASSERT( P_VALID( m_pColorTex ) );
	tagPoint ptClient = ptPointView - m_ptView;
	tagPoint ptOffset = ptClient - m_ptCenterClient;
	ptOffset.x = LONG( ptOffset.x * m_fZoom + 0.5f );
	ptOffset.y = LONG( ptOffset.y * m_fZoom + 0.5f );
	return m_ptMapCenter + ptOffset;
}

tagPoint MiniMapStatic::UISizeToMap( tagPoint ptUI )
{
	ptUI.x = (LONG)( ptUI.x * m_fZoom + 0.5f );
	ptUI.y = (LONG)( ptUI.y * m_fZoom + 0.5f );
	return ptUI;
}

VOID MiniMapStatic::DrawLocalPlayer()
{
	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();

	if( NULL == pLocalPlayer )
		return;

	const Vector3& vLPPos = pLocalPlayer->GetPos();
	tagPoint ptPos = WorldToMap( Vector2( vLPPos.x, vLPPos.z ) );

	DrawElement( EMME_LocalPlayer, ptPos, pLocalPlayer->GetYaw() + D3DX_PI );
}


VOID MiniMapStatic::DrawElement( const INT nIndex, const tagPoint& ptPos, FLOAT fRotate )
{
	ASSERT( nIndex >= 0 && nIndex < EMME_Num );
	if( !m_bElementShow[nIndex] || !P_VALID( m_pElementImg[nIndex] ) || !P_VALID( m_pElementImg[nIndex]->dwHandle ) )
		return;

	tagGUIImage* pImg = m_pElementImg[nIndex];

	DrawImg( m_pElementImg[nIndex], ptPos, 0xFFFFFFFF, fRotate );
}

VOID MiniMapStatic::DrawImg( const tagGUIImage* pImg, const tagPoint& ptPos, DWORD dwMulColor/* = 0xFFFFFFFF*/, FLOAT fRotate/* = 0.0f */)
{
	tagRect rc;
	rc.left   =  m_ptCenterClient.x - pImg->ptSize.x / 2;
	rc.top    =  m_ptCenterClient.y - pImg->ptSize.y / 2;
	rc.right  = rc.left + pImg->ptSize.x;
	rc.bottom = rc.top + pImg->ptSize.y;

	tagPoint ptOffset( (LONG)( ( ptPos.x - m_ptMapCenter.x ) / m_fZoom + 0.5f ), 
					   (LONG)( ( ptPos.y - m_ptMapCenter.y ) / m_fZoom + 0.5f ) );
	rc = rc + ptOffset + m_ptView;

	// 如果在圆形区域之外，不画
	tagPoint ptCenterView( ( rc.right + rc.left ) / 2, ( rc.bottom + rc.top ) / 2 );
	if( !PointInMap( ptCenterView ) )
		return;

	IDraw2D::Inst()->Draw( (const RECT*)&rc, 
		(const RECT*)&pImg->rc, 
		(ResTexture*)pImg->dwHandle, 
		dwMulColor, ETFilter_POINT, fRotate );
}

VOID MiniMapStatic::LPAutoMoveTo( const Vector3& vDest, float fValidDist )
{
	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
	if( NULL == pLocalPlayer )
		return;

	tagMouseMoveEvent evt;
	evt.start = pLocalPlayer->GetPos();
	evt.end = vDest;
	evt.validDist = fValidDist;
	pLocalPlayer->OnGameEvent( &evt );

	ShowDestMark( MapMgr::Inst()->GetCurMapID(), Vector2( vDest.x, vDest.z ) );
}

VOID MiniMapStatic::LPAutoMoveTo( const Vector2& vDest, float fValidDist )
{
	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
	if( NULL == pLocalPlayer )
		return;

	tagMouseMoveEvent evt;
	evt.start = pLocalPlayer->GetPos();
	evt.end = Vector3( vDest.x, evt.start.y, vDest.y );
	evt.validDist = fValidDist;
	pLocalPlayer->OnGameEvent( &evt );

	ShowDestMark( MapMgr::Inst()->GetCurMapID(), vDest );
}

EMiniMapElement MiniMapStatic::GetElementType( const Role* pRole )
{
	ASSERT( P_VALID( pRole ) );

	if( pRole->IS_STATIC_CLASS( Player ) )
	{
		Player* pPlayer = (Player*)pRole;
		if( TeamSys::Inst()->IsTeammate( pPlayer->GetID() ) )
			return EMME_Teammate;									// 队友
		else
			return EMME_OtherPlayer;								// 其他玩家
	}
	else if( pRole->IS_STATIC_CLASS( LocalPlayer ) )
		return EMME_LocalPlayer;									// 本地玩家
	else if( pRole->IS_STATIC_CLASS( NPC ) )
	{
		NPC* pNPC = (NPC*)pRole;
		const tagCreatureProto* pProto = pNPC->GetProto();
		if( !P_VALID( pProto ) )
			return EMME_Num;
		if( ECT_Monster == pProto->eType )
		{
			if( EMTT_Normal == pProto->nType2 )
			{
				if( ER_Rare == pProto->eLite )
					return EMME_Boss;							// BOSS
				else
					return EMME_NormalMonster;					// 普通怪
			}
			else if( EMTT_Nest == pProto->nType2 )
			{
				return EMME_Nest;								// 怪物巢穴
			}
		}
		else if( ECT_GameObject == pProto->eType )
		{
			if( EGOT_Gather == pProto->nType2 )
				return EMME_NatuRes;							// 采集点
		}
		else if( ECT_NPC == pProto->eType )
		{
			if( QuestMgr::Inst()->GetQuery()->IsAcceptNPC( pNPC->GetTypeID() ) )
				return EMME_AcceptQuestNPC;                                                // 可接任务NPC
			else if( QuestMgr::Inst()->GetQuery()->IsCompleteNPC( pNPC->GetTypeID() ) )
				return EMME_CompleteQuestNPC;                                              // 可交任务NPC
			else
				return EMME_NormalNPC;                                                     // 其他NPC
		} 
	}
	return EMME_Num;
}

VOID MiniMapStatic::ShowDestMark( DWORD dwMapID, const Vector2& vDest )
{
	m_bShowDestMark = TRUE;
	m_dwDestMarkColor = 0xFFFFFFFF;
	m_vDestMark = vDest;
	m_dwDestMarkMapID = dwMapID;
}

VOID MiniMapStatic::CloseDestMark()
{
	m_bShowDestMark = FALSE;
	m_dwDestMarkMapID = GT_INVALID;
}

BOOL MiniMapStatic::SetDestMarkImg( const tstring& strFilename )
{
	if( strFilename.empty() )
		return FALSE;

	if( P_VALID( m_pImgDestMark ) )
		m_pRender->DestroyImage( m_pImgDestMark );

	m_pImgDestMark = m_pRender->CreateImage( strFilename, tagRect() );

	if( !P_VALID( m_pImgDestMark ) )
		return FALSE;

	if( !P_VALID( m_pImgDestMark->dwHandle ) )
	{
		m_pRender->DestroyImage( m_pImgDestMark );
		m_pImgDestMark = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL MiniMapStatic::IsMousePoint( const INT nIndex, const Vector2& vPosWorld, tagPoint& ptMouse )
{
	ASSERT( nIndex >= 0 && nIndex < EMME_Num );
	if( !m_bElementShow[nIndex] || !P_VALID( m_pElementImg[nIndex] ) || !P_VALID( m_pElementImg[nIndex]->dwHandle ) )
		return FALSE;

	tagPoint ptPosInMap = WorldToMap( vPosWorld );
	tagPoint ptSizeInMap = UISizeToMap( m_pElementImg[nIndex]->ptSize );
	tagRect rc;
	rc.left   =  ptPosInMap.x - ptSizeInMap.x / 2;
	rc.top    =  ptPosInMap.y - ptSizeInMap.y / 2;
	rc.right  =  rc.left + ptSizeInMap.x;
	rc.bottom =  rc.top  + ptSizeInMap.y;

	return rc.IsContain( PointToMap( ptMouse ) );
}