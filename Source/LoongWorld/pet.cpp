//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet.cpp
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物实体
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "pet.h"

#include "pet_soul.h"
#include "pet_tracker.h"
#include "role.h"
#include "pet_define.h"
//----------------------------------------------------------------------------------------------------
// 宠物的dumy原型
//----------------------------------------------------------------------------------------------------
const DWORD TYPE_ID_PET = 9999997;

//----------------------------------------------------------------------------------------------------
// 创建宠物实体
//----------------------------------------------------------------------------------------------------
Pet* Pet::Create( DWORD dwPetID, PetSoul* pSoul )
{
	MTRANS_ELSE_RET(pMaster, pSoul->GetMaster(), Role, NULL);
	Pet* pNew = new Pet(dwPetID, pMaster->GetMapID(), pMaster->GetID(), pMaster->GetMoveData().m_vPos, pMaster->GetMoveData().m_vFace, pSoul);
	pNew->Init(pMaster);
	return pNew;
}

//----------------------------------------------------------------------------------------------------
// 删除宠物实体
//----------------------------------------------------------------------------------------------------
VOID Pet::Delete(Pet* pToDel)
{
	SAFE_DEL(pToDel);
}

//----------------------------------------------------------------------------------------------------
// 构造函数
//----------------------------------------------------------------------------------------------------
Pet::Pet( DWORD dwID, DWORD dwMapID, DWORD dwMasterID, Vector3& vPos, Vector3& vFace, PetSoul* pSoul ) 
:Creature(dwID, dwMapID, vPos, vFace, ECST_ByRole, dwMasterID, FALSE),m_pSoul(pSoul)
{
	m_pTracker = new PetTracker(pSoul, this);
}

//----------------------------------------------------------------------------------------------------
// 析构函数
//----------------------------------------------------------------------------------------------------
Pet::~Pet()
{
	SAFE_DEL(m_pTracker);
}

//----------------------------------------------------------------------------------------------------
// 与灵魂整合
//----------------------------------------------------------------------------------------------------
BOOL Pet::IntegrateSoul(PetSoul* pSoul)
{
	// precondition
	if (!P_VALID(pSoul) ||
		GetID() != pSoul->GetID())
	{
		return FALSE;
	}

	//Init PetSoul of this Object
	m_pSoul = pSoul;

	//IntegrateInPet PetSoul with this Pet
	if (!m_pSoul->IntegrateInPet(this))
	{
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------------------
// 与实体分离
//----------------------------------------------------------------------------------------------------
VOID Pet::DetachSoul()
{
	//DetachFromPet PetSoul
	m_pSoul->DetachFromPet();

	//Reset PetSoul to NULL
	m_pSoul = NULL;

}

//----------------------------------------------------------------------------------------------------
// 更新
//----------------------------------------------------------------------------------------------------
VOID Pet::Update()
{
	m_pTracker->Update();

}

//----------------------------------------------------------------------------------------------------
// 属性改变
//----------------------------------------------------------------------------------------------------
VOID Pet::OnAttChange(INT nIndex)
{
	switch(nIndex)
	{
	case ERA_Speed_XZ:
		m_fXZSpeed = X_DEF_XZ_SPEED * (m_nAtt[ERA_Speed_XZ] / 10000.0f);
		m_MoveData.StopMove();
		break;
	}

}

//----------------------------------------------------------------------------------------------------
// 初始化
//----------------------------------------------------------------------------------------------------
BOOL Pet::Init(Role* pMaster)
{
	const tagCreatureProto* pProto = g_attRes.GetCreatureProto(TYPE_ID_PET);
	ASSERT(P_VALID(pProto));

	Creature::Init(pProto, NULL);

	ASSERT(P_VALID(GetSoul()));
	SetSize(GetSoul()->GetProto()->vSize * GetSoul()->GetProto()->fScale);
	m_fXZSpeed = pMaster->GetXZSpeed();
	return TRUE;
}

//----------------------------------------------------------------------------------------------------
// 获取typeid
//----------------------------------------------------------------------------------------------------
DWORD Pet::GetTypeID()
{
	return m_pSoul->GetProtoID();
}

//----------------------------------------------------------------------------------------------------
// 获得跟随模型
//----------------------------------------------------------------------------------------------------
BYTE Pet::GetPetState()
{
	return GetSoul()->GetPetAtt().GetState();
}

//----------------------------------------------------------------------------------------------------
// 获得主人
//----------------------------------------------------------------------------------------------------
Role* Pet::GetMaster()
{
	return GetSoul()->GetMaster();
}

VOID Pet::Teleport( Unit* pTarget )
{
	Follow(pTarget);

	if (m_pTracker->NeedPutdown(pTarget->GetCurPos(), GetCurPos()))
	{
		m_pTracker->PutDownBack(this, pTarget->GetCurPos(), pTarget->GetFaceTo());
	}
}

VOID Pet::Follow( Unit* pTarget )
{
	m_pTracker->SetTarget(pTarget->GetID());
}