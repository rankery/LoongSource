#include "StdAfx.h"
#include "NPC.h"
#include "FSM_NPC.h"
#include "MapMgr.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\creature_define.h"
#include "..\WorldDefine\move_define.h"
#include "..\WorldDefine\msg_combat.h"
#include "CreatureData.h"
#include "EffectMgr.h"
#include "QuestMgr.h"
#include "..\Cool3D\Util\CSVFile.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "TeamSys.h"
#include "Pet.h"

IMP_RTTI(NPC,Role);

NPC::NPC(void)
: m_pEntityNode(0)
, m_pFSM(0)
, m_dwTypeID(GT_INVALID)
, m_pProto(0)
, m_pMdlData(0)
, m_bGetAttribute(false)
, m_bHide(false)
, m_dwQueryEffectID(GT_INVALID)
, m_originYaw(0.0f)
, m_lastYaw(0.0f)
, m_lastPitch(0.0f)
, m_dwTaggedOwnerID(GT_INVALID)
{
}

NPC::~NPC(void)
{
	SAFE_DELETE(m_pFSM);
	DetachShadow();
	SAFE_DELETE(m_pEntityNode);
}

void NPC::InitFSM()
{
	SAFE_DELETE(m_pFSM);
	m_pFSM=new FSM_NPC(this);
	m_pFSM->Init();
}

void NPC::AttachSceneNodeToSG( SceneGraph* pSG )
{
	ASSERT(m_pEntityNode!=NULL);
	ASSERT(pSG!=NULL);
	pSG->AttachDynamicNode(m_pEntityNode);
}

void NPC::DetachSceneNodeFromSG( SceneGraph* pSG )
{
	ASSERT(m_pEntityNode!=NULL);
	ASSERT(pSG!=NULL);
	pSG->DetachDynamicNode(m_pEntityNode);
}

Vector3 NPC::GetRoleSize()
{
	if(!m_bGetAttribute)
		return Vector3(X_DEF_CREATURE_SIZE_X,X_DEF_CREATURE_SIZE_Y,X_DEF_CREATURE_SIZE_Z);

	Vector3 roleSize;
	roleSize=m_pProto->vSize;
	roleSize*=(float(m_roledata[ERA_Shape])/10000.0f);
	return roleSize;
}

void NPC::LoadEntity( const TCHAR* szFileName )
{
	ASSERT(m_pEntityNode==NULL);//防止重复加载
	m_pEntityNode = new EntityNode;
	m_pEntityNode->LoadRes( szFileName );

	m_pEntityNode->Hide( m_bHide );
}

void NPC::LoadSkin( const TCHAR* szFileName )
{
	ASSERT(m_pEntityNode!=NULL);

	SGSkinNode *pNewSkin = new SGSkinNode;
	pNewSkin->LoadRes( szFileName );
	m_pEntityNode->AttachChild( pNewSkin );
}

void NPC::UpdateBoundingBox( void )
{
	ASSERT(m_pEntityNode!=NULL);

	m_pEntityNode->UpdateBoundingBox();
}

void NPC::Update()
{
	//--处理消息点
	AnimationCtrl* pAniCtrl = GetAniCtrl();
	HandleMsgCode(pAniCtrl);

	//--update rotater
	m_rotaterYaw.Update();
	m_rotaterPitch.Update();
	m_upperRotater.Update();
	m_headRotater.Update();
	m_scaler.Update();

	//--update FSM
	if(m_pFSM!=NULL)
		m_pFSM->Update();

	//--updata pitch
	if( NULL != m_pMdlData && m_pMdlData->bCanPitch )
    {
        if (this->IS_KIND_OF(Pet))
        {
            Pet* pPet = (Pet*)this;
            if (pPet->bAllowPitch())
            {
                UpdataPitch();
            }
        }
        else
        {
		    UpdataPitch();
        }
    }

	//--update scenenode world matrix
	if( NULL != m_pEntityNode )
		m_pEntityNode->SetWorldMat(m_space.GetMatrix());
}

void NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->OnNetCmd(pNetCmd);
}

void NPC::OnGameEvent( tagGameEvent* pGameEvent )
{
	m_pFSM->OnGameEvent(pGameEvent);
}

AnimationCtrl* NPC::GetAniCtrl()
{
	if(NULL==m_pEntityNode)
		return NULL;
	return m_pEntityNode->GetAniCtrl();
}

void NPC::PlayTrack( const TCHAR* szTrackName,bool loop,bool replay,float timePos )
{
	AnimationCtrl* pAniCtrl=GetAniCtrl();
	if(NULL==pAniCtrl)
		return;

	if(replay
		||m_curTrack!=szTrackName)
	{
		m_curTrack=szTrackName;
		pAniCtrl->OpenTrack(szTrackName,loop,timePos,0.2f);
	}
}

bool NPC::IsTrackEnd( const TCHAR* szTrackName )
{
	AnimationCtrl* pAniCtrl=GetAniCtrl();
	if(NULL==pAniCtrl)
		return false;

	return pAniCtrl->IsTrackEnd(szTrackName);
}

void NPC::GetHeadPos( Vector3& out )
{
	out=m_space.GetPos();
	if(P_VALID(m_pEntityNode))
	{
		if( m_dwTypeID == 1500022 )
		{
			IMSG( _T("NPC: %u \n"), m_dwTypeID );
		}
		AABBox box;
		m_pEntityNode->GetBox(box);
		DWORD bunsBoneIndex=-1;

		const ResRenderable* pRes = m_pEntityNode->GetResObj();
		//是骨骼动画
		if( pRes->IS_STATIC_CLASS(ResSkeleton) )
		{
			ResSkeleton *pSkel = dynamic_cast<ResSkeleton*>(m_pEntityNode->GetResObj());
			if( pSkel!=NULL && pSkel->IsCreated() )
			{
				const tstring& strHeadBone = CreatureData::Inst()->FindNpcBoneName(m_dwTypeID, _T("tag_Hat"));
				//有头顶骨骼
				if( !strHeadBone.empty() )
				{
					bunsBoneIndex = pSkel->GetBoneID(strHeadBone.c_str());
					if (bunsBoneIndex != -1)
					{
						NodeAnimationCtrl *pAniCtrl = dynamic_cast<NodeAnimationCtrl*>(m_pEntityNode->GetAniCtrl());
						if(pAniCtrl != NULL)
						{
							const Matrix4* pMatrix4 = pAniCtrl->GetBoneSpace(bunsBoneIndex);
							Vector3 vec3(pMatrix4->_41, pMatrix4->_42, pMatrix4->_43);
							out = vec3 * m_pEntityNode->GetWorldMat();
						}
						else
						{
							out.y = box.max.y;
						}
					}
					else
					{
						out.y = box.max.y;
					}
				}
				//没有头顶骨骼
				else
				{
					out.y=box.max.y;
				}
			}
		}
		else
			out.y = box.max.y;
	}
	else
	{
		out.y += 180;
	}
}

bool NPC::IsNPC()
{
	ASSERT( P_VALID( m_pProto ) );
    if ( P_VALID( m_pProto ) )
	    return ( ECT_NPC == m_pProto->eType );

    return false;
}

bool NPC::IsPet()
{
    ASSERT(P_VALID(m_pProto));
    if ( P_VALID( m_pProto ) )
        return (ECT_Pet == m_pProto->eType);

    return false;
}

bool NPC::IsMonster()
{
	ASSERT( P_VALID( m_pProto ) );
    if ( P_VALID( m_pProto ) )
	    return ( ECT_Monster == m_pProto->eType );

    return false;
}

bool NPC::IsGather()
{
	ASSERT( P_VALID( m_pProto ) );
    if ( P_VALID( m_pProto ) )
	    return ( ECT_GameObject == m_pProto->eType && EGOT_Gather == m_pProto->nType2 );

    return false;
}

bool NPC::IsChest()
{
	ASSERT( P_VALID( m_pProto ) );
    if ( P_VALID( m_pProto ) )
	    return ( ECT_GameObject == m_pProto->eType && EGOT_Chest == m_pProto->nType2 );

    return false;
}

bool NPC::IsNormalGameObject()
{
	ASSERT( P_VALID( m_pProto ) );
    if ( P_VALID( m_pProto ) )
	    return ( ECT_GameObject == m_pProto->eType && EGOT_Normal == m_pProto->nType2 );

    return false;
}

bool NPC::IsCommonInvesGO()
{
	ASSERT( P_VALID( m_pProto ) );
    if ( P_VALID( m_pProto ) )
	    return ( ECT_GameObject == m_pProto->eType && EGOT_CommonInves == m_pProto->nType2 );

    return false;
}

bool NPC::IsQuestInvesGO()
{
	ASSERT( P_VALID( m_pProto ) );
    if ( P_VALID( m_pProto ) )
	    return ( ECT_GameObject == m_pProto->eType && EGOT_QuestInves == m_pProto->nType2 );

    return false;
}

bool NPC::IsQuestTalkGameObject()
{
	ASSERT( P_VALID( m_pProto ) );
    if ( P_VALID( m_pProto ) )
	    return ( ECT_GameObject == m_pProto->eType && EGOT_QuestTalk == m_pProto->nType2 );

    return false;
}

void NPC::InitAttribute(tagNS_GetRemoteCreatureState* pCmd)
{
	// 确保SeneneNode已经创建
	ASSERT( P_VALID( m_pEntityNode ) );

	// 设置角色位置和朝向
	Vector3 Pos(pCmd->CreatureData.fPos[0], pCmd->CreatureData.fPos[1], pCmd->CreatureData.fPos[2]);
	Vector3 FaceTo(pCmd->CreatureData.fFaceTo[0], pCmd->CreatureData.fFaceTo[1], pCmd->CreatureData.fFaceTo[2]);
	SetPos(Pos);
	SetYaw(CalcYaw(FaceTo));
	m_originYaw = GetYaw();

	// 设置角色属性
	INT i;
	for (i=0; i<ERRA_End; ++i)
	{
		SetRemoteRoleAtt((ERemoteRoleAtt)i, pCmd->CreatureData.nAtt[i], true);			
	}
	m_dwTypeID = pCmd->CreatureData.dwTypeID;

	// 设置NPC类型等
	m_pProto = (tagCreatureProto*)CreatureData::Inst()->FindNpcAtt( m_dwTypeID );
	m_pMdlData = (tagCreatureMdlData*)CreatureData::Inst()->FindMdlData( m_pProto->szModelName );

	// 设置角色等级
	SetRoleLevel(pCmd->CreatureData.nLevel);

	// 设置角色名称
	SetRoleName(m_pProto->szName);

	// 设置角色状态
	m_state.Init(pCmd->CreatureData.dwState);

	// 添加buff列表
	FreeRoleBuff();
	for(INT j=0; j<pCmd->CreatureData.nBuffNum; ++j)
	{
		AddRoleBuff(pCmd->CreatureData.Buff[j]);
	}

	// 给状态机伪造AddBuff消息
	tagNS_AddRoleBuff tmpCmd;
	for(i=0; i<pCmd->CreatureData.nBuffNum; ++i)
	{
		tmpCmd.dwRoleID=GetID();
		tmpCmd.Buff=pCmd->CreatureData.Buff[i];
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

	m_dwTaggedOwnerID = pCmd->CreatureData.dwTaggedOwner;

	m_bGetAttribute=true;

	// 更新任务相关特效显示
	UpdateGameObjectQuestEffect();

	// 播放自身特效
	EffectMgr::Inst()->PlayRoleEffect( m_id, m_pProto->szEffect, NULL, EffectMgr::EEST_XYZ );

	// 挂载特效
	TCHAR szFile[255];
	_stprintf( szFile, _T("data\\animation\\npc\\Effect\\%d.csv"), GetTypeID() );
	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");
	DWORD dwFileSize=pIFS->GetSize( szFile );
	if(dwFileSize>0 && dwFileSize!=GT_INVALID)
	{
		CSVFile csvFile;
		if(csvFile.OpenFile(pIFS,szFile))
		{
			//只有一行
			csvFile.BeginReadLine();
			const vector<tstring>& line = csvFile.GetNextLine();
			int numToken=line.size();
			for(int i=0;i<numToken;i+=2)
			{
				if(i+1<numToken)
				{
					EffectMgr::Inst()->PlayRoleEffect( GetID(), line[i+1].c_str(), line[i].c_str(), EffectMgr::EEST_RoleScale, NULL );
				}
			}
			csvFile.CloseFile();
		}
	}	
}

bool NPC::IsDead()
{
	return m_pFSM->IsDeadState();
}

bool NPC::IsAttributeInited() const
{
	return m_bGetAttribute;
}

void NPC::Hide( const bool bHide )
{
	if( bHide == m_bHide )
		return;
	m_bHide = bHide;
	if( P_VALID( m_pEntityNode ) )
		m_pEntityNode->Hide( m_bHide );
}

void NPC::SetTransparent( float alpha )
{
	if( P_VALID( m_pEntityNode ) )
		m_pEntityNode->SetTransparent(alpha);
	m_fTransparent=alpha;
}

void NPC::UpdateGameObjectQuestEffect()
{
	if( !m_bGetAttribute )
		return;

	if( ( IsQuestInvesGO() && QuestMgr::Inst()->GetQuery()->IsInvesObject( m_dwTypeID ) ) ||
	    ( IsQuestTalkGameObject() && QuestMgr::Inst()->GetQuery()->IsUpdateNPC( m_dwTypeID ) ) 
		|| IsChest() && QuestMgr::Inst()->GetQuery()->IsUpdateNPC( m_dwTypeID ) )
	{
		if( GT_INVALID == m_dwQueryEffectID )
		{
			m_dwQueryEffectID = EffectMgr::Inst()->PlayRoleEffect( m_id, _T("txqt08"), NULL, EffectMgr::EEST_XYZ, _T("combat"), true );
		}
	}
	else
	{
		if( GT_INVALID != m_dwQueryEffectID )
		{
			EffectMgr::Inst()->StopEffect( m_dwQueryEffectID );
			m_dwQueryEffectID = GT_INVALID;
		}
	}
}

void NPC::UpdataPitch()
{
	if( NULL == m_pEntityNode )
		return;

	const AABBox& boxNode = m_pEntityNode->GetLocalBox();
	Vector3 vSize = boxNode.max - boxNode.min;

	NavMap* pNav = MapMgr::Inst()->GetNavMap();
	NavCollider* pCollider = pNav->GetCollider();

	Vector3 vCheckPos = Vector3( 0.0f, vSize.y, -max( vSize.z * 0.25f, 51.0f ) );
	Transform trans;
	trans.Scale( m_space.GetScale().x, m_space.GetScale().y, m_space.GetScale().z );
	trans.Rotate( m_space.GetYaw(), 0.0f, 0.0f );
	trans.Translate( m_space.GetPos() );
	trans.TransVec3( vCheckPos );

	AABBox box;
	box.min = Vector3( vCheckPos.x - 0.5f, -1.0e+6f, vCheckPos.z - 0.5f );
	box.max = Vector3( vCheckPos.x + 0.5f, vCheckPos.y, vCheckPos.z + 0.5f );

	if( !pCollider->AABBoxCollideBoxTopAndTrn( box, Kernel::Inst()->GetAccumTime(), vCheckPos.y ) )
	{
		TRACE( _T("UpdataPitch.AABBoxCollideBoxTopAndTrn(%f, %f, %f) FAILED!!"), vCheckPos.x, vCheckPos.y, vCheckPos.z );
		return;
	}

	const float fPitch = CalcPitch( vCheckPos - m_space.GetPos() );
	SetPitch( fPitch );
}

VOID NPC::FaceTo( const Vector3& pos, BOOL bFaceRound )
{
	if( m_rotaterYaw.IsRotating() )//可能正在转动过程中，所以要取最终yaw
		m_originYaw = m_rotaterYaw.GetDestAngle();
	else
		m_originYaw = GetYaw();

	/*
		当yaw在一定范围时，转动头部，否则转动身体
		上仰 25 0.436 下看 -0.262 左右 35 0.611f
	*/
	AnimationCtrl *pAniCtrl = GetAniCtrl();
	if( P_VALID(pAniCtrl) && pAniCtrl->IS_STATIC_CLASS(NodeAnimationCtrl) )//只有骨骼动画的NPC才能转动头部
	{
		NodeAnimationCtrl *pNodeAniCtrl = static_cast<NodeAnimationCtrl*>(GetAniCtrl());

		Vector3 npcHead;
		GetHeadPos(npcHead);

		Vector3 dir = pos - npcHead;
		float newYaw = CalcYaw(dir);
		float newPitch = CalcPitch(dir);

		// 设立一个缓冲区，以免在临界点上频繁切换
		if( fabsf(newYaw-m_lastYaw) < 0.0436f )
			return;
		else
			m_lastYaw = newYaw;

		float oldYaw;
		if( m_rotaterYaw.IsRotating() )//可能正在转动过程中，所以要取最终yaw
		{
			oldYaw = m_rotaterYaw.GetDestAngle();
		}
		else
		{
			oldYaw = GetYaw();
		}
		float deltaYaw = newYaw - oldYaw;

		// 获得转动最小的yaw
		if( deltaYaw > 3.14f )
			deltaYaw = -(FLOAT_PI*2.0f - deltaYaw);
		else if( deltaYaw < -3.14f )
			deltaYaw = FLOAT_PI*2.0f + deltaYaw;

		if( fabsf(newPitch-m_lastPitch) < 0.0436f )
			newPitch = m_lastPitch;
		else
			m_lastPitch = newPitch;

		// 限制头部的pitch
		if( newPitch > 0.436f && newPitch < 3.14f )
		{
			newPitch = 0.436f;
		}
		else if( newPitch > 3.14f && newPitch < 6.018f )
		{
			newPitch = 6.018f;
		}

		if( bFaceRound == FALSE )//转动上身
		{
			if( deltaYaw > 0.611f )
				deltaYaw = 0.611f;
			if( deltaYaw < -0.611f )
				deltaYaw = -0.611f;

			m_headRotater.Start( deltaYaw, newPitch );
		}
		else//转动身体
		{
			if( deltaYaw>=-0.611f && deltaYaw<=0.611f )//只需要转动头部
			{
				m_headRotater.Start( deltaYaw, newPitch );
			}
			else
			{
				SetYaw(newYaw);
				m_headRotater.Start( 0.0f, newPitch );
			}
		}
	}
}

VOID NPC::FaceBack( void )
{
	AnimationCtrl *pAniCtrl = GetAniCtrl();
	if( P_VALID(pAniCtrl) )
	{
		m_lastYaw = 0.0f;
		m_lastPitch = 0.0f;
		if( GetAniCtrl()->IS_STATIC_CLASS(NodeAnimationCtrl) )//只有骨骼动画的NPC才能转动头部
		{
			NodeAnimationCtrl *pNodeAniCtrl = static_cast<NodeAnimationCtrl*>(GetAniCtrl());
			m_upperRotater.Start( 0.0f );
			m_headRotater.Start( 0.0f, 0.0f );
		}

		SetYaw(m_originYaw);
	}
}

void NPC::OnMsgCode( const DWORD dwMsgCode )
{
}

bool NPC::IsBelongToLP() const
{
	if( GT_INVALID == m_dwTaggedOwnerID )
		return true;

	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( !P_VALID( pLP ) )
		return false;

	if( pLP->GetID() == m_dwTaggedOwnerID )
		return true;

	const vector<tagTeammateData>& vecTeammates = TeamSys::Inst()->GetTeammateList();
	for( size_t i = 0; i < vecTeammates.size(); i++ )
	{
		const tagTeammateData& data = vecTeammates[i];
		if( data.dwRoleID == m_dwTaggedOwnerID )
			return true;
	}
	return false;
}
