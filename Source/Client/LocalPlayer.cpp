#include "StdAfx.h"
#include "LocalPlayer.h"
#include "FSM_LP.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\msg_pk.h"
#include "..\WorldDefine\msg_combat.h"
#include "RoleEvent.h"
#include "MapMgr.h"
#include "NetSession.h"
#include "PlayerNameTab.h"
#include "LevelUpData.h"
#include "Pet.h"

IMP_RTTI(LocalPlayer,Player);

LocalPlayer::LocalPlayer(void)
: m_pFSM(0)
, m_rolecurlevelexp(0)
, m_roleStateEx(0)
{
}

LocalPlayer::~LocalPlayer(void)
{
	SAFE_DELETE(m_pFSM);
}

void LocalPlayer::InitFSM()
{
	SAFE_DELETE(m_pFSM);
	m_pFSM=new FSM_LP(this);
	m_pFSM->Init();
}

void LocalPlayer::Update()
{
	//--处理消息点
	NodeAnimationCtrl* pAniCtrl = GetAniCtrl();
	HandleMsgCode(pAniCtrl);

	//--update rotater
	m_rotaterYaw.Update();
	m_rotaterPitch.Update();
	m_upperRotater.Update();
	m_headRotater.Update();
	m_scaler.Update();

	//--update FSM
	m_pFSM->Update();

	//--update animation player
	m_aniPlayer.Update();

	//--update world matrix
	if( NULL != m_pAvatarNode )
	{
		if( P_VALID(m_pMount) )//骑乘状态，需要重新计算世界矩阵
		{
			if( m_pMount->GetOwnerID() == m_id )//主骑乘状态
			{
				Matrix4 matRide;
				if( CalRideMat(m_pMount,matRide,0) )
				{
					m_pAvatarNode->SetWorldMat( &matRide );
				}
			}
			else//搭乘状态
			{
				Matrix4 matRide;
				if( CalRideMat(m_pMount,matRide,1) )
				{
					m_pAvatarNode->SetWorldMat( &matRide );
				}
			}
		}
		else//普通状态
		{
			m_pAvatarNode->SetWorldMat(m_space.GetMatrix());
		}
	}
}

void LocalPlayer::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_SetRoleState"))
	{
		tagNS_SetRoleState* pCmd=(tagNS_SetRoleState*)pNetCmd;
		SetRoleState(pCmd->eState, true);
	}
	else if(pNetCmd->dwID==pNetCmd->Crc32("NS_UnSetRoleState"))
	{
		tagNS_UnSetRoleState* pCmd=(tagNS_UnSetRoleState*)pNetCmd;
		SetRoleState(pCmd->eState, false);
	}
	else if(pNetCmd->dwID==pNetCmd->Crc32("NS_RolePKStateChange"))
	{
		tagNS_RolePKStateChange* pCmd=(tagNS_RolePKStateChange*)pNetCmd;
		m_ePKState=pCmd->eState;
	}

	m_pFSM->OnNetCmd(pNetCmd);
}

void LocalPlayer::OnGameEvent( tagGameEvent* pGameEvent )
{
	m_pFSM->OnGameEvent(pGameEvent);
}

bool LocalPlayer::IsReadyState()
{
	return m_pFSM->IsReadyState();
}

bool LocalPlayer::IsStandState()
{
	return m_pFSM->IsStandState();
}

bool LocalPlayer::IsStyleActionState()
{
	return m_pFSM->IsStyleActionState();
}

bool LocalPlayer::IsCombatState()
{
	return m_pFSM->IsCombatState();
}

bool LocalPlayer::IsSkillState()
{
	return m_pFSM->IsSkillState();
}

bool LocalPlayer::IsDead()
{
	return m_pFSM->IsDeadState();
}

bool LocalPlayer::IsSwimState()
{
	return m_pFSM->IsSwimState();
}

bool LocalPlayer::IsOnWaterState()
{
	return m_pFSM->IsOnWaterState();
}

bool LocalPlayer::IsIdleState()
{
	return m_pFSM->IsUpperIdleState()
		&& (m_pFSM->IsStandState()||m_pFSM->IsFloatState()||m_pFSM->IsWaterStandState());
}

VOID LocalPlayer::InitAttribute(tagNS_GetRoleInitState_Att* pCmd)
{
	// 确保SeneneNode已经创建
	ASSERT( P_VALID( m_pAvatarNode ) );

	//// 设置角色ID和姓名
	//SetID(pCmd->RoleData.dwID);
	//SetPlayerName(pCmd->RoleData.szRoleName, pCmd->RoleData.dwRoleNameCrc);

	// 设置角色位置和朝向
	//Vector3 Pos(pCmd->RoleData.fPos[0], pCmd->RoleData.fPos[1], pCmd->RoleData.fPos[2]);
	//Vector3 FaceTo(pCmd->RoleData.fFaceTo[0], pCmd->RoleData.fFaceTo[1], pCmd->RoleData.fFaceTo[2]);
	//SetPos(Pos);
	//SetYaw(CalcYaw(FaceTo));
	
	// 设置角色属性
	INT i;
	for (i=0; i<ERA_End; ++i)
	{
		SetAttribute((ERoleAttribute)i, pCmd->nAtt[i], true);			
		
		// 一级属性需记录属性投放点数
		if (ERA_AttA_Start <= i && ERA_AttA_End >= i)
		{
			SetAttPointAdd((ERoleAttribute)i, pCmd->nAttPointAdd[i]);
		}
	}
	
	// 设置角色等级
	SetRoleLevel(pCmd->nLevel);
	SetRoleLevelExp(pCmd->nCurLevelExp);

	// 留言
	if( pCmd->bHasLeftMsg )
	{
		tagGameEvent evt(_T("tagLeaveMsg"), NULL);
		TObjRef<GameFrameMgr>()->SendEvent(&evt);
	}

	// 设置角色名称
	tstring strName = PlayerNameTab::Inst()->FindNameByID(m_id);
	if(strName != _T(""))
	{
		SetRoleName(strName);
	}

	//// 设置玩家职业
	//SetPlayerClass(pCmd->RoleData.eClass, pCmd->RoleData.eClassEx);

	// 设置玩家状态
	SetRoleState(pCmd->dwRoleState);
	SetRoleStateEx(pCmd->dwRoleStateEx);

    // 设置角色当前称号
    SetPlayerTitle(pCmd->u16ActTitleID);

    // 设置职业
    SetPlayerClass(pCmd->eClassType);
    SetPlayerClassEx(pCmd->eClassTypeEx);

	// 设置摊位等级
	SetStallLevel(pCmd->byStallLevel);

    // 设置配偶
    SetPlayerLoverID(pCmd->dwLoverID);
    SetPlayerIsHasWedding(pCmd->bHaveWedding);

    // 设置复活点
    SetRebornPoint(pCmd->dwRebornMapID, pCmd->vRebornPoint);

    // 设置可记录传送点
    SetTransportPoint(pCmd->dwTransportID, pCmd->vTransportPoint);

	// 设置PK状态
	m_ePKState=pCmd->ePKState;

	// 设置角色状态
	m_state.Init(pCmd->dwState);

	// 如果角色上线时已死亡，直接进入死亡状态
	if(IsInState(ES_Dead))
	{
		m_pFSM->Change2LowerState_AutoStand();
		m_pFSM->Change2UpperState_Dead(false);
	}

	// 设置玩家门派信息
	SetRoleGuildID(pCmd->dwGuildID);
	

	// 添加buff列表
	FreeRoleBuff();
	for(INT j=0; j<pCmd->nBuffNum; ++j)
	{
		AddRoleBuff(pCmd->Buff[j]);
	}

	// 给状态机伪造AddBuff消息
	tagNS_AddRoleBuff tmpCmd;
	for(i=0; i<pCmd->nBuffNum; ++i)
	{
		tmpCmd.dwRoleID=GetID();
		tmpCmd.Buff=pCmd->Buff[i];
		m_pFSM->OnNetCmd(&tmpCmd);
	}

	// 给状态机伪造SetState消息
	tagNS_SetState setStateCmd;
	setStateCmd.dwRoleID=GetID();
	if(IsInState(ES_Dizzy))
	{
		setStateCmd.eState=ES_Dizzy;
		m_pFSM->OnNetCmd(&setStateCmd);
	}
	if(IsInState(ES_Tie))
	{
		setStateCmd.eState=ES_Tie;
		m_pFSM->OnNetCmd(&setStateCmd);
	}			
	if(IsInState(ES_Spor))
	{
		setStateCmd.eState=ES_Spor;
		m_pFSM->OnNetCmd(&setStateCmd);
	}			
	if(IsInState(ES_Invincible))
	{
		setStateCmd.eState=ES_Invincible;
		m_pFSM->OnNetCmd(&setStateCmd);
	}			
	if(IsInState(ES_Lurk))
	{
		setStateCmd.eState=ES_Lurk;
		m_pFSM->OnNetCmd(&setStateCmd);
	}			
	if(IsInState(ES_DisArm))
	{
		setStateCmd.eState=ES_DisArm;
		m_pFSM->OnNetCmd(&setStateCmd);
	}			
	if(IsInState(ES_NoSkill))
	{
		setStateCmd.eState=ES_NoSkill;
		m_pFSM->OnNetCmd(&setStateCmd);
	}

	// 设置其他

	m_bAttributeInited = true;
}

VOID LocalPlayer::SetAttPointAdd(ERoleAttribute eType, INT nValue)
{
	m_rolepointadd[eType] = nValue;
	
	// 发送更新事件
	tagUpdateLPAttEvent event(_T("tagUpdateLPBidAttEvent"), NULL);
	event.eType = eType;
	TObjRef<GameFrameMgr>()->SendEvent(&event);
}

VOID LocalPlayer::SetRoleLevelExp( INT nValue, INT nInc )
{
	m_rolecurlevelexp = nValue;

	// 发送更新事件
	tagUpdateLPLevelExp event(_T("tagUpdateLPLevelExp"), NULL);
	event.nCurLevelExp = nValue;
	event.nExpInc = nInc;
	
	TObjRef<GameFrameMgr>()->SendEvent(&event);
}

void LocalPlayer::OnGotoMap(const Vector3& pos,const Vector3 faceTo)
{
	//--设置位置
	SetPos(pos);

	//--设置朝向
	float yaw=CalcYaw(faceTo);
	SetMoveYaw(yaw);
	SetYaw(yaw);
	
	//--状态机切换状态
	m_pFSM->Change2LowerState_AutoStand();
	if( !IsDead() )
		m_pFSM->Change2UpperState_Idle();
}

INT LocalPlayer::GetOriginAtt( ERoleAttribute eType )
{
	ASSERT(eType >= ERA_AttA_Start && eType <= ERA_AttA_End);
	if(eType >= ERA_AttA_Start && eType <= ERA_AttA_End)
	{
		const tagExperience* pLevelup = LevelUpData::Inst()->FindExpData(m_nRoleLevel);
		if (P_VALID(pLevelup))
		{
			return pLevelup->nAtt[MTransERAAttA2Index(eType)] + m_rolepointadd[MTransERAAttA2Index(eType)];
		}
	}
	return NULL;
}

void LocalPlayer::SetRoleStateEx( DWORD dwValue )
{
	m_roleStateEx = dwValue;
}

void LocalPlayer::SetRoleStateEx( ERoleStateEx eType, bool bFlag )
{
	if (bFlag)
		m_roleStateEx |= eType;
	else
		m_roleStateEx ^= eType;
}

EActionState LocalPlayer::GetActionState()
{
	return m_pFSM->GetActionState();
}

void LocalPlayer::ReplayLowerAni()
{
	m_pFSM->ReplayLowerAni();
}


void LocalPlayer::DontPlayEaseSkeleton()
{
	if( P_VALID(m_pFSM) )
		m_pFSM->DontPlayEaseSkeleton();
}


void LocalPlayer::ChangeUpperToIdle()
{
	if( P_VALID(m_pFSM) )
		m_pFSM->Change2UpperState_Idle();
}

bool LocalPlayer::IsUpperIdleState()
{
	if( P_VALID(m_pFSM) )
		return m_pFSM->IsUpperIdleState();
	else 
		return false;
}

DWORD LocalPlayer::GetUsingItemTypeID()
{ 
	if( P_VALID(m_pFSM) )
		return m_pFSM->GetUsingItemTypeID();
	else
		return 0;
}