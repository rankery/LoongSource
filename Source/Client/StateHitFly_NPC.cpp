#include "StdAfx.h"
#include "StateHitFly_NPC.h"
#include "CombatEvent.h"
#include "NPC.h"
#include "FSM_NPC.h"
#include "CombatSysUtil.h"
#include "WorldFrame.h"
#include "..\WorldDefine\action.h"


StateHitFly_NPC::StateHitFly_NPC(void)
{}

StateHitFly_NPC::~StateHitFly_NPC(void)
{}

void StateHitFly_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
	ASSERT(pNetCmd->dwID==pNetCmd->Crc32("NS_HitFly"));
	tagNS_HitFly* pFly=(tagNS_HitFly*)pNetCmd;

	m_bCollide=pFly->bCollide;

	//--初始化碰撞器
	m_collider.Init(pFly->curPos,//start pos
		-1.0f*pFly->faceTo,//dir
		pRole->GetRoleSize(),//role size
		pRole->GetHitFlyXzSpeed() / 2,//xz speed
		pRole->GetHitFlyYSpeed(),//y speed
		1200.0f,//g
		60.0f,//time out
		!m_bCollide,//terrain only?
		true);//ignore carrier?

	//--设置朝向
	pRole->SetYaw(CalcYaw(pFly->faceTo));

	//--播放动作
	pRole->PlayTrack(_T("dfly"),false);

	m_step=EStep_Fly;
}

void StateHitFly_NPC::Update(NavMap* pNav,NPC* pRole)
{
	//--击飞中-------------------------------------------------------------------
	if(m_step==EStep_Fly)
	{
		//--碰撞检测
		float deltaTime=Kernel::Inst()->GetDeltaTime();
		float startTime=Kernel::Inst()->GetAccumTime()-deltaTime;

		BitMap* pSpecialCango=NULL;
		if(!m_bCollide)
			pSpecialCango=pNav->GetNPCNavMap()->GetCanGoMap();

		Vector3 nowPos;
		float endTime;
		DWORD dwCarrierObjID;
		ENavResult result=m_collider.Update(pNav->GetCollider(),startTime,deltaTime,pSpecialCango,nowPos,endTime,dwCarrierObjID,false);

		pRole->SetPos(nowPos);

		//--
		if(result!=ENR_ToBeContinued)
		{
			if(result==ENR_Landed)
			{
                if (m_pFSM->bNSDead())
                {
                    m_pFSM->Change2DeadState();
                    return;
                }

				m_step=EStep_Craw;

				pRole->PlayTrack(_T("clim"),false);
			}
			else
			{
                if (m_pFSM->bNSDead())
                    m_pFSM->Change2DeadState();
                else
                    m_pFSM->Change2IdleState();

				return;
			}
		}
	}
	//--爬起中-------------------------------------------------------------------
	if(m_step==EStep_Craw)
	{
		if(pRole->IsTrackEnd(_T("clim")))
		{
            if (m_pFSM->bNSDead())
                m_pFSM->Change2DeadState();
            else
                m_pFSM->Change2IdleState();
		}
	}
}

void StateHitFly_NPC::OnGameEvent( tagGameEvent* pEvent )
{}

void StateHitFly_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2State(pNetCmd);
}