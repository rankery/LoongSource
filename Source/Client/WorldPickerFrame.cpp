#include "StdAfx.h"
#include "WorldPickerFrame.h"
#include "MapMgr.h"
#include "Role.h"
#include "RoleMgr.h"
#include "GroundItemMgr.h"
#include "NPC.h"
#include "CursorMgr.h"
#include "player.h"
#include "LocalPlayer.h"
#include "StallMgr.h"
#include "Render2D.h"
#include "CombatActionChecker.h"
#include "QuestMgr.h"
#include "QuestQuery.h"

WorldPickerFrame::WorldPickerFrame(void) : 
m_trunk(this), 
m_eMousePointType( EMPT_None ),
m_dwMousePointRoleID( GT_INVALID ),
m_vMousePointScenePos( 0.0f, 0.0f, 0.0f ),
m_vMousePointWaterPos( 0.0f, 0.0f, 0.0f ),
m_pGroundItem( NULL ),
m_pStall( NULL ),
m_pScenePickDecal( NULL ),
m_bDecalAttached( false ),
m_dwSelectedRoleID( GT_INVALID )
{
}

WorldPickerFrame::~WorldPickerFrame(void)
{
}

BOOL WorldPickerFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	if( !GameFrame::Init(pMgr,pFather,dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("OpenMap"), (FRAMEEVENTPROC)m_trunk.sfp1(&WorldPickerFrame::OnOpenMap));
	m_pMgr->RegisterEventHandle( _T("CloseMap"), (FRAMEEVENTPROC)m_trunk.sfp1(&WorldPickerFrame::OnCloseMap));
	m_pMgr->RegisterEventHandle( _T("tagShowScenePickDecalEvent"), (FRAMEEVENTPROC)m_trunk.sfp1(&WorldPickerFrame::OnShowScenePickDecalEvent));
	m_pMgr->RegisterEventHandle( _T("tagRolePickEvent"), (FRAMEEVENTPROC)m_trunk.sfp1(&WorldPickerFrame::OnRolePickEvent));

	return TRUE;
}

BOOL WorldPickerFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("OpenMap"), (FRAMEEVENTPROC)m_trunk.sfp1(&WorldPickerFrame::OnOpenMap));
	m_pMgr->UnRegisterEventHandler( _T("CloseMap"), (FRAMEEVENTPROC)m_trunk.sfp1(&WorldPickerFrame::OnCloseMap));
	m_pMgr->UnRegisterEventHandler( _T("tagShowScenePickDecalEvent"), (FRAMEEVENTPROC)m_trunk.sfp1(&WorldPickerFrame::OnShowScenePickDecalEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagRolePickEvent"), (FRAMEEVENTPROC)m_trunk.sfp1(&WorldPickerFrame::OnRolePickEvent));

	CursorMgr::Inst()->SetIdleCursor( ECIN_Default );

	return GameFrame::Destroy();
}

VOID WorldPickerFrame::Update()
{
	if( NULL != m_pScenePickDecal && !m_bDecalAttached )
	{
		SceneGraph* pSG = MapMgr::Inst()->GetSceneGraph();
		if( NULL != pSG )
		{
			pSG->AttachDynamicNode( m_pScenePickDecal );
			m_bDecalAttached = true;
			m_pScenePickDecal->Close();
		}
	}

	CursorMgr::Inst()->SetIdleCursor( ECIN_Default );
	if( m_pGUI->GetMousePoint() != m_pGUI->GetDesktop() )
		return;


	if( NULL == MapMgr::Inst()->GetSceneGraph() )
		return;

	CameraBase *pCamera = MapMgr::Inst()->GetSceneGraph()->GetCamera();
	if( NULL == pCamera )
		return;

	NavMap* pNav = MapMgr::Inst()->GetNavMap();
	if( NULL == pNav )
		return;
	NavCollider* pCollider = pNav->GetCollider();
	if( NULL == pCollider || !pCollider->IsLoaded() )
		return;

	const tagPoint& ptCursor = CursorMgr::Inst()->GetCursorPos();
	Ray ray;
	pCamera->GetMouseRay( ptCursor.x, ptCursor.y, ray );
	UpdataMousePoint( ray, m_pInput->IsKeyDown( DIK_LSHIFT ) );
	UpdataCursor();

	// 左键单击
	if( m_pInput->IsMouseDownOnce(0) )
	{
		if( EMPT_Role == m_eMousePointType )									// 发送选中角色游戏事件
		{
			// 已经死亡的NPC不可选中
			Role* pRole = RoleMgr::Inst()->FindRole( m_dwMousePointRoleID );
			if( P_VALID( pRole ) && 
				!( pRole->IS_KIND_OF( NPC ) && pRole->IsDead() ) )
			{
				tagRolePickEvent evt( _T("tagRolePickEvent"), this );
				evt.eType = EWPT_LClick;
				evt.dwRoleID = m_dwMousePointRoleID;
				m_pMgr->SendEvent( &evt );
			}
		}		
		else if( EMPT_Scene == m_eMousePointType )								// 发送场景点击游戏事件
		{
			tagScenePickEvent evt( _T("tagScenePickEvent"), this );
			evt.eType = EWPT_LClick;
			evt.vPickPos = m_vMousePointScenePos;
			m_pMgr->SendEvent( &evt );
			ReBuildScenePickDecal( m_vMousePointScenePos, ray );
		}
		else if( EMPT_Water == m_eMousePointType )								// 发送场景点击游戏事件(水面)
		{
			tagScenePickEvent evt( _T("tagScenePickEvent"), this );
			evt.eType = EWPT_LClick;
			evt.vPickPos = m_vMousePointWaterPos;
			evt.vPickPos.y = MapMgr::Inst()->GetNavMap()->GetCollider()->CalcSwimFootHeight( m_vMousePointWaterPos, RoleMgr::Inst()->GetLocalPlayer()->GetRoleSize().y, m_vMousePointWaterPos.y );
			m_pMgr->SendEvent( &evt );
			ReBuildScenePickDecal( m_vMousePointWaterPos, ray, true );
		}
		else if( EMPT_GroundItem == m_eMousePointType )							// 发送地面物品点击游戏事件
		{
			tagGroundItemPickEvent evt( _T("tagGroundItemPickEvent"), this );
			evt.eType = EWPT_LClick;
			evt.pGroundItem = m_pGroundItem;
			m_pMgr->SendEvent( &evt );
		}
		else if( EMPT_Stall == m_eMousePointType )								// 发送地面摊位点击游戏事件
		{
			tagRolePickEvent evt( _T("WorldEvent_StallPick"), this );
			evt.eType = EWPT_LClick;
			evt.dwRoleID = m_pStall->dwRoleID;
			m_pMgr->SendEvent(&evt);
		}
	}
}

VOID WorldPickerFrame::UpdataMousePoint( const Ray& ray, const BOOL bIgnorePlayer )
{
	// 清空数据
	m_eMousePointType = EMPT_None;
	m_dwMousePointRoleID = GT_INVALID;
	m_vMousePointScenePos = Vector3( 0.0f, 0.0f, 0.0f );
	m_vMousePointWaterPos = Vector3( 0.0f, 0.0f, 0.0f );
	m_pGroundItem = NULL;
	m_pStall = NULL;

	tagMousePointObject mpo[EMPT_Num];

	// 拾取场景
	NavMap* pNav = MapMgr::Inst()->GetNavMap();
	NavCollider* pCollider = pNav->GetCollider();
	mpo[EMPT_Scene].bValid = pCollider->RayCollideBoxAndTrn( ray, mpo[EMPT_Scene].vPos );
	m_vMousePointScenePos = mpo[EMPT_Scene].vPos;

	// 拾取水面
	mpo[EMPT_Water].bValid = pCollider->RayCollideWater( ray, mpo[EMPT_Water].vPos );
	m_vMousePointWaterPos = mpo[EMPT_Water].vPos;

	// 拾取角色
	DWORD dwPickRoleID = RoleMgr::Inst()->PickRole( ray, bIgnorePlayer );
	Role* pRole = RoleMgr::Inst()->FindRole( dwPickRoleID );
	if( P_VALID( pRole ) &&
		!( pRole->IS_KIND_OF( NPC ) && pRole->IsDead() ) )
	{
		mpo[EMPT_Role].bValid = true;
		mpo[EMPT_Role].vPos = pRole->GetPos();
		m_dwMousePointRoleID = dwPickRoleID;
	}

	// 拾取地面物品
	tagGroundItem* pGroundItem = GroundItemMgr::Inst()->PickItem( ray );
	if( NULL != pGroundItem )
	{
		mpo[EMPT_GroundItem].bValid = true;
		mpo[EMPT_GroundItem].vPos = pGroundItem->space.GetPos();
		m_pGroundItem = pGroundItem;
	}
	
	// 拾取摊位
	tagStall* pStall = StallMgr::Inst()->PickStall( ray );
	if (NULL != pStall )
	{
		mpo[EMPT_Stall].bValid = true;
		mpo[EMPT_Stall].vPos = pStall->space.GetPos();
		m_pStall = pStall;
	}

	// 取最近的
	float fPickDistSq = FLOAT_MAX;
	for( int i = 0; i < EMPT_Num; i++ )
	{
		// 先不取水面
		if( EMPT_Water == i )
			continue; 
		if( mpo[i].bValid )
		{
			if (EMPT_Role == i)
			{
				if (ray.origin.x <= mpo[i].vPos.x)
					mpo[i].vPos.x -= pRole->GetRoleSize().x / 2;
				else
					mpo[i].vPos.x += pRole->GetRoleSize().x / 2;
				if (ray.origin.z <= mpo[i].vPos.z)
					mpo[i].vPos.z -= pRole->GetRoleSize().z / 2;
				else
					mpo[i].vPos.z += pRole->GetRoleSize().z / 2;

				mpo[i].vPos.y += pRole->GetRoleSize().y;
			}


			float fDistSq = Vec3DistSq( ray.origin, mpo[i].vPos );
			if( fDistSq < fPickDistSq)
			{
				m_eMousePointType = (EMousePointType)i;
				fPickDistSq = fDistSq;
			}
		}
	}
	// 点到地面时，再取一下水面
	if( EMPT_Scene == m_eMousePointType && mpo[EMPT_Water].bValid )
	{
		const float fDistWaterSq = Vec3DistSq( ray.origin, mpo[EMPT_Water].vPos );
		if( fDistWaterSq < fPickDistSq )
			m_eMousePointType = EMPT_Water;
	}
}

VOID WorldPickerFrame::UpdataCursor()
{
	if( EMPT_Role == m_eMousePointType )
	{
		Role* pRole = RoleMgr::Inst()->FindRole( m_dwMousePointRoleID );
		if( !P_VALID( pRole ) )
			return;
		if( pRole->IsDead() )
			return;

		// 可以攻击的目标
		if( CombatActionChecker::Inst()->IfCanAttack( pRole->GetID() ) )
		{
			CursorMgr::Inst()->SetIdleCursor( ECIN_Attack );
			return;
		}

		if( pRole->IS_STATIC_CLASS( NPC ) )
		{
			NPC* pNPC = (NPC*)pRole;
			const tagCreatureProto* pProto = pNPC->GetProto();
			if( !P_VALID( pProto ) )
				return;

			if( ECT_GameObject == pProto->eType )                               
			{
				if( EGOT_Gather == pProto->nType2 )								// 采集点
					CursorMgr::Inst()->SetIdleCursor( ECIN_Gather );
				else if( EGOT_Normal == pProto->nType2 )						// 普通可交互地物
					CursorMgr::Inst()->SetIdleCursor( ECIN_NPCTalk );
				else if ( EGOT_Chest == pProto->nType2 )                        // 宝箱
					CursorMgr::Inst()->SetIdleCursor( ECIN_DigBao );
				else if ( EGOT_CommonInves == pProto->nType2 || ( EGOT_QuestInves == pProto->nType2 && QuestMgr::Inst()->GetQuery()->IsInvesObject(pProto->dwTypeID) ) )
					CursorMgr::Inst()->SetIdleCursor( ECIN_NPCTalk );           // 可调查地物
				else if ( EGOT_QuestTalk == pProto->nType2 )
					CursorMgr::Inst()->SetIdleCursor( ECIN_NPCTalk );           // 任务对话地物
			}
			else if( ECT_NPC == pProto->eType )
			{
				if( EFNPCT_Shop == pProto->eFunctionType )                      // 商店NPC
					CursorMgr::Inst()->SetIdleCursor( ECIN_NPCShop );
				else															// 其他NPC
					CursorMgr::Inst()->SetIdleCursor( ECIN_NPCTalk );
			}
		}
	}
	else if( EMPT_GroundItem == m_eMousePointType )
	{
		CursorMgr::Inst()->SetIdleCursor( ECIN_PickGroundItem );
	}
	else if( EMPT_Stall == m_eMousePointType )
	{
		CursorMgr::Inst()->SetIdleCursor( ECIN_NPCShop );
	}
}

DWORD WorldPickerFrame::GetCurMousePointRoleID() const
{
	if( EMPT_Role == m_eMousePointType )
		return m_dwMousePointRoleID;
	else
		return GT_INVALID;
}

tagGroundItem* WorldPickerFrame::GetCurMousePointGroundItem() const
{
	if( EMPT_GroundItem == m_eMousePointType )
		return m_pGroundItem;
	else
		return NULL;
}

DWORD WorldPickerFrame::OnOpenMap( tagGameEvent* )
{
	m_pScenePickDecal = new SGEffectNode;
	m_pScenePickDecal->LoadRes( _T("data\\system\\Effect\\combat\\Txqt02.sfx") );
	return 0;
}

DWORD WorldPickerFrame::OnCloseMap( tagGameEvent* )
{
	if( !m_bDecalAttached )
		SAFE_DEL( m_pScenePickDecal );
	m_pScenePickDecal = NULL;
	m_bDecalAttached = false;

	return 0;
}

DWORD WorldPickerFrame::OnShowScenePickDecalEvent( tagShowScenePickDecalEvent* pEvent )
{
	if( NULL != m_pScenePickDecal )
	{
		if( pEvent->bShow )
			m_pScenePickDecal->Play();
		else
			m_pScenePickDecal->Close();
	}
	return 0;
}

DWORD WorldPickerFrame::OnRolePickEvent( tagRolePickEvent* pEvent )
{
	if( EWPT_LClick == pEvent->eType )
		m_dwSelectedRoleID = pEvent->dwRoleID;

	return 0;
}

VOID WorldPickerFrame::ReBuildScenePickDecal( const Vector3& pickPos, const Ray& ray, const bool bWater/* = false*/ )
{
	if( NULL == m_pScenePickDecal )
		return;

	vector<EffectBase*> vecEff;
	m_pScenePickDecal->GetEffectArray( vecEff );
	if( 0 == vecEff.size() )
		return;

	Transform trans;
	trans.Translate(pickPos);
	m_pScenePickDecal->SetWorldMat( trans.GetMatrix() );

	if( bWater )
	{
		for( size_t i = 0; i < vecEff.size(); i++ )
		{
			EffectBase* pEff = vecEff[i];
			if( !pEff->IS_STATIC_CLASS( EffectSceneDecal ) )
				continue;
			EffectSceneDecal* pDecal = (EffectSceneDecal*)pEff;
			pDecal->SetDir( -MathConst::AxisY, MathConst::AxisZ );
		}
		m_pScenePickDecal->Play();
		return;
	}

	// 设置贴花法线
	float fHalfDecalWidth = 0.0f;
	float fHalfDecalHeight = 0.0f;
	for( size_t i = 0; i < vecEff.size(); i++ )
	{
		EffectBase* pEff = vecEff[i];
		if( !pEff->IS_STATIC_CLASS( EffectSceneDecal ) )
			continue;
		EffectSceneDecal* pDecal = (EffectSceneDecal*)pEff;
		if( fHalfDecalWidth < pDecal->GetDecalSize().x * 0.5f )
			fHalfDecalWidth = pDecal->GetDecalSize().x * 0.5f;
		if( fHalfDecalHeight < pDecal->GetDecalSize().y * 0.5f )
			fHalfDecalHeight = pDecal->GetDecalSize().y * 0.5f;
	}

	// 计算贴花方向，计算出与射线垂直的平面上贴花矩形的四个顶点,从这四个点发出4条射线拾取场景中的4个点
	const Vector3 vRef = MathConst::AxisY;
	float d = D3DXVec3Dot( &ray.origin, &ray.dir );
	Vector3 vRefPointInPlane = vRef + ray.dir * ( d - D3DXVec3Dot( &vRef, &ray.dir ) );
	Vector3 vTop, vRight;

	D3DXVec3Normalize( &vTop, &( vRefPointInPlane - ray.origin ) );
	vTop *= fHalfDecalHeight;
	D3DXVec3Cross( &vRight, &ray.dir, &vTop );
	D3DXVec3Normalize( &vRight, &vRight );
	vRight *= fHalfDecalWidth;

	Vector3 vTopRight = vTop + vRight;
	Vector3 vTopLeft = vTop - vRight;

	Ray rayPick[4];
	Vector3 vPickPos[4];
	bool bCollideOK[4];
	rayPick[0].origin = ray.origin + vTopLeft;
	rayPick[1].origin = ray.origin + vTopRight;
	rayPick[2].origin = ray.origin - vTopLeft;
	rayPick[3].origin = ray.origin - vTopRight;

	NavMap* pNav = MapMgr::Inst()->GetNavMap();
	for( int i = 0; i < 4; i++ )
	{
		rayPick[i].dir = ray.dir;
		bCollideOK[i] = pNav->GetCollider()->RayCollideBoxAndTrn( rayPick[i], vPickPos[i] );
	}

	// 根据4个采样点计算贴花的法线
	Vector3 vDecalNormal( 0.0f, 0.0f, 0.0f );
	for( int i = 0; i < 4; i++ )
	{
		int nNext = ( i + 1 ) % 4;
		if( !bCollideOK[i] || !bCollideOK[nNext] )
			continue;

		// 为了减少纹理扭曲的现象，使用采样点与中心点的距离调制法线
		float fLen0 =  D3DXVec3Length( &( vPickPos[i] - pickPos ) );
		float fLen1 =  D3DXVec3Length( &( vPickPos[nNext] - pickPos ) );
		float fInvLen = 1.0f / max( fLen0, fLen1 );

		Vector3 vNormal;
		D3DXVec3Cross( &vNormal, &( vPickPos[i] - pickPos ), &( vPickPos[nNext] - pickPos ) );
		D3DXVec3Normalize( &vNormal, &vNormal );
		vNormal *= fInvLen;
		vDecalNormal += vNormal;
	}
	D3DXVec3Normalize( &vDecalNormal, &vDecalNormal );

	for( size_t i = 0; i < vecEff.size(); i++ )
	{
		EffectBase* pEff = vecEff[i];
		if( !pEff->IS_STATIC_CLASS( EffectSceneDecal ) )
			continue;
		EffectSceneDecal* pDecal = (EffectSceneDecal*)pEff;
		pDecal->SetDir( -vDecalNormal, vTop );
	}

	m_pScenePickDecal->Play();
}