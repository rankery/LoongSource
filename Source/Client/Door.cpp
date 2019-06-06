#include "StdAfx.h"
#include "Door.h"
#include "MapMgr.h"
#include "..\worlddefine\role_att.h"
#include "..\WorldDefine\creature_define.h"
#include "CreatureData.h"


IMP_RTTI(Door,NPC);

Door::Door(void)
{
	m_dwMapObjID=GT_INVALID;
}

Door::~Door(void)
{
}

void Door::InitFSM()
{
	//no FSM
}

void Door::OnNetCmd( tagNetCmd* pNetCmd )
{
	if(pNetCmd->dwID==pNetCmd->Crc32("NS_OpenDoor"))//开门
	{
		OpenDoor(true);		
	}
	else if(pNetCmd->dwID==pNetCmd->Crc32("NS_CloseDoor"))//关门
	{
		CloseDoor(true);
	}
}

void Door::OnGameEvent( tagGameEvent* pGameEvent )
{}



bool Door::IsDead()
{
	//return m_bState;
	return false;
}

void Door::InitAttribute( tagNS_GetRemoteCreatureState* pCmd )
{
	// 确保SeneneNode已经创建
	ASSERT( P_VALID( m_pEntityNode ) );

	// 设置角色位置、缩放和朝向
	const tagMapDoor* pMapDoor=MapMgr::Inst()->GetGameMap().FindMapDoor(pCmd->CreatureData.dwMapObjID);
	ASSERT( P_VALID( pMapDoor ) );

	Vector3 vScale(pMapDoor->fScale[0], pMapDoor->fScale[1], pMapDoor->fScale[2]);

	SetPos(pMapDoor->vPos);
	SetYaw(pMapDoor->fYaw);
	m_space.SetScale(vScale);

	// 记录地图对象ID
	m_dwMapObjID = pCmd->CreatureData.dwMapObjID;

	// 设置角色属性
	INT i;
	for (i=0; i<ERRA_End; ++i)
	{
		// 如果是shape，不处理
		if ((ERemoteRoleAtt)i != ERRA_Shape)
			SetRemoteRoleAtt((ERemoteRoleAtt)i, pCmd->CreatureData.nAtt[i], true);			
	}
	m_dwTypeID = pCmd->CreatureData.dwTypeID;

	// 设置NPC类型等
	const tagCreatureProto* pProto = CreatureData::Inst()->FindNpcAtt( m_dwTypeID );
	m_pProto = (tagCreatureProto*)pProto;

	// 设置角色等级
	SetRoleLevel(pCmd->CreatureData.nLevel);

	// 设置角色名称
	SetRoleName(pProto->szName);

	// 设置角色状态
	m_state.Init(pCmd->CreatureData.dwState);

	// 添加buff列表
	FreeRoleBuff();
	for(INT j=0; j<pCmd->CreatureData.nBuffNum; ++j)
	{
		AddRoleBuff(pCmd->CreatureData.Buff[j]);
	}

	// 设置初始状态
	if(pCmd->CreatureData.bOpen)
	{
		OpenDoor(false);
	}
	else
	{
		CloseDoor(false);
	}

	//--设置世界矩阵
	if(NULL!=m_pEntityNode )
		m_pEntityNode->SetWorldMat(m_space.GetMatrix());

	m_bGetAttribute=true;

	if (pCmd->CreatureData.bOpen == TRUE)
	{
		MapMgr::Inst()->GetNavMap()->GetCollider()->EnableDoorCollide(m_dwMapObjID,false);
	}
	else
	{
		MapMgr::Inst()->GetNavMap()->GetCollider()->EnableDoorCollide(m_dwMapObjID,true);
	}


}

void Door::OpenDoor(bool bPlayAction)
{
	m_bState = true;
	//--关闭碰撞
	MapMgr::Inst()->GetNavMap()->GetCollider()->EnableDoorCollide(m_dwMapObjID,false);

	AnimationCtrl *pAniCtrl = GetAniCtrl();
	if (NULL == pAniCtrl)
		return;
	//--播放动画
	if(bPlayAction)
		pAniCtrl->OpenTrack(_T("open"),false,0.0f, 0.0f);
	else
	{
		pAniCtrl->OpenTrack(_T("open"),false,2.0f, 0.0f);
	}
}

void Door::CloseDoor(bool bPlayAction)
{
	m_bState = false;
	//--开启碰撞
	MapMgr::Inst()->GetNavMap()->GetCollider()->EnableDoorCollide(m_dwMapObjID,true);

	AnimationCtrl *pAniCtrl = GetAniCtrl();
	if (NULL == pAniCtrl)
		return;
	//--播放动画
	if(bPlayAction)
	{
		pAniCtrl->OpenTrack(_T("clos"),false,0.0f, 0.0f);
	}
	else
	{
		pAniCtrl->OpenTrack(_T("clos"),false,2.0f, 0.0f);
	}
}