#include "StdAfx.h"
#include "StateDead_NPC.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "RoleMgr.h"
#include "CombatSysUtil.h"
#include "EffectMgr.h"

StateDead_NPC::StateDead_NPC(void)
{}

StateDead_NPC::~StateDead_NPC(void)
{}

void StateDead_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
    //--初始化碰撞检测器
    m_collider.Init(pRole->GetPos(), Vector3(0.0f, -1.0f, 0.0f) ,pRole->GetRoleSize(),pRole->GetHitFlyYSpeed(),0.0f);
    m_lastCDTime=Kernel::Inst()->GetAccumTime();

	if(pRole->GetAniCtrl()!=NULL)
	{
		//--播放音效
		CombatSysUtil::Inst()->PlayRoleDeadSound(pRole);

		//--播放动作
		pRole->PlayTrack(_T("dead"),false);
		m_step=EStep_Drop;
	}
	else
	{
		m_startFadeTime=Kernel::Inst()->GetAccumTime();
		m_step=EStep_Fadeout;
	}

	//--隐藏自身绑定的特效
	EffectMgr::Inst()->ShowBindRoleEffect( pRole->GetID(), false );

	//--播放死亡特效
	if( pRole->IsAttributeInited() && pRole->IsMonster() && !EffectMgr::Inst()->IsEffectNumFull() )
	{
		AABBox box;
		pRole->GetSceneNode()->GetBox(box);
		Vector3 size = box.max-box.min;
		EffectMgr::Inst()->PlayEffect( pRole->GetPos(), 0.0f, _T("Txxy07"), &size, EffectMgr::EEST_YOutOnly );
	}
}

void StateDead_NPC::Update(NavMap* pNav,NPC* pRole)
{
    //--碰撞检测
    Vector3 nowPos;
    float endTime;
    DWORD dwCarrierObjID;
    float cdCarrierTime;

    ENavResult result=m_collider.Update(pNav->GetCollider(),m_lastCDTime,Kernel::Inst()->GetAccumTime()-m_lastCDTime,NULL,nowPos,endTime,dwCarrierObjID,cdCarrierTime,false);
    m_lastCDTime=Kernel::Inst()->GetAccumTime();

    pRole->SetPos(nowPos);

    if (m_step == EStep_Drop)
    {
        if(result!=ENR_ToBeContinued)
        {
            m_step=EStep_FallDown;
        }
    }
	else if(m_step==EStep_FallDown)
	{
		//--播放摔倒音效
		const DWORD dwFallMsg = _MakeFourCC(_T("fall"));
		const vector<DWORD>& msgCodes = pRole->GetMsgCodes();
		for( size_t nMsgCode = 0; nMsgCode < msgCodes.size(); nMsgCode++ )
		{
			if( dwFallMsg == msgCodes[nMsgCode] )
			{
				CombatSysUtil::Inst()->PlayFallSound(pRole->GetPos());
			}
		}
		if(pRole->IsTrackEnd(_T("dead")))
		{
			m_step=EStep_Lie;
			m_startLieTime=Kernel::Inst()->GetAccumTime();
		}
	}
	else if(m_step==EStep_Lie)
	{
		if(Kernel::Inst()->GetAccumTime()-m_startLieTime>3.0f)
		{
			m_startFadeTime=Kernel::Inst()->GetAccumTime();
			m_step=EStep_Fadeout;
		}
	}
	else if(m_step==EStep_Fadeout)
	{
		const float FadeOutTime=2.0f;
		float time=Kernel::Inst()->GetAccumTime()-m_startFadeTime;
		float alpha=1.0f-time/FadeOutTime;
		if(alpha<0)alpha=0;
		if(alpha>1)alpha=1;
		pRole->SetTransparent(alpha);
		if(alpha==0)
		{
			pRole->Free();
			m_step=EStep_Closed;
		}
	}
}

void StateDead_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{}