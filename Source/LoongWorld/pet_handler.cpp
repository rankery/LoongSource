//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: pet_handler.cpp
// author: xlguo
// actor:
// data: 2009-07-13
// last:
// brief: 宠物消息处理器
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "player_session.h"

#include "role.h"

#include "../WorldDefine/msg_pet_equip.h"
#include "../WorldDefine/msg_pet_skill.h"
#include "../WorldDefine/msg_pet.h"

#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/pet_skill_define.h"

#include "role_mgr.h"
#include "pet_skill.h"
#include "pet_pocket.h"
#include "pet.h"
#include "pet_soul.h"

//-------------------------------------------------------------------------------------------------------
// 获取宠物属性
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandleGetPetAttr(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_GetPetAttr);
	MTRANS_ELSE_RET(pRole, GetOtherInMap(pRecv->dwRoleID), Role, GT_INVALID);

	DWORD dwPetIDs[MAX_PETSOUL_NUM] = {0};
	INT nPetNum = 0;
	if (GT_VALID(pRecv->dwPetID))
	{
		nPetNum		= 1;
		dwPetIDs[0] = pRecv->dwPetID;
	}
	else
	{
		pRole->GetPetPocket()->GetAllPetID(dwPetIDs, nPetNum);
	}

	BYTE buffer[1000]={0};
	for (INT i=0; i<nPetNum; ++i)
	{
		tagNS_GetPetAttr* pSend = (tagNS_GetPetAttr*)buffer;
		tagNS_GetPetAttr tmp;
		pSend->dwID = tmp.dwID;
		pSend->dwRoleID = pRecv->dwRoleID;
		PetSoul* pSoul = pRole->GetPetPocket()->GetPetSoul(dwPetIDs[i]);
		if (!P_VALID(pSoul))
			continue;

		pSoul->FillClientPetAtt(&pSend->petAttr);
		pSend->dwSize = sizeof(tagNS_GetPetAttr) - 1 + 
			sizeof(tagPetSkillMsgInfo) * pSend->petAttr.nPetSkillNum + 
			sizeof(tagPetEquipMsgInfo) * pSend->petAttr.nPetEquipNum;

		SendMessage(pSend, pSend->dwSize);
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 使用宠物蛋
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandleUsePetEgg( tagNetCmd* pCmd )
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MGET_MSG(pRecv, pCmd, NC_UsePetEgg);

	Role*		pRole	= GetRole();
	PetPocket*	pPocket = pRole->GetPetPocket();

	DWORD dwRtv = pPocket->HatchEgg(pRecv->n64ItemID, pRecv->szPetName);

	if (dwRtv != E_Pets_Success)
	{
		tagNS_UsePetEgg send;
		send.dwErrCode = dwRtv;
		send.n64ItemID = pRecv->n64ItemID;
		SendMessage(&send, send.dwSize);
	}
		
	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 删除宠物
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandleDeletePet(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_DeletePet);

	PetSoul* pSoul = GetRole()->GetPetPocket()->GetAway(pRecv->dwPetID, TRUE);
	if (P_VALID(pSoul))
	{
		PetSoul::LogPet(GetRole()->GetID(), pSoul->GetID(), ELCLD_PET_DEL_PET_CLIENT);
		PetSoul::DeleteSoul(pSoul, TRUE);
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------------
// 宠物技能处理消息
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetSkill( tagNetCmd* pCmd )
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MGET_MSG(pRecv, pCmd, NC_PetSkillUse);

	DWORD dwRecvDataSize = 0;
	
	if (sizeof(tagNC_PetSkillUse) != pRecv->dwSize)
	{
		dwRecvDataSize = pRecv->dwSize + 1 - sizeof(tagNC_PetSkillUse);
	}

	DWORD dwErrCode = E_Pets_Success;

	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pPocket, pRole->GetPetPocket(), PetPocket, GT_INVALID);
	MTRANS_ELSE_RET(pSoul, pPocket->GetPetSoul(pRecv->dwPetID), PetSoul, GT_INVALID);

	// 处理消息
	pSoul->HandleSkillCmd(pRecv->dwSkillID, pRecv->byData, dwRecvDataSize);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 停止正在工作的宠物技能
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetSkillStopWorking( tagNetCmd* pCmd )
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_PetSkillStopWorking);
	
	MTRANS_ELSE_RET(pSoul, GetRole()->GetPetPocket()->GetPetSoul(pRecv->dwPetID), PetSoul, GT_INVALID);
	DWORD dwRtv = pSoul->StopSkillWorking(pRecv->dwSkillID);
	
	tagNS_PetSkillStopWorking send;
	send.dwPetID = pRecv->dwPetID;
	send.dwSkillID = pRecv->dwSkillID;
	send.dwErrCode = dwRtv;

	SendMessage(&send, send.dwSize);

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 获取宠物显示信息
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandleGetPetDispInfo(tagNetCmd* pCmd)
{
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pRecv, pCmd, tagNC_GetPetDispInfo, GT_INVALID);

	DWORD dwErr = E_Success;

	tagNS_GetPetDispInfo send;
	send.dwPetID = pRecv->dwPetID;

	Role* pOther =  GetOtherInMap(pRecv->dwRoleID);
	if (!P_VALID(pOther))
	{
		dwErr = E_Pets_Soul_MasterNotFound;
	}
	else
	{
		PetSoul* pSoul = pOther->GetPetPocket()->GetPetSoul(pRecv->dwPetID);
		if (!P_VALID(pSoul))
		{
			dwErr = E_Pets_Soul_NotExist;
		}
		else
		{
			pSoul->GetPetAtt().GetName(send.DispData.szName);
			send.DispData.dwTypeID = pSoul->GetPetAtt().GetProtoID();
		}
	}

	
	SendMessage(&send, send.dwSize);	

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 召唤宠物
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandleSetPetState(tagNetCmd* pCmd)
{
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pRecv, pCmd, tagNC_SetPetState, GT_INVALID);
	MTRANS_ELSE_RET(pPocket, pRole->GetPetPocket(), PetPocket, GT_INVALID);

	tagNS_SetPetState send;
	send.dwPetID	= pRecv->dwPetID;
	send.ePetState	= pRecv->ePetState;
	switch(pRecv->ePetState)
	{
	case EPS_Called:
		send.dwErrCode = pRecv->bVal ? pPocket->CallPet(pRecv->dwPetID) : pPocket->RestPet(pRecv->dwPetID);
		break;
	case EPS_Preparing:
		send.dwErrCode = pRecv->bVal ? pPocket->PreparePet(pRecv->dwPetID) : pPocket->UnPreparePet(pRecv->dwPetID);
		break;
// 	case EPS_Mounting:
// 		break;
	default:
		send.dwErrCode = -1;
		break;
	}
	
	SendMessage(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 宠物换装
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetEquip(tagNetCmd* pCmd)
{
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pRecv, pCmd, tagNC_PetEquip, GT_INVALID);
	
	tagNS_PetEquip send;
	send.dwErrCode = E_Success;
	send.dwPetID = pRecv->dwPetID;	

	PetSoul* pSoul = pRole->GetPetPocket()->GetPetSoul(pRecv->dwPetID);
	if (P_VALID(pSoul))
	{
		send.dwErrCode = pSoul->Equip(pRecv->n64ItemID, pRecv->n8DstPos, FALSE);
		if (E_Success == send.dwErrCode)
		{
			pSoul->GetEquipInfo(pRecv->n64ItemID, &send.itemData);
		}
	}
	else
	{
		send.dwErrCode = E_Pets_PetEquip_Soul_NotExist;
	}
	
	SendMessage(&send, send.dwSize);
	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 宠物卸装
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetUnEquip(tagNetCmd* pCmd)
{
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pRecv, pCmd, tagNC_PetUnequip, GT_INVALID);

	tagNS_PetUnequip send;
	send.dwErrCode = E_Success;
	send.n64ItemID = pRecv->n64ItemID;
	send.dwPetID = pRecv->dwPetID;
 
	PetSoul* pSoul = pRole->GetPetPocket()->GetPetSoul(pRecv->dwPetID);
	if (P_VALID(pSoul))
	{
		send.dwErrCode = pSoul->UnEquip(pRecv->n64ItemID, pRecv->n16PosDst, FALSE);
	}

	SendMessage(&send, send.dwSize);
	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 宠物装备换位置
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetSwapEquipPos(tagNetCmd* pCmd)
{
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pRecv, pCmd, tagNC_PetEquipPosSwap, GT_INVALID);

	PetSoul* pSoul = pRole->GetPetPocket()->GetPetSoul(pRecv->dwPetID);
	if (P_VALID(pSoul))
	{
		pSoul->EquipSwapPos(pRecv->n64ItemID, pRecv->n8PosDst, TRUE);
	}
	
	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 宠物灌注经验需要money
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandleGetPetPourExpMoneyNeed(tagNetCmd* pCmd)
{
	MTRANS_POINTER(pRecv, pCmd, tagNC_GetPetPourExpMoneyNeed);
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);

	PetSoul* pSoul = pRole->GetPetPocket()->GetPetSoul(pRecv->dwPetID);
	
	tagNS_GetPetPourExpMoneyNeed send;
	send.dwPetID		= pRecv->dwPetID;
	send.n64MoneyNeed	= P_VALID(pSoul) ? pSoul->GetPetAtt().CalPourMoney() : 0;

	SendMessage(&send, send.dwSize);
	
	return 0;
}


//-------------------------------------------------------------------------------------------------------
// 宠物灌注经验
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetPourExp(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_PetPourExp);
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);

	tagNS_PetPourExp send;
	send.dwPetID	= pRecv->dwPetID;
	
	PetSoul* pSoul = pRole->GetPetPocket()->GetPetSoul(pRecv->dwPetID);
	if (!P_VALID(pSoul))
	{
		send.dwErrCode	= E_Pets_Soul_NotExist;
	}
	else
	{
		send.dwErrCode	= pSoul->PourExp(pRecv->n64ItemID);
	}

	SendMessage(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 宠物灌注经验
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetUpStep(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_PetUpStep);
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);

	tagNS_PetUpStep send;
	send.n64ItemID	= pRecv->n64ItemID;
	send.dwSkillID	= pRecv->dwSkillID;
	send.nDstStep	= pRecv->nDstStep;
	
	send.dwErrCode	= pRole->GetPetPocket()->CalledSoulUpStep(send.n64ItemID, send.dwSkillID, send.nDstStep);

	if (GT_VALID(pRecv->n64ItemID))
	{
		SendMessage(&send, send.dwSize);
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------
// 宠物资质提升
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetEnhance(tagNetCmd* pCmd)
{	
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_PetEnhance);
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);

	tagNS_PetEnhance send;
	send.n64ItemID	= pRecv->n64ItemID;
	send.dwErrCode	= pRole->GetPetPocket()->CalledSoulEnhance(pRecv->n64ItemID);
	SendMessage(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 宠物学习技能
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetLearnSkill(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_PetLearnSkill);
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);

	tagNS_PetLearnSkill send;
	send.dwPetID		= pRecv->dwPetID;
	send.n64ItemID		= pRecv->n64ItemID;
	
	PetSoul* pSoul = pRole->GetPetPocket()->GetPetSoul(pRecv->dwPetID);
	if (P_VALID(pSoul))
	{
		send.dwErrCode	= pSoul->LearnBookSkill(pRecv->n64ItemID);
	}
	else
	{
		send.dwErrCode	= E_Pets_Soul_NotExist;
	}

	SendMessage(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 邀请驾驭
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetInvite( tagNetCmd* pCmd )
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_MountInvite);
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);

	DWORD dwRtv = E_Pets_Success;

	Role* pDstRole = GetOtherInMap(pRecv->dwDstRoleID);
	if (!P_VALID(pDstRole))
	{
		dwRtv = E_Pets_Mount_DstRoleNotFound;
	}
	else 
	{
		PetSoul* pSoul = GetRole()->GetPetPocket()->GetMountPetSoul();
		if (!P_VALID(pSoul))
		{
			dwRtv = E_Pets_Mount_MountSoulNotFound;
		}
		else
		{
			dwRtv = GetRole()->GetPetPocket()->CanAddPassenger(pDstRole);
			if(E_Pets_Success == dwRtv)
			{
				dwRtv = pDstRole->GetPetPocket()->CanSetRideAfter(GetRole(), TRUE);
				if (E_Pets_Success == dwRtv)
				{
					tagNS_MountOnInvite send;
					send.dwSrcRoleID = GetRole()->GetID();
					pDstRole->SendMessage(&send, send.dwSize);
				}			
			}

		}
	}
	
	if (E_Pets_Success != dwRtv)
	{
		tagNS_MountInvite send;
		send.dwDstRoleID = pRecv->dwDstRoleID;
		send.dwErrCode = dwRtv;
		SendMessage(&send, send.dwSize);
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 邀请驾驭应答
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetOnInvite(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_MountOnInvite);
	MTRANS_ELSE_RET(pRole, GetRole(), Role, GT_INVALID);
	MTRANS_ELSE_RET(pSrcRole, GetOtherInMap(pRecv->dwSrcRoleID), Role, GT_INVALID);

	DWORD dwRtv = E_Pets_Success;

	dwRtv = pRecv->bReceive ? E_Pets_Success : E_Pets_Mount_DstRoleRefuse;
	if (E_Pets_Success != dwRtv)		goto FINISH;

	dwRtv = pSrcRole->GetPetPocket()->CanAddPassenger(pRole);
	if (E_Pets_Success != dwRtv)		goto FINISH;

	dwRtv = pRole->GetPetPocket()->CanSetRideAfter(pSrcRole, TRUE);
	if (E_Pets_Success != dwRtv)		goto FINISH;

	dwRtv = pRole->GetPetPocket()->RideAfter(pSrcRole);
	if (E_Pets_Success != dwRtv)		goto FINISH;

FINISH:
	tagNS_MountInvite send;
	send.dwDstRoleID = pRole->GetID();
	send.dwErrCode = dwRtv;
	pSrcRole->SendMessage(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 锁定解锁宠物
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetSetLock(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole())) return GT_INVALID;

 	MTRANS_POINTER(pRecv, pCmd, tagNC_PetSetLock);
 
 	DWORD dwRtv = E_Pets_Success;

	if (pRecv->bSet)
	{
		dwRtv = GetRole()->GetPetPocket()->LockPet(pRecv->dwPetID, pRecv->n64ItemID);
	}
	else
	{
		dwRtv = GetRole()->GetPetPocket()->UnLockPet(pRecv->dwPetID, pRecv->n64ItemID);
	}

	tagNS_PetSetLock send;
	send.dwPetID = pRecv->dwPetID;
	send.dwErrCode = dwRtv;
	SendMessage(&send, send.dwSize);

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 宠物食品
//-------------------------------------------------------------------------------------------------------
DWORD PlayerSession::HandlePetFood(tagNetCmd* pCmd)
{
	if (!P_VALID(GetRole())) return GT_INVALID;

	MTRANS_POINTER(pRecv, pCmd, tagNC_PetFood);

	tagNS_PetFood send;
	send.n64ItemID		= pRecv->n64ItemID;
	send.dwErrCode		= GetRole()->GetPetPocket()->CalledPetFeed(pRecv->n64ItemID);
	
	SendMessage(&send, send.dwSize);

	return 0;
}
