//-------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: move_data.cpp
// author: Aslan
// actor:
// data: 2009-01-15
// last:
// brief: 移动结构
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "map.h"
#include "unit.h"
#include "role.h"
#include "creature.h"
#include "move_data.h"
#include "pet_pocket.h"

//---------------------------------------------------------------------------------------------
// 静态变量
//---------------------------------------------------------------------------------------------
Timer MoveData::m_Timer;

//---------------------------------------------------------------------------------------------
// 格子移动控制器
//---------------------------------------------------------------------------------------------
NavCollider_TileMove::NavCollider_TileMove() : m_vStart(), m_vDest(), m_fTotalTime(0.0f), m_fCurTime(0.0f), m_nMoveTicks(0), m_fXZSpeed(0.0f)
{

}

NavCollider_TileMove::~NavCollider_TileMove() {}

VOID NavCollider_TileMove::Init(const Vector3& vStart, const Vector3& vDest, FLOAT fXZSpeed)
{
	ASSERT( fXZSpeed > 0.0f );

	m_vStart	=	vStart;
	m_vDest		=	vDest;
	m_fXZSpeed	=	fXZSpeed;

	// 计算两点距离
	FLOAT fDistanceX = m_vDest.x - m_vStart.x;
	FLOAT fDistanceZ = m_vDest.z - m_vStart.z;
	FLOAT fDistanceSquare = fDistanceX * fDistanceX + fDistanceZ * fDistanceZ;

	// 计算总时间和总心跳数
	m_fTotalTime = IFASTCODE->FastSqrt(fDistanceSquare) / m_fXZSpeed;
	m_nMoveTicks = (INT)(m_fTotalTime * 1000.0f) / TICK_TIME;
	m_nMoveTicks += ( (INT(m_fTotalTime * 1000.0f) - TICK_TIME * m_nMoveTicks) > 0 ? 1 : 0 );

	// 当前移动时间
	m_fCurTime = 0.0f;
}

ENavResult NavCollider_TileMove::Update(Map* pMap, Vector3& vOutPos)
{
	// 如果本身就不能移动
	if( m_nMoveTicks <= 0 )
	{
		vOutPos = m_vStart;
		return ENR_Arrive;
	}

	// 减去tick
	if( --m_nMoveTicks > 0 )
	{
		m_fCurTime += (FLOAT)TICK_TIME / 1000.0f;
	}
	else
	{
		m_fCurTime = m_fTotalTime;
	}

	vOutPos.x = m_vStart.x + (m_vDest.x - m_vStart.x) * (m_fCurTime / m_fTotalTime);
	vOutPos.z = m_vStart.z + (m_vDest.z - m_vStart.z) * (m_fCurTime / m_fTotalTime);
	vOutPos.y = pMap->GetGroundHeight(vOutPos.x, vOutPos.z);

	if( m_nMoveTicks > 0 )
	{
		return ENR_ToBeContinued;
	}
	else
	{
		return ENR_Arrive;
	}
}

//---------------------------------------------------------------------------------------------
// 检测坐标
//---------------------------------------------------------------------------------------------
BOOL MoveData::IsPosInvalid(const Vector3& vPos)
{
	// 检测是不是实数
	if( _isnan(vPos.x) || _isnan(vPos.y) || _isnan(vPos.z) )
		return TRUE;

	// 检测是不是无穷
	if( 0 == _finite(vPos.x) || 0 == _finite(vPos.y) || 0 == _finite(vPos.z) )
	{
		return TRUE;
	}

	return FALSE;	
}

//---------------------------------------------------------------------------------------------
// 检查朝向
//---------------------------------------------------------------------------------------------
BOOL MoveData::IsFaceInvalid(const Vector3& vFace)
{
	// 检测是不是实数
	if( _isnan(vFace.x) || _isnan(vFace.y) || _isnan(vFace.z) )
		return TRUE;

	// 检测是不是无穷
	if( 0 == _finite(vFace.x) || 0 == _finite(vFace.y) || 0 == _finite(vFace.z) )
	{
		return TRUE;
	}

	// 将坐标归一化后查看有问题
	Vector3 vNewFace = vFace;
	Vec3Normalize(vNewFace);

	if( _isnan(vNewFace.x) || _isnan(vNewFace.y) || _isnan(vNewFace.z) )
		return TRUE;

	if( 0 == _finite(vNewFace.x) || 0 == _finite(vNewFace.y) || 0 == _finite(vNewFace.z) )
	{
		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------------------------
// 初始化移动结构，主要用于玩家和生物初始化时的移动状态
//---------------------------------------------------------------------------------------------
VOID MoveData::Init(Unit* pOwner, const FLOAT fX, const FLOAT fY, const FLOAT fZ, const FLOAT fFaceX, const FLOAT fFaceY, const FLOAT fFaceZ)
{
	m_pOwner			=		pOwner;
	m_eCurMove			=		EMS_Stand;
	m_eNextPreMove		=		EMS_Stand;
	m_vPos.x			=		fX;
	m_vPos.y			=		fY;
	m_vPos.z			=		fZ;
	m_vFace.x			=		fFaceX;
	m_vFace.y			=		fFaceY;
	m_vFace.z			=		fFaceZ;
	m_vPosStart			=		m_vPos;
	m_vDest				=		m_vPos;
	m_vDir				=		m_vFace;
	m_bWaitClientMsg	=		FALSE;
	m_nVisTileIndex		=		GT_INVALID;
	m_bIsStopped		=		FALSE;
}

//----------------------------------------------------------------------------------------------
// 重置移动结构，这主要用于玩家切换地图和怪物重生时，一般很少用
//----------------------------------------------------------------------------------------------
VOID MoveData::Reset(const FLOAT fX, const FLOAT fY, const FLOAT fZ, const FLOAT fFaceX, const FLOAT fFaceY, const FLOAT fFaceZ)
{
	m_eCurMove			=		EMS_Stand;
	m_eNextPreMove		=		EMS_Stand;
	m_vPos.x			=		fX;
	m_vPos.y			=		fY;
	m_vPos.z			=		fZ;
	m_vFace.x			=		fFaceX;
	m_vFace.y			=		fFaceY;
	m_vFace.z			=		fFaceZ;
	m_vPosStart			=		m_vPos;
	m_vDest				=		m_vPos;
	m_vDir				=		m_vFace;
	m_bWaitClientMsg	=		FALSE;
	m_nVisTileIndex		=		GT_INVALID;
	m_bIsStopped		=		FALSE;
}

//-----------------------------------------------------------------------------------------------
// 人物行走
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StartRoleWalk(const Vector3& vStart, const Vector3& vEnd)
{
	EMoveRet eRet = IsCanRoleWalk(vStart, vEnd);

	if( EMR_Success == eRet )
	{
		GetColliderWalk()->Init(vStart, vEnd, m_pOwner->GetSize(), m_pOwner->GetXZSpeed());

		m_eCurMove			=	EMS_Walk;
		m_eNextPreMove		=	EMS_Stand;
		m_vPosStart			=	vStart;
		m_vDest				=	vEnd;
		m_vDir				=	vEnd - vStart;
		m_bWaitClientMsg	=	FALSE;
		m_fStartTime		=	m_Timer.GetElapse();
		m_bIsStopped		=	FALSE;

		SetFaceTo(m_vDir);
		OnStartMove();
		OnPosChange(vStart);
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//-----------------------------------------------------------------------------------------------
// 人物跳跃
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StartRoleJump(const Vector3& vStart, const Vector3& vDir)
{
	EMoveRet eRet = IsCanRoleJump(vStart, vDir);

	if( EMR_Success == eRet )
	{
		m_pOwner->m_fJumpXZSpeed	=	m_pOwner->GetXZSpeed();
		m_pOwner->m_fJumpYSpeed		=	m_pOwner->m_fYSpeed;
		GetColliderJump()->Init(vStart, vDir, m_pOwner->GetSize(), m_pOwner->m_fJumpXZSpeed, m_pOwner->m_fJumpYSpeed);

		m_eCurMove			=	EMS_Jump;
		m_eNextPreMove		=	EMS_Stand;
		m_vPosStart			=	vStart;
		m_vDest				=	vStart;
		m_vDir				=	vDir;
		m_bWaitClientMsg	=	FALSE;
		m_fStartTime		=	m_Timer.GetElapse();
		m_bIsStopped		=	FALSE;

		SetFaceTo(m_vDir);
		OnStartMove();
		OnPosChange(vStart);
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//------------------------------------------------------------------------------------------------
// 人物掉落
//------------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StartRoleDrop(const Vector3& vStart, const Vector3& vDir)
{
	EMoveRet eRet = IsCanRoleDrop(vStart, vDir);

	if( EMR_Success == eRet )
	{
		GetColliderJump()->Init(vStart, vDir, m_pOwner->GetSize(), m_pOwner->m_fDropXZSpeed, 0.0f);

		m_eCurMove			=	EMS_Drop;
		m_eNextPreMove		=	EMS_Stand;
		m_vPosStart			=	vStart;
		m_vDest				=	vStart;
		m_vDir				=	vDir;
		m_bWaitClientMsg	=	FALSE;
		m_fStartTime		=	m_Timer.GetElapse();
		m_bIsStopped		=	FALSE;

		SetFaceTo(m_vDir);
		OnStartMove();
		OnPosChange(vStart);
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//------------------------------------------------------------------------------------------------
// 人物垂直掉落
//------------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StartRoleVDrop(const Vector3& vStart)
{
	EMoveRet eRet = IsCanRoleVDrop(vStart);

	if( EMR_Success == eRet )
	{
		GetColliderJump()->Init(vStart, MathConst::Zero3, m_pOwner->GetSize(), 0.0f, 0.0f);

		m_eCurMove			=	EMS_VDrop;
		m_eNextPreMove		=	EMS_Stand;
		m_vPosStart			=	vStart;
		m_vDest				=	vStart;
		m_bWaitClientMsg	=	FALSE;
		m_fStartTime		=	m_Timer.GetElapse();
		m_bIsStopped		=	FALSE;

		OnStartMove();
		OnPosChange(vStart);
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//------------------------------------------------------------------------------------------------
// 人物滑落
//------------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StartRoleSlide(const Vector3& vStart)
{
	EMoveRet eRet = IsCanRoleSlide(vStart);

	if( EMR_Success == eRet )
	{
		GetColliderSlide()->Init(vStart, m_pOwner->GetSize());

		m_eCurMove			=	EMS_Slide;
		m_eNextPreMove		=	EMS_Stand;
		m_vPosStart			=	vStart;
		m_vDest				=	vStart;
		m_bWaitClientMsg	=	FALSE;
		m_fStartTime		=	m_Timer.GetElapse();
		m_bIsStopped		=	FALSE;

		OnStartMove();
		OnPosChange(vStart);
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//-----------------------------------------------------------------------------------------------
// 人物游泳
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StartRoleSwim(const Vector3& vStart, const Vector3& vEnd)
{
	EMoveRet eRet = IsCanRoleSwim(vStart, vEnd);

	if( EMR_Success == eRet )
	{
		GetColliderSwim()->Init(vStart, vEnd, m_pOwner->GetSize(), m_pOwner->m_fSwimXZSpeed);

		m_eCurMove			=	EMS_Swim;
		m_eNextPreMove		=	EMS_SwimStand;
		m_vPosStart			=	vStart;
		m_vDest				=	vEnd;
		m_vDir				=	vEnd - vStart;
		m_bWaitClientMsg	=	FALSE;
		m_fStartTime		=	m_Timer.GetElapse();
		m_bIsStopped		=	FALSE;

		SetFaceTo(m_vDir);
		OnStartMove();
		OnPosChange(vStart);
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//-----------------------------------------------------------------------------------------------
// 怪物行走（bPatrol代表是否巡逻，bCheck代表是否检测路径）
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StartCreatureWalk(const Vector3& vEnd, EMoveState eState, BOOL bCheck)
{
	EMoveRet eRet = IsCanCreatureWalk(vEnd, eState, bCheck);

	if( EMR_Success == eRet )
	{
		FLOAT fRealSpeed = GetCreatureMoveStateSpeed(eState);

		Creature* pCreature = static_cast<Creature*>(m_pOwner);

		if( !pCreature->NeedCollide() )		// 非碰撞怪
		{
			GetColliderTileWalk()->Init(m_vPos, vEnd, fRealSpeed);
		}
		else
		{
			GetColliderNPCWalk()->Init(m_vPos, vEnd, m_pOwner->GetSize(), fRealSpeed, (bool)bCheck);
		}

		m_eCurMove			=	eState;
		m_eNextPreMove		=	EMS_Stand;
		m_vPosStart			=	m_vPos;
		m_vDest				=	vEnd;
		m_vDir				=	vEnd - m_vPos;
		m_bWaitClientMsg	=	FALSE;
		m_fStartTime		=	m_Timer.GetElapse();
		m_bIsStopped		=	FALSE;

		SetFaceTo(m_vDir);
		OnStartMove();
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//-----------------------------------------------------------------------------------------------
// 被击飞
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StartHitFly(const Vector3& vDir)
{
	return MoveData::EMR_Success;
}

//-----------------------------------------------------------------------------------------------
// 人物停止移动
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StopRoleMove(const Vector3& vPos)
{
	EMoveRet eRet = IsCanRoleStop(vPos);

	if( EMR_Success == eRet )
	{
		m_eCurMove			=		EMS_Stand;
		m_eNextPreMove		=		EMS_Stand;
		m_vPosStart			=		vPos;
		m_vDest				=		vPos;
		m_bWaitClientMsg	=		FALSE;
		m_bIsStopped		=		FALSE;

		OnPosChange(vPos);
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//-----------------------------------------------------------------------------------------------
// 停止游泳
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StopRoleSwim(const Vector3& vPos)
{
	EMoveRet eRet = IsCanRoleStop(vPos);

	if( EMR_Success == eRet )
	{
		m_eCurMove			=		EMS_SwimStand;
		m_eNextPreMove		=		EMS_SwimStand;
		m_vPosStart			=		vPos;
		m_vDest				=		vPos;
		m_bWaitClientMsg	=		FALSE;
		m_bIsStopped		=		FALSE;

		OnPosChange(vPos);
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return eRet;
}

//-----------------------------------------------------------------------------------------------
// 服务器停止移动，一般用于减速等情况，如果处在不能随便停止移动的状态，则不停止
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StopMove(BOOL bSend)
{
	// 不能停止的移动
	if( EMS_Drop	==	m_eCurMove	||
		EMS_VDrop	==	m_eCurMove	||
		EMS_Slide	==	m_eCurMove	||
		EMS_Jump	==	m_eCurMove )
		return EMR_Conflict;

	// 本来就是站立或漂浮状态
	if( EMS_Stand == m_eCurMove || EMS_SwimStand == m_eCurMove )
		return EMR_Success;

	// 根据是移动还是游泳分别切换到站立或漂浮
	if( EMS_Swim == m_eCurMove )
	{
		m_eCurMove			=		EMS_SwimStand;
		m_eNextPreMove		=		EMS_SwimStand;
	}
	else if( EMS_Walk			==	m_eCurMove ||
		EMS_CreaturePatrol	==	m_eCurMove ||
		EMS_CreatureWalk	==	m_eCurMove ||
		EMS_CreatureFlee	==	m_eCurMove )
	{
		m_eCurMove			=		EMS_Stand;
		m_eCurMove			=		EMS_Stand;
	}

	m_vPosStart			=		m_vPos;
	m_vDest				=		m_vPos;
	m_bWaitClientMsg	=		FALSE;
	m_bIsStopped		=		TRUE;

	// 同步给周围玩家
	if( bSend )
	{
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return EMR_Success;
}

//-----------------------------------------------------------------------------------------------
// 停止移动，并朝向一个新的方向
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StopMove(const Vector3& vNewFace, BOOL bSend)
{
	// 检测一下朝向
	if( IsFaceInvalid(vNewFace) )
	{
		ILOG->Write(_T("face invalid when try to rotate, face=<%f, %f, %f>\r\n"),
					vNewFace.x, vNewFace.y, vNewFace.z);
		return EMR_Invalid;
	}

	// 不能停止的移动
	if( EMS_Drop	==	m_eCurMove	||
		EMS_VDrop	==	m_eCurMove	||
		EMS_Slide	==	m_eCurMove	||
		EMS_Jump	==	m_eCurMove )
		return EMR_Conflict;

	// 本来就是站立或漂浮状态
	if( EMS_Stand == m_eCurMove || EMS_SwimStand == m_eCurMove )
		return EMR_Success;

	// 根据是移动还是游泳分别切换到站立或漂浮
	if( EMS_Swim == m_eCurMove )
	{
		m_eCurMove			=		EMS_SwimStand;
		m_eNextPreMove		=		EMS_SwimStand;
	}
	else if( EMS_Walk			==	m_eCurMove ||
		EMS_CreaturePatrol	==	m_eCurMove ||
		EMS_CreatureWalk	==	m_eCurMove ||
		EMS_CreatureFlee	==	m_eCurMove )
	{
		m_eCurMove			=		EMS_Stand;
		m_eCurMove			=		EMS_Stand;
	}

	m_vPosStart			=		m_vPos;
	m_vDest				=		m_vPos;
	m_bWaitClientMsg	=		FALSE;
	m_bIsStopped		=		TRUE;

	SetFaceTo(vNewFace);

	// 同步给周围玩家
	if( bSend )
	{
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}

	return EMR_Success;
}

//-----------------------------------------------------------------------------------------------
// 服务器强制停止移动
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::StopMoveForce(BOOL bSend)
{
	// 本来就是站立或漂浮状态
	if( EMS_Stand == m_eCurMove || EMS_SwimStand == m_eCurMove )
		return EMR_Success;

	// 如果正在移动或游泳或滑落，则立即切换到当前状态
	if( EMS_Walk			==	m_eCurMove ||
		EMS_CreaturePatrol	==	m_eCurMove ||
		EMS_CreatureWalk	==	m_eCurMove ||
		EMS_Swim			==	m_eCurMove ||
		EMS_Slide			==	m_eCurMove ||
		EMS_CreatureFlee	==	m_eCurMove )
	{
		if( EMS_Swim == m_eCurMove )
		{
			m_eCurMove			=		EMS_SwimStand;
			m_eNextPreMove		=		EMS_SwimStand;
		}
		else
		{
			m_eCurMove			=		EMS_Stand;
			m_eCurMove			=		EMS_Stand;
		}

		m_vPosStart			=		m_vPos;
		m_vDest				=		m_vPos;
		m_bWaitClientMsg	=		FALSE;
		m_bIsStopped		=		TRUE;

		// 同步给周围玩家
		if( bSend )
		{
			m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
		}

		return EMR_Success;
	}

	// 如果正在跳跃，掉落或滑落
	if( EMS_Jump		==	m_eCurMove ||
		EMS_Drop		==	m_eCurMove ||
		EMS_VDrop		==	m_eCurMove )
	{
		DropDownStandPoint(bSend);
		m_bIsStopped = TRUE;
		return EMR_Success;
	}

	return EMR_Success;
}

//-----------------------------------------------------------------------------------------------
// 瞬移到某个位置站立
//-----------------------------------------------------------------------------------------------
VOID MoveData::ForceTeleport(const Vector3& vPos, BOOL bSend)
{
	DropDownStandPoint(vPos, bSend);
}

//-----------------------------------------------------------------------------------------------
// 更新移动
//-----------------------------------------------------------------------------------------------
VOID MoveData::Update()
{
	if( EMS_Stand == m_eCurMove || EMS_SwimStand == m_eCurMove )
		return;

	if( m_bWaitClientMsg )
		return;

	switch( m_eCurMove )
	{
	case EMS_Walk:
		UpdateRoleWalk();
		break;

	case EMS_Jump:
		UpdateRoleJump();
		break;

	case EMS_Drop:
		UpdateRoleDrop();
		break;

	case EMS_VDrop:
		UpdateRoleVDrop();
		break;

	case EMS_Slide:
		UpdateRoleSlide();
		break;

	case EMS_Swim:
		UpdateRoleSwim();
		break;

	case EMS_CreaturePatrol:
	case EMS_CreatureWalk:
	case EMS_CreatureFlee:
		UpdateCreatureWalk();
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------------------------
// 更新人物行走
//-----------------------------------------------------------------------------------------------
VOID MoveData::UpdateRoleWalk()
{
	NavCollider* pCollider = m_pOwner->GetMap()->GetNavMap()->GetCollider();
	Role* pOwnerRole = static_cast<Role*>(m_pOwner);

	// 更新坐标
	Vector3 vOutPos;
	float fEndTime = 0.0f;
	DWORD dwCarrierID = GT_INVALID;

	ENavResult eResult = GetColliderWalk()->Update(pCollider, m_Timer.GetElapse() - m_Timer.GetDelta(), 
						m_Timer.GetDelta(), vOutPos, fEndTime, dwCarrierID, (bool)pOwnerRole->IsInRoleState(ERS_WaterWalk));
	OnPosChange(vOutPos);


	if( ENR_ToBeContinued != eResult )
	{
		// 当前移动停止
		if( ENR_Arrive == eResult || ENR_Blocking == eResult || ENR_WillCarry )
		{
			m_eCurMove			=	EMS_Stand;
			m_eNextPreMove		=	EMS_Stand;
		}
		else if( ENR_WillOnWater == eResult )
		{
			m_eCurMove			=	EMS_SwimStand;
			m_eNextPreMove		=	EMS_SwimStand;
		}
		else if( ENR_WillSwim == eResult )
		{
			m_eCurMove			=	EMS_SwimStand;
			m_eNextPreMove		=	EMS_SwimStand;
		}
		else if( ENR_WillDrop == eResult )
		{
			m_eCurMove			=	EMS_Drop;
			m_eNextPreMove		=	EMS_Stand;
			m_bWaitClientMsg	=	TRUE;
		}
		else if( ENR_WillSlide == eResult )
		{
			m_eCurMove			=	EMS_Slide;
			m_eNextPreMove		=	EMS_Stand;
			m_bWaitClientMsg	=	TRUE;
		}
		else
		{
			m_eCurMove			=	EMS_Stand;
			m_eNextPreMove		=	EMS_Stand;
		}

		// 如果是返回blocking的话，则发送给远程玩家
		if( ENR_Blocking == eResult )
		{
			m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
		}
	}
}

//-----------------------------------------------------------------------------------------------
// 更新人物跳跃
//-----------------------------------------------------------------------------------------------
VOID MoveData::UpdateRoleJump()
{
	NavCollider* pCollider = m_pOwner->GetMap()->GetNavMap()->GetCollider();
	Role* pOwnerRole = static_cast<Role*>(m_pOwner);

	// 更新坐标
	Vector3 vOutPos;
	FLOAT fEndTime = 0.0f;
	DWORD dwCarrierID = GT_INVALID;
	FLOAT fCDCarrierTime = 0.0f;

	ENavResult eResult = GetColliderJump()->Update(pCollider, m_Timer.GetElapse() - m_Timer.GetDelta(), 
						m_Timer.GetDelta(), NULL, vOutPos, fEndTime, dwCarrierID, fCDCarrierTime, (bool)pOwnerRole->IsInRoleState(ERS_WaterWalk));
	OnPosChange(vOutPos);

	if( ENR_ToBeContinued != eResult )
	{
		if( ENR_Landed == eResult || ENR_WillCarry == eResult )
		{
// 			Role* pOwnerRole = static_cast<Role*>(m_pOwner);
// 			pOwnerRole->GetPetPocket()->CalledPetLeavePocket();

			m_eCurMove			=	EMS_Stand;
			m_eNextPreMove		=	EMS_Stand;
		}
		else if( ENR_WillSwim == eResult )
		{
			m_eCurMove			=	EMS_SwimStand;
			m_eNextPreMove		=	EMS_SwimStand;
		}
		else if( ENR_WillOnWater == eResult  )
		{
			m_eCurMove			=	EMS_SwimStand;
			m_eNextPreMove		=	EMS_SwimStand;
		}
		else if( ENR_WillVDrop == eResult )
		{
			m_eCurMove			=	EMS_VDrop;
			m_eNextPreMove		=	EMS_Stand;
			m_bWaitClientMsg	=	TRUE;
		}
		else if( ENR_WillSlide == eResult )
		{
			m_eCurMove			=	EMS_Slide;
			m_eNextPreMove		=	EMS_Stand;
			m_bWaitClientMsg	=	TRUE;
		}
		else
		{
			m_eCurMove			=	EMS_Stand;
			m_eNextPreMove		=	EMS_Stand;
		}
	}
}

//-----------------------------------------------------------------------------------------------
// 更新人物掉落
//-----------------------------------------------------------------------------------------------
VOID MoveData::UpdateRoleDrop()
{
	UpdateRoleJump();
}

//-----------------------------------------------------------------------------------------------
// 更新人物垂直掉落
//-----------------------------------------------------------------------------------------------
VOID MoveData::UpdateRoleVDrop()
{
	UpdateRoleJump();
}

//-----------------------------------------------------------------------------------------------
// 更新人物滑落
//-----------------------------------------------------------------------------------------------
VOID MoveData::UpdateRoleSlide()
{
	NavCollider* pCollider = m_pOwner->GetMap()->GetNavMap()->GetCollider();
	Role* pOwnerRole = static_cast<Role*>(m_pOwner);

	// 更新坐标
	Vector3 vOutPos;
	FLOAT fEndTime = 0.0f;

	ENavResult eResult = GetColliderSlide()->Update(pCollider, m_Timer.GetElapse() - m_Timer.GetDelta(), 
						m_Timer.GetDelta(), vOutPos, fEndTime, (bool)pOwnerRole->IsInRoleState(ERS_WaterWalk));
	OnPosChange(vOutPos);

	// 检查返回值
	if( ENR_ToBeContinued != eResult )
	{
		if( ENR_Blocking == eResult )
		{
			m_eCurMove		=	EMS_Stand;
			m_eNextPreMove	=	EMS_Stand;
		}
		else if( ENR_WillSwim == eResult )
		{
			m_eCurMove		=	EMS_SwimStand;
			m_eNextPreMove	=	EMS_SwimStand;
		}
		else if( ENR_WillOnWater == eResult )
		{
			m_eCurMove		=	EMS_SwimStand;
			m_eNextPreMove	=	EMS_SwimStand;
		}
		else
		{
			m_eCurMove		=	EMS_Stand;
			m_eNextPreMove	=	EMS_Stand;
		}
	}
}

//-----------------------------------------------------------------------------------------------
// 更新人物游泳
//-----------------------------------------------------------------------------------------------
VOID MoveData::UpdateRoleSwim()
{
	NavCollider* pCollider = m_pOwner->GetMap()->GetNavMap()->GetCollider();
	Role* pOwnerRole = static_cast<Role*>(m_pOwner);

	// 更新坐标
	Vector3 vOutPos;
	FLOAT fEndTime = 0.0f;
	DWORD dwCarrierID = GT_INVALID;

	ENavResult eResult = GetColliderSwim()->Update(pCollider, m_Timer.GetElapse() - m_Timer.GetDelta(), 
							m_Timer.GetDelta(), vOutPos, fEndTime, (bool)pOwnerRole->IsInRoleState(ERS_WaterWalk));
	OnPosChange(vOutPos);

	// 检测返回值
	if( ENR_ToBeContinued != eResult )
	{
		if( ENR_Arrive == eResult || ENR_Blocking == eResult )
		{
			m_eCurMove		=	EMS_SwimStand;
			m_eNextPreMove	=	EMS_SwimStand;
		}
		else if( ENR_SwimLanded == eResult )
		{
// 			Role* pOwnerRole = static_cast<Role*>(m_pOwner);
// 			pOwnerRole->GetPetPocket()->CalledPetLeavePocket();

			m_eCurMove		=	EMS_Stand;
			m_eNextPreMove	=	EMS_Stand;
		}
		else
		{
			m_eCurMove		=	EMS_Stand;
			m_eNextPreMove	=	EMS_Stand;
		}
	}
}

//-----------------------------------------------------------------------------------------------
// 更新怪物行走
//-----------------------------------------------------------------------------------------------
VOID MoveData::UpdateCreatureWalk()
{
	Creature* pCreature = static_cast<Creature*>(m_pOwner);

	// 更新坐标
	Vector3 vOutPos;
	FLOAT fEndTime = 0.0f;
	DWORD dwCarrierID = GT_INVALID;

	ENavResult eResult = ENR_ToBeContinued;

	// 非碰撞怪
	if( !pCreature->NeedCollide() )
	{
		eResult = GetColliderTileWalk()->Update(pCreature->GetMap(), vOutPos);
	}
	// 碰撞怪
	else
	{
		NavCollider* pCollider = m_pOwner->GetMap()->GetNavMap()->GetCollider();
		eResult = GetColliderNPCWalk()->Update(pCollider, m_Timer.GetElapse() - m_Timer.GetDelta(), 
					m_Timer.GetDelta(), vOutPos, fEndTime, dwCarrierID);
	}
	OnPosChange(vOutPos);

	// 检查返回值
	if( ENR_ToBeContinued != eResult )
	{
		if( ENR_Arrive == eResult || ENR_Blocking == eResult || ENR_WillCarry == eResult )
		{
			m_eCurMove		=	EMS_Stand;
			m_eNextPreMove	=	EMS_Stand;
		}
		else
		{
			m_eCurMove		=	EMS_Stand;
			m_eNextPreMove	=	EMS_Stand;
		}
	}

	//m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
}

//-----------------------------------------------------------------------------------------------
// 更新被击飞
//-----------------------------------------------------------------------------------------------
VOID MoveData::UpdateHitFly()
{

}


//-----------------------------------------------------------------------------------------------
// 根据当前坐标落下，找到一个碰撞位置站立，可能会到游泳
//------------------------------------------------------------------------------------------------
VOID MoveData::DropDownStandPoint(BOOL bSend)
{
	AABBox box = m_pOwner->GetAABBox();
	m_vPos.y = m_pOwner->GetMap()->GetGroundAndModelHeight(box);


	BOOL bWaterWalk = FALSE;	// 是否水上行走

	if( m_pOwner->IsRole() )
	{
		Role* pRole = static_cast<Role*>(m_pOwner);
		bWaterWalk = pRole->IsInRoleState(ERS_WaterWalk);
	}

	BOOL bOnWater = FALSE;
	FLOAT fY = 0.0f;
	if( bWaterWalk )
	{
		bOnWater = m_pOwner->GetMap()->IfWillOnWater(m_vPos, fY);
	}
	else
	{
		bOnWater = m_pOwner->GetMap()->IfWillSwim(m_vPos, m_pOwner->GetSize().y, fY);
	}

	if( bOnWater )
	{
		m_vPos.y = fY;	// 修正一下坐标
		// 要游泳
		m_eCurMove		=	EMS_SwimStand;
		m_eNextPreMove	=	EMS_SwimStand;
	}
	else
	{
		// 站立
		m_eCurMove		=	EMS_Stand;
		m_eNextPreMove	=	EMS_Stand;
	}

	m_vPosStart			=		m_vPos;
	m_vDest				=		m_vPos;
	m_bWaitClientMsg	=		FALSE;

	// 同步给周围玩家
	if( bSend )
	{
		m_pOwner->GetMap()->SyncMovementToBigVisTile(m_pOwner);
	}
}

//-------------------------------------------------------------------------------------
// 根据一个新坐标落下，找到一个碰撞位置站立，可能会到游泳
//-------------------------------------------------------------------------------------
VOID MoveData::DropDownStandPoint(const Vector3& vPos, BOOL bSend)
{
	// 检测一下坐标
	if( IsPosInvalid(vPos) )
	{
		ILOG->Write(_T("pos invalid when try to drop stand point, pos=<%f, %f, %f>\r\n"),
			vPos.x, vPos.y, vPos.z);
		return;
	}

	StopMoveForce(FALSE);
	OnPosChange(vPos);

	DropDownStandPoint(bSend);
}

//-------------------------------------------------------------------------------------
// 位置改变
//-------------------------------------------------------------------------------------
VOID MoveData::OnPosChange(const Vector3& vNewPos)
{
	// 加入坐标检测函数
	if( IsPosInvalid(vNewPos) )
	{
		ILOG->Write(_T("Invalid Pos, Current movestate: %d\r\n"), m_eCurMove);
	}

	// 修正坐标
	Vector3 vPos = vNewPos;
	m_pOwner->GetMap()->FixPos(vPos);

	m_vPos = vPos;	// 设置为新坐标

	INT nNewVisIndex = m_pOwner->GetMap()->WorldPosToVisIndex(m_vPos);
	if( m_nVisTileIndex != nNewVisIndex )
	{
		m_pOwner->GetMap()->SyncChangeVisTile(m_pOwner, m_nVisTileIndex, nNewVisIndex);
	}

	// 如果是隐身单位，需同步给周围玩家
	if( m_pOwner->IsInStateInvisible() )
	{
		m_pOwner->GetMap()->UpdateLurk2BigVisTileRole(m_pOwner);
	}

	// 如果是玩家，则检测地图区域，及将周围隐身玩家同步给他
	if( m_pOwner->IsRole() )
	{
		Role* pRole = static_cast<Role*>(m_pOwner);
		pRole->CheckMapArea();

		pRole->GetMap()->UpdateBigVisTileLurkUnit2Role(pRole);
	}
}

//-------------------------------------------------------------------------------------
// 开始移动
//-------------------------------------------------------------------------------------
VOID MoveData::OnStartMove()
{
	if( EMS_Stand == m_eCurMove || EMS_SwimStand == m_eCurMove )
		return;

	// 检测技能起手打断
	m_pOwner->GetCombatHandler().InterruptPrepare(CombatHandler::EIT_Move, FALSE);

	// 检测技能施放打断
	m_pOwner->GetCombatHandler().InterruptOperate(CombatHandler::EIT_Move, m_eCurMove);

	// 检测buff打断
	m_pOwner->OnInterruptBuffEvent(EBIF_Move);
}

//-------------------------------------------------------------------------------------
// 地图是否合法
//-------------------------------------------------------------------------------------
BOOL MoveData::IsMapValid()
{
	// 如果当前没有地图或者地图没有导航图，则直接返回
	Map* pMap = m_pOwner->GetMap();
	if( FALSE == P_VALID(pMap) ) return FALSE;

	NavMap* pNav = pMap->GetNavMap();
	if( FALSE == P_VALID(pNav) ) return FALSE;

	return TRUE;
}

//-------------------------------------------------------------------------------------
// 检测距离
//-------------------------------------------------------------------------------------
BOOL MoveData::IsInValidDistance(const Vector3& vStart)
{
	FLOAT fDistanceSQ =  Vec3DistSq(m_vPos, vStart);
	if( fDistanceSQ > g_world.GetLaxCheatDistanceSQ() )
	{
		return FALSE;
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------
// 玩家是否可以行走
//-------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::IsCanRoleWalk(const Vector3& vStart, const Vector3& vEnd)
{
	// 如果不是玩家，返回
	if( !m_pOwner->IsRole() ) return EMR_Invalid;

	// 地图非法
	if( !IsMapValid() ) return EMR_Invalid;

	// 检测一下坐标
	if( IsPosInvalid(vStart) || IsPosInvalid(vEnd) )
	{
		ILOG->Write(_T("client pos invalid when try to walk, start=<%f, %f, %f>, end=<%f, %f, %f>\r\n"),
			vStart.x, vStart.y, vStart.z, vEnd.x, vEnd.y, vEnd.z);
		return EMR_Invalid;
	}

	// 地图判断坐标
	if( !m_pOwner->GetMap()->IsPosValid(vStart) || !m_pOwner->GetMap()->IsPosValid(vEnd) )
		return EMR_Invalid;


	// 检测一下距离
	if( !IsInValidDistance(vStart) )
	{
		return EMR_Invalid;
	}

	// 检测一下速度
	if( m_pOwner->GetXZSpeed() <= 0.0f )
	{
		ILOG->Write(_T("find a role which have a xz speed less than 0, ID: %u, Speed: %f!!!!!!!!!\r\n"), 
			m_pOwner->GetID(), m_pOwner->GetXZSpeed());
		return EMR_SelfLimit;
	}

	// 检测当前是否处在不能移动的状态
	if( m_pOwner->IsInStateCantMove() )
	{
		return EMR_SelfLimit;
	}

	return EMR_Success;
}

//----------------------------------------------------------------------------------------
// 玩家是否可以跳跃
//----------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::IsCanRoleJump(const Vector3& vStart, const Vector3& vDir)
{
	// 如果不是玩家，返回
	if( !m_pOwner->IsRole() ) return EMR_Invalid;

	// 地图非法
	if( !IsMapValid() ) return EMR_Invalid;

	// 检测一下坐标和朝向
	if( IsPosInvalid(vStart) || IsFaceInvalid(vDir) )
	{
		ILOG->Write(_T("client pos invalid when try to jump, start=<%f, %f, %f>, dir=<%f, %f, %f>\r\n"),
			vStart.x, vStart.y, vStart.z, vDir.x, vDir.y, vDir.z);
		return EMR_Invalid;
	}

	// 地图判断坐标
	if( !m_pOwner->GetMap()->IsPosValid(vStart) )
		return EMR_Invalid;

	// 检测一下距离
	if( !IsInValidDistance(vStart) )
	{
		return EMR_Invalid;
	}

	// 检测一下速度
	if( m_pOwner->GetXZSpeed() <= 0.0f || m_pOwner->m_fYSpeed <= 0.0f )
	{
		IMSG(_T("find a role whose xz speed or y speed less than 0, ID: %u, xzspeed: %f, yspeed: %f\r\n"), 
			m_pOwner->GetID(), m_pOwner->GetXZSpeed(), m_pOwner->m_fYSpeed);
		return EMR_SelfLimit;
	}

	// 检测当前是否处在不能移动的状态
	if( m_pOwner->IsInStateCantMove() )
	{
		return EMR_SelfLimit;
	}
// 	Role* pRole = dynamic_cast<Role*>(m_pOwner);
// 	if (P_VALID(pRole) && pRole->IsInRoleState(ERS_Mount))
// 	{
// 		return EMR_SelfLimit;
// 	}

	return EMR_Success;
}

//----------------------------------------------------------------------------------------
// 玩家是否可以掉落
//----------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::IsCanRoleDrop(const Vector3& vStart, const Vector3& vDir)
{
	// 如果不是玩家，返回
	if( !m_pOwner->IsRole() ) return EMR_Invalid;

	// 地图非法
	if( !IsMapValid() ) return EMR_Invalid;

	// 检测一下坐标和朝向
	if( IsPosInvalid(vStart) || IsFaceInvalid(vDir) )
	{
		ILOG->Write(_T("client pos invalid when try to drop, start=<%f, %f, %f>, dir=<%f, %f, %f>\r\n"),
			vStart.x, vStart.y, vStart.z, vDir.x, vDir.y, vDir.z);
		return EMR_Invalid;
	}

	// 地图判断坐标
	if( !m_pOwner->GetMap()->IsPosValid(vStart) )
		return EMR_Invalid;

	// 检测一下距离
	if( !IsInValidDistance(vStart) )
	{
		return EMR_Invalid;
	}

	return EMR_Success;
}

//-----------------------------------------------------------------------------------------
// 是否可以垂直掉落
//-----------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::IsCanRoleVDrop(const Vector3& vStart)
{
	// 如果不是玩家，返回
	if( !m_pOwner->IsRole() ) return EMR_Invalid;

	// 地图非法
	if( !IsMapValid() ) return EMR_Invalid;

	// 检测一下坐标
	if( IsPosInvalid(vStart) )
	{
		ILOG->Write(_T("client pos invalid when try to vDrop, start=<%f, %f, %f>\r\n"),
			vStart.x, vStart.y, vStart.z);
		return EMR_Invalid;
	}

	// 地图判断坐标
	if( !m_pOwner->GetMap()->IsPosValid(vStart) )
		return EMR_Invalid;

	// 检测一下距离
	if( !IsInValidDistance(vStart) )
	{
		return EMR_Invalid;
	}

	return EMR_Success;
}

//------------------------------------------------------------------------------------------
// 是否可以滑落
//------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::IsCanRoleSlide(const Vector3& vStart)
{
	// 如果不是玩家，返回
	if( !m_pOwner->IsRole() ) return EMR_Invalid;

	// 地图非法
	if( !IsMapValid() ) return EMR_Invalid;

	// 检测一下坐标
	if( IsPosInvalid(vStart) )
	{
		ILOG->Write(_T("client pos invalid when try to vdrop, start=<%f, %f, %f>\r\n"),
			vStart.x, vStart.y, vStart.z);
		return EMR_Invalid;
	}

	// 地图判断坐标
	if( !m_pOwner->GetMap()->IsPosValid(vStart) )
		return EMR_Invalid;

	// 检测一下距离
	if( !IsInValidDistance(vStart) )
	{
		return EMR_Invalid;
	}

	return EMR_Success;
}

//---------------------------------------------------------------------------------------------
// 是否可以游泳
//---------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::IsCanRoleSwim(const Vector3& vStart, const Vector3& vEnd)
{
	// 如果不是玩家，返回
	if( !m_pOwner->IsRole() ) return EMR_Invalid;

	// 地图非法
	if( !IsMapValid() ) return EMR_Invalid;

	if( IsPosInvalid(vStart) || IsPosInvalid(vEnd) )
	{
		ILOG->Write(_T("client pos invalid when try to swim, start=<%f, %f, %f>, end=<%f, %f, %f>\r\n"),
			vStart.x, vStart.y, vStart.z, vEnd.x, vEnd.y, vEnd.z);
		return EMR_Invalid;

	}

	// 地图判断坐标
	if( !m_pOwner->GetMap()->IsPosValid(vStart) || !m_pOwner->GetMap()->IsPosValid(vEnd) )
		return EMR_Invalid;

	// 检测一下距离
	if( !IsInValidDistance(vStart) )
	{
		return EMR_Invalid;
	}

	// 检测一下游泳的速度
	if( m_pOwner->m_fSwimXZSpeed <= 0.0f )
	{
		return EMR_SelfLimit;
	}

	// 检测当前是否处在不能移动的状态
	if( m_pOwner->IsInStateCantMove() )
	{
		return EMR_SelfLimit;
	}

	return EMR_Success;
}

//-----------------------------------------------------------------------------------------------
// 玩家是否可以停下
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::IsCanRoleStop(const Vector3& vPos)
{
	// 如果不是玩家，返回
	if( !m_pOwner->IsRole() ) return EMR_Invalid;

	// 地图非法
	if( !IsMapValid() ) return EMR_Invalid;

	// 检查一下坐标
	if( IsPosInvalid(vPos) )
	{
		ILOG->Write(_T("client pos invalid when try to stop, pos=<%f, %f, %f>\r\n"),
			vPos.x, vPos.y, vPos.z);
		return EMR_Invalid;
	}

	// 地图判断坐标
	if( !m_pOwner->GetMap()->IsPosValid(vPos) )
		return EMR_Invalid;

	// 检测一下距离
	if( !IsInValidDistance(vPos) )
	{
		return EMR_Invalid;
	}

	return EMR_Success;
}

//-----------------------------------------------------------------------------------------------
// 怪物是否可以行走
//-----------------------------------------------------------------------------------------------
MoveData::EMoveRet MoveData::IsCanCreatureWalk(const Vector3& vEnd, EMoveState eState, BOOL bCheck, Vector3* vNearPos)
{
	// 如果不是怪物，返回
	if( !m_pOwner->IsCreature() ) return EMR_Invalid;

	// 地图非法
	if( !IsMapValid() ) return EMR_Invalid;

	// 检查一下坐标
	if( IsPosInvalid(vEnd) )
	{
		ILOG->Write(_T("creature pos invalid when try to walk, dest=<%f, %f, %f>\r\n"),
			vEnd.x, vEnd.y, vEnd.z);
		return EMR_Invalid;
	}

	// 地图判断坐标
	if( !m_pOwner->GetMap()->IsPosValid(vEnd) )
		return EMR_Invalid;

	// 检测一下速度
	FLOAT fRealSpeed = GetCreatureMoveStateSpeed(eState);

	if( fRealSpeed < 0.0001f )
	{
		return EMR_SelfLimit;
	}

	// 如果需要检测路径
	if( bCheck )
	{
		// 检测当前是否处在不能移动的状态
		if( m_pOwner->IsInStateCantMove() )
		{
			return EMR_SelfLimit;
		}

		Creature* pCreature = static_cast<Creature*>(m_pOwner);

		// 非碰撞怪物
		if( !pCreature->NeedCollide() )
		{
			POINT nearPos;
			if( !pCreature->GetMap()->IfCanDirectGo(m_vPos.x, m_vPos.z, vEnd.x, vEnd.z, &nearPos) )
			{
				if( P_VALID(vNearPos) )
				{
					vNearPos->x = FLOAT(nearPos.x * TILE_SCALE);
					vNearPos->z = FLOAT(nearPos.y * TILE_SCALE);
					vNearPos->y = pCreature->GetMap()->GetGroundHeight(vNearPos->x, vNearPos->z);
				}
				return EMR_NoArrive;
			}
			else
			{

			}
		}
		// 碰撞怪物
		else
		{
			NavCollider_NPCMove NPCMove; 
			NPCMove.Init(m_vPos, vEnd, m_pOwner->GetSize(), fRealSpeed, (bool)bCheck);

			if(ENR_Arrive != NPCMove.IfCanGo(m_pOwner->GetMap()->GetNavCollider(), m_vPos, vEnd, m_pOwner->GetSize(), vNearPos))
				return EMR_NoArrive;
		}
	}

	return EMR_Success;
}

//-------------------------------------------------------------------------------------------
// 根据怪物的移动方式的到当前速度
//-------------------------------------------------------------------------------------------
FLOAT MoveData::GetCreatureMoveStateSpeed(EMoveState eState)
{
	switch (eState)
	{
	case EMS_CreatureWalk:
		return m_pOwner->GetXZSpeed();

	case EMS_CreaturePatrol:
		return m_pOwner->GetXZSpeed() * 0.4;

	case EMS_CreatureFlee:
		return m_pOwner->GetXZSpeed() * 0.7;

	default:
		return m_pOwner->GetXZSpeed();
	}
}
