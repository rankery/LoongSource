//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: buff_effect.cpp
// author: Aslan
// actor:
// data: 2008-11-26
// last:
// brief: 状态特殊效果
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/msg_loot.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "unit.h"
#include "buff_effect.h"
#include "role.h"
#include "creature.h"
#include "loot_mgr.h"
#include "script_mgr.h"
#include "pet_pocket.h"
#include "pet_soul.h"
#include "creature_ai.h"

BUFFEFFECTRUTINE BuffEffect::m_Effect[EBET_End];

VOID BuffEffect::Init()
{
	RegisterBuffEffectRutine();
}

//--------------------------------------------------------------------------------
// 初始化
//--------------------------------------------------------------------------------
VOID BuffEffect::RegisterBuffEffectRutine()
{
	m_Effect[EBET_Null]					=	&BuffEffect::BuffEffectNull;
	m_Effect[EBET_Teleport]				=	&BuffEffect::BuffEffectTeleport;
	m_Effect[EBET_InterruptSpell]		=	&BuffEffect::BuffEffectInterruptSpell;
	m_Effect[EBET_Dizzy]				=	&BuffEffect::BuffEffectDizzy;
	m_Effect[EBET_Repel]				=	&BuffEffect::BuffEffectRepel;
	m_Effect[EBET_Assault]				=	&BuffEffect::BuffEffectAssault;
	m_Effect[EBET_NoSkill]				=	&BuffEffect::BuffEffectNoSkill;
	m_Effect[EBET_Spor]					=	&BuffEffect::BuffEffectSpor;
	m_Effect[EBET_Tie]					=	&BuffEffect::BuffEffectTie;
	m_Effect[EBET_Dispel]				=	&BuffEffect::BuffEffectDispel;
	m_Effect[EBET_Cancel]				=	&BuffEffect::BuffEffectCancel;
	m_Effect[EBET_Invincible]			=	&BuffEffect::BuffEffectInvincible;
	m_Effect[EBET_InstantDie]			=	&BuffEffect::BuffEffectInstantDie;
	m_Effect[EBET_Pull]					=	&BuffEffect::BuffEffectPull;
	m_Effect[EBET_ReboundDmg]			=	&BuffEffect::BuffEffectNull;
	m_Effect[EBET_AbsorbDmg]			=	&BuffEffect::BuffEffectNull;
	m_Effect[EBET_TransmitDmg]			=	&BuffEffect::BuffEffectNull;
	m_Effect[EBET_HPDrain]				=	&BuffEffect::BuffEffectNull;
	m_Effect[EBET_MPDrain]				=	&BuffEffect::BuffEffectNull;
	m_Effect[EBET_HPTransfer]			=	&BuffEffect::BuffEffectHPTransfer;
	m_Effect[EBET_MPTransfer]			=	&BuffEffect::BuffEffectMPTransfer;
	m_Effect[EBET_Revive]				=	&BuffEffect::BuffEffectRevive;
	m_Effect[EBET_InstantCD]			=	&BuffEffect::BuffEffectInstantCD;
	m_Effect[EBET_Lurk]					=	&BuffEffect::BuffEffectLurk;
	m_Effect[EBET_Fly]					=	&BuffEffect::BuffEffectFly;
	m_Effect[EBET_ChangeEnmity]			=	&BuffEffect::BuffEffectChangeEnmity;
	m_Effect[EBET_Transmit]				=	&BuffEffect::BuffEffectTransmit;
	m_Effect[EBET_Gather]				=	&BuffEffect::BuffEffectGather;
	m_Effect[EBET_DisArm]				=	&BuffEffect::BuffEffectDisArm;
	m_Effect[EBET_ExchangePos]			=	&BuffEffect::BuffEffectExchangePos;
	m_Effect[EBET_Explode]				=	&BuffEffect::BuffEffectExplode;
	m_Effect[EBET_Funk]					=	&BuffEffect::BuffEffectFunk;
	m_Effect[EBET_Pursue]				=	&BuffEffect::BuffEffectPursue;
	m_Effect[EBET_NoPrepare]			=	&BuffEffect::BuffEffectNoPrepare;
	m_Effect[EBET_OnWater]				=	&BuffEffect::BuffEffectOnWater;
	m_Effect[EBET_MoveHPDmg]			=	&BuffEffect::BuffEffectMoveHPDmg;
	m_Effect[EBET_IgnoreArmor]			=	&BuffEffect::BuffEffectIgnoreArmor;
	m_Effect[EBET_Sneer]				=	&BuffEffect::BuffEffectSneer;
	m_Effect[EBET_Mount]				=	&BuffEffect::BuffEffectMount;
	m_Effect[EBET_MountInvite]			=	&BuffEffect::BuffEffectMountInvite;
}

//----------------------------------------------------------------------------------
// 计算Buff特殊效果
//----------------------------------------------------------------------------------
VOID BuffEffect::CalBuffEffect(Unit* pTarget, Unit* pSrc, EBuffEffectType eEffect, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet, const tagBuffProto* pProto)
{
	if( eEffect < EBET_Null || eEffect >= EBET_End )
		return;

	BUFFEFFECTRUTINE handler = m_Effect[eEffect];
	(*handler)(pTarget, pSrc, dwEffectMisc1, dwEffectMisc2, bSet, pProto);
}

//----------------------------------------------------------------------------------
// 特殊效果：无
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectNull(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{

}
//----------------------------------------------------------------------------------
// 特殊效果：瞬移
// dwEffectMisc1：移动的距离，大于0时为向前移动，小于0时为向后移动
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectTeleport(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	INT nTileDist = (INT)dwEffectMisc1;
	if( 0 == nTileDist ) return;

	BOOL bForward = (nTileDist > 0);	// 向前还是向后移动

	FLOAT fDistAbs = FLOAT(abs(nTileDist) * TILE_SCALE);	// 绝对距离

	Vector3 vFaceTo = pTarget->GetFaceTo();	// 得到玩家朝向

	// 对该朝向进行归一化
	Vec3Normalize(vFaceTo);

	// 得到终点坐标
	Vector3 vDest;

	if( bForward )
	{
		vDest = pTarget->GetCurPos() + vFaceTo * fDistAbs;
	}
	else
	{
		vDest = pTarget->GetCurPos() - vFaceTo * fDistAbs;
	}

	// 得到一个最大合理的终点
	Vector3 vRealDest;
	NavCollider* pCollider = pTarget->GetMap()->GetNavMap()->GetCollider();

	if( pTarget->IsRole() )
	{
		NavCollider_Move collider;
		collider.IfCanGo(pCollider, pTarget->GetCurPos(), vDest, pTarget->GetSize(), &vRealDest);
	}
	else
	{
		Creature* pTargetCreature = static_cast<Creature*>(pTarget);

		// 非碰撞怪
		if( !pTargetCreature->NeedCollide() )
		{
			POINT nearPos;
			if( !pTargetCreature->GetMap()->IfCanDirectGo(pTargetCreature->GetCurPos().x, pTargetCreature->GetCurPos().z, vDest.x, vDest.z, &nearPos) )
			{
				vRealDest.x = FLOAT(nearPos.x * TILE_SCALE);
				vRealDest.z = FLOAT(nearPos.y * TILE_SCALE);
				vRealDest.y = pTargetCreature->GetMap()->GetGroundHeight(vRealDest.x, vRealDest.z);
			}
			else
			{
				vRealDest = vDest;
			}
		}
		// 碰撞怪
		else
		{
			NavCollider_NPCMove collider;
			collider.IfCanGo(pCollider, pTarget->GetCurPos(), vDest, pTarget->GetSize(), &vRealDest);
		}
	}

	// 如果两个点不相等，则瞬移
	if( pTarget->GetCurPos() != vRealDest )
	{
		// 给客户端发送消息
		tagNS_SpecialMove send;
		send.dwRoleID = pTarget->GetID();
		send.eType = ESMT_Teleport;
		send.vDestPos = vRealDest;
		pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);
	
		// 瞬移，但不发送消息
		pTarget->GetMoveData().ForceTeleport(vRealDest, FALSE);
	}

	


}

//----------------------------------------------------------------------------------
// 特殊效果：打断起手
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectInterruptSpell(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	// 打断技能
	pTarget->GetCombatHandler().InterruptPrepare(CombatHandler::EIT_Null, FALSE, TRUE);
}
//----------------------------------------------------------------------------------
// 特殊效果：眩晕
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectDizzy(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	bSet ? pTarget->SetState(ES_Dizzy) : pTarget->UnsetState(ES_Dizzy);
	
	// buffer结束,如怪物可到达玩家，则继续攻击
	EndbuffContinueIsAttack(pTarget, pSrc, bSet);
}

//----------------------------------------------------------------------------------
// 特殊效果：击退
// dwEffectMisc1：击退的距离
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectRepel(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !P_VALID(pSrc) ) return;

	INT nTileDist = (INT)dwEffectMisc1;
	if( 0 >= nTileDist ) return;
	FLOAT fDistAbs = FLOAT(nTileDist * TILE_SCALE);	// 绝对距离

	// 得到两个目标之间的向量
	Vector3 vVec = pTarget->GetCurPos() - pSrc->GetCurPos();

	// 对该向量进行归一化
	Vec3Normalize(vVec);

    //-------------------------------------------------------------------------------
	//添加说明  当怪物 或 其他玩家  和 所控角色的位置坐标重合时
	//将这种情况将怪物和其他玩家的位置坐标视为与所控制角色附近的坐标  
	//将上面 得到的两个目标之间的向量进行微小调整  
	//-------------------------------------------------------------------------------
    Vector3 vZero(0.0f,0.0f,0.0f);
	Vector3 vI(0.0f,-1.0f,0.0f);
	Vector3 vE(0.0f,1.0f,0.0f);
	
    if( vVec == vZero || vVec.y > 0.91f || vVec.y < -0.95f)
	{
		Vector3 vTemp(-0.73671913f,0.29935482f,0.60632634f);
        vVec = vTemp;
	} 
	//添加结束


	// 得到终点坐标
	Vector3 vDest = pTarget->GetCurPos() + vVec * fDistAbs;

	// 得到一个最大合理的终点
	Vector3 vRealDest;
	NavCollider* pCollider = pTarget->GetMap()->GetNavMap()->GetCollider();

	if( pTarget->IsRole() )
	{
		NavCollider_Move collider;
		collider.IfCanGo(pCollider, pTarget->GetCurPos(), vDest, pTarget->GetSize(), &vRealDest);
	}
	else
	{
		Creature* pTargetCreature = static_cast<Creature*>(pTarget);

		// 非碰撞怪
		if( !pTargetCreature->NeedCollide() )
		{
			POINT nearPos;
			if( !pTargetCreature->GetMap()->IfCanDirectGo(pTargetCreature->GetCurPos().x, pTargetCreature->GetCurPos().z, vDest.x, vDest.z, &nearPos) )
			{
				vRealDest.x = FLOAT(nearPos.x * TILE_SCALE);
				vRealDest.z = FLOAT(nearPos.y * TILE_SCALE);
				vRealDest.y = pTargetCreature->GetMap()->GetGroundHeight(vRealDest.x, vRealDest.z);
			}
			else
			{
				vRealDest = vDest;
			}
		}
		// 碰撞怪
		else
		{
			NavCollider_NPCMove collider;
			collider.IfCanGo(pCollider, pTarget->GetCurPos(), vDest, pTarget->GetSize(), &vRealDest);
		}
	}

	// 如果两个点不相等，则瞬移
	if( pTarget->GetCurPos() != vRealDest )
	{
		// 给客户端发送消息
		tagNS_SpecialMove send;
		send.dwRoleID = pTarget->GetID();
		send.eType = ESMT_Repel;
		send.vDestPos = vRealDest;
		pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);

		// 瞬移，但不发送消息
		pTarget->GetMoveData().ForceTeleport(vRealDest, FALSE);

		// 结束当前释放的技能
		pTarget->GetCombatHandler().End();
	}

}
//----------------------------------------------------------------------------------
// 特殊效果：冲锋
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectAssault(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !P_VALID(pSrc) || pSrc != pTarget ) return;	// 冲锋技能必须自己释放

	// 得到第一目标者
	DWORD dwTargetUnitID = pTarget->GetCombatHandler().GetTargetUnitID();

	// 在地图内查找该目标
	Unit* pUnit = pTarget->GetMap()->FindUnit(dwTargetUnitID);
	if( !P_VALID(pUnit) ) return;

	// todo：要取到目标跟前的坐标，现在先看看效果，取目标坐标
	Vector3 vPosUnit	=	pUnit->GetCurPos();
	Vector3 vPosTarget	=	pTarget->GetCurPos();

	// 得到两者之间的距离
	FLOAT fDist = Vec3Dist(vPosUnit, vPosTarget);
	if( fDist <= 0.001f ) return;

	Vector3 vVec = vPosUnit - vPosTarget;	// 得到两者之间向量

	// 将该距离减去玩家的两者的包裹盒之和
	FLOAT fPullDist = fDist - pSrc->GetSize().z - pTarget->GetSize().z;
	vVec *= ( fPullDist / fDist );

	// 将目标的当前坐标加上该向量就是拖拽的目标点
	vPosTarget += vVec;

	// 查看能否通过
	NavCollider* pCollider = pTarget->GetMap()->GetNavMap()->GetCollider();
	ENavResult eResult = ENR_Arrive;

	if( pTarget->IsRole() )
	{
		NavCollider_Move collider;
		eResult = collider.IfCanGo(pCollider, pTarget->GetCurPos(), vPosTarget, pTarget->GetSize(), NULL);
	}
	else
	{
		NavCollider_NPCMove collider;
		eResult = collider.IfCanGo(pCollider, pTarget->GetCurPos(), vPosTarget, pTarget->GetSize(), NULL);
	}

	// 冲锋不过去
	if( ENR_Arrive != eResult ) return;

	// 可以冲锋过去，发送消息
	tagNS_SpecialMove send;
	send.dwRoleID = pTarget->GetID();
	send.eType = ESMT_Assault;
	send.vDestPos = vPosTarget;
	pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);

	// 瞬移过去
	pTarget->GetMoveData().ForceTeleport(vPosTarget, FALSE);

}
//----------------------------------------------------------------------------------
// 特殊效果：封技
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectNoSkill(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	bSet ? pTarget->SetState(ES_NoSkill) : pTarget->UnsetState(ES_NoSkill);

	// buffer结束,如怪物可到达玩家，则继续攻击
	EndbuffContinueIsAttack(pTarget, pSrc, bSet);
}
//----------------------------------------------------------------------------------
// 特殊效果：昏睡
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectSpor(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	bSet ? pTarget->SetState(ES_Spor) : pTarget->UnsetState(ES_Spor);
	
	// buffer结束,如怪物可到达玩家，则继续攻击
	EndbuffContinueIsAttack(pTarget, pSrc, bSet);
}
//----------------------------------------------------------------------------------
// 特殊效果：定身
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectTie(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	bSet ? pTarget->SetState(ES_Tie) : pTarget->UnsetState(ES_Tie);
	if (P_VALID(pSrc) && pSrc->IsRole())
	{
		((Role*)pSrc)->StopMount();
	}

	// buffer结束,如怪物可到达玩家，则继续攻击
	EndbuffContinueIsAttack(pTarget, pSrc, bSet);
}
//----------------------------------------------------------------------------------
// 特殊效果：驱散
// dwEffectMisc1：
//	1：消除角色身上最后一个增益状态
//	2：消除角色身上最后一个有害状态
//	3：消除角色身上最后一个某种抗性类型的状态
//  4：消除角色身上指定ID的状态
//	5：消除角色身上指定效果类型的buff
// dwEffectMisc2：对于dwEffectMisc1为3时，则为对应的状态抗性类型
// dwEffectMisc2：对于dwEffectMisc1为5时，则为对应的效果类型，bSet：去除最后一个，还是所有
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectDispel(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	switch(dwEffectMisc1)
	{
	case 1:
		pTarget->DispelBuff(TRUE);
		break;

	case 2:
		pTarget->DispelBuff(FALSE);
		break;

	case 3:
		pTarget->DispelBuff((EBuffResistType)dwEffectMisc2);
		break;

	case 4:
		pTarget->DispelBuff(dwEffectMisc2);
		break;

	default:
		break;
	}

}

//----------------------------------------------------------------------------------
// 特殊效果：消除角色身上指定效果类型的buff
// dwEffectMisc2：去除最后一个，还是去除所有  dwEffectMisc1：对应的效果类型
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectCancel(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	ASSERT(P_VALID(pTarget));

	pTarget->DispelBuff(dwEffectMisc1, (BOOL)dwEffectMisc2);
}

//----------------------------------------------------------------------------------
// 特殊效果：无敌
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectInvincible(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	bSet ? pTarget->SetState(ES_Invincible) : pTarget->UnsetState(ES_Invincible);
}
//----------------------------------------------------------------------------------
// 特殊效果：即死
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectInstantDie(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	ASSERT(P_VALID(pTarget));

	pTarget->ChangeHP(-pTarget->GetAttValue(ERA_HP), pSrc, NULL, pProto);
}
//----------------------------------------------------------------------------------
// 特殊效果：拖拽
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectPull(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !P_VALID(pSrc) ) return;

	Vector3 vPosSrc		=	pSrc->GetCurPos();
	Vector3 vPosTarget	=	pTarget->GetCurPos();

	// 得到两者之间的距离
	FLOAT fDist = Vec3Dist(vPosSrc, vPosTarget);
	if( fDist <= 0.001f ) return;
	
	// 坐标重复，一上一下，不做处理
	if (vPosTarget.x == vPosSrc.x && vPosTarget.z == vPosSrc.z && vPosTarget.y != vPosSrc.y) return;

	Vector3 vVec = vPosSrc - vPosTarget;	// 得到两者之间向量

	// 将该距离减去玩家的两者的包裹盒之和
	FLOAT fPullDist = fDist - pSrc->GetSize().z - pTarget->GetSize().z;
	vVec *= ( fPullDist / fDist );

	// 将目标的当前坐标加上该向量就是拖拽的目标点
	vPosTarget += vVec;

	// 通知客户端瞬移
	tagNS_SpecialMove send;
	send.dwRoleID = pTarget->GetID();
	send.eType = ESMT_Teleport;
	send.vDestPos = vPosTarget;
	pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);

	// 瞬移目标
	pTarget->GetMoveData().ForceTeleport(vPosTarget, FALSE);

}
//----------------------------------------------------------------------------------
// 特殊效果：转魂
// dwEffectMisc1：体力吸收的量
// dwEffectMisc2：转化的百分比
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectHPTransfer(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( FALSE == P_VALID(pSrc) )
		return;

	if( pTarget->IsDead() || pSrc->IsDead() )
		return;

	INT nHP = (INT)dwEffectMisc1;
	INT nPct = (INT)dwEffectMisc2;

	nHP = min(pTarget->GetAttValue(ERA_HP), nHP);

	if( nHP <= 0 || nPct <= 0 ) return;

	// 自身减去血量
	pTarget->ChangeHP(-nHP, pSrc, NULL, pProto);

	// 对方加上血量
	INT nHPAdd = INT((FLOAT)nHP * ((FLOAT)nPct / 10000.0f));
	pSrc->ChangeHP(nHPAdd, pTarget, NULL, pProto);
}
//----------------------------------------------------------------------------------
// 特殊效果：转精
// dwEffectMisc1：真气吸收的量
// dwEffectMic2：转化的百分比
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectMPTransfer(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( FALSE == P_VALID(pSrc) )
		return;

	if( pTarget->IsDead() || pSrc->IsDead() )
		return;

	INT nMP = (INT)dwEffectMisc1;
	INT nPct = (INT)dwEffectMisc2;

	nMP = min(pTarget->GetAttValue(ERA_MP), nMP);

	if( nMP <= 0 || nPct <= 0 ) return;

	// 自身减去真气
	pTarget->ChangeMP(-nMP);

	// 对方加上真气
	INT nMPAdd = INT((FLOAT)nMP * ((FLOAT)nPct / 10000.0f));
	pSrc->ChangeMP(nMPAdd);
}
//----------------------------------------------------------------------------------
// 特殊效果：复活
// dwEffectMisc1：<100000：恢复血量真气固定值；>100000：恢复血量真气百分比
// dwEffectMisc2：如果为1，则不会获得新的内伤
//----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectRevive(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( FALSE == pTarget->IsRole() ) return;
	if( FALSE == pTarget->IsDead() ) return;

	Role* pRole = static_cast<Role*>(pTarget);

	INT nHPMPRegain = INT(dwEffectMisc1);
	
	if( nHPMPRegain <= 0 ) return;

	INT nHP = 0;
	INT nMP = 0;
	if( nHPMPRegain < 100000 )
	{
		nHP = nMP = nHPMPRegain;
	}
	else
	{
		nHP = INT((FLOAT)pRole->GetAttValue(ERA_MaxHP) * (FLOAT(nHPMPRegain  - 100000) / 10000.0f));
		nMP = INT((FLOAT)pRole->GetAttValue(ERA_MaxMP) * (FLOAT(nHPMPRegain  - 100000) / 10000.0f));
	}

	if( nHP <= 0 ) return;

	// 玩家被复活
	pRole->BeRevived(nHP, nMP, pSrc);
}
//-----------------------------------------------------------------------------------
// 特殊效果：瞬间冷却
// dwEffectMisc1：对应相应的天资类型，如果为-1，则为所有技能
// dwEffectMisc2：例外的不删除的技能
//-----------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectInstantCD(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	pTarget->ClearAllSkillCoodDown((ETalentType)dwEffectMisc1, dwEffectMisc2);
}
//------------------------------------------------------------------------------------
// 特殊效果：隐身
//------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectLurk(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	ASSERT( P_VALID(pTarget) );

	if( bSet )
	{
		pTarget->Lurk((INT)dwEffectMisc1);
	}
	else
	{
		pTarget->UnLurk();
	}
}
//-------------------------------------------------------------------------------------
// 特殊效果：飞行
//-------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectFly(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{

}
//-------------------------------------------------------------------------------------
// 特殊效果：改变仇恨
// dwEffectMisc1：改变仇恨的百分比
//-------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectChangeEnmity(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	pSrc->ChangeEnmityCreatureValue(dwEffectMisc1);
}
//-------------------------------------------------------------------------------------
// 特殊效果：传送
// dwEffectMisc1：
//	0：回到玩家的城镇记录点
//	1：传送玩家到最近的城市（暂不实现）
//	2：传送玩家到最近的乾坤石（暂不实现）
//	3：心有灵犀
//-------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectTransmit(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !P_VALID(pTarget) || FALSE == pTarget->IsRole() )
		return;

	Role* pTargetRole = static_cast<Role*>(pTarget);
	Role* pSrcRole = static_cast<Role*>(pSrc);

	if( 0 == dwEffectMisc1 )
	{
		pTargetRole->ReturnCity();
	}
	else if( 1 == dwEffectMisc1 )
	{

	}
	else if( 2 == dwEffectMisc1 )
	{

	}
	//心有灵犀
	else if( 3 == dwEffectMisc1 )
	{	
		//把老婆叫过来
		pTargetRole->GotoNewMap(pSrcRole->GetMapID(),pSrcRole->GetCurPos().x,pSrcRole->GetCurPos().y,pSrcRole->GetCurPos().z);
		//扣元宝
		pSrcRole->GetCurMgr().DecBagYuanBao(1, ELCLD_Skill_Use);
		
	}
}
//-------------------------------------------------------------------------------------
// 特殊效果：采集调查
//-------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectGather(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if ( 0 == dwEffectMisc1 )
	{
		GatherEffect(pTarget, pSrc, dwEffectMisc1, dwEffectMisc2, bSet);
	}
	else if ( 1 == dwEffectMisc1)
	{
		InvesEffect(pTarget, pSrc, dwEffectMisc1, dwEffectMisc2, bSet);
	}
}

//-------------------------------------------------------------------------------------
// 特殊效果：缴械
//-------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectDisArm(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	bSet ? pTarget->SetState(ES_DisArm) : pTarget->UnsetState(ES_DisArm);
}

//---------------------------------------------------------------------------------------
// 交换位置
//---------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectExchangePos(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !P_VALID(pSrc) ) return;

	// 分别得到各自的当前坐标
	Vector3 vPosSrc = pSrc->GetCurPos();
	Vector3 vPosTarget = pTarget->GetCurPos();

	// 发送给源方
	tagNS_SpecialMove send1;
	send1.dwRoleID = pSrc->GetID();
	send1.eType = ESMT_Teleport;
	send1.vDestPos = vPosTarget;
	pSrc->GetMap()->SendBigVisTileMsg(pSrc, &send1, send1.dwSize);

	// 发送给目标方
	tagNS_SpecialMove send2;
	send2.dwRoleID = pTarget->GetID();
	send2.eType = ESMT_Teleport;
	send2.vDestPos = vPosSrc;
	pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send2, send2.dwSize);

	// 瞬移源玩家
	pSrc->GetMoveData().ForceTeleport(vPosTarget, FALSE);

	// 瞬移目标玩家
	pTarget->GetMoveData().ForceTeleport(vPosSrc, FALSE);
}

//----------------------------------------------------------------------------------------
// 引爆
// dwEffectMisc1： 要删除的BuffID
// dwEffectMisc2： 要添加的BuffTypeID
//----------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectExplode(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	DWORD dwSrcBuffID		=	dwEffectMisc1;		// 要去掉的BuffID
	DWORD dwDestBuffTypeID	=	dwEffectMisc2;		// 要添加的Buff的TypeID

	// 如果要求有源Buff，则如果目标身上没有，则返回
	if( P_VALID(dwSrcBuffID) )
	{
		if( !pTarget->IsHaveBuff(dwSrcBuffID) )
			return;
	}

	// 先删除
	if( P_VALID(dwSrcBuffID) )
	{
		pTarget->RemoveBuff(dwSrcBuffID, TRUE);
	}

	// 再添加
	if( P_VALID(dwEffectMisc2) )
	{
		const tagBuffProto* pProto = g_attRes.GetBuffProto(dwDestBuffTypeID);
		if( P_VALID(pProto) )
		{
			pTarget->TryAddBuff(pSrc, pProto, NULL, NULL, NULL);
		}
	}
}

//----------------------------------------------------------------------------------------
// 恐惧
//----------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectFunk(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{

}

//-----------------------------------------------------------------------------------------
// 追踪
// dwEffectMisc1: 0：最近点 1：目标前方 2：目标后方
//-----------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectPursue(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !P_VALID(pSrc) || pTarget != pSrc ) return;		// 追踪也一定是释放者自身添加的buff

	// 取到target的第一目标
	DWORD dwTargetUnitID = pTarget->GetCombatHandler().GetTargetUnitID();
	Unit* pUnit = pTarget->GetMap()->FindUnit(dwTargetUnitID);

	if( !P_VALID(pUnit) ) return;

	// 取到这个目标的位置
	Vector3 vUnitPos = pUnit->GetCurPos();

	// 根据dwEffectMisc1来取目标点
	Vector3 vDest;

	if( 0 == dwEffectMisc1 )	// 直线最近点
	{
		Vector3 vTargetPos	=	pUnit->GetCurPos();

		// 计算两者之间的距离
		FLOAT fDist = Vec3Dist(vUnitPos, vTargetPos);
		if( fDist <= 0.001f ) return;

		Vector3 vVec = vUnitPos - vTargetPos;	// 得到两者之间向量

		// 将该距离减去玩家的两者的包裹盒之和
		FLOAT fPursueDist = fDist - pSrc->GetSize().z - pTarget->GetSize().z;
		vVec *= ( fPursueDist / fDist );

		// 将目标的当前坐标加上该向量就是拖拽的目标点
		vDest = pTarget->GetCurPos() + vVec;
	}
	else if( 1 == dwEffectMisc1 )	// 目标身前
	{
		Vector3 vFace = pUnit->GetFaceTo();
		Vec3Normalize(vFace);		// 归一化

		vFace *= pUnit->GetSize().z + pTarget->GetSize().z;

		vDest = vUnitPos + vFace;
	}
	else if( 2 == dwEffectMisc1 )
	{
		Vector3 vFace = pUnit->GetFaceTo();
		Vec3Normalize(vFace);		// 归一化

		vFace *= -(pUnit->GetSize().z + pTarget->GetSize().z);

		vDest = vUnitPos + vFace;
	}
	else
	{
		return;
	}

	// 发送给客户端瞬移消息
	tagNS_SpecialMove send;
	send.dwRoleID = pTarget->GetID();
	send.eType = ESMT_Teleport;
	send.vDestPos = vDest;
	pTarget->GetMap()->SendBigVisTileMsg(pTarget, &send, send.dwSize);

	// 瞬移一下
	pTarget->GetMoveData().ForceTeleport(vDest, FALSE);

	// 改变一下朝向
	pTarget->GetMoveData().SetFaceTo(pUnit->GetCurPos() - pTarget->GetCurPos());

}

//------------------------------------------------------------------------------------------
// 无起手时间
//------------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectNoPrepare(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	ASSERT(P_VALID(pTarget));

	// 获取起手时间影响百分比(+：增加起手时间；-：减少起手时间)
	INT nSkillPrepareModPct = (INT)dwEffectMisc1;

	if( bSet )
	{
		pTarget->GetCombatHandler().ModSkillPrepareModPct(nSkillPrepareModPct);
	}
	else
	{
		pTarget->GetCombatHandler().ModSkillPrepareModPct(-nSkillPrepareModPct);
	}
}

//-------------------------------------------------------------------------------------------
// 水上行走
//-------------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectOnWater(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !pTarget->IsRole() ) return;

	Role* pRole = static_cast<Role*>(pTarget);

	if( bSet )
	{
		if( !pRole->IsInRoleState(ERS_WaterWalk) )
		{
			pRole->SetRoleState(ERS_WaterWalk);
		}
	}
	else
	{
		if( pRole->IsInRoleState(ERS_WaterWalk) )
		{
			pRole->UnsetRoleState(ERS_WaterWalk);
		}
	}
}

//--------------------------------------------------------------------------------------------
// 走路掉血
//--------------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectMoveHPDmg(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	ASSERT(P_VALID(pTarget));

	INT nVal = (INT)dwEffectMisc2;		// 正值为加，负值为减
	
	switch(dwEffectMisc1)
	{
	case 1:		// 掉血
		pTarget->ChangeHP(nVal, pSrc, NULL, pProto);
		break;
	case 2:		// 掉蓝
		pTarget->ChangeMP(nVal);
		break;
	}
}

//---------------------------------------------------------------------------------------------
// 无视护甲
//---------------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectIgnoreArmor(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	ASSERT(P_VALID(pTarget));

	// 获取护甲影响百分比(只能为正值)
	INT nArmorDecPct = (INT)dwEffectMisc1;

	if( bSet )
	{
		pTarget->GetCombatHandler().ModTargetArmorLeftPct(-nArmorDecPct);
	}
	else
	{
		pTarget->GetCombatHandler().ModTargetArmorLeftPct(nArmorDecPct);
	}
}

//----------------------------------------------------------------------------------------------
// 嘲讽
//----------------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectSneer(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if(!P_VALID(pTarget) || !P_VALID(pSrc) )	return;

	if( !pTarget->IsCreature() ) return;
	Creature* pCreature	= static_cast<Creature *>(pTarget);
	if( !P_VALID(pCreature->GetAI()) ) return; 

	if( bSet )
	{
		DWORD dwMaxEnmityUnitID = pCreature->GetAI()->GetMaxEnmityUnitID();
		if( dwMaxEnmityUnitID == pSrc->GetID() )
			return;

		INT nEnmityMod = pCreature->GetAI()->GetEnmityValue(dwMaxEnmityUnitID) - pCreature->GetAI()->GetEnmityValue(pSrc->GetID())+1;

		// 增加仇恨加成值
		pCreature->GetAI()->AddEnmityMod(pSrc, nEnmityMod);
		pCreature->GetAI()->SetTargetUnitID(pSrc->GetID());
	}
	else
	{
		pCreature->GetAI()->ClearEnmityModValue(pSrc->GetID());
		DWORD dwMaxEnmityID = pCreature->GetAI()->GetMaxEnmityUnitID();
		pCreature->GetAI()->SetTargetUnitID(dwMaxEnmityID);
	}
}

//----------------------------------------------------------------------------------------------
// 采集效果
//----------------------------------------------------------------------------------------------
VOID BuffEffect::GatherEffect(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !pTarget->IsCreature() ) return;
	if( !pSrc->IsRole() ) return;

	// 获得资源和角色
	Creature* pCreature	= static_cast<Creature *>(pTarget);	
	Role* pRole	= static_cast<Role *>(pSrc);	
	if ( !P_VALID(pCreature) || !P_VALID(pRole) )
		return ;

	// 判断资源并获得技能
	Skill* pGatherSkill = NULL;
	if ( pCreature->IsNatuRes() )
		pGatherSkill = pRole->GetSkill(GATHER_SKILL_MINING);
	else if ( pCreature->IsManRes() )
		pGatherSkill = pRole->GetSkill(GATHER_SKILL_HARVEST);
	if ( !P_VALID(pGatherSkill) )
		return;

// 	// 若角色背包已满				
// 	ItemMgr& itemMgr = pRole->GetItemMgr();	
// 	if (itemMgr.GetBagFreeSize() <= 0)
// 	{
// 		tagNS_RolePickUpItem send;
// 		send.dwErrorCode	= E_Loot_BAG_NOT_ENOUGH_SPACE;
// 		send.dwRoleID		= pRole->GetID();
// 		pRole->SendMessage(&send, send.dwSize);
// 		return ;
// 	}
// 
// 	// 若资源距离够近 
// 	if (!pRole->IsInCombatDistance(*pCreature, pGatherSkill->GetOPDist()))
// 		return ;	
// 
// 	// 资源已被占有
// 	if( pCreature->IsTagged() )
// 		return;

	// 以产出率掉落资源 	
	sLootMgr.ResourceLoot(pCreature, pRole);

	// 开采熟练度增加（包含升级技能）
	const tagCreatureProto* pCreatureProto = g_attRes.GetCreatureProto(pCreature->GetTypeID());

	// 获得熟练度 = 资源点提供的熟练度×(1+玩家悟性/100)
	INT nExpAdd =  INT((FLOAT)pCreatureProto->nExpGive * (1.0f + (FLOAT)pRole->GetAttValue(ERA_Savvy) / 100.0f));
	pRole->ChangeSkillExp(pGatherSkill, nExpAdd);

	// 资源降血
	pCreature->ChangeHP(-1, pSrc);
}

//----------------------------------------------------------------------------------------------
// 调查地物
//----------------------------------------------------------------------------------------------
VOID BuffEffect::InvesEffect(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet,  const tagBuffProto* pProto)
{
	if( !pTarget->IsCreature() ) return;
	if( !pSrc->IsRole() ) return;

	// 获得地物和角色
	Creature* pCreature	= static_cast<Creature *>(pTarget);	
	Role* pRole	= static_cast<Role *>(pSrc);	
	if ( !P_VALID(pCreature) || !P_VALID(pRole) )
		return ;

	// 判断地物并获得技能
	Skill* pInvesSkill = NULL;
	if ( pCreature->IsInves() )
		pInvesSkill = pRole->GetSkill(INVES_SKILL);
	if ( !P_VALID(pInvesSkill) )
		return;

// 	// 若资源距离够近 
// 	if (!pRole->IsInCombatDistance(*pCreature, pInvesSkill->GetOPDist()))
// 		return ;	
// 
// 	// 地物已被占有
// 	if( pCreature->IsTagged() )
// 		return;

	// 以产出率掉落物品	
	sLootMgr.InvestigateLoot(pCreature, pRole);

	// 任务触发
	pRole->OnQuestEvent(EQE_Invest, pTarget->GetID(), pCreature->GetTypeID());

	// 地物脚本触发
	pCreature->OnInvest(pRole);

	// 地物降血
	pCreature->ChangeHP(-1, pSrc);
}

//-----------------------------------------------------------------------------------------------------------------
// 骑乘
//-----------------------------------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectMount( Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet/*=TRUE*/, const tagBuffProto* pProto/*=NULL*/ )
{
	if (!P_VALID(pTarget) || !P_VALID(pSrc)) return;
	if (!pTarget->IsRole()) return;

	Role* pRole = dynamic_cast<Role* >(pSrc);
	if (!P_VALID(pRole)) return;

	if (bSet)
	{
		pRole->GetPetPocket()->RidePet();
	}
	else
	{
		PetSoul* pSoul = pRole->GetPetPocket()->GetMountPetSoul();
		if (P_VALID(pSoul))
		{
			Role* pPassenger = pSoul->GetPassenger();
			if (P_VALID(pPassenger))
			{
				pPassenger->GetPetPocket()->StopRideAfter(pRole);
			}
		}
		
		pRole->GetPetPocket()->UnRidePet();
	}
}

//-----------------------------------------------------------------------------------------------------------------
// 邀请骑乘
//-----------------------------------------------------------------------------------------------------------------
VOID BuffEffect::BuffEffectMountInvite( Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet/*=TRUE*/, const tagBuffProto* pProto/*=NULL*/ )
{
	if (!P_VALID(pTarget) || !P_VALID(pSrc)) return;
	if (!pTarget->IsRole() || !pSrc->IsRole()) return;
	Role* pHost = static_cast<Role* >(pSrc);
	Role* pInvited = static_cast<Role* >(pTarget);

	DWORD dwRtv = E_Success;

	if (bSet)
	{
		dwRtv = pInvited->GetPetPocket()->SetRideAfter(pHost, TRUE);
		ASSERT(E_Success == dwRtv);
		dwRtv = pHost->GetPetPocket()->AddPassenger(pInvited);
		ASSERT(E_Success == dwRtv);
	}
	else
	{
		dwRtv = pInvited->GetPetPocket()->SetRideAfter(pHost, FALSE);
		ASSERT(E_Success == dwRtv);
		dwRtv = pHost->GetPetPocket()->RemovePassenger(pInvited);
		ASSERT(E_Success == dwRtv);
	}
}

//-----------------------------------------------------------------------------------------------------------------
// 辅助函数: 释放完buffer怪物是否继续攻击玩家
//-----------------------------------------------------------------------------------------------------------------
BOOL BuffEffect::EndbuffContinueIsAttack(Unit* pTarget, Unit* pSrc, BOOL bSet)
{
	if (!P_VALID(pTarget) || !P_VALID(pSrc))
		return FALSE;

	// 开始昏睡，增加怪物追击时间
	if ( FALSE ==bSet && P_VALID(pTarget) && pTarget->IsCreature() )
	{
		Creature * pCreature = static_cast<Creature *>(pTarget);
		
		// 查看是否可达
		if( MoveData::EMR_Success == pTarget->GetMoveData().IsCanCreatureWalk(pSrc->GetCurPos(), EMS_CreatureWalk, TRUE) )
		{
			
			if (!P_VALID( pCreature->GetAI()) )
				return FALSE;

			pCreature->GetAI()->AddEnmity(pSrc, 1);

			// 设置源玩家为怪物最大仇恨度玩家
			pCreature->GetAI()->SetMaxEnmityUnitID(pSrc->GetID());
		}

	}
	return TRUE;
}