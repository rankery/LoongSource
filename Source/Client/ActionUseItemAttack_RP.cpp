#include "StdAfx.h"
#include "ActionUseItemAttack_RP.h"
#include "Player.h"
#include "RoleMgr.h"
#include "EffectMgr.h"
#include "CombatEvent.h"
#include "ItemProtoData.h"

ActionUseItemAttack_RP::ActionUseItemAttack_RP(void)
: m_bClosed(true)
{}

ActionUseItemAttack_RP::~ActionUseItemAttack_RP(void)
{}

void ActionUseItemAttack_RP::Active( Player* pRole,Role* pTarget,DWORD itemID,DWORD dwSerial )
{	
	m_bClosed=false;

	//--记录技能ID
	m_itemID=itemID;
	m_dwSerial=dwSerial;
	m_targetID=pTarget ? pTarget->GetID() : GT_INVALID;

	//--获得显示相关物品属性
	const tagItemDisplayInfo* pItemDisplay=ItemProtoData::Inst()->FindItemDisplay(m_itemID);
	THROW_FALSE(P_VALID(pItemDisplay),_T("cannot find itemdisplayinfo"));
	m_trackName=pItemDisplay->szActionMod;
	m_bAniBlend=pItemDisplay->bActionBlend==TRUE;

	//--获取物品属性
	const tagItemProto* pItemProto=ItemProtoData::Inst()->FindItemProto(m_itemID);
	THROW_FALSE(P_VALID(pItemProto),_T("cannot find itemproto"));
	m_bSingle=pItemProto->fOPRadius==0;
	m_bMoveable=pItemProto->bMoveable==TRUE;

	if(m_trackName.size()>0)
	{
		//--播放攻击动作
		if(m_bMoveable)
		{
			if(m_bAniBlend)
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_Upper,m_trackName.c_str(),false,true);
			else
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,m_trackName.c_str(),false,true);
		}
		else
			pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,m_trackName.c_str(),false,true);

		//--播放特效
		if(pItemDisplay->szSourceEffect[0]!=_T('\0'))
		{
			EffectMgr::Inst()->PlayRoleEffect(pRole->GetID(),pItemDisplay->szSourceEffect);
		}
	}

	//--如果没有技能动作，马上发送命中目标事件
	if(m_trackName.empty())
	{
		m_bClosed=true;

		tagHitTargetEvent event(_T("tagHitTargetEvent"),NULL);
		event.dwSrcRoleID=pRole->GetID();
		event.dwTarRoleID=m_targetID;
		event.bSingle=m_bSingle;
		event.bLast=true;
		event.eCause=EHTC_Item;
		event.dwSerial=m_dwSerial;
		event.dwMisc=m_itemID;
		event.dwMisc2=0;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}
}

void ActionUseItemAttack_RP::Update(Player* pRole,Role* pTarget)
{
	if(m_bClosed)
		return;

	if(pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_trackName.c_str()))
	{
		//--站立或滑动攻击完成以后，恢复下身动作
		if(!m_bMoveable
			||!m_bAniBlend)
		{
			pRole->ReplayLowerAni();
		}

		//--
		m_bClosed=true;
		return;
	}
}

void ActionUseItemAttack_RP::Stop( Player* pRole )
{
	if(m_bClosed)
		return;

	m_bClosed=true;

	//--站立或滑动攻击完成以后，恢复下身动作
	if(!m_bMoveable
		||!m_bAniBlend)
	{
		pRole->ReplayLowerAni();
	}

	//--发送命中目标事件
	tagHitTargetEvent event(_T("tagHitTargetEvent"),NULL);
	event.dwSrcRoleID=pRole->GetID();
	event.dwTarRoleID=m_targetID;
	event.bSingle=m_bSingle;
	event.bLast=true;
	event.eCause=EHTC_Item;
	event.dwSerial=m_dwSerial;
	event.dwMisc=m_itemID;
	event.dwMisc2=0;
	TObjRef<GameFrameMgr>()->SendEvent(&event);
}