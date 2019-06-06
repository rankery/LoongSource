#include "StdAfx.h"
#include "LowerStateSlide_RP.h"
#include "..\WorldDefine\action.h"
#include "Player.h"
#include "FSM_RP.h"

LowerStateSlide_RP::LowerStateSlide_RP(void)
{}

LowerStateSlide_RP::~LowerStateSlide_RP(void)
{}

void LowerStateSlide_RP::Active( tagNetCmd* pNetCmd,NavMap* pNav,Player* pRole )
{
	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_SyncSlide"));
	tagNS_SyncSlide* pCmd=(tagNS_SyncSlide*)pNetCmd;

	//--³õÊ¼»¯Åö×²¼ì²âÆ÷
	m_collider.Init(pCmd->srcPos,pRole->GetRoleSize());
	m_lastCDTime=Kernel::Inst()->GetAccumTime()-pCmd->curTime;

	//--²¥·Å¶¯×÷
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Slide,AvatarAniMixer::EAP_Lower,_T("jmp2"),true);
}

void LowerStateSlide_RP::Update( NavMap* pNav,Player* pRole )
{
	//--Åö×²¼ì²â
	Vector3 nowPos;
	float endTime;

	ENavResult result=m_collider.Update(pNav->GetCollider(),m_lastCDTime,Kernel::Inst()->GetAccumTime()-m_lastCDTime,nowPos,endTime,pRole->CanWaterWalk());
	m_lastCDTime=Kernel::Inst()->GetAccumTime();

	pRole->SetPos(nowPos);

	//--
	if(result!=ENR_ToBeContinued)
		m_pFSM->Change2LowerState_Stand();
}

void LowerStateSlide_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2LowerState(pNetCmd);
}

void LowerStateSlide_RP::ReplayAni( Player* pRole )
{
	pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Slide,AvatarAniMixer::EAP_Lower,_T("jmp2"),true);
}

bool LowerStateSlide_RP::IfCanChange2UpperState( FSM_RP::EUpperState state )
{
	switch(state)
	{
	case FSM_RP::EUpperState_BeAttack:
		return false;
	}

	return true;
}