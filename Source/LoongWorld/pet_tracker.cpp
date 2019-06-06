//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_tracker.cpp
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物跟踪器
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "pet_tracker.h"

#include "pet_soul.h"
#include "unit.h"
#include "role.h"
#include "pet.h"
#include "move_data.h"

//----------------------------------------------------------------------------------------------------
// 更新
//----------------------------------------------------------------------------------------------------
VOID PetTracker::Update()
{
	Unit* pTarget = GetTarget();
	Role* pMaster = GetMaster();
	// 若没有目标，则跟着主人
	if (!P_VALID(pTarget) && P_VALID(pMaster))
	{
		SetTarget(pMaster->GetID());
		pTarget = GetTarget();
	}
	else if (!P_VALID(pMaster))
		return ;

	// 若目标或者宠物无效
	if (!P_VALID(pTarget) ||	!P_VALID(m_pPet) ) 
		return;

	// 更新宠物速度
	UpdatePetSpeed(pTarget);

	// 更新移动数据
	m_pPet->GetMoveData().Update();

	// 根据目标状态找到目的地点
	Vector3 vTargetPos		= pTarget->GetMoveData().m_vPos;
	Vector3 vTargetFace		= pTarget->GetMoveData().m_vFace;
	Vector3 vPetDest		= m_pPet->GetMoveData().m_vDest;
	Vector3 vPetCur			= m_pPet->GetCurPos();

	// 是否需要瞬移
	if (NeedPutdown(vTargetPos, m_pPet->GetCurPos()))
	{
		PutDownBack(m_pPet, pTarget->GetMoveData().m_vPos, pTarget->GetMoveData().m_vFace);
		return ;
	}

	// 是否需要移动
	if (!NeedMove(vTargetPos, m_pPet->GetCurPos()))
		return;

	// 尝试走到目标身边
	Vector3 vTmp = GetNearPos(vTargetPos, vTargetFace, (FLOAT)MAX_NEED_MOVE_LEN, (FLOAT)MIN_NEED_MOVE_LEN);
	if (TryGoto(m_pPet, vTmp) )
		m_pPet->GetMoveData().StartCreatureWalk(vTmp, EMS_CreatureWalk, FALSE);
}

//----------------------------------------------------------------------------------------------------
// 根据目标找到目的点
//----------------------------------------------------------------------------------------------------
Vector3 PetTracker::FindTargetDest( Unit* pTarget )
{
	Vector3	vTargetPos;
	EMoveState eTarMove	= pTarget->GetMoveData().m_eCurMove;
	Vector3 vTargetCurPos	= pTarget->GetMoveData().m_vPos;
	Vector3	vTargetDestPos	= pTarget->GetMoveData().m_vDest;
	if (EMS_Stand == eTarMove)
	{
		vTargetPos = vTargetCurPos;
	}
	else
	{
		vTargetPos = vTargetDestPos;
	}
	return vTargetPos;
}

//----------------------------------------------------------------------------------------------------
// 是否需要移动
//----------------------------------------------------------------------------------------------------
BOOL PetTracker::NeedMove(const Vector3 &vMasterPos, const Vector3 &vPet)
{
	BOOL bNeedMove	= FALSE;
	FLOAT fDist = Vec3Dist(vMasterPos, vPet);
	if (fDist >= (FLOAT)MAX_NEED_MOVE_LEN)
	{
		bNeedMove	= TRUE;
	}
	else if (fDist <= (FLOAT)MIN_NEED_MOVE_LEN)
	{
		bNeedMove	= TRUE;
	}
	return bNeedMove;
}

//----------------------------------------------------------------------------------------------------
// 在一个点周围找到随机点
//----------------------------------------------------------------------------------------------------
Vector3	PetTracker::GetNearPos(const Vector3 &vMasterPos, const Vector3 &vMasterFace, FLOAT fMaxRange, FLOAT fMinRange)
{
	//随机选择一个角度
	FLOAT fAngle	= (IUTIL->Rand() % 360) / 360.0f * 3.14f;
	//随机选择一个合理的范围
	FLOAT fDist		= IUTIL->Rand() % INT(fMaxRange - fMinRange) + fMinRange;
	//计算目标点
	Vector3 vRt		= vMasterPos;
	vRt.x +=	sin(fAngle) * fDist;
	vRt.z +=	cos(fAngle) * fDist;
	return vRt;
}

//----------------------------------------------------------------------------------------------------
// 可否移动到某个点
//----------------------------------------------------------------------------------------------------
BOOL PetTracker::TryGoto( Pet* pPet, const Vector3 &vDstPos )
{
	BOOL	bRtv = TRUE;
	if (!P_VALID(pPet))
	{
		bRtv = FALSE;
	}
	else if( MoveData::EMR_Success != pPet->GetMoveData().IsCanCreatureWalk(vDstPos, EMS_CreatureWalk, TRUE))
	{
		bRtv = FALSE;
	}

	return bRtv;		
}

//----------------------------------------------------------------------------------------------------
// 把宠物放到身后
//----------------------------------------------------------------------------------------------------
BOOL PetTracker::PutDownBack( Pet* pPet, const Vector3 &vMasterPos, const Vector3 &vMasterFace )
{
	Vector3 vFaceNormal = -vMasterFace;
	Vec3Normalize(vFaceNormal);
	Vector3 vNewPos		= vMasterPos + vFaceNormal * MAX_NEED_MOVE_LEN;

	if (!P_VALID(pPet))
	{
		return FALSE;
	}
	//tbc:
	pPet->GetMoveData().ForceTeleport(vNewPos);
	return TRUE;
}

//----------------------------------------------------------------------------------------------------
// 设置宠物的速度
//----------------------------------------------------------------------------------------------------
VOID PetTracker::UpdatePetSpeed(Unit* pTarget)
{
	if(P_VALID(m_pPet) && P_VALID(pTarget) && m_pPet->GetXZSpeed() != pTarget->GetXZSpeed() && pTarget->GetXZSpeed() != 0)
	{
		Role* pRole = static_cast<Role*>(pTarget);
		if(pTarget->IsRole() && pRole->IsInRoleStateAny(ERS_Mount | ERS_Mount2))
		{
			m_pPet->SetAttValue(ERA_Speed_XZ, pTarget->GetAttValue(ERA_Speed_Mount), TRUE);	
		}
		else
		{
			m_pPet->SetAttValue(ERA_Speed_XZ, pTarget->GetAttValue(ERA_Speed_XZ), TRUE);	
		}
		
		m_pPet->GetMoveData().StopMove();
	}
}

//----------------------------------------------------------------------------------------------------
// 构造函数Z
//----------------------------------------------------------------------------------------------------
PetTracker::PetTracker(PetSoul* pSoul, Pet* pPet)
{
	ASSERT(P_VALID(pSoul));
	ASSERT(P_VALID(pPet));
	m_pPet		= pPet;
	m_pSoul		= pSoul;

	m_dwTargetID = GT_INVALID;

}

Role* PetTracker::GetMaster() const
{
	return m_pSoul->GetMaster();
}

BOOL PetTracker::NeedPutdown( const Vector3 &vMasterPos, const Vector3 &vPet )
{
	BOOL bNeedMove	= FALSE;
	FLOAT fDist = Vec3Dist(vMasterPos, vPet);
	if (fDist >= NEED_TRANS_LEN)
	{
		bNeedMove	= TRUE;
	}
	if (m_nPutBackTicks-- > 0)
	{
		return FALSE;
	}
	m_nPutBackTicks = MAX_TRANS_TICKS;

	return bNeedMove;
}

VOID PetTracker::SetTarget( DWORD dwTargetID )
{
	Unit* pTarget = GetUnit(dwTargetID);
	if (P_VALID(pTarget))
	{
		m_dwTargetID = dwTargetID;	
		UpdatePetSpeed(pTarget);
	}
	else
	{
		m_dwTargetID = GetMaster()->GetID();
		UpdatePetSpeed(GetMaster());
	}
}

Unit* PetTracker::GetUnit( DWORD dwID ) const
{
	Unit* pUnit = NULL;
	if (P_VALID(m_pPet) && m_pPet->GetMap())
	{
		pUnit = m_pPet->GetMap()->FindUnit(dwID);
	}
	return pUnit;
}