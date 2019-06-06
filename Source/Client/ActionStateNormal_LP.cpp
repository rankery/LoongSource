#include "stdafx.h"
#include "ActionStateNormal_LP.h"
#include "PetEvent.h"
#include "Role.h"
#include "RoleMgr.h"
#include "Pet.h"
#include "LocalPlayer.h"

ActionStateNormal_LP::ActionStateNormal_LP( void )
{}

ActionStateNormal_LP::~ActionStateNormal_LP( void )
{}

void ActionStateNormal_LP::Active( NavMap* pNav,LocalPlayer* pRole )
{
}

void ActionStateNormal_LP::Update( NavMap* pNav,LocalPlayer* pRole )
{
}

void ActionStateNormal_LP::OnGameEvent( tagGameEvent* pEvent )
{
}

void ActionStateNormal_LP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if( pNetCmd->dwID == TObjRef<Util>()->Crc32("NS_Mount") )//骑乘消息
	{
		tagNS_Mount *pMountCmd = static_cast<tagNS_Mount*>(pNetCmd);

		// 向宠物系统发送进入骑乘事件
		tagEnterMount event(_T("tagEnterMount"), NULL);
		event.dwOwnerID = pMountCmd->dwRoleID;
		event.dwPetID = pMountCmd->dwPetID;
		event.dwTypeID = pMountCmd->dwPetTypeID;
		TObjRef<GameFrameMgr>()->SendEvent(&event);

		map<DWORD,Role*>::const_iterator finder = RoleMgr::Inst()->GetRoleMap().find(pMountCmd->dwPetID);
		if( finder != RoleMgr::Inst()->GetRoleMap().end() )
		{
			Role *pRole = finder->second;
			if( pRole->IS_STATIC_CLASS(Pet) )
			{
				Pet *pPet = static_cast<Pet*>(pRole);
				m_pFSM->GetLocalPlayer()->SetMount(pPet);

				m_pFSM->Change2ActionState(EActionState_Ride);
			}
		}
	}
	else if( pNetCmd->dwID == TObjRef<Util>()->Crc32("NS_Mount2") )//搭乘消息
	{
		tagNS_Mount2 *pMountCmd = static_cast<tagNS_Mount2*>(pNetCmd);

		map<DWORD,Role*>::const_iterator finder = RoleMgr::Inst()->GetRoleMap().find(pMountCmd->dwPetID);
		if( finder != RoleMgr::Inst()->GetRoleMap().end() )
		{
			Role *pRole = finder->second;
			if( pRole->IS_STATIC_CLASS(Pet) )
			{
				Pet *pPet = static_cast<Pet*>(pRole);
				m_pFSM->GetLocalPlayer()->SetMount(pPet);

				m_pFSM->Change2ActionState(EActionState_Board);
			}
		}
	}
}

bool ActionStateNormal_LP::IfCanChange2UpperState( FSM_LP::EUpperState state,tagGameEvent* pEvent )
{
	return true;
}

bool ActionStateNormal_LP::IfCanChange2LowerState( FSM_LP::ELowerState state,tagGameEvent* pEvent )
{
	return true;
}