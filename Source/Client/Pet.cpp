#include "StdAfx.h"
#include "Pet.h"
#include "FSM_PET.h"
#include "PetProtoData.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\move_define.h"
#include "CreatureData.h"

IMP_RTTI(Pet,NPC);

Pet::Pet(void) :
    m_pFSM_PET(NULL),
    m_pPetProto(NULL),
    m_dwTypeID(GT_INVALID),
    m_dwOwnerID(GT_INVALID),
    m_bFSMEnabled(true),
    m_bInitialized(false),
    m_nRidersCount(0),
    m_vCurrScale(1.0f, 1.0f, 1.0f),
    m_bPitch(true)
{   
    // 临时
    m_dwTypeID = 0;

    // 伪造NPC原型数据
    m_pProto = new tagCreatureProto;
    m_pProto->eType = ECT_Pet;

    // 清空状态
    m_uPetState.byPetState      =   0;
}

Pet::~Pet(void)
{
    // 释放NPC原型数据
    SAFE_DELETE(m_pProto);

    // 释放状态机
    SAFE_DELETE(m_pFSM_PET);
}

void Pet::InitFSM()
{
    SAFE_DELETE(m_pFSM_PET);
    m_pFSM_PET = new FSM_PET(this);
    m_pFSM_PET->Init();
}

void Pet::Update()
{
    if (m_bFSMEnabled)
	{
		m_pFSM_PET->Update();
		NPC::Update();
	}
    else
    {
        // 骑乘状态防止npc远程消息改变缩放
        if (P_VALID(m_pPetProto))
            SetScale(m_pPetProto->scalePercent);
    }
}

void Pet::OnNetCmd( tagNetCmd* pNetCmd )
{
    if (m_bFSMEnabled)
        m_pFSM_PET->OnNetCmd(pNetCmd);
}

void Pet::OnGameEvent( tagGameEvent* pGameEvent )
{
    if (m_bFSMEnabled)
        m_pFSM_PET->OnGameEvent(pGameEvent);
}

Cool3D::Vector3 Pet::GetRoleSize()
{
    // 对于骑乘模型，返回碰撞包围盒大小
    if (m_bInitialized && P_VALID(m_pPetProto))
    {
        return m_pPetProto->boundingBox * m_pPetProto->scalePercent;
    }

    return Vector3(X_DEF_CREATURE_SIZE_X / 2,X_DEF_CREATURE_SIZE_Y / 2,X_DEF_CREATURE_SIZE_Z / 2);
}

bool Pet::IsDead()
{
    return false;
}

bool Pet::IsAttributeInited() const
{
    return m_bInitialized;
}

void Pet::InitPetAttribute( tagNS_GetRemotePetState* pCmd )
{
    m_uPetState     =   pCmd->PetData.uState;
    m_dwTypeID      =   pCmd->PetData.dwProtoID;
    m_dwOwnerID     =   pCmd->PetData.dwRoleID;

    m_pPetProto     =   const_cast<tagPetProto*>(PetProtoData::Inst()->FindPetProto(m_dwTypeID));

	if( m_bFSMEnabled )
		m_pMdlData = (tagCreatureMdlData*)CreatureData::Inst()->FindMdlData( m_pPetProto->strModelName[0].c_str() );
	else
		m_pMdlData = (tagCreatureMdlData*)CreatureData::Inst()->FindMdlData( m_pPetProto->strModelName[1].c_str() );

    m_bInitialized  =   true;
}

void Pet::DeleteSceneNode( SceneGraph* pSG )
{
    ASSERT(pSG!=NULL);
    if (NULL == pSG)
        return;

    ASSERT(m_pEntityNode!=NULL);
    if (m_pEntityNode != NULL)
    {
        pSG->DetachDynamicNode(m_pEntityNode);
        SAFE_DELETE(m_pEntityNode);
    }
}

void Pet::PlayDrivingTrack( EPetStus eStus )
{
    switch (eStus)
    {
    case EPS_Followed:
        {
            //do nothing,由状态机管理
        }
    	break;
    case EPS_RidingMove:
        {
            LPCTSTR szTrackName=CombatSysUtil::Inst()->GetNPCMoveTrackName(GetPetSpeed());
            PlayTrack(szTrackName, true);
        }
        break;
    case EPS_RidingIdle:
        {
            PlayTrack(_T("std"), true);
        }
    case EPS_Flying:
        {
            //to do something
        }
        break;
    default:
        {

        }
        break;
    }
}

float Pet::GetPetSpeed()
{
    // 根据宠物id通过宠物管理器返回宠物速度
    // temp,to do more
    return 150;
}

EPetModelType Pet::GetCurrMdlType()
{
    if (m_uPetState.bisRiding)
        return EPMT_Mount;
    else if (m_uPetState.bisCalled)
        return EPMT_Follow;
    else
        return EPMT_Follow;
}

void Pet::SetScale(float scale)
{
     m_space.SetScale(scale * m_vCurrScale);
}