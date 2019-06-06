#include "StdAfx.h"
#include "CombatSysUtil.h"
#include "CombatEvent.h"
#include "ChatFrame.h"
#include "SceneCenterInfoFrame.h"
#include "MapMgr.h"
#include "AudioSys.h"
#include "NPC.h"
#include "Player.h"
#include "CreatureData.h"
#include "ItemProtoData.h"
#include "..\WorldDefine\msg_combat.h"
#include "EffectMgr.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "SkillMgr.h"
#include "SkillProtoData.h"

CombatSysUtil::CombatSysUtil(void)
{}

CombatSysUtil::~CombatSysUtil(void)
{}

CombatSysUtil g_Inst;
CombatSysUtil* CombatSysUtil::Inst()
{
	return &g_Inst;
}

void CombatSysUtil::Init()
{
	m_pGameFrameMgr=TObjRef<GameFrameMgr>();
	m_pAudioSys=TObjRef<AudioSys>();
	m_pChatFrame=(ChatFrame*)m_pGameFrameMgr->GetFrame(_T("Chat"));
	m_pSceneCenterInfoFrame = (SceneCenterInfoFrame*)m_pGameFrameMgr->GetFrame(_T("SceneCenterInfo"));
}

void CombatSysUtil::Destroy()
{

}

Vector3 CombatSysUtil::CalcKeyMoveDir( EWalkDir dir,float cameraYaw )
{
	//--计算移动方向yaw
	float yaw=0.0f;
	switch(dir)
	{
	case EWalkDir_Front:		yaw=_DegToRad(180.0f);	break;
	case EWalkDir_RightFront:	yaw=_DegToRad(225.0f);	break;
	case EWalkDir_Right:		yaw=_DegToRad(270.0f);	break;
	case EWalkDir_RightBack:	yaw=_DegToRad(315.0f);	break;
	case EWalkDir_Back:			yaw=_DegToRad(0.0f);	break;
	case EWalkDir_LeftBack:		yaw=_DegToRad(45.0f);	break;
	case EWalkDir_Left:			yaw=_DegToRad(90.0f);	break;
	case EWalkDir_LeftFront:	yaw=_DegToRad(135.0f);	break;
	default:ASSERT(0);
	}

	//--计算移动方向向量
	Vector3 ret(0,0,-1);
	Transform trans;
	trans.RotateAxis('Y',yaw+cameraYaw);
	trans.TransVec3(ret);
	Vec3Normalize(ret);

	return ret;
}

void CombatSysUtil::ShowCombatMsg( const TCHAR* szFormat,... )
{
	va_list args;
	va_start(args, szFormat);

	TCHAR szBuffer[512];
	_vstprintf_s(szBuffer, 512, szFormat, args);

	va_end(args);

	m_pChatFrame->PushInfo(szBuffer,ESCC_Combat);
}

void CombatSysUtil::ShowSystemMsg(const TCHAR* szFormat,...)
{
	va_list args;
	va_start(args, szFormat);

	TCHAR szBuffer[512];
	_vstprintf_s(szBuffer, 512, szFormat, args);

	va_end(args);

	m_pChatFrame->PushInfo(szBuffer,ESCC_System);
}

void CombatSysUtil::ShowPKErrorMsg( DWORD dwErrorCode )
{
	TCHAR szKey[100];
	_stprintf(szKey,_T("PKErrorCode_%u"),dwErrorCode);
	ShowScreenCenterMsg(g_StrTable[szKey]);
}

void CombatSysUtil::ShowScreenCenterMsg( const TCHAR* szFormat,... )
{
	va_list args;
	va_start(args, szFormat);

	TCHAR szBuffer[512];
	_vstprintf_s(szBuffer, 512, szFormat, args);

	va_end(args);
	m_pSceneCenterInfoFrame->AddCenterinfo(szBuffer);

}

void CombatSysUtil::ShowScreenCenterMsgEx(const TCHAR* szFormat,...)
{
	va_list args;
	va_start(args, szFormat);

	TCHAR szBuffer[512];
	_vstprintf_s(szBuffer, 512, szFormat, args);

	va_end(args);
	m_pSceneCenterInfoFrame->AddCenterinfoEx(szBuffer);
}

void CombatSysUtil::ShowActionCheckMsg( DWORD dwErrorCode )
{
	TCHAR szKey[100];
	_stprintf(szKey,_T("CombatCheck%u"),dwErrorCode);

	const TCHAR* szMsg=g_StrTable[szKey];
	ShowScreenCenterMsg(szMsg);
}


void CombatSysUtil::ShowSkillSvrErrMsg( DWORD dwErrorCode )
{
	TCHAR szKey[100];
	_stprintf(szKey,_T("SkillSvrErr%u"),dwErrorCode);

	const TCHAR* szMsg=g_StrTable[szKey];
	ShowScreenCenterMsg(szMsg);
}


void CombatSysUtil::ShowItemSvrErrMsg( DWORD dwErrorCode )
{
	TCHAR szKey[100];
	_stprintf(szKey,_T("ItemSvrErr%u"),dwErrorCode);

	const TCHAR* szMsg=g_StrTable[szKey];
	ShowScreenCenterMsg(szMsg);
}

void CombatSysUtil::ShowPlayStyleActionErrMsg(DWORD dwErrorCode)
{
	TCHAR szKey[100];
	_stprintf(szKey,_T("Motion_ErrorCode_%u"),dwErrorCode);

	const TCHAR* szMsg=g_StrTable[szKey];
	ShowScreenCenterMsg(szMsg);
}

const TCHAR* CombatSysUtil::GetReadyTrackName( EItemTypeEx weapon )
{
	switch(weapon)
	{
	case EITE_Null:		return _T("rdy1");
	case EITE_Sword:	return _T("rdy2");// 剑
	case EITE_Blade:	return _T("rdy3");// 刀
	case EITE_Wand:		return _T("rdy4");// 长兵
	case EITE_Bow:		return _T("rdy5");// 弓 
	case EITE_Faqi:		return _T("rdy1");// 法器
	case EITE_Qin:		return _T("rdy6");// 琴
	}
	return _T("rdy1");
}

const TCHAR* CombatSysUtil::GetWalkTrackName( EItemTypeEx weapon,bool bInCombat )
{
	if(!bInCombat)
		return _T("wlk1");

	switch(weapon)
	{
	case EITE_Null:		return _T("wlk2");
	case EITE_Sword:	return _T("wlk3");// 剑
	case EITE_Blade:	return _T("wlk4");// 刀
	case EITE_Wand:		return _T("wlk5");// 长兵
	case EITE_Bow:		return _T("wlk6");// 弓 
	case EITE_Faqi:		return _T("wlk1");// 法器
	case EITE_Qin:		return _T("wlk7");// 琴
	}

	return _T("wlk1");
}

const TCHAR* CombatSysUtil::GetRunTrackName( EItemTypeEx weapon,bool bInCombat )
{
	if(!bInCombat)
		return _T("run1");

	switch(weapon)
	{
	case EITE_Null:		return _T("run2");
	case EITE_Sword:	return _T("run3");// 剑
	case EITE_Blade:	return _T("run4");// 刀
	case EITE_Wand:		return _T("run5");// 长兵
	case EITE_Bow:		return _T("run6");// 弓 
	case EITE_Faqi:		return _T("run1");// 法器
	case EITE_Qin:		return _T("run7");// 琴
	}

	return _T("run1");
}

const TCHAR* CombatSysUtil::GetRushTrackName( EItemTypeEx weapon,bool bInCombat )
{
	if(!bInCombat)
		return _T("rsh1");

	switch(weapon)
	{
	case EITE_Null:		return _T("rsh2");
	case EITE_Sword:	return _T("rsh3");// 剑
	case EITE_Blade:	return _T("rsh4");// 刀
	case EITE_Wand:		return _T("rsh5");// 长兵
	case EITE_Bow:		return _T("rsh6");// 弓 
	case EITE_Faqi:		return _T("rsh1");// 法器
	case EITE_Qin:		return _T("rsh7");// 琴
	}

	return _T("rsh1");
}

const TCHAR* CombatSysUtil::GetBlockTrackName( EItemTypeEx weapon )
{
	switch(weapon)
	{
	case EITE_Null:		return _T("blk1");
	case EITE_Sword:	return _T("blk2");// 剑
	case EITE_Blade:	return _T("blk3");// 刀
	case EITE_Wand:		return _T("blk4");// 长兵
	case EITE_Bow:		return _T("blk5");// 弓 
	case EITE_Faqi:		return _T("blk1");// 法器
	case EITE_Qin:		return _T("blk6");// 琴
	}

	return _T("rdy1");
}

const TCHAR* CombatSysUtil::GetMoveTrackName( EItemTypeEx weapon,bool bInCombat,float speed )
{
	if(speed<300.0f)
		return GetWalkTrackName(weapon,bInCombat);
	else if(speed<=800.0f)
		return GetRunTrackName(weapon,bInCombat);
	else 
		return GetRushTrackName(weapon,bInCombat);
}

const TCHAR* CombatSysUtil::GetNPCMoveTrackName( float moveSpeed )
{
	if(moveSpeed<300.0f)
		return _T("walk");
	else if(moveSpeed<=800.0f)
		return _T("run");
	else 
		return _T("dush");

}

const TCHAR* CombatSysUtil::GetMountTrackName( AvatarAniPlayer::EAniType aniType,float moveSpeed )
{
	switch( aniType )
	{
	case AvatarAniPlayer::EAT_Stand:
	case AvatarAniPlayer::EAT_Ready:
		return _T("std");
	case AvatarAniPlayer::EAT_Move:
		{
			if( moveSpeed <= 300.0f )
				return _T("walk");
			else if( moveSpeed <= 1000.0f )
				return _T("run");
			else
				return _T("dush");
		}
	case AvatarAniPlayer::EAT_Slide:
	case AvatarAniPlayer::EAT_Drop:
	case AvatarAniPlayer::EAT_VDrop:
		return _T("run");
	case AvatarAniPlayer::EAT_Ease:
		return _T("lei");
	case AvatarAniPlayer::EAT_StartJump:
		return _T("jmp1");
	case AvatarAniPlayer::EAT_Jumpping:
		return _T("jmp2");
	case AvatarAniPlayer::EAT_Land:
		return _T("jmp3");
	}

	return _T("");
}

const TCHAR* CombatSysUtil::GetStandTrackName( EItemTypeEx RWeapon )
{
	if(RWeapon==EITE_Blade)
		return _T("std2");

	return _T("std1");
}

const TCHAR* CombatSysUtil::GetEnterCombatTrackName( EItemTypeEx RWeapon )
{
	switch(RWeapon)
	{
	//--徒手
	case EITE_Null:		//徒手
	case EITE_Faqi:		//法器
		return _T("ent1");
	//--腰间
	case EITE_Sword:	//剑
		return _T("ent2");
	//--腰后
	case EITE_Blade:	//刀
		return _T("ent3");
	//--背后
	case EITE_Wand:		//长兵
		return _T("ent4");
	case EITE_Bow:		//弓
		return _T("ent5");
	case EITE_Qin:		//琴
		return _T("ent6");
	}

	return _T("ent1");
}

const TCHAR* CombatSysUtil::GetLeaveCombatTrackName( EItemTypeEx RWeapon )
{
	switch(RWeapon)
	{
	//--徒手
	case EITE_Null:		//徒手
	case EITE_Faqi:		//法器
		return _T("out1");
	//--腰间
	case EITE_Sword:	//剑
		return _T("out2");
	//--腰后
	case EITE_Blade:	//刀
		return _T("out3");
	//--背后
	case EITE_Wand:		//长兵
		return _T("out4");
	case EITE_Bow:		//弓
		return _T("out5");
	case EITE_Qin:		//琴
		return _T("out6");
	}

	return _T("out1");
}

const TCHAR* CombatSysUtil::GetBeattackTrackName()
{
	if(rand()%2==0)
		return _T("bak1");
	else
		return _T("bak2");
}

DWORD CombatSysUtil::MakeSkillMsgSerial()
{
	static DWORD serial=1;
	return serial++;
}

void CombatSysUtil::PlayFootSound( const Vector3& rolePos )
{
	tstring szSound;

	//--在水中?
	float waterH=0;
	if(MapMgr::Inst()->GetNavMap()->GetCollider()->GetWaterHeight(rolePos,waterH)
		&&rolePos.y<=waterH)
	{
		szSound=_T("foot1");
	}
	else
	{
		//--取得地面或模型表面类型
		ESurfaceType surfaceType=MapMgr::Inst()->GetNavMap()->GetCollider()->QuerySurfaceType(rolePos,Kernel::Inst()->GetAccumTime());
		switch(surfaceType)
		{
		case EST_None:
			break;
		case EST_Snow:
			szSound=_T("foot2");
			break;
		case EST_Rock:
			szSound=_T("foot3");
			break;
		case EST_Earth:
			szSound=_T("foot4");
			break;
		case EST_Grass:
			szSound=_T("foot5");
			break;
		case EST_Sand:
			szSound=_T("foot6");
			break;
		case EST_Ice:
			szSound=_T("foot7");
			break;
		case EST_Metal:
			szSound=_T("foot8");
			break;
		case EST_Wood:
			szSound=_T("foot9");
			break;
		}
	}

	if(!szSound.empty())
		m_pAudioSys->Play3DSound(szSound.c_str(),50.0f,100.0f*20.0f,rolePos,SOUND_WISPER);
}

void CombatSysUtil::PlayNPCFootSound(NPC* pNPC)
{
	if( NULL == pNPC || !pNPC->IsAttributeInited() )
		return;

	const Vector3& rolePos = pNPC->GetPos();
	int surfaceType = EST_None;

	//--在水中?
	float waterH=0;
	if(MapMgr::Inst()->GetNavMap()->GetCollider()->GetWaterHeight(rolePos,waterH)
		&&rolePos.y<=waterH)
	{
		surfaceType = 1;
	}
	else
	{
		surfaceType=MapMgr::Inst()->GetNavMap()->GetCollider()->QuerySurfaceType(rolePos,Kernel::Inst()->GetAccumTime());

		// 其他表面都用一种音效
		if( EST_None != surfaceType )
			surfaceType = 2;
	}
	if( EST_None == surfaceType )
		return;

	const tagCreatureMdlData* pMdlData = pNPC->GetMdlData();
	if( NULL == pMdlData )
		return;
	
	if( pMdlData->szFootSound.empty() )
		return;

	tstringstream streamSondName;
	streamSondName<<pMdlData->szFootSound<<_T('_')<<surfaceType;
	m_pAudioSys->Play3DSound(streamSondName.str().c_str(),50.0f,100.0f*20.0f,rolePos,SOUND_WISPER);
}

void CombatSysUtil::PlayFallSound( const Vector3& rolePos )
{
	tstring szSound;

	//--在水中?
	float waterH=0;
	if(MapMgr::Inst()->GetNavMap()->GetCollider()->GetWaterHeight(rolePos,waterH)
		&&rolePos.y<waterH)
	{
		szSound=_T("fall1");
	}
	else
	{
		//--取得地面或模型表面类型
		ESurfaceType surfaceType=MapMgr::Inst()->GetNavMap()->GetCollider()->QuerySurfaceType(rolePos,Kernel::Inst()->GetAccumTime());
		switch(surfaceType)
		{
		case EST_None:
			break;
		case EST_Snow:
			szSound=_T("fall2");
			break;
		case EST_Rock:
			szSound=_T("fall3");
			break;
		case EST_Earth:
			szSound=_T("fall4");
			break;
		case EST_Grass:
			szSound=_T("fall5");
			break;
		case EST_Sand:
			szSound=_T("fall6");
			break;
		case EST_Ice:
			szSound=_T("fall7");
			break;
		case EST_Metal:
			szSound=_T("fall8");
			break;
		case EST_Wood:
			szSound=_T("fall9");
			break;
		}
	}

	if(!szSound.empty())
		m_pAudioSys->Play3DSound(szSound.c_str(),100.0f,100.0f*50.0f,rolePos,SOUND_NORMAL);
}

void CombatSysUtil::PlayRoleDeadSound( Role* pRole )
{
	tstring szSound;

	if(pRole->IS_KIND_OF(NPC))
	{
		NPC* pNPC=(NPC*)pRole;

		const tagCreatureMdlData* pData=pNPC->GetMdlData();
		if(!P_VALID(pData))
			return;
	
		szSound=pData->szDeadSound;
	}
	else if(pRole->IS_KIND_OF(Player))
	{
		Player* pPlayer=(Player*)pRole;
		if(pPlayer->GetPlayerSex()==0)//女
			szSound=_T("player_dead2");
		else if(pPlayer->GetPlayerSex()==1)//男
			szSound=_T("player_dead1");
	}

	if(!szSound.empty())
		m_pAudioSys->Play3DSound(szSound.c_str(),100.0f,100.0f*50.0f,pRole->GetPos(),SOUND_NORMAL);
}

void CombatSysUtil::PlayMonsterCombatSound( NPC* pNPC )
{
	const tagCreatureProto* pProto=pNPC->GetProto();
	if(!P_VALID(pProto))
		return;

	if(ECT_Monster!=pProto->eType)
		return;

	const tagCreatureMdlData* pData=pNPC->GetMdlData();
	if(!P_VALID(pData))
		return;

	if(!pData->szCombatSound.empty())
		m_pAudioSys->Play3DSound(pData->szCombatSound.c_str(),100.0f,100.0f*50.0f,pNPC->GetPos(),SOUND_NORMAL);
}

void CombatSysUtil::PlayWeaponSound(Player* pPlayer)
{
	ASSERT(NULL!=pPlayer);

	const int nRand = rand() % 8 + 1;

	TCHAR szSound[X_SHORT_NAME] = { 0 };
	const EItemTypeEx eRWeaponType = pPlayer->GetRWeaponType();
	switch(eRWeaponType)
	{
		case EITE_Sword: _sntprintf( szSound, X_SHORT_NAME, _T("sword%d"), nRand ); break;
		case EITE_Blade: _sntprintf( szSound, X_SHORT_NAME, _T("blade%d"), nRand ); break;
		case EITE_Wand:  _sntprintf( szSound, X_SHORT_NAME, _T("wand%d"), nRand );  break;
		default: return;
	}
	m_pAudioSys->Play3DSound(szSound,100.0f,100.0f*50.0f,pPlayer->GetPos(),SOUND_NORMAL);
}

void CombatSysUtil::PlayPlayerRoarSound(Player* pPlayer)
{
	ASSERT(NULL!=pPlayer);
	if( !pPlayer->IsAttributeInited() )
		return;

	const int nRoarSound = rand() % 5 + 1;

	TCHAR szSound[X_SHORT_NAME] = { 0 };
	if( 0 == pPlayer->GetAvatarAtt().bySex )
		_sntprintf( szSound, X_SHORT_NAME, _T("girl_roar%d"), nRoarSound );
	else
		_sntprintf( szSound, X_SHORT_NAME, _T("boy_roar%d"), nRoarSound );

	m_pAudioSys->Play3DSound(szSound,100.0f,100.0f*50.0f,pPlayer->GetPos(),SOUND_NORMAL);
}

void CombatSysUtil::PlayPlayerHowlSound(Player* pPlayer)
{
	if( !pPlayer->IsAttributeInited() )
		return;

	const int nHowlSound = rand() % 3 + 1;

	TCHAR szSound[X_SHORT_NAME] = { 0 };
	if( 0 == pPlayer->GetAvatarAtt().bySex )
		_sntprintf( szSound, X_SHORT_NAME, _T("girl_howl%d"), nHowlSound );
	else
		_sntprintf( szSound, X_SHORT_NAME, _T("boy_howl%d"), nHowlSound );

	m_pAudioSys->Play3DSound(szSound,100.0f,100.0f*50.0f,pPlayer->GetPos(),SOUND_NORMAL);
}

void CombatSysUtil::PlayMonsterRoarSound(NPC* pRole)
{
	if( !pRole->IsAttributeInited() )
		return;

	const tagCreatureMdlData* pData=pRole->GetMdlData();
	if(!P_VALID(pData))
		return;

	if(!pData->szRoarSound.empty())
		m_pAudioSys->Play3DSound(pData->szRoarSound.c_str(),100.0f,100.0f*50.0f,pRole->GetPos(),SOUND_NORMAL);
}

void CombatSysUtil::PlayMonsterHowlSound(NPC* pRole)
{
	if( !pRole->IsAttributeInited() )
		return;

	const tagCreatureMdlData* pData=pRole->GetMdlData();
	if(!P_VALID(pData))
		return;

	if(!pData->szHowlSound.empty())
		m_pAudioSys->Play3DSound(pData->szHowlSound.c_str(),100.0f,100.0f*50.0f,pRole->GetPos(),SOUND_NORMAL);
}

void CombatSysUtil::SelfUseItemImmediate( INT64 n64ItemID,DWORD dwItemTypeID,DWORD dwSrcRoleID,DWORD dwTargetRoleID,const Vector3& srcRolePos )
{
	const tagItemDisplayInfo* pItemDisplay=ItemProtoData::Inst()->FindItemDisplay(dwItemTypeID);
	if(!P_VALID(pItemDisplay))
		return;

	const tagItemProto* pItemProto=ItemProtoData::Inst()->FindItemProto(dwItemTypeID);
	if(!P_VALID(pItemProto))
		return;

	//--发送消息
	tagNC_UseItem cmd;
	cmd.n64ItemID		= n64ItemID;
	cmd.dwTargetRoleID	= dwTargetRoleID;
	cmd.dwSerial		= MakeSkillMsgSerial();
	cmd.vSrcPos			= srcRolePos;
	TObjRef<NetSession>()->Send(&cmd);

	//--发送命中目标事件
	tagHitTargetEvent event(_T("tagHitTargetEvent"),NULL);
	event.dwSrcRoleID=dwSrcRoleID;
	event.dwTarRoleID=dwTargetRoleID;
	event.bSingle=pItemProto->fOPRadius==0;
	event.bLast=true;
	event.eCause=EHTC_Item;
	event.dwMisc=dwItemTypeID;
	event.dwMisc2=0;
	event.dwSerial=cmd.dwSerial;
	TObjRef<GameFrameMgr>()->SendEvent(&event);

	//--播放特效
	if(pItemDisplay->szSourceEffect[0]!=_T('\0'))
		EffectMgr::Inst()->PlayRoleEffect(dwSrcRoleID,pItemDisplay->szSourceEffect);
}

void CombatSysUtil::PullLocalPlayer()
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();
	NavCollider* pCD=MapMgr::Inst()->GetNavMap()->GetCollider();

	Vector3 rolePos=pSelf->GetPos();
	Vector3 roleSize=pSelf->GetRoleSize();
	Vector3 halfSize=roleSize*0.5f;

	AABBox box;
	box.min.x=rolePos.x-halfSize.x;
	box.max.x=rolePos.x+halfSize.x;
	box.min.z=rolePos.z-halfSize.z;
	box.max.z=rolePos.z+halfSize.z;
	box.min.y=-1000000.0f;
	box.max.y=rolePos.y+roleSize.y;

	if(pCD->AABBoxCollideBoxTopAndTrn(box,0.0f,rolePos.y,NULL,NavSceneNode::EFlag_NotCarrier,NULL))
	{
		float swimFootH;
		if(pCD->IfWillSwim(rolePos,roleSize.y,swimFootH))
		{
			if(pSelf->CanWaterWalk())
			{
				float waterH;
				if(pCD->GetWaterHeight(rolePos,waterH))
				{
					rolePos.y=waterH;
					pSelf->SetPos(rolePos);
				}
			}
			else
			{
				rolePos.y=swimFootH;
				pSelf->SetPos(rolePos);
			}
		}
		else
		{
			pSelf->SetPos(rolePos);
		}
	}
}

bool CombatSysUtil::NormalAttackCanMove()
{
	DWORD dwSkillID=SkillMgr::Inst()->GetNormalAttackTypeID();
	const tagSkillProtoClient* pSkillProto=SkillProtoData::Inst()->FindSkillProto(dwSkillID);
	THROW_FALSE(P_VALID(pSkillProto),_T("cannot find skill proto"));
	return pSkillProto->bMoveable==TRUE;
}