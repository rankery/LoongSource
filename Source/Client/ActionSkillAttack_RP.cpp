#include "StdAfx.h"
#include "ActionSkillAttack_RP.h"
#include "Player.h"
#include "RoleMgr.h"
#include "CombatEvent.h"
#include "SkillProtoData.h"
#include "EffectMgr.h"
#include "AudioSys.h"
#include "CombatSysUtil.h"

ActionSkillAttack_RP::ActionSkillAttack_RP(void)
: m_bClosed(true)
, m_vPlayerPos( 0.0f, 0.0f, 0.0f )
{
	m_dwMoveEffectID[0] = GT_INVALID;
	m_dwMoveEffectID[1] = GT_INVALID;
}

ActionSkillAttack_RP::~ActionSkillAttack_RP(void)
{}

void ActionSkillAttack_RP::Active( Player* pRole,Role* pTarget,DWORD skillID,DWORD dwSerial )
{	
	m_bClosed=false;

	//--记录技能ID
	m_skillID=skillID;
	m_dwSerial=dwSerial;
	m_targetID=pTarget ? pTarget->GetID() : GT_INVALID;

	//--获取技能属性
	const tagSkillProtoClient* pSkillProto=SkillProtoData::Inst()->FindSkillProto(skillID);
	THROW_FALSE(P_VALID(pSkillProto),_T("cannot find skillproto"));
	m_trackName=pSkillProto->szActionMod;
	m_bSingle=pSkillProto->fOPRadius==0;
	m_dmgTimes=pSkillProto->nDmgTimes;
	m_bMoveable=pSkillProto->bMoveable==TRUE;
	m_bAniBlend=pSkillProto->bActionBlend==TRUE;
	m_bHostile=pSkillProto->bHostile==TRUE;

	//--保存特效/音效数据（避免重复查表）
	m_strAtkSfx=pSkillProto->szAtkSfx;
	m_vecAtkSfxTagNode=pSkillProto->vecTagAtkSfx;
	m_strLchSfx=pSkillProto->szLchSfx;
	m_strLchSfxTagNode=pSkillProto->szTagLchSfx;
	m_vecAttackEffectID.resize(m_vecAtkSfxTagNode.size(),GT_INVALID);
	m_strAtkSound=pSkillProto->szSound;

	if(m_trackName.size()>0)
	{
		//--播放攻击动作
		if(m_bMoveable)
		{
			if(m_bAniBlend)
			{
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_Upper,m_trackName.c_str(),false,true);
			}
			else
			{
				pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,m_trackName.c_str(),false,true);

				//--播放滑动攻击特效
				ASSERT( GT_INVALID == m_dwMoveEffectID[0] && GT_INVALID == m_dwMoveEffectID[1] );
				m_dwMoveEffectID[0] = EffectMgr::Inst()->PlayRoleEffect( pRole->GetID(), _T("Txxy08"), _T("Bip01 R Foot") );
				m_dwMoveEffectID[1] = EffectMgr::Inst()->PlayRoleEffect( pRole->GetID(), _T("Txxy08"), _T("Bip01 L Foot") );
				//--先隐藏，移动时再显示
				EffectMgr::Inst()->ShowEffect( m_dwMoveEffectID[0], false );
				EffectMgr::Inst()->ShowEffect( m_dwMoveEffectID[0], false );
				m_vPlayerPos = pRole->GetPos();
			}
		}
		else
			pRole->GetAniPlayer()->PlayTrack(AvatarAniPlayer::EAT_Other,AvatarAniMixer::EAP_All,m_trackName.c_str(),false,true);

		//--朝向目标
		if(pTarget!=NULL && pTarget!=pRole)
		{
			FaceToTarget(pRole,pTarget);
		}
	}

	//--如果没有技能动作，马上发送命中事件
	if(m_trackName.empty())
	{
		tagHitTargetEvent event(_T("tagHitTargetEvent"),NULL);
		event.dwSrcRoleID=pRole->GetID();
		event.dwTarRoleID=m_targetID;
		event.bSingle=m_bSingle;
		event.bLast=true;
		event.eCause=EHTC_Skill;
		event.dwMisc=m_skillID;
		event.dwMisc2=max(m_dmgTimes,0);
		event.dwSerial=m_dwSerial;
		TObjRef<GameFrameMgr>()->SendEvent(&event);

		m_bClosed=true;
	}
}

void ActionSkillAttack_RP::Update(Player* pRole,Role* pTarget)
{
	if(m_bClosed)
		return;

	const vector<DWORD>& msgCodes = pRole->GetMsgCodes();
	for( size_t nMsgCode = 0; nMsgCode < msgCodes.size(); nMsgCode++ )
	{
		OnMsgCode(pRole,msgCodes[nMsgCode]);
	}

	const bool bTrackEnd = pRole->GetAniPlayer()->IsTrackEnd(AvatarAniPlayer::EAT_Other,m_trackName.c_str());
	if( bTrackEnd || (m_dmgTimes>1 && NULL!=pTarget && pTarget->IsDead()) )
	{
		m_bClosed=true;

		//--恢复下身动作和朝向
		RestoreYawAndLowerAni(pRole);

		//--停止特效
		StopEffect(!bTrackEnd);
		return;
	}

	//--朝向目标
	if(pTarget!=NULL && pTarget!=pRole)
	{
		FaceToTarget(pRole,pTarget);
	}

	//--显示/隐藏滑步特效
	const Vector3 vCurPos = pRole->GetPos();
	const bool bShowMoveEffect = TRUE == ( vCurPos != m_vPlayerPos );
	EffectMgr::Inst()->ShowEffect( m_dwMoveEffectID[0], bShowMoveEffect );
	EffectMgr::Inst()->ShowEffect( m_dwMoveEffectID[0], bShowMoveEffect );
}

void ActionSkillAttack_RP::StopEffect(bool bAtkEffect)
{
	//--停止攻击特效(正常结束时不需要停止攻击特效)
	if( bAtkEffect )
		StopAtkSfx();

	for( size_t nEff = 0; nEff < m_vecAttackEffectID.size(); nEff++ )
		m_vecAttackEffectID[nEff] = GT_INVALID;

	//--停止滑动攻击特效
	if( GT_INVALID != m_dwMoveEffectID[0] )
	{
		EffectMgr::Inst()->StopEffect( m_dwMoveEffectID[0] );
		m_dwMoveEffectID[0] = GT_INVALID;
	}
	if( GT_INVALID != m_dwMoveEffectID[1] )
	{
		EffectMgr::Inst()->StopEffect( m_dwMoveEffectID[1] );
		m_dwMoveEffectID[1] = GT_INVALID;
	}
}

void ActionSkillAttack_RP::Stop(Player* pRole)
{
	if(m_bClosed)
		return;

	m_bClosed=true;

	//--恢复下身动作和朝向
	RestoreYawAndLowerAni(pRole);

	//--发送命中目标事件
	tagHitTargetEvent event(_T("tagHitTargetEvent"),NULL);
	event.dwSrcRoleID=pRole->GetID();
	event.dwTarRoleID=m_targetID;
	event.eCause=EHTC_Skill;
	event.bSingle=m_bSingle;
	event.bLast=true;
	event.dwMisc=m_skillID;
	event.dwMisc2=max(m_dmgTimes-1,0);
	event.dwSerial=m_dwSerial;
	TObjRef<GameFrameMgr>()->SendEvent(&event);
	
	//--停止特效
	StopEffect(true);
}

void ActionSkillAttack_RP::FaceToTarget( Player* pSelf,Role* pTarget )
{
	Vector3 dir=pTarget->GetPos()-pSelf->GetPos();

	if(m_bMoveable)
	{
		if(m_bAniBlend)//上下身分开
		{
			//--如果是站立状态，面向目标
			if(pSelf->IsStandState())
				pSelf->SetYaw(CalcYaw(dir));

			//--旋转上身
			Vector3 targetPos;
			pTarget->GetHeadPos(targetPos);
			pSelf->FaceTo(targetPos,FALSE);
		}
		else//滑动战斗
		{
			pSelf->SetYaw(CalcYaw(dir));
		}	
	}
	else
	{
		pSelf->SetYaw(CalcYaw(dir));
	}
}

void ActionSkillAttack_RP::RestoreYawAndLowerAni( Player* pSelf )
{
	if(m_bMoveable)
	{
		if(m_bAniBlend)//上下身分开
			pSelf->FaceBack();
		else//滑动战斗
			pSelf->ReplayLowerAni();	
	}
	else
		pSelf->ReplayLowerAni();

	//--站定：不恢复, 移动：恢复朝向，
	if( pSelf->IsStandState() )
	{
		pSelf->SetMoveYaw( pSelf->GetYaw() );
	}
	else
	{
		pSelf->SetYaw( pSelf->GetMoveYaw() );
	}
}

void ActionSkillAttack_RP::OnMsgCode(Player* pRole,const DWORD dwMsgCode)
{
	TCHAR szAniMsg[5];
	ZeroMemory(szAniMsg,sizeof(szAniMsg));
	_FourCC2Str(dwMsgCode,szAniMsg);

	//--发送命中目标事件
	if(_tcsncmp(szAniMsg,_T("bak"),3)==0)
	{
		int channel=_tstoi(&szAniMsg[3]);

		tagHitTargetEvent event(_T("tagHitTargetEvent"),NULL);
		event.dwSrcRoleID=pRole->GetID();
		event.dwTarRoleID=m_targetID;
		event.eCause=EHTC_Skill;
		event.bSingle=m_bSingle;
		event.bLast=(channel+1>=m_dmgTimes);
		event.dwMisc=m_skillID;
		event.dwMisc2=channel;
		event.dwSerial=m_dwSerial;
		TObjRef<GameFrameMgr>()->SendEvent(&event);
	}

	//--播放/停止攻击特效
	if(_tcsncmp(szAniMsg,_T("atk"),3)==0 )
	{
		if( _T('e') != szAniMsg[3] )
			PlayAtkSfx(pRole->GetID());
		else
			StopAtkSfx();
	}

	//--播放发射特效
	if(_tcsncmp(szAniMsg,_T("lch"),3)==0)
	{
		int channel=_tstoi(&szAniMsg[3]);

		tagHitTargetEvent event(_T("tagHitTargetEvent"),NULL);
		event.dwSrcRoleID=pRole->GetID();
		event.dwTarRoleID=m_targetID;
		event.eCause=EHTC_Skill;
		event.bSingle=m_bSingle;
		event.bLast=(channel+1>=m_dmgTimes);
		event.dwMisc=m_skillID;
		event.dwMisc2=channel;
		event.dwSerial=m_dwSerial;
		if( m_strLchSfx.empty() )
			TObjRef<GameFrameMgr>()->SendEvent(&event);
		else
			EffectMgr::Inst()->PlayLaunchEffect(event,m_strLchSfx.c_str(),m_strLchSfxTagNode.c_str());

		//--播放攻击音效
		if(m_strAtkSound != _T(""))
			TObjRef<AudioSys>()->Play3DSound(m_strAtkSound.c_str(),100.0f,100.0f*10.0f,pRole->GetPos(),SOUND_WISPER);

	}

	//--播放攻击喊叫声
	if(m_bHostile&&_tcsncmp(szAniMsg,_T("bf"),2)==0)
	{
		// 20%几率
		if( 0 == rand() % 5 )
			CombatSysUtil::Inst()->PlayPlayerRoarSound(pRole);
	}
}

void ActionSkillAttack_RP::PlayAtkSfx(DWORD dwRoleID)
{
	if( !m_strAtkSfx.empty() )
	{
		for( size_t nEff = 0; nEff < m_vecAtkSfxTagNode.size(); nEff++ )
		{
			//ASSERT( GT_INVALID == m_vecAttackEffectID[nEff] );
			m_vecAttackEffectID[nEff]=EffectMgr::Inst()->PlayRoleEffect(dwRoleID,m_strAtkSfx.c_str(),m_vecAtkSfxTagNode[nEff].c_str());
		}
	}
}
void ActionSkillAttack_RP::StopAtkSfx()
{
	for( size_t nEff = 0; nEff < m_vecAttackEffectID.size(); nEff++ )
	{
		if( GT_INVALID != m_vecAttackEffectID[nEff] )
		{
			EffectMgr::Inst()->StopEffect(m_vecAttackEffectID[nEff]);
			m_vecAttackEffectID[nEff] = GT_INVALID;
		}
	}
}
