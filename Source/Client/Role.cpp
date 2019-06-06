#include "StdAfx.h"
#include "Role.h"
#include "RoleMgr.h"
#include "RoleEvent.h"
#include "..\WorldDefine\move_define.h"
#include "SkillProtoData.h"
#include "SGDynamicProjector.h"
#include "EffectMgr.h"
#include "Player.h"
#include "Pet.h"
#include "GameSet.h"
#include "..\WorldDefine\pk_define.h"

IMP_RTTI(Role,RTTIObj);

Role::Role(void)
: m_id(0)
, m_pShadow(NULL)
, m_state(0)
, m_nRoleLevel(0)
, m_bNeedToFree(false)
, m_headRotater(this)
, m_upperRotater(this)
, m_fTransparent(1.0f)
, m_vMulColor(1.0f,1.0f,1.0f)
{
	memset( m_roledata, GT_INVALID, sizeof(m_roledata) );
	m_roledata[ERA_Shape]=10000;
	m_roledata[ERA_Speed_XZ]=10000;
	m_roledata[ERA_Speed_Y]=10000;
	m_roledata[ERA_Speed_Swim]=10000;
	m_rotaterYaw.BindRole(this,RoleRotater::ET_Yaw,false,FLOAT_PI*4.0f);
	m_rotaterPitch.BindRole(this,RoleRotater::ET_Pitch,true,0.1f);
	m_scaler.BindRole(this);
}

Role::~Role(void)
{
	FreeRoleBuff();
}

void Role::AttachShadow( SGDynamicProjector *pShadow )
{
	DetachShadow();

	ASSERT( NULL == m_pShadow );
	if( NULL != GetSceneNode() )
	{
		GetSceneNode()->AttachChild(pShadow);
		m_pShadow = pShadow;
	}
}

void Role::DetachShadow()
{
	if( NULL != m_pShadow && NULL != GetSceneNode() )
	{
		GetSceneNode()->DetachChild(m_pShadow);
		m_pShadow = NULL;
	}
}

void Role::SetPos( const Vector3& pos )
{
	m_space.SetPos(pos);
}

const Vector3& Role::GetPos()
{
	return m_space.GetPos();
}

void Role::SetYaw( float angle,bool bShade)
{
	if(bShade)
		m_rotaterYaw.Start(angle);
	else
		m_space.SetYaw(angle);
}

void Role::SetPitch( float angle,bool bShade)
{
	if(bShade)
		m_rotaterPitch.Start(angle);
	else
		m_space.SetPitch(angle);
}

float Role::GetYaw() const
{
	return m_space.GetYaw();
}

float Role::GetDestYaw()
{
	if(m_rotaterYaw.IsRotating())
		return m_rotaterYaw.GetDestAngle();
	else
		return m_space.GetYaw();
}

void Role::SetScale(const Vector3& scale,bool bShade)
{
	if(bShade)
		m_scaler.Start(scale);
	else
		m_space.SetScale(scale);

    // 坐骑缩放
    if (this->IS_KIND_OF(Player))
    {
        Player* pPlayer = static_cast<Player*>(this);
        Pet* pMount = pPlayer->GetMount();
        if (P_VALID(pMount))
        {
            pMount->SetCurrScale(pPlayer->GetDesScale());
        }
    }
}

const Vector3& Role::GetDesScale() const
{
	if(m_scaler.IsScaling())
		return m_scaler.GetDestScale();
	else
		return m_space.GetScale();
}

float Role::GetPitch() const
{
	return m_space.GetPitch();
}

float Role::GetMoveSpeed(BOOL bIsMount)
{
	if( bIsMount == TRUE )
		return X_DEF_XZ_SPEED*(float(m_roledata[ERA_Speed_Mount])/10000.0f);
	else
		return X_DEF_XZ_SPEED*(float(m_roledata[ERA_Speed_XZ])/10000.0f);
}

float Role::GetDropXzSpeed()
{
	return X_DEF_DROP_XZ_SPEED;
}

float Role::GetJumpYSpeed()
{
	return X_DEF_Y_SPEED*(float(m_roledata[ERA_Speed_Y])/10000.0f);
}

float Role::GetSwimSpeed()
{
	return X_DEF_XZ_SPEED*(float(m_roledata[ERA_Speed_Swim])/10000.0f);
}

float Role::GetHitFlyXzSpeed()
{
	return X_DEF_HIT_FLY_XZ_SPEED;
}

float Role::GetHitFlyYSpeed()
{
	return X_DEF_HIT_FLY_Y_SPEED;
}

void Role::SetAttribute(ERoleAttribute eType, INT nValue, bool bInit)
{
	m_roledata[eType] = nValue;

	// 处理体形变化
	if( ERA_Shape == eType )
	{
		float fScale = nValue / 10000.0f;
		SetScale( Vector3( fScale, fScale, fScale ), !bInit );
	}

	// 发送游戏事件
	if(m_id == RoleMgr::Inst()->GetLocalPlayerID())
	{
		tagUpdateLPAttEvent event(_T("tagUpdateLPAttEvent"),NULL);
		event.eType = eType;
		TObjRef<GameFrameMgr>()->SendEvent(&event);

		if(ERA_Morality == eType)
		{
			if( GameSet::Inst()->Get( EGS_MoralityWarning ) && m_roledata[eType] <= PK_STATE_MORALITY[ERPKS_Peace] )
			{
				if( P_VALID(GetObj("MoralityWarning")) )
					KillObj("MoralityWarning");
				CreateObj("MoralityWarning", "MsgBoxEx");

				TObjRef<MsgBoxEx>("MoralityWarning")->Init( _T(""), g_StrTable[_T("MoralityWarning")]);
				GameSet::Inst()->Set( EGS_MoralityWarning, 0 );
				GameSet::Inst()->SaveMorality();
			}
		}
	}
	else
	{
		tagUpdateRRAttEvent event(_T("tagUpdateRRAttEvent"),NULL);
		event.dwRoleID = m_id;
		event.eType = eType;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}
}

void Role::SetRemoteRoleAtt(ERemoteRoleAtt eType, INT nValue, bool bInit)
{
	ERoleAttribute eAtt;
	switch (eType)
	{
	case ERRA_MaxHP:
		eAtt = ERA_MaxHP;
		break;
	case ERRA_HP:
		eAtt = ERA_HP;
		break;
	case ERRA_MaxMP:
		eAtt = ERA_MaxMP;
		break;
	case ERRA_MP:
		eAtt = ERA_MP;
		break;
	case ERRA_Rage:
		eAtt = ERA_Rage;
		break;
	case ERRA_Speed_XZ:
		eAtt = ERA_Speed_XZ;
		break;
	case ERRA_Speed_Y:
		eAtt = ERA_Speed_Y;
		break;
	case ERRA_Speed_Swim:
		eAtt = ERA_Speed_Swim;
		break;
	case ERRA_Speed_Mount:
		eAtt = ERA_Speed_Mount;
		break;
	case ERRA_Shape:
		eAtt = ERA_Shape;
		break;
	case ERRA_Hostility:
		eAtt = ERA_Hostility;
		break;
	default:
		eAtt = ERA_Null;
	}

	SetAttribute(eAtt, nValue, bInit);
}

void Role::SetRoleLevel(INT nLevel, BOOL bLevelUp)
{
	INT nOldLevel = m_nRoleLevel;
	m_nRoleLevel = nLevel;

	//发送更新事件
	if(m_id == RoleMgr::Inst()->GetLocalPlayerID())
	{
		tagUpdateLPLevel event(_T("tagUpdateLPLevel"),NULL);
		event.nCurLevel = nLevel;
		event.nInc = nLevel - nOldLevel;
		event.bLevelUp = bLevelUp;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}
	else
	{
		tagUpdateRRLevel event(_T("tagUpdateRRLevel"),NULL);
		event.dwRoleID = m_id;
		event.nCurLevel = nLevel;
		event.bLevelUp = bLevelUp;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}

	// 播放升级特效
	if( m_nRoleLevel > nOldLevel && nOldLevel > 0 )
		EffectMgr::Inst()->PlayRoleEffect( m_id, _T("Txxy01") );
}

void Role::SetRoleName( tstring strName )
{
	m_strRoleName = strName;

	//发送更新事件
}

VOID Role::AddRoleBuff( tagBuffMsgInfo data )
{
	tagBuffProtoClient* pProto = const_cast<tagBuffProtoClient*>(SkillProtoData::Inst()->FindBuffProto(data.dwBuffTypeID));
	ASSERT(P_VALID(pProto));

	// 瞬时BUFF不加入列表
	if( !P_VALID(pProto) )
		return;
	if( pProto->bInstant )
		return;

	tagRoleBuff* pBuff = new tagRoleBuff;
	pBuff->dwBuffTypeID = data.dwBuffTypeID;
	pBuff->nPersistTimeLeft = data.nPersistTimeLeft;
	pBuff->nMaxPersistTime = data.nMaxPersistTime;
	pBuff->nWarpTimes = data.nWarpTimes;
	pBuff->pBuffProto = pProto;
	pBuff->dwStartTime = Kernel::Inst()->GetAccumTimeDW();
	m_listRoleBuff.PushBack(pBuff);

	SendBuffChangeEvent();
}

VOID Role::RemoveRoleBuff( DWORD dwBuffID )
{
	tagRoleBuff* pBuff = NULL;
	m_listRoleBuff.ResetIterator();
	while (m_listRoleBuff.PeekNext(pBuff))
	{
		if(P_VALID(pBuff) && pBuff->dwBuffTypeID == dwBuffID)
		{
			m_listRoleBuff.Erase(pBuff);
			SAFE_DEL(pBuff);
		}
	}

	SendBuffChangeEvent();
}

VOID Role::UpdateRoleBuff( tagBuffMsgInfo data )
{
	tagRoleBuff* pBuff = NULL;
	m_listRoleBuff.ResetIterator();
	while (m_listRoleBuff.PeekNext(pBuff))
	{
		if(P_VALID(pBuff) && pBuff->dwBuffTypeID == data.dwBuffTypeID)
		{
			pBuff->nPersistTimeLeft = data.nPersistTimeLeft;
			pBuff->nMaxPersistTime = data.nMaxPersistTime;
			pBuff->nWarpTimes = data.nWarpTimes;
			pBuff->dwStartTime = Kernel::Inst()->GetAccumTimeDW();
		}
	}

	SendBuffChangeEvent();
}

VOID Role::FreeRoleBuff()
{
	tagRoleBuff* pBuff = NULL;
	m_listRoleBuff.ResetIterator();
	while(m_listRoleBuff.PeekNext(pBuff))
	{
		if(P_VALID(pBuff))
		{
			SAFE_DEL(pBuff);
		}
	}
	m_listRoleBuff.Clear();

}

VOID Role::SendBuffChangeEvent()
{
	// 发送buff改变游戏事件
	tagRoleBuffChangeEvent event(_T("tagRoleBuffChangeEvent"), NULL);
	event.dwRoleID = m_id;
	TObjRef<GameFrameMgr>()->SendEvent(&event);
}

tagRoleBuff* Role::FindBuff( DWORD dwBuffID )
{
	tagRoleBuff* pBuff=NULL;
	m_listRoleBuff.ResetIterator();
	while(m_listRoleBuff.PeekNext(pBuff))
	{
		if((pBuff->dwBuffTypeID/100)==dwBuffID)
			return pBuff;
	}
	return NULL;
}

void Role::SetState( EState state )
{
	m_state.SetFlag(state);
}

void Role::UnSetState( EState state )
{
	m_state.UnsetFlag(state);
}

BOOL Role::GetBoneSpace( const TCHAR* szBoneName, Matrix4& matOut )
{
	SceneNode* pSGNode = GetSceneNode();
	if( NULL == pSGNode )
		return FALSE;

	if( NULL == szBoneName || _T('\0') == szBoneName[0] )
		return FALSE;

	if( pSGNode->IS_KIND_OF(AvatarNode) )
	{
		AvatarNode* pAvatarNode = (AvatarNode*)pSGNode;
		ResSkeleton *pSkel=const_cast<ResSkeleton *>(pAvatarNode->GetResource());
		if( NULL == pSkel || !pSkel->IsCreated() )
			return FALSE;

		const int nBoneID = pSkel->GetBoneID( szBoneName );
		if( -1 == nBoneID )
			return FALSE;

		matOut = *pAvatarNode->GetSkeletonAniCtrl()->GetBoneSpace( nBoneID );
		matOut *= pAvatarNode->GetWorldMat();
		return TRUE;
	}
	else if( pSGNode->IS_KIND_OF(EntityNode) )
	{
		EntityNode* pEntityNode = (EntityNode*)pSGNode;
		ResRenderable* pRes = pEntityNode->GetResObj();
		if( NULL == pRes || !pRes->IsCreated() )
			return FALSE;

		if( !pRes->IS_STATIC_CLASS(ResSkeleton) )
			return FALSE;

		ResSkeleton *pSkel=(ResSkeleton*)pRes;

		const int nBoneID = pSkel->GetBoneID( szBoneName );
		if( -1 == nBoneID )
			return FALSE;

		NodeAnimationCtrl* pAni = (NodeAnimationCtrl*)pEntityNode->GetAniCtrl();
		matOut = *pAni->GetBoneSpace( nBoneID );
		matOut *= pEntityNode->GetWorldMat();
		return TRUE;
	}
	return FALSE;
}

void Role::HandleMsgCode(AnimationCtrl* pAniCtrl)
{
	m_vecMsgCode.clear();
	if( NULL == pAniCtrl )
		return;
	DWORD dwMsgCode = pAniCtrl->PeekFrameMsg();
	while( 0 != dwMsgCode )
	{
		OnMsgCode( dwMsgCode );
		m_vecMsgCode.push_back( dwMsgCode );
		dwMsgCode=pAniCtrl->PeekFrameMsg();
	}
}

void Role::SetMulColor(const float r, const float g, const float b)
{
	SceneNode* pSGNode = GetSceneNode();
	if( NULL == pSGNode )
		return;

	if(pSGNode->IS_KIND_OF(AvatarNode))
	{
		((AvatarNode*)pSGNode)->SetMulColor(r,g,b);
		m_vMulColor = Vector3(r,g,b);
	}
	else if(pSGNode->IS_KIND_OF(EntityNode))
	{
		((EntityNode*)pSGNode)->SetMulColor(r,g,b);
		m_vMulColor = Vector3(r,g,b);
	}
}