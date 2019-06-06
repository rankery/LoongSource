#include "StdAfx.h"
#include "PreState.h"
#include "FSM_LP.h"
#include "LocalPlayer.h"


tagPreState::tagPreState():state(EState_None)
{}

void tagPreState::Change2LowerState( FSM_LP* pFSM,LocalPlayer* pRole,bool bSwim,bool bOnWater )
{
	switch(state)
	{
	case EState_MouseMove:
		{
			mouseMove.start=pRole->GetPos();
			
			if(bOnWater)
			{
				pFSM->Change2LowerState(FSM_LP::ELowerState_MouseWaterWalk,&mouseMove);
			}
			else if(bSwim)
			{
				pFSM->Change2LowerState(FSM_LP::ELowerState_MouseSwim,&mouseMove);
			}
			else
			{
				pFSM->Change2LowerState(FSM_LP::ELowerState_MouseMove,&mouseMove);
			}
		}
		break;
	case EState_KeyboardMove:
		{
			keyboardMove.start=pRole->GetPos();

			if(bOnWater)
			{
				pFSM->Change2LowerState(FSM_LP::ELowerState_KeyboardWaterWalk,&keyboardMove);
			}
			else if(bSwim)
			{
				pFSM->Change2LowerState(FSM_LP::ELowerState_KeyboardSwim,&keyboardMove);
			}
			else
			{
				pFSM->Change2LowerState(FSM_LP::ELowerState_KeyboardMove,&keyboardMove);
			}
		}
		break;
	case EState_Pursue:
		{
			pursue.start=pRole->GetPos();

			pFSM->Change2LowerState(FSM_LP::ELowerState_Pursue,&pursue);
		}
		break;
	default:
		ASSERT(0);
	}
}

bool tagPreState::IsMoving()
{
	if(state==EState_None)
		return false;
	return true;
}
