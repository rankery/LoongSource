//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: creature.cpp
// author: Aslan
// actor:
// data: 2008-8-11
// last:
// brief: 游戏内生物类，代表所有的怪物、普通NPC、可交互式地物、宠物等等
//-----------------------------------------------------------------------------
#include "StdAfx.h"

#include "../WorldDefine/move_define.h"
#include "../WorldDefine/creature_define.h"
#include "../WorldDefine/msg_combat.h"
#include "../ServerDefine/NPCTeam_define.h"

#include "unit.h"
#include "buff.h"
#include "creature.h"
#include "creature_ai.h"
#include "role.h"
#include "map.h"
#include "loot_mgr.h"
#include "team.h"
#include "group_mgr.h"
#include "script_mgr.h"
#include "path_finder.h"
#include "NPCTeam.h"
#include "NPCTeam_mgr.h"
#include "ai_trigger.h"

Creature* Creature::Create( DWORD dwID, DWORD dwMapID, const tagCreatureProto* pProto, Vector3& vPos, Vector3& vFaceTo, ECreatureSpawnedType eSpawnedType, DWORD dwSpawnerID, BOOL bCollide, const tagMapWayPointInfoList* patrolList, 
						   DWORD dwTeamID/*=GT_INVALID*/, DWORD dwMapObjID /*= GT_INVALID*/ )
{
	Creature* pNew = new Creature(dwID, dwMapID, vPos, vFaceTo, eSpawnedType, dwSpawnerID, bCollide, dwTeamID, dwMapObjID);
	pNew->Init(pProto, patrolList);

	return pNew;
}

VOID Creature::Delete( Creature* &pCreature )
{
	SAFE_DEL(pCreature);
}
//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
Creature::Creature(DWORD dwID, DWORD dwMapID, Vector3& vPos, Vector3& vFaceTo, 
				   ECreatureSpawnedType eSpawnedType, DWORD dwSpawnerID, BOOL bCollide, DWORD dwTeamID, DWORD dwMapObjID)
				   :Unit(dwID, dwMapID, vPos.x, vPos.y, vPos.z, vFaceTo.x, vFaceTo.y, vFaceTo.z), ScriptData(),
				   m_pProto(NULL), m_pAIController(NULL), m_eSpawnedType(eSpawnedType), m_dwSpawnerID(dwSpawnerID),m_nLiveTick(0),
				   m_vBornPos(vPos), m_vBornFace(vFaceTo), m_bCollide(bCollide), m_bTagged(FALSE), m_dwTaggedOwner(GT_INVALID),
				   m_nRespawnTickCountDown(0), m_pMeleeSkill(NULL), m_pRangedSkill(NULL), m_dwTeamID(dwTeamID), m_pScript(NULL),
				   m_bDoorOpen(FALSE),m_dwMapObjID(dwMapObjID)

{
	
}

//-------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------
Creature::~Creature()
{
	Destroy();
}

//--------------------------------------------------------------------------------
// 初始化
//--------------------------------------------------------------------------------
BOOL Creature::Init(const tagCreatureProto* pProto, const tagMapWayPointInfoList* patrolList)
{
	m_pProto = pProto;

	// 初始化脚本
	m_pScript = g_ScriptMgr.GetCreatureScript(pProto->dwTypeID);

	// 初始化属性
	InitAtt(pProto);

	// 初始化AI
	InitAI(pProto, patrolList);

	// 计算生物初始属性
	CalInitAtt();

	// 调用脚本
	if( P_VALID(m_pScript) )
	{
		m_pScript->OnLoad(this);
	}

	return TRUE;
}

//---------------------------------------------------------------------------------
// 初始化基本属性
//---------------------------------------------------------------------------------
BOOL Creature::InitAtt(const tagCreatureProto* pProto)
{
	// 设置包裹盒
	m_Size = pProto->vSize;

	// 从静态属性中拷贝出怪物基本属性
	IFASTCODE->MemCpy(m_nBaseAtt, pProto->nBaseAtt, sizeof(m_nBaseAtt));

	m_nLevel = pProto->nLevel;
	m_nCurLevelExp = 0;

	// 根据怪物的普通攻击id来生成技能
	const tagSkillProto* pMeleeSkillProto = g_attRes.GetSkillProto(pProto->dwNormalSkillIDMelee);
	if( P_VALID(pMeleeSkillProto) )
	{
		DWORD dwSkillID = Skill::GetIDFromTypeID(pMeleeSkillProto->dwID);
		DWORD nLevel = Skill::GetLevelFromTypeID(pMeleeSkillProto->dwID);

		m_pMeleeSkill = new Skill(dwSkillID, nLevel, 0, 0, 0);
		ASSERT(P_VALID(m_pMeleeSkill));
		m_mapSkill.Add(m_pMeleeSkill->GetID(), m_pMeleeSkill);
	}
	else
	{
		m_pMeleeSkill = NULL;
	}

	const tagSkillProto* pRangedSkillProto = g_attRes.GetSkillProto(pProto->dwNormalSkillIDRanged);
	if( P_VALID(pRangedSkillProto) )
	{
		DWORD dwSkillID = Skill::GetIDFromTypeID(pRangedSkillProto->dwID);
		DWORD nLevel = Skill::GetLevelFromTypeID(pRangedSkillProto->dwID);

		m_pRangedSkill = new Skill(dwSkillID, nLevel, 0, 0, 0);

		m_mapSkill.Add(m_pRangedSkill->GetID(), m_pRangedSkill);
	}
	else
	{
		m_pRangedSkill = NULL;
	}

	return TRUE;
}

//---------------------------------------------------------------------------------
// 初始化怪物AI
//---------------------------------------------------------------------------------
BOOL Creature::InitAI(const tagCreatureProto* pProto, const tagMapWayPointInfoList*	patrolList)
{
	// 生成状态机
	if( pProto->eAICreateType != EACT_Null )
	{
		m_pAIController = new AIController(this, patrolList);
	}

	return TRUE;
}

//---------------------------------------------------------------------------------
// 复活的时候刷新属性
//---------------------------------------------------------------------------------
VOID Creature::RefreshAtt()
{
	// 移动清空
	m_MoveData.Reset(m_vBornPos.x, m_vBornPos.y, m_vBornPos.z, m_vBornFace.x, m_vBornFace.y, m_vBornFace.z);

	// 速度清空
	m_fXZSpeed = X_DEF_XZ_SPEED;
	m_fYSpeed = X_DEF_Y_SPEED;

	// 包裹盒清空
	m_Size = m_pProto->vSize;

	// 清空之前的状态
	ClearState();

	// 某些怪物属性
	m_bTagged = FALSE;
	m_dwTaggedOwner = GT_INVALID;
	m_nRespawnTickCountDown = 0;
	m_nLiveTick = 0;

	// 属性调整
	ZeroMemory(m_nAttMod, sizeof(m_nAttMod));
	ZeroMemory(m_nAttModPct, sizeof(m_nAttModPct));
	ZeroMemory(m_nAtt, sizeof(m_nAtt));
	IFASTCODE->MemCpy(m_nBaseAtt, m_pProto->nBaseAtt, sizeof(m_nBaseAtt));

	// 技能清空
	TMap<DWORD, Skill*>::TMapIterator it = m_mapSkill.Begin();
	Skill* pSkill = NULL;
	while( m_mapSkill.PeekNext(it, pSkill) )
	{
		// 初始技能不清空
		if( pSkill != m_pMeleeSkill && pSkill != m_pRangedSkill )
		{
			RemoveSkill(pSkill->GetID());
		}
	}
}

//---------------------------------------------------------------------------------
// 复活的时候刷新AI
//---------------------------------------------------------------------------------
VOID Creature::RefreshAI()
{
	// 设置状态机
	if( P_VALID(m_pAIController) )
	{
		m_pAIController->Refresh();
	}
}


//---------------------------------------------------------------------------------
// 计算怪物当前基本属性
//---------------------------------------------------------------------------------
VOID Creature::CalInitAtt()
{
	for(INT n = 0; n < ERA_End; n++)
	{
		m_nAtt[n] = m_nBaseAtt[n] + m_nAttMod[n] + (INT)(m_nBaseAtt[n] * (FLOAT)m_nAttModPct[n] / 10000.0f);
		// todo：取上下限
	}

	// 将某些“当前属性”设置成最大值
	m_nAtt[ERA_HP] = m_nAtt[ERA_MaxHP];
	m_nAtt[ERA_MP] = m_nAtt[ERA_MaxMP];
	m_nAtt[ERA_Vitality] = m_nAtt[ERA_MaxVitality];
	m_nAtt[ERA_Endurance] = m_nAtt[ERA_MaxEndurance];

	// 设置某些由基本属性影响的值
	m_fXZSpeed *= FLOAT(m_nAtt[ERA_Speed_XZ]) / 10000.0f;
	m_fYSpeed *= FLOAT(m_nAtt[ERA_Speed_Y]) / 10000.0f;
	m_Size *= FLOAT(m_nAtt[ERA_Shape]) / 10000.0f;
}

//---------------------------------------------------------------------------------
// 销毁
//---------------------------------------------------------------------------------
VOID Creature::Destroy()
{
	SAFE_DEL(m_pAIController);
}

//---------------------------------------------------------------------------------
// 更新函数
//---------------------------------------------------------------------------------
VOID Creature::Update()
{
	GetMoveData().Update();
	UpdateCombat();
	UpdateAI();
	UpdateSkill();
	UpdateBuff();
	UpdateTimedIssue();
	UpdateLiveTime();
}

//---------------------------------------------------------------------------------
// 更新技能
//---------------------------------------------------------------------------------
VOID Creature::UpdateCombat()
{
	// 当前技能还在作用
	if( m_CombatHandler.IsValid() )
	{
		m_CombatHandler.Update();

		if( !m_CombatHandler.IsValid() )
		{
			// 本技能释放完成，设置下次攻击的等待时间
			GetAI()->SetNextAttackWaitTick(m_pProto->nAttackInterTick);
		}
	}
}

//---------------------------------------------------------------------------------
// 更新AI
//---------------------------------------------------------------------------------
VOID Creature::UpdateAI()
{
	// 根据怪物目前处在哪一个AI状态决定如何进行更新
	if( P_VALID(m_pAIController) )
		m_pAIController->Update();
}

//---------------------------------------------------------------------------------
// 更新生存时间
//---------------------------------------------------------------------------------
VOID Creature::UpdateLiveTime()
{
	if(0 != m_pProto->nLiveTick)
	{
		++m_nLiveTick;
		if(m_nLiveTick >= m_pProto->nLiveTick)
		{
			OnDisappear();
		}
	}
}

//---------------------------------------------------------------------------------
// 检测玩家和NPC谈话的距离
//---------------------------------------------------------------------------------
BOOL Creature::CheckNPCTalkDistance(Role* pRole)
{
	if( !P_VALID(pRole) ) return FALSE;
	return IsInDistance(*pRole, MAX_NPC_TALK_DISTANCE);
}

//---------------------------------------------------------------------------------
// 被攻击
//---------------------------------------------------------------------------------
VOID Creature::OnBeAttacked(Unit* pSrc, Skill* pSkill, BOOL bHited, BOOL bBlock, BOOL bCrited)
{
	ASSERT( P_VALID(pSkill) && P_VALID(pSrc) );
	
	// 判断技能的敌我目标类型
	if( !pSkill->IsHostile() && !pSkill->IsFriendly() ) return;
	DWORD dwFriendEnemyFlag = pSrc->FriendEnemy(this);

	if( pSkill->IsHostile() && (ETFE_Hostile & dwFriendEnemyFlag) )
	{
		if( pSrc->IsRole() )
		{
			// 如果怪物之前无所属，则变为所属
			if( ECTT_Hit == GetTaggedType() && !IsTagged() )
			{
				SetTagged(pSrc->GetID());
			}
		}
		
		// 计算仇恨，怪物的AI触发
		if( P_VALID(m_pAIController) )
		{
			INT	nValue = pSkill->GetEnmity() + pSrc->GetAttValue(ERA_Enmity_Degree);
			m_pAIController->AddEnmity(pSrc, nValue);
			m_pAIController->OnEvent(pSrc, ETEE_BeAttacked);
		}

		if( bHited )
		{
			OnBuffTrigger(pSrc, ETEE_Hited, 0, 0);
			if( bBlock )
			{
				OnBuffTrigger(pSrc, ETEE_Block, 0, 0);
			}
			if( bCrited )
			{
				OnBuffTrigger(pSrc, ETEE_Crited, 0, 0);
			}

			// 检测当前技能的打断
			GetCombatHandler().InterruptPrepare(CombatHandler::EIT_Skill, ESSTE_Default == pSkill->GetTypeEx());
			// 检测某些被攻击打断的buff
			OnInterruptBuffEvent(EBIF_BeAttacked);
		}
		else
		{
			OnBuffTrigger(pSrc, ETEE_Dodge, 0, 0);
		}
		OnBuffTrigger(pSrc, ETEE_BeAttacked, 0, 0);
	}
}

VOID Creature::OnBuffInjury(Unit* pSrc)
{
	if( !P_VALID(pSrc) )	return;

	if( pSrc->IsRole() )
	{
		// 如果怪物之前无所属，则变为所属
		if( ECTT_Hit == GetTaggedType() && !IsTagged() )
		{
			SetTagged(pSrc->GetID());
		}
	}

	// 计算仇恨，怪物的AI触发
	if( P_VALID(m_pAIController) )
	{
		m_pAIController->AddEnmity(pSrc, 1);
		m_pAIController->OnEvent(pSrc, ETEE_BeAttacked);
	}
}

VOID Creature::OnAIAttack()
{
	DWORD dwSkillID = m_CombatHandler.GetSkillID();

	// 如果是普通攻击，触发怪物触发器攻击事件
	if( P_VALID(m_pMeleeSkill) && dwSkillID  == m_pMeleeSkill->GetID() )
	{
		if(P_VALID(m_pAIController))
		{
			AITriggerMgr* pTrggerMgr = GetAI()->GetAITriggerMgr();
			if(P_VALID(pTrggerMgr))
			{
				pTrggerMgr->OnEvent(ETEE_Attack); 
				return;
			}
		}
	}

	if(P_VALID(m_pRangedSkill) && dwSkillID == m_pRangedSkill->GetID())
	{
		if(P_VALID(GetAI()))
		{
			AITriggerMgr* pTrggerMgr = GetAI()->GetAITriggerMgr();
			if(P_VALID(pTrggerMgr))
			{
				pTrggerMgr->OnEvent(ETEE_Attack); 
				return;
			}
		}
	}

}

//---------------------------------------------------------------------------------
// 怪物死亡
//---------------------------------------------------------------------------------
VOID Creature::OnDead(Unit* pSrc, Skill* pSkill, const tagBuffProto* pBuff, DWORD dwSerial, DWORD dwMisc)
{
	// 如果本身已经死亡，则直接返回
	if( IsInState(ES_Dead) ) return;

	// 触发器
	OnBuffTrigger(pSrc, ETEE_Die);

	//触发脚本 目前不使用bRemove这个脚本返回值
	BOOL bRemove = TRUE;
	if (P_VALID(m_pScript))
	{
		m_pScript->OnDie(this, pSrc, bRemove);
	}

	// 设置为死亡状态
	SetState(ES_Dead);

	// 停止移动
	m_MoveData.StopMoveForce();

	// 停止当前的技能
	m_CombatHandler.End();

	// 怪物死亡时AI触发

	// 移除身上所有的buff
	RemoveAllBuff();

	// 清空仇恨列表
	if( P_VALID(GetAI()) )
		GetAI()->ClearAllEnmity();

	// 找到奖励授予者
	Role* pReward = FindRewardRole(pSrc);
	if( P_VALID(pReward) )
	{
		// 计算经验奖励
		BOOL bKill = FALSE;
		if( P_VALID(pSkill) )
			bKill = TRUE;

		if(pReward->GetTeamID() ==GT_INVALID)
		{
			ExpReward(pReward, bKill);

			// 计算任务杀怪计数
			pReward->OnQuestEvent(EQE_Kill, GetTypeID(), GetLevel());
		}
		else
		{
			const Team *pTeam = g_groupMgr.GetTeamPtr(pReward->GetTeamID());
			std::vector<Role*>	vecRole;
			INT   nShareRole = CalExpShareRole(pTeam, vecRole);

			std::vector<Role*>::iterator it = vecRole.begin();
			while (it != vecRole.end())
			{
				Role* pMember = *it;
				++it;

				if( !P_VALID(pMember) )
					continue;

				ExpReward(pMember, bKill, pTeam, nShareRole);

				// 计算任务杀怪计数
				pMember->OnQuestEvent(EQE_Kill, GetTypeID(), GetLevel());
			}			
		}
		
		// 计算掉落奖励
		sLootMgr.MonsterLoot(this, pReward);
	}

	// 击杀者
	if( P_VALID(pSrc) )
	{
		pSrc->OnKill(this);
	}

	// 地图事件
	GetMap()->OnCreatureDie(this, pSrc);

	// 发送怪物死亡消息
	tagNS_RoleDead send;
	send.dwRoleID = GetID();
	send.dwSrcRoleID = (P_VALID(pSrc) ? pSrc->GetID() : GT_INVALID);
	send.dwSerial = dwSerial;
	if( P_VALID(pSkill) )
	{
		send.eCause = ERDC_Skill;
		send.dwMisc = pSkill->GetTypeID();
		send.dwMisc2 = dwMisc;
	}
	else if( P_VALID(pBuff) )
	{
		send.eCause = ERDC_Buff;
		send.dwMisc = pBuff->dwID;
		send.dwMisc2 = dwMisc;
	}
	else
	{
		send.eCause = ERDC_Other;
		send.dwMisc = GT_INVALID;
		send.dwMisc2 = dwMisc;
	}

	GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);

	// 设置死亡复活时间
	SetRespawnCountDown(GetRespawnTick());

	// 从地图中拿掉该生物
//	if (bRemove)
//	{
		GetMap()->RemoveCreature(this);
//	}
}

//---------------------------------------------------------------------------------
// 怪物消失
//---------------------------------------------------------------------------------
VOID Creature::OnDisappear()
{
	// 如果本身已经死亡，则直接返回
	if( IsInState(ES_Dead) ) return;

	// 设置为死亡状态
	SetState(ES_Dead);

	// 停止移动
	m_MoveData.StopMoveForce();

	// 停止当前的技能
	m_CombatHandler.End();

	// 怪物死亡时AI触发

	// 移除身上所有的buff
	RemoveAllBuff();

	// 清空仇恨列表
	if( P_VALID(GetAI()) )
		GetAI()->ClearAllEnmity();

	// 地图事件
	GetMap()->OnCreatureDisappear(this);

	// 设置死亡复活时间
	SetRespawnCountDown(GetRespawnTick());

	// 从地图中拿掉该生物
	GetMap()->RemoveCreature(this);

	// 怪物消失脚本触发
	if (P_VALID(m_pScript))
		m_pScript->OnDisappear(this);

	// 同步给客户端
	GetMap()->SyncRemoveUnitToBigVisTile(this);
}

//----------------------------------------------------------------------------------
// 生物击杀
//----------------------------------------------------------------------------------
VOID Creature::OnKill(Unit* pTarget)
{
	OnBuffTrigger(pTarget, ETEE_Kill);
}

//----------------------------------------------------------------------------------
// 尝试复活
//----------------------------------------------------------------------------------
ECreatureReviveResult Creature::TryRevive()
{
	// 如果怪物是动态生成的，则需要删除内存了
	if( ECST_Dynamic == m_eSpawnedType ) return ECRR_NeedDestroy;

	// 复活倒计时减少一个Tick
	CountDownRespawnTick();

	// 如果复活时间没到，则返回失败
	if( GetRespawnCountDown() > 0 ) return ECRR_Failed;

	// 如果是刷新点怪，则需要产生替代品
	if ( ECST_SpawnPoint == m_eSpawnedType ) return ECRR_NeedReplace;

	// 如果是巢穴的怪物
	if( ECST_Nest == m_eSpawnedType )
	{
		// 找到刷出的巢穴是否还存在，如果不存在了，说明巢穴还没刷新，那么自己也不刷新
		Creature* pNestCreature = GetMap()->FindCreature(m_dwSpawnerID);
		if( !P_VALID(pNestCreature) ) return ECRR_Failed;

		// 如果找到，则找到该巢穴的的巢穴属性
		const tagCreatureProto* pProto = pNestCreature->GetProto();
		if( !P_VALID(pProto) || !P_VALID(pProto->pNest) ) return ECRR_Failed;

		const tagNestProto* pNest = pProto->pNest;

		// 找到一个可行走的随机点
		Vector3 vNewBornPos;

		vNewBornPos.x = FLOAT(IUTIL->Rand() % (pNest->nSpawnRadius * 2) - pNest->nSpawnRadius) + pNestCreature->GetCurPos().x;
		vNewBornPos.z = FLOAT(IUTIL->Rand() % (pNest->nSpawnRadius * 2) - pNest->nSpawnRadius) + pNestCreature->GetCurPos().z;

		if( GetMap()->IfCanGo(vNewBornPos.x, vNewBornPos.z) )
		{
			vNewBornPos.y = GetMap()->GetGroundHeight(vNewBornPos.x, vNewBornPos.z);
		}
		else
		{
			// 没有找到一个复活点，本Tick不复活
			return ECRR_Failed;
		}

		// 随机一个朝向
		Vector3 vNewBornFaceTo;
		FLOAT fYaw = FLOAT(IUTIL->Rand() % 360);
		vNewBornFaceTo.x = cosf(fYaw * 3.1415927f / 180.0f);
		vNewBornFaceTo.z = sinf(fYaw * 3.1415927f / 180.0f);
		vNewBornFaceTo.y = 0.0f;


		// 让生物的出生坐标和出生朝向等于这个新值
		m_vBornPos = vNewBornPos;
		m_vBornFace = vNewBornFaceTo;
	}

	// 检测通过，怪物可以复活了
	OnRevive();

	return ECRR_Success;
}

//---------------------------------------------------------------------------------
// 生物复活
//---------------------------------------------------------------------------------
VOID Creature::OnRevive()
{
	// 刷新属性
	RefreshAtt();
	// 刷新AI
	RefreshAI();
	// 重新计算属性
	CalInitAtt();

	if(P_VALID(m_pScript))
	{
		m_pScript->OnRespawn(this);
	}
}

//---------------------------------------------------------------------------------
// 查找到奖励授予者（参数中为击杀者）
//---------------------------------------------------------------------------------
Role* Creature::FindRewardRole(Unit* pSrc)
{
	Role* pReward = NULL;
	if( ECTT_Hit == m_pProto->eTaggedType )
	{
		pReward = GetMap()->FindRole(m_dwTaggedOwner);
	}
	else
	{
		if( P_VALID(pSrc) && pSrc->IsRole() )
			pReward = static_cast<Role*>(pSrc);
	}

	return pReward;
}

//---------------------------------------------------------------------------------
// 经验奖励
//---------------------------------------------------------------------------------
VOID Creature::ExpReward(Role* pReward, BOOL bKill, const Team *pTeam, INT nShareNum)
{
	if( !P_VALID(pReward) ) return;

	INT nExp  = 0;

	// 基础经验
	FLOAT fBaseExp = (FLOAT)m_pProto->nExpGive;
	
	// 当-5<=Z-A<=5时，De=100%；
	// 当Z-A<=-6或6<=Z-A时，De=1-（|Z-A|-3）*10%
	FLOAT fCoef = 1.0f;
	FLOAT fLevelDist = FLOAT(abs(GetLevel() - pReward->GetLevel()));
	if(fLevelDist >= 6.0f)
	{
		fCoef = 1.0f - (fLevelDist - 3.0f)  * 0.1f;
	}


	// 取上下限：0——1.2
	if( fCoef < 0.0f ) fCoef = 0.0f;
	if( fCoef > 1.0f ) fCoef = 1.0f;

	// 组队相关的经验奖励
	if(P_VALID(pTeam))
	{
		// D1=1+n*30%, 该值用于调节队伍最大获得经验量
		FLOAT fD1 = 1.0f + ((FLOAT)nShareNum - 1.0f) * 0.3f;
		// D2=1-n*(1-队长统御力/999)*5%, 该值用于调节统御力影响程度
		FLOAT fD2 = 1.0f - ((FLOAT)nShareNum - 1.0f) * (1.0f - pTeam->GetMemPtr(0)->GetAttValue(ERA_Rein)/ 999.0f) * 0.05f;

		// Exp=(Xm*D1*D2/n) *De1*De2
		nExp = INT(fBaseExp * fCoef * fD1 * fD2 * pTeam->GetExpFactor() / nShareNum);

	}
	else
	{
		nExp = INT(fBaseExp * fCoef);
	}
	nExp = pReward->CalRealGainExp(nExp);
	pReward->ExpChange(nExp);
}

//--------------------------------------------------------------------------
// 组队时,玩家是否在经验分配的有效范围
//--------------------------------------------------------------------------
BOOL Creature::IsExpShareDistance(Role* pReward)
{
	const Vector3& vSrc = GetCurPos();
	const Vector3& vDest = pReward->GetCurPos();

	FLOAT fDistSq = Vec3DistSq(vSrc, vDest);

	if(GetMap() == pReward->GetMap() && pReward->IsDead() == FALSE &&fDistSq <= X_DEF_PICKUP_DIST_SQ)
		return TRUE;
	else
		return FALSE;
}

//--------------------------------------------------------------------------
// 组队时,玩家是否在掉落分配的有效范围
//--------------------------------------------------------------------------
BOOL Creature::IsLootShareDistance(Role* pReward)
{
	const Vector3& vSrc = GetCurPos();
	const Vector3& vDest = pReward->GetCurPos();

	FLOAT fDistSq = Vec3DistSq(vSrc, vDest);

	if(GetMapID() == pReward->GetMapID() && fDistSq <= X_DEF_PICKUP_DIST_SQ)
		return TRUE;
	else
		return FALSE;
}

//--------------------------------------------------------------------------
// 计算组队分享经验的小队玩家(返回玩家个数）
//--------------------------------------------------------------------------
INT Creature::CalExpShareRole(const Team *pTeam, std::vector<Role*>  &vecRole)
{
	INT		nShareNum = 0;
	Role	*pReward = (Role*)GT_INVALID;
	if(P_VALID(pTeam))
	{
		INT nTeamNum = pTeam->GetMemNum();

		for(INT i = 0; i < nTeamNum; ++i)
		{
			pReward = pTeam->GetMemPtr(i);

			if(!P_VALID(pReward) || !IsExpShareDistance(pReward))
				continue;
			
			++nShareNum;
			vecRole.push_back(pReward);
		}
	}

	return nShareNum;
}

//--------------------------------------------------------------------------
// 目标标示类型判断
//--------------------------------------------------------------------------
DWORD Creature::TargetTypeFlag(Unit* pTarget)
{
	if( !P_VALID(pTarget) ) return 0;

	if( pTarget->IsRole() )
		return TargetTypeFlag(static_cast<Role*>(pTarget));
	else if( pTarget->IsCreature() )
		return TargetTypeFlag(static_cast<Creature*>(pTarget));
	else
		return 0;
}

DWORD Creature::TargetTypeFlag(Role* pTarget)
{
	ASSERT( P_VALID(pTarget) );

	return ETF_Player;
}
DWORD Creature::TargetTypeFlag(Creature* pTarget)
{
	if( !P_VALID(pTarget) )
		return 0;

	if( this == pTarget )
		return ETF_Self;

	DWORD dwFlag = 0;

	// 怪物
	if( pTarget->IsMonster() )
	{
		// 普通怪物
		if( pTarget->IsNormalMonster() )
		{
			if( !pTarget->IsBoss())
				dwFlag |= ETF_NormalMonster;
			else
				dwFlag |= ETF_Boss;
		}

		// 巢穴
		else if( pTarget->IsNest())
		{
			dwFlag |= ETF_Nest;
		}
	}

	// NPC
	else if( pTarget->IsNPC() )
	{
		dwFlag |= ETF_NPC;
	}

	// 宠物
	else if( pTarget->IsPet() )
	{
		dwFlag |= ETF_Pet;
	}

	// 资源
	else if( pTarget->IsRes() )
	{
		if ( pTarget->IsNatuRes() )
			dwFlag |= ETF_NatuRes;
		else if ( pTarget->IsManRes() )
			dwFlag |= ETF_ManRes;
		else
			ASSERT(0);
	}

	// 可调查地物
	else if( pTarget->IsInves() )
	{
		dwFlag |= ETF_InvesGameObject;
	}

	// 门和建筑再看看放到哪个子类里去

	return dwFlag;
}

//---------------------------------------------------------------------------
// 与目标的敌我中立判断
//---------------------------------------------------------------------------
DWORD Creature::FriendEnemy(Unit* pTarget)
{
	if( !P_VALID(pTarget) ) return 0;
	if( !P_VALID(GetMap()) || GetMap() != pTarget->GetMap() ) return 0;

	// 自身的话，则均满足
	if( this == pTarget )
	{
		return ETFE_Friendly | ETFE_Hostile | ETFE_Independent;
	}

	// 取所在地图的敌我关系判定
	BOOL bIgnore = FALSE;
	DWORD dwMapFlag = GetMap()->GetFriendEnemyFlag(this, pTarget, bIgnore);

	// 如果还需要判断单体自身的，则判断自身
	if( !bIgnore )
	{
		DWORD dwSelfFlag = 0;
		if( pTarget->IsRole() )
			dwSelfFlag = FriendEnemy(static_cast<Role*>(pTarget));
		else if( pTarget->IsCreature() )
			dwSelfFlag = FriendEnemy(static_cast<Creature*>(pTarget));
		else
			dwSelfFlag = 0;

		return (dwMapFlag | dwSelfFlag);
	}
	else
	{
		return dwMapFlag;
	}
}

DWORD Creature::FriendEnemy(Role* pTarget)
{
	DWORD dwFlag = 0;

	// 如果自身是怪物
	if( IsMonster() )
	{
		dwFlag |= ETFE_Hostile;
	}

	// 如果自身是NPC
	else if( IsNPC() )
	{
		// 如果玩家是行凶状态，则为敌方
		if( pTarget->IsInRoleState(ERS_PK) || pTarget->IsInRoleState(ERS_PKEX) )
		{
			dwFlag |= ETFE_Hostile;
		}
		// 否则为友方
		else
		{
			dwFlag |= ETFE_Friendly;
		}
	}

	// 如果自身是地物，则什么都不做
	else if( IsGameObject() )
	{

	}

	// 如果是宠物，则先不做
	else if( IsPet() )
	{

	}

	return dwFlag;
}

DWORD Creature::FriendEnemy(Creature* pTarget)
{
	DWORD dwFlag = 0;

	// 如果自身是怪物
	if( IsMonster() )
	{
		if( pTarget->IsMonster() )
		{
			dwFlag |= ETFE_Friendly;
		}
		else if( pTarget->IsNPC() )
		{
			dwFlag |= ETFE_Hostile;
		}
	}

	// 如果自身是NPC
	else if( IsNPC() )
	{
		if( pTarget->IsMonster() )
		{
			dwFlag |= ETFE_Hostile;
		}
		else if( pTarget->IsNPC() )
		{
			dwFlag |= ETFE_Friendly;
		}
	}

	// 如果自身是地物，先不做处理
	else if( IsGameObject() )
	{

	}

	// 如果自身是宠物，先不做处理
	else if( IsPet() )
	{

	}

	return dwFlag;
}

//----------------------------------------------------------------------------------
// 触发调查地物事件
//----------------------------------------------------------------------------------
VOID Creature::OnInvest(Role* pSrc)
{
	if(!P_VALID(m_pScript))	return;

	m_pScript->OnInvest(this, pSrc);
}

//----------------------------------------------------------------------------------
// 得到怪物小队指针
//----------------------------------------------------------------------------------
NPCTeam* Creature::GetTeamPtr()
{
	NPCTeam* pTeam = (NPCTeam*)GT_INVALID;
	if(GT_INVALID == m_dwTeamID)
		return pTeam;

	Map* pMap = GetMap();
	if(P_VALID(pMap))
	{
		NPCTeamMgr* pTeamMgr = pMap->GetTeamMgr();
		if(P_VALID(pTeamMgr))
		{
			pTeam = pTeamMgr->GetNPCTeam(m_dwTeamID);
		}
	}
	return pTeam;
}

//----------------------------------------------------------------------------------
// 得到周围怪物的ID，并返回数量
//----------------------------------------------------------------------------------
INT Creature::GetAroundCreature(std::vector<DWORD> &vecCreature, FLOAT fOPRadius, FLOAT fHigh)
{
	// 如果范围为0，则直接返回
	if( 0.0f == fOPRadius )
		return 0;

	INT nCreatureNum = 0;
	tagVisTile* pVisTile[ED_End] = {0};

	// 得到范围内的vistile列表
	GetMap()->GetVisTile(GetCurPos(), fOPRadius, pVisTile);
	Creature*	pCreature	= NULL;

	for(INT n = ED_Center; n < ED_End; n++)
	{
		if( !P_VALID(pVisTile[n]) ) continue;

		// 检测生物
		TMap<DWORD, Creature*>& mapCreature = pVisTile[n]->mapCreature;
		TMap<DWORD, Creature*>::TMapIterator it2 = mapCreature.Begin();

		while( mapCreature.PeekNext(it2, pCreature) )
		{
			if(pCreature->IsDead()) continue;

			// 检测距离
			FLOAT fDistSQ = Vec3Dist(GetCurPos(), pCreature->GetCurPos());
			if(fDistSQ > fOPRadius)	continue;

			// 将怪物加入到列表中
			if(abs(pCreature->GetCurPos().y - GetCurPos().y) <= fHigh)
			{
				vecCreature.push_back(pCreature->GetID());
				nCreatureNum++;
			}
		}
	}

	return nCreatureNum;
}

//----------------------------------------------------------------------------------
// 得到周围玩家的ID，并返回数量
//----------------------------------------------------------------------------------
INT	Creature::GetAroundRole(std::vector<DWORD> &vecRole, FLOAT fOPRadius, FLOAT fHigh)
{
	// 如果攻击范围为0，则直接返回
	if( 0.0f == fOPRadius )
		return 0;

	INT nRoleNum = 0;
	tagVisTile* pVisTile[ED_End] = {0};

	// 得到范围内的vistile列表
	GetMap()->GetVisTile(GetCurPos(), fOPRadius, pVisTile);
	Role*		pRole		= NULL;

	for(INT n = ED_Center; n < ED_End; n++)
	{
		if( !P_VALID(pVisTile[n]) ) continue;

		// 检测人物
		TMap<DWORD, Role*>& mapRole = pVisTile[n]->mapRole;
		TMap<DWORD, Role*>::TMapIterator it = mapRole.Begin();

		while( mapRole.PeekNext(it, pRole) )
		{
			if(pRole->IsDead()) continue;

			// 检测距离
			FLOAT fDistSQ = Vec3DistSq(GetCurPos(), pRole->GetCurPos());
			if(fDistSQ > fOPRadius)	continue;

			// 将玩家加入到列表中
			if(abs(pRole->GetCurPos().y - GetCurPos().y) <= fHigh)
			{
				vecRole.push_back(pRole->GetID());
				nRoleNum++;
			}
		}
	}

	return nRoleNum;
}

//-------------------------------------------------------------------------------
// 喊话
//-------------------------------------------------------------------------------
VOID Creature::Say(DWORD dwSayID)
{
	tagNS_MonsterSay send;
	send.dwRoleID = GetID();
	send.dwSayID = dwSayID;

	if( P_VALID(GetMap()) )
	{
		GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
	}
}

//--------------------------------------------------------------------------------
// 播放动作
//--------------------------------------------------------------------------------
VOID Creature::PlayerAction(DWORD dwActionID)
{
	tagNS_MonsterPlayAction send;
	send.dwRoleID = GetID();
	send.dwActionFourCC = dwActionID;

	if( P_VALID(GetMap()) )
	{
		GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
	}
}

//----------------------------------------------------------------------------------
// 设置所属
//----------------------------------------------------------------------------------
VOID Creature::SetTagged(DWORD dwOwner)
{
	m_dwTaggedOwner = dwOwner;
	m_bTagged = GT_VALID(m_dwTaggedOwner);

	// 发送给客户端
	tagNS_CreatureTaggedOwnerChange send;
	send.dwCreatureID = GetID();
	send.dwTaggedOwner = m_dwTaggedOwner;
	
	if( P_VALID(GetMap()) )
	{
		GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
	}
}

//----------------------------------------------------------------------------------
// 设置门状态
//----------------------------------------------------------------------------------
VOID Creature::SetDoorStatus( BOOL bOpen, BOOL bSync /*= TRUE*/ )
{
	if (!IsDoor())	return;
	if (bOpen == m_bDoorOpen)	return;
	
	// 设置新状态
	m_bDoorOpen = bOpen;

	if (!bSync)		return;

	// 同步给周围玩家
	if (m_bDoorOpen)
	{
		tagNS_OpenDoor send;
		send.dwRoleID = GetID();
		GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
	}
	else
	{
		tagNS_CloseDoor send;
		send.dwRoleID = GetID();
		GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
	}
}
