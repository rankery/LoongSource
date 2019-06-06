#include "stdafx.h"
#include "ActionStateNormal_RP.h"
#include "RoleMgr.h"
#include "RoleEvent.h"
#include "Player.h"
#include "PetEvent.h"
#include "Pet.h"

ActionStateNormal_RP::ActionStateNormal_RP( void )
: m_mountID(GT_INVALID)
{}

ActionStateNormal_RP::~ActionStateNormal_RP( void )
{}

void ActionStateNormal_RP::Active( NavMap* pNav,Player* pRole )
{
}

void ActionStateNormal_RP::Update( NavMap* pNav,Player* pRole )
{
	if( m_mountID != GT_INVALID )
	{
		map<DWORD,Role*>::const_iterator finder = RoleMgr::Inst()->GetRoleMap().find(m_mountID);
		if( finder != RoleMgr::Inst()->GetRoleMap().end() )
		{
			Role *pMount = finder->second;
			if( pMount->IS_STATIC_CLASS(Pet) )
			{
				Pet *pPet = static_cast<Pet*>(pMount);
				pRole->SetMount(pPet);
				pPet->RidersCountInc();
				m_pFSM->Change2ActionState(EActionState_Board);
			}

			m_mountID = GT_INVALID;
		}
	}
}

void ActionStateNormal_RP::OnGameEvent( tagGameEvent* pEvent )
{
}

void ActionStateNormal_RP::OnNetCmd( tagNetCmd* pNetCmd )
{
	if( pNetCmd->dwID == TObjRef<Util>()->Crc32("NS_GetRemoteRoleState") )
	{
		tagNS_GetRemoteRoleState *pRPState = static_cast<tagNS_GetRemoteRoleState*>(pNetCmd);
		
		Role *pRole = RoleMgr::Inst()->FindRole(pRPState->RoleData.dwID);
		if( pRole->IS_STATIC_CLASS(Player) )
		{
			Player *pPlayer = static_cast<Player*>(pRole);

			if( pPlayer->GetRoleState(ERS_Mount) )
			{
				// 向宠物系统发送进入骑乘事件
				tagEnterMount event(_T("tagEnterMount"), NULL);
				event.dwOwnerID = pPlayer->GetID();
				event.dwPetID = pRPState->RoleData.dwMountPetID;
				event.dwTypeID = pRPState->RoleData.dwMountPetTypeID;
				TObjRef<GameFrameMgr>()->SendEvent(&event);

				map<DWORD,Role*>::const_iterator finder = RoleMgr::Inst()->GetRoleMap().find(pRPState->RoleData.dwMountPetID);
				if( finder != RoleMgr::Inst()->GetRoleMap().end() )
				{
					Role *pRole = finder->second;
					if( pRole->IS_STATIC_CLASS(Pet) )
					{
						Pet *pPet = static_cast<Pet*>(pRole);

						pPlayer->SetMount(pPet);
						pPet->RidersCountInc();
						m_pFSM->Change2ActionState(EActionState_Ride);
					}
				}
			}
			else if( pPlayer->GetRoleState(ERS_Mount2) )
			{
				map<DWORD,Role*>::const_iterator finder = RoleMgr::Inst()->GetRoleMap().find(pRPState->RoleData.dwMountPetID);
				if( finder != RoleMgr::Inst()->GetRoleMap().end() )
				{
					Role *pRole = finder->second;
					if( pRole->IS_STATIC_CLASS(Pet) )
					{
						Pet *pPet = static_cast<Pet*>(pRole);

						pPlayer->SetMount(pPet);
						pPet->RidersCountInc();
						m_pFSM->Change2ActionState(EActionState_Board);
					}
				}
				else
				{
					m_mountID = pRPState->RoleData.dwMountPetID;
				}
			}
		}
	}
	else if( pNetCmd->dwID == TObjRef<Util>()->Crc32("NS_Mount") )
	{
		tagNS_Mount *pMountCmd = static_cast<tagNS_Mount*>(pNetCmd);

		Role *pRole = RoleMgr::Inst()->FindRole(pMountCmd->dwRoleID);
		if( pRole->IS_STATIC_CLASS(Player) )
		{
			Player *pPlayer = static_cast<Player*>(pRole);
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
					pPlayer->SetMount(pPet);
					pPet->RidersCountInc();

					m_pFSM->Change2ActionState(EActionState_Ride);
				}
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
				m_pFSM->GetPlayer()->SetMount(pPet);
				pPet->RidersCountInc();

				m_pFSM->Change2ActionState(EActionState_Board);
			}
		}
	}
}

bool ActionStateNormal_RP::IfCanChange2UpperState( FSM_RP::EUpperState state ) 
{
	return true;
}
