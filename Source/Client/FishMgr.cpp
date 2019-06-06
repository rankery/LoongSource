#include "StdAfx.h"
#include "FishMgr.h"
#include "ItemEvent.h"
#include "ItemFrame.h"
#include "Item.h"
#include "ItemMgr.h"
#include "CombatSysUtil.h"
#include "EffectMgr.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "ItemProtoData.h"
#include "../WorldDefine/msg_fishing.h"

FishMgr::FishMgr(void):
m_pItemFrame( NULL ),
m_dwFishFoodType( GT_INVALID )
{
}

FishMgr::~FishMgr(void)
{
}


FishMgr g_fishMgr;

FishMgr* FishMgr::Inst()
{
	return &g_fishMgr;
}

VOID FishMgr::SetFishFoodType( DWORD dwFishFoodType )
{
	m_dwFishFoodType = dwFishFoodType;
}

DWORD FishMgr::GetFishFoodType()
{
	return m_dwFishFoodType;
}

VOID FishMgr::StartFish( Item* pItem )
{
	if( !P_VALID(pItem) )
		return;

	if( !P_VALID(m_pItemFrame) )
	{
		m_pItemFrame = (ItemFrame*)( TObjRef<GameFrameMgr>()->GetFrame( _T("Item") ) );
		if( !P_VALID( m_pItemFrame ) )
			return;
	}

	m_pItemFrame->OnUseItem( pItem );
}