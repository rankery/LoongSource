#include "StdAfx.h"
#include "StateIdle_NPC.h"
#include "FSM_NPC.h"
#include "NPC.h"
#include "CombatEvent.h"

StateIdle_NPC::StateIdle_NPC(void)
{
}

StateIdle_NPC::~StateIdle_NPC(void)
{
}

void StateIdle_NPC::Active( tagNetCmd* pNetCmd,NavMap* pNav,NPC* pRole )
{
	if(PlayBuffAni(pRole))
	{
		m_step=EStep_Buff;
	}
	else
	{
		pRole->PlayTrack(_T("std"),true);
		m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
		m_step=EStep_Stand;
	}
}

void StateIdle_NPC::Update(NavMap* pNav,NPC* pRole)
{
	//--站立状态-------------------------------------------------------------
	if(m_step==EStep_Stand)
	{
		if( pRole->IsAttributeInited() && ( pRole->IsMonster() || pRole->IsNPC() ) )
		{
			DWORD intervalTime=30000+(rand()%20000);

			if(Kernel::Inst()->GetAccumTimeDW()-m_lastPlayEaseTime>=intervalTime)
			{
				m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
				pRole->PlayTrack(_T("lei"),false);
				m_step=EStep_Ease;
			}
		}
	}
	//--悠闲动作--------------------------------------------------------------
	else if(m_step==EStep_Ease)
	{
		if(pRole->IsTrackEnd(_T("lei")))
		{
			pRole->PlayTrack(_T("std"),true);
			m_step=EStep_Stand;
		}
	}
	//--Buff动作--------------------------------------------------------------
	else if(m_step==EStep_Buff)
	{
	}
}

void StateIdle_NPC::OnNetCmd( tagNetCmd* pNetCmd )
{
	m_pFSM->Change2State(pNetCmd);
}

void StateIdle_NPC::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagRoleBuffChangeEvent"))//增加、删除、更新Buff
	{
		NPC* pRole=m_pFSM->GetNPC();

		if(m_step==EStep_Stand
			||m_step==EStep_Ease)
		{
			if(PlayBuffAni(pRole))
			{
				m_step=EStep_Buff;
			}
		}
		else if(m_step==EStep_Buff)
		{
			if(!PlayBuffAni(pRole))
			{
				pRole->PlayTrack(_T("std"),true);
				m_lastPlayEaseTime=Kernel::Inst()->GetAccumTimeDW();
				m_step=EStep_Stand;
			}
		}
	}
	else
		m_pFSM->Change2State(pEvent);
}

bool StateIdle_NPC::PlayBuffAni( NPC* pRole )
{
	list<tagRoleBuff*>& buffList=pRole->GetBuffList().GetList();

	for(list<tagRoleBuff*>::const_reverse_iterator iter=buffList.rbegin();
		iter!=buffList.rend();++iter)
	{
		tagRoleBuff* pBuff=*iter;
		if(pBuff->pBuffProto->szActionMod[0]!=_T('\0'))
		{
			pRole->PlayTrack(pBuff->pBuffProto->szActionMod,true,true);
			return true;
		}
	}

	return false;
}