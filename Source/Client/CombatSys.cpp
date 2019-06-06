#include "StdAfx.h"
#include "..\WorldDefine\action.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\msg_pk.h"
#include "..\WorldDefine\base_define.h"
#include "..\WorldDefine\msg_loot.h"
#include "..\WorldDefine\MapAttDefine.h"
#include "..\WorldBase\GameMap\GameMap.h"
#include "CombatSys.h"
#include "RoleMgr.h"
#include "CombatEvent.h"
#include "CombatSysUtil.h"
#include "CombatActionChecker.h"
#include "LocalPlayer.h"
#include "EffectMgr.h"
#include "CombatFrame_PK.h"
#include "CombatFrame_Cursor.h"
#include "CombatFrame_EventCreater.h"
#include "CombatFrame_MsgShow.h"
#include "CombatFrame_DeadMenu.h"
#include "ClientCamera.h"
#include "SkillProtoData.h"
#include "ItemProtoData.h"
#include "RoleEvent.h"
#include "GroundItemMgr.h"
#include "EffectMgr.h"
#include "MapMgr.h"
#include "SceneEffectData.h"
#include "AudioSys.h"
#include "..\WorldDefine\msg_motion.h"
#include "..\WorldDefine\msg_fishing.h"

CombatSys::CombatSys(void):m_trunk(this)
{
	m_pEventCreaterFrame=NULL;
	m_pPKFrame=NULL;
	m_pCursorFrame=NULL;
	m_pMsgShowFrame=NULL;
	m_pDeadMenuFrame=NULL;
}

CombatSys::~CombatSys(void)
{
}

BOOL CombatSys::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	//--初始化工具类
	CombatSysUtil::Inst()->Init();

	//--初始化行为检查器
	CombatActionChecker::Inst()->Init();

	//--初始化特效管理器
	EffectMgr::Inst()->Init();

    m_multiObjId2EffectId.clear();

	//--注册游戏事件处理函数
	pMgr->RegisterEventHandle(_T("tagRotateCameraEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnRotateCameraEvent));
	pMgr->RegisterEventHandle(_T("tagHitTargetEvent"),			(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnHitTargetEvent));
	pMgr->RegisterEventHandle(_T("tagFollowRoleEvent"),			(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnFollowRoleEvent));
	pMgr->RegisterEventHandle(_T("tagShowDeadMenuEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnShowDeadMenuEvent));
	pMgr->RegisterEventHandle(_T("tagUseSkillEvent"),			(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnUseSkillEvent));
	pMgr->RegisterEventHandle(_T("tagUseItemEvent"),			(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnUseItemEvent));
	pMgr->RegisterEventHandle(_T("tagLPRoleStateChangeEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnLPRoleStateChangeEvent));
	pMgr->RegisterEventHandle(_T("tagRPRoleStateChangeEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnRPRoleStateChangeEvent));
	pMgr->RegisterEventHandle(_T("tagRoleBuffChangeEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnRoleBuffChangeEvent));
	pMgr->RegisterEventHandle(_T("tagPlayPersonalityActEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnPlayPersonalityActEvent));
	pMgr->RegisterEventHandle(_T("tagWeaponChangeEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnWeaponChangeEvent));

	//--注册网络消息处理函数
	m_pCmdMgr->Register("NS_SyncWalk",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncWalk),			_T("NS_SyncWalk"));
	m_pCmdMgr->Register("NS_SyncJump",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncJump),			_T("NS_SyncJump"));
	m_pCmdMgr->Register("NS_SyncDrop",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncDrop),			_T("NS_SyncDrop"));
	m_pCmdMgr->Register("NS_SyncVDrop",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncVDrop),			_T("NS_SyncVDrop"));
	m_pCmdMgr->Register("NS_SyncSlide",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncSlide),			_T("NS_SyncSlide"));
	m_pCmdMgr->Register("NS_SyncStand",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncStand),			_T("NS_SyncStand"));
	m_pCmdMgr->Register("NS_MoveFailed",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MoveFailed),			_T("NS_MoveFailed"));
	m_pCmdMgr->Register("NS_Skill",				(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_Skill),				_T("NS_Skill"));
	m_pCmdMgr->Register("NS_SkillInterrupt",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SkillInterrupt),		_T("NS_SkillInterrupt"));
	m_pCmdMgr->Register("NS_UseItem",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_UseItem),				_T("NS_UseItem"));
	m_pCmdMgr->Register("NS_UseItemInterrupt",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_UseItemInterrupt),	_T("NS_UseItemInterrupt"));
	m_pCmdMgr->Register("NS_RoleDead",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleDead),			_T("NS_RoleDead"));
	m_pCmdMgr->Register("NS_RoleRevive",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleRevive),			_T("NS_RoleRevive"));
	m_pCmdMgr->Register("NS_RoleHPChange",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleHPChange),		_T("NS_RoleHPChange"));
	m_pCmdMgr->Register("NS_SetRoleState",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SetRoleState),		_T("NS_SetRoleState"));
	m_pCmdMgr->Register("NS_UnSetRoleState",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_UnSetRoleState),		_T("NS_UnSetRoleState"));
	m_pCmdMgr->Register("NS_SetState",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SetState),			_T("NS_SetState"));
	m_pCmdMgr->Register("NS_UnSetState",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_UnSetState),			_T("NS_UnSetState"));
	m_pCmdMgr->Register("NS_RolePKStateChange",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RolePKStateChange),	_T("NS_RolePKStateChange"));
	m_pCmdMgr->Register("NS_AddRoleBuff",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_AddRoleBuff),			_T("NS_AddRoleBuff"));
	m_pCmdMgr->Register("NS_RemoveRoleBuff",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RemoveRoleBuff),		_T("NS_RemoveRoleBuff"));
	m_pCmdMgr->Register("NS_HitTarget",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_HitTarget),			_T("NS_HitTarget"));
	m_pCmdMgr->Register("NS_SyncGroundItem",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncGroundItem),		_T("NS_SyncGroundItem"));
	m_pCmdMgr->Register("NS_RoleExpChange",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleExpChange),		_T("NS_RoleExpChange"));
	m_pCmdMgr->Register("NS_RoleLevelChange",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleLevelChange),		_T("NS_RoleLevelChange"));
	m_pCmdMgr->Register("NS_RemoveRemote",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RemoveRemote),		_T("NS_RemoveRemote"));
	m_pCmdMgr->Register("NS_MonsterEnterCombat",(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MonsterEnterCombat),	_T("NS_MonsterEnterCombat"));
	m_pCmdMgr->Register("NS_OpenDoor",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_OpenDoor),			_T("NS_OpenDoor"));
	m_pCmdMgr->Register("NS_CloseDoor",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_CloseDoor),			_T("NS_CloseDoor"));
	m_pCmdMgr->Register("NS_SpecialMove",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SpecialMove),			_T("NS_SpecialMove"));
	m_pCmdMgr->Register("NS_RoleStyleAction",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleStyleAction),		_T("NS_RoleStyleAction"));
	m_pCmdMgr->Register("NS_MoveSpeedChange",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MoveSpeedChange),		_T("NS_MoveSpeedChange"));
    m_pCmdMgr->Register("NS_PlaySceneEffect",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_PlaySceneEffect),		_T("NS_PlaySceneEffect"));
    m_pCmdMgr->Register("NS_StopSceneEffect",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_StopSceneEffect),		_T("NS_StopSceneEffect"));
	m_pCmdMgr->Register("NS_MonsterPlayAction", (NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MonsterPlayAction),	_T("NS_MonsterPlayAction"));
	m_pCmdMgr->Register("NS_MonsterSay",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MonsterSay),			_T("NS_MonsterSay"));
	m_pCmdMgr->Register("NS_PlaySceneMusic",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_PlaySceneMusic),		_T("NS_PlaySceneMusic"));
	m_pCmdMgr->Register("NS_Mount",				(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_Mount),				_T("NS_Mount"));
	m_pCmdMgr->Register("NS_Mount2",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_Mount2),				_T("NS_Mount2"));
	m_pCmdMgr->Register("NS_GetRemoteRoleState",(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_GetRemoteRoleState),	_T("NS_GetRemoteRoleState"));
	m_pCmdMgr->Register("NS_MountInvite",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MountInvite),			_T("NS_MountInvite"));
    m_pCmdMgr->Register("NS_HitFly",		    (NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_HitFly),			    _T("NS_HitFly"));
    m_pCmdMgr->Register("NS_Fishing",		    (NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_Fishing),			    _T("NS_Fishing"));

	//--注册控制台命令
	m_pConsole->Register(_T("fp"),	m_trunk.sfp2(&CombatSys::OnCon_FindPath), _T("pathfinder test"), 2);

	//--创建子Frame
	m_pPKFrame=				(CombatFrame_PK*)			pMgr->CreateFrame(m_strName.c_str(),_T("CombatFrame_PK"),			_T("CombatFrame_PK"),0);
	m_pCursorFrame=			(CombatFrame_Cursor*)		pMgr->CreateFrame(m_strName.c_str(),_T("CombatFrame_Cursor"),		_T("CombatFrame_Cursor"),0);
	m_pMsgShowFrame=		(CombatFrame_MsgShow*)		pMgr->CreateFrame(m_strName.c_str(),_T("CombatFrame_MsgShow"),		_T("CombatFrame_MsgShow"),0);
	m_pEventCreaterFrame=	(CombatFrame_EventCreater*)	pMgr->CreateFrame(m_strName.c_str(),_T("CombatFrame_EventCreater"),	_T("CombatFrame_EventCreater"),0);

	return GameFrame::Init(pMgr,pFather,dwParam);
}

BOOL CombatSys::Destroy()
{
    m_multiObjId2EffectId.clear();

	//--注销游戏事件处理函数
	m_pMgr->UnRegisterEventHandler(_T("tagRotateCameraEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnRotateCameraEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagHitTargetEvent"),			(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnHitTargetEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagFollowRoleEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnFollowRoleEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagShowDeadMenuEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnShowDeadMenuEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagUseSkillEvent"),			(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnUseSkillEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagUseItemEvent"),			(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnUseItemEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagLPRoleStateChangeEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnLPRoleStateChangeEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagRPRoleStateChangeEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnRPRoleStateChangeEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagRoleBuffChangeEvent"),	(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnRoleBuffChangeEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagPlayPersonalityActEvent"),(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnPlayPersonalityActEvent));
	m_pMgr->UnRegisterEventHandler(_T("tagWeaponChangeEvent"),		(FRAMEEVENTPROC)m_trunk.sfp1(&CombatSys::OnWeaponChangeEvent));

	//--注销网络消息处理函数
	m_pCmdMgr->UnRegister("NS_SyncWalk",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncWalk));
	m_pCmdMgr->UnRegister("NS_SyncJump",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncJump));
	m_pCmdMgr->UnRegister("NS_SyncDrop",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncDrop));
	m_pCmdMgr->UnRegister("NS_SyncVDrop",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncVDrop));
	m_pCmdMgr->UnRegister("NS_SyncSlide",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncSlide));
	m_pCmdMgr->UnRegister("NS_SyncStand",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncStand));
	m_pCmdMgr->UnRegister("NS_MoveFailed",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MoveFailed));
	m_pCmdMgr->UnRegister("NS_Skill",				(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_Skill));
	m_pCmdMgr->UnRegister("NS_SkillInterrupt",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SkillInterrupt));
	m_pCmdMgr->UnRegister("NS_UseItem",				(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_UseItem));
	m_pCmdMgr->UnRegister("NS_UseItemInterrupt",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_UseItemInterrupt));
	m_pCmdMgr->UnRegister("NS_RoleDead",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleDead));
	m_pCmdMgr->UnRegister("NS_RoleRevive",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleRevive));
	m_pCmdMgr->UnRegister("NS_RoleHPChange",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleHPChange));
	m_pCmdMgr->UnRegister("NS_SetRoleState",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SetRoleState));
	m_pCmdMgr->UnRegister("NS_UnSetRoleState",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_UnSetRoleState));
	m_pCmdMgr->UnRegister("NS_SetState",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SetState));
	m_pCmdMgr->UnRegister("NS_UnSetState",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_UnSetState));
	m_pCmdMgr->UnRegister("NS_RolePKStateChange",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RolePKStateChange));
	m_pCmdMgr->UnRegister("NS_AddRoleBuff",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_AddRoleBuff));
	m_pCmdMgr->UnRegister("NS_RemoveRoleBuff",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RemoveRoleBuff));
	m_pCmdMgr->UnRegister("NS_HitTarget",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_HitTarget));
	m_pCmdMgr->UnRegister("NS_SyncGroundItem",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SyncGroundItem));
	m_pCmdMgr->UnRegister("NS_RoleExpChange",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleExpChange));
	m_pCmdMgr->UnRegister("NS_RoleLevelChange",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleLevelChange));
	m_pCmdMgr->UnRegister("NS_RemoveRemote",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RemoveRemote));
	m_pCmdMgr->UnRegister("NS_MonsterEnterCombat",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MonsterEnterCombat));
	m_pCmdMgr->UnRegister("NS_OpenDoor",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_OpenDoor));
	m_pCmdMgr->UnRegister("NS_CloseDoor",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_CloseDoor));
	m_pCmdMgr->UnRegister("NS_SpecialMove",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_SpecialMove));
	m_pCmdMgr->UnRegister("NS_RoleStyleAction",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_RoleStyleAction));
	m_pCmdMgr->UnRegister("NS_MoveSpeedChange",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MoveSpeedChange));
    m_pCmdMgr->UnRegister("NS_PlaySceneEffect",	    (NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_PlaySceneEffect));
    m_pCmdMgr->UnRegister("NS_StopSceneEffect",	    (NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_StopSceneEffect));
	m_pCmdMgr->UnRegister("NS_MonsterPlayAction",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MonsterPlayAction));
	m_pCmdMgr->UnRegister("NS_MonsterSay",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MonsterSay));
	m_pCmdMgr->UnRegister("NS_PlaySceneMusic",		(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_PlaySceneMusic));
	m_pCmdMgr->UnRegister("NS_Mount",				(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_Mount));
	m_pCmdMgr->UnRegister("NS_Mount2",				(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_Mount2));
	m_pCmdMgr->UnRegister("NS_GetRemoteRoleState",	(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_GetRemoteRoleState));
	m_pCmdMgr->UnRegister("NS_MountInvite",			(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_MountInvite));
    m_pCmdMgr->UnRegister("NS_HitFly",              (NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_HitFly));
	m_pCmdMgr->UnRegister("NS_Fishing",				(NETMSGPROC)m_trunk.sfp2(&CombatSys::OnNS_Fishing));

	//--销毁特效管理器
	EffectMgr::Inst()->Destroy();

	//--销毁行为检查器
	CombatActionChecker::Inst()->Destroy();

	//--销毁工具类
	CombatSysUtil::Inst()->Destroy();

	//--
	return GameFrame::Destroy();
}

VOID CombatSys::Update()
{
	//--更新特效管理器
	EffectMgr::Inst()->Update();

	//--更新地面物品处理器
	GroundItemMgr::Inst()->Update();
}

void CombatSys::RegisterFrames(GameFrameMgr* pMgr)
{
	pMgr->Register(TWrap<CombatFrame_PK>(),				_T("CombatFrame_PK"));
	pMgr->Register(TWrap<CombatFrame_Cursor>(),			_T("CombatFrame_Cursor"));
	pMgr->Register(TWrap<CombatFrame_MsgShow>(),		_T("CombatFrame_MsgShow"));
	pMgr->Register(TWrap<CombatFrame_EventCreater>(),	_T("CombatFrame_EventCreater"));
	pMgr->Register(TWrap<CombatFrame_DeadMenu>(),		_T("CombatFrame_DeadMenu"));
}

bool CombatSys::IsSelectEnemy()
{
	return m_pEventCreaterFrame->IsSelectEnemy();
}

DWORD CombatSys::GetCurTargetID()
{
	return m_pEventCreaterFrame->GetCurTargetID();
}

DWORD CombatSys::OnRotateCameraEvent( tagRotateCameraEvent* pEvent )
{
	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
	pLocalPlayer->OnGameEvent(pEvent);
	return 0;
}

DWORD CombatSys::OnHitTargetEvent( tagHitTargetEvent* pEvent )
{
	if(pEvent->bSingle)//单体攻击
	{
		Role* pRole = RoleMgr::Inst()->FindRole(pEvent->dwTarRoleID);
		if(pRole!=NULL)
			pRole->OnGameEvent(pEvent);
	}
	else//群攻
	{
		const map<DWORD,Role*>& roleMap=RoleMgr::Inst()->GetRoleMap();
		for(map<DWORD,Role*>::const_iterator iter=roleMap.begin();
			iter!=roleMap.end();++iter)
		{
			Role* pRole=iter->second;
			pRole->OnGameEvent(pEvent);
		}
		RoleMgr::Inst()->GetLocalPlayer()->OnGameEvent(pEvent);
	}
	return 0;
}

DWORD CombatSys::OnFollowRoleEvent( tagFollowRoleEvent* pEvent )
{
	if(!CombatActionChecker::Inst()->IfCanMove(true))
		return false;

	LocalPlayer* pLocalPlayer = RoleMgr::Inst()->GetLocalPlayer();
	pLocalPlayer->OnGameEvent(pEvent);
	return 0;
}


DWORD CombatSys::OnUseSkillEvent( tagUseSkillEvent* pEvent )
{
	//--生成TargetID
	DWORD curTargetID=m_pEventCreaterFrame->GetCurTargetID();

	LocalPlayer* pSelf = RoleMgr::Inst()->GetLocalPlayer();

	const tagSkillProtoClient* pSkillData=SkillProtoData::Inst()->FindSkillProto(pEvent->dwSkillID);
	if(pSkillData==NULL)
		return 0;

	//生产技能不发消息
	if(pSkillData->nType2 == ESSTE_Produce)
		return 0;

	if(pSkillData->eOPType==ESOPT_Explode
		&&pSkillData->fOPDist==0)//以自已为中心的爆炸技能?
	{
		if(pSkillData->fOPRadius==0)//对自已使用
			pEvent->dwTargetRoleID=pSelf->GetID();
		else//范围作用
			pEvent->dwTargetRoleID=GT_INVALID;
	}
	else
	{
		pEvent->dwTargetRoleID=curTargetID;

		//对于能对自身使用的技能，如果当前目标无效，自动将目标设置为自身
		if( (pSkillData->dwTargetLimit&ETF_Self)!=0
			&& !CombatActionChecker::Inst()->IfTargetIsValid(pEvent->dwTargetRoleID,
				pSkillData->dwTargetLimit,pSkillData->bHostile,pSkillData->bFriendly,pSkillData->bIndependent))
		{
			pEvent->dwTargetRoleID=pSelf->GetID();
		}
	}

	//--行为检查
	if(!CombatActionChecker::Inst()->IfCanUseSkill(pEvent->dwSkillID,pEvent->dwTargetRoleID,true))
		return 0;

	//--交给角色状态机处理
	pSelf->OnGameEvent(pEvent);
	return 0;
}

DWORD CombatSys::OnUseItemEvent( tagUseItemEvent* pEvent )
{
	//--生成TargetID
	DWORD curTargetID=m_pEventCreaterFrame->GetCurTargetID();

	LocalPlayer* pSelf = RoleMgr::Inst()->GetLocalPlayer();

	const tagItemProto* pItemData=ItemProtoData::Inst()->FindItemProto(pEvent->dwItemTypeID);
	if(pItemData==NULL)
		return 0;

	//给游戏事件设置是否带起手标志
	pEvent->bSpell=pItemData->nPrepareTime>0;

	if(pItemData->eOPType==ESOPT_Explode
		&&pItemData->fOPDist==0)//以自已为中心的爆炸物品?
	{
		if(pItemData->fOPRadius==0)//对自已使用
			pEvent->dwTargetRoleID=pSelf->GetID();
		else//范围作用
			pEvent->dwTargetRoleID=GT_INVALID;
	}
	else
	{
		pEvent->dwTargetRoleID=pSelf->GetID();
	}

	//--行为检查
	if(!CombatActionChecker::Inst()->IfCanUseItem(pEvent->dwItemTypeID,pEvent->dwTargetRoleID,true))
		return 0;

	//--交给角色状态机处理
	pSelf->OnGameEvent(pEvent);
	return 0;
}

DWORD CombatSys::OnWeaponChangeEvent( tagWeaponChangeEvent* pEvent )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pEvent->dwRoleID);
	if(pRole!=NULL)
		pRole->OnGameEvent(pEvent);
	return 0;
}

DWORD CombatSys::OnLPRoleStateChangeEvent(tagLPRoleStateChangeEvent* pEvent)
{
	Role* pRole = RoleMgr::Inst()->GetLocalPlayer();
	if(pRole!=NULL)
		pRole->OnGameEvent(pEvent);
	return 0;
}

DWORD CombatSys::OnRPRoleStateChangeEvent(tagRPRoleStateChangeEvent* pEvent)
{
	Role* pRole = RoleMgr::Inst()->FindRole(pEvent->dwRoleID);
	if(pRole!=NULL)
		pRole->OnGameEvent(pEvent);
	return 0;
}

DWORD CombatSys::OnShowDeadMenuEvent( tagShowDeadMenuEvent* pEvent )
{
	if( P_VALID(TObjRef<GameFrameMgr>()->GetFrame(_T("CombatFrame_DeadMenu"))) )
		return GT_INVALID;

	m_pDeadMenuFrame = (CombatFrame_DeadMenu*)TObjRef<GameFrameMgr>()->CreateFrame(m_strName.c_str(),
		_T("CombatFrame_DeadMenu"),	_T("CombatFrame_DeadMenu"),0);
	if(!P_VALID(m_pDeadMenuFrame))
		return GT_INVALID;

	return 0;
}

DWORD CombatSys::OnRoleBuffChangeEvent( tagRoleBuffChangeEvent* pEvent )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pEvent->dwRoleID);
	if(pRole!=NULL)
		pRole->OnGameEvent(pEvent);
	return 0;
}

DWORD CombatSys::OnPlayPersonalityActEvent( tagPlayPersonalityActEvent* pEvent )
{
	RoleMgr::Inst()->GetLocalPlayer()->OnGameEvent(pEvent);
	return 0;
}

DWORD CombatSys::OnNS_SyncWalk( tagNS_SyncWalk* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID, true);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SyncJump( tagNS_SyncJump* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID, true);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SyncDrop( tagNS_SyncDrop* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID, true);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SyncVDrop( tagNS_SyncVDrop* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID, true);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SyncSlide( tagNS_SyncSlide* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID, true);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SyncStand( tagNS_SyncStand* pNetCmd, DWORD dwParam )
{
	//IMSG(_T("recv NS_SyncStand roleid=%d\n"),pNetCmd->dwRoleID);

	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID, true);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_MoveFailed( tagNS_MoveFailed* pNetCmd, DWORD dwParam )
{
	RoleMgr::Inst()->GetLocalPlayer()->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_Skill( tagNS_Skill* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwSrcRoleID, true);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SkillInterrupt(tagNS_SkillInterrupt* pNetCmd, DWORD dwParam)
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_UseItemInterrupt(tagNS_UseItemInterrupt* pNetCmd, DWORD dwParam)
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_RoleHPChange( tagNS_RoleHPChange* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID,false,true);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_RoleDead( tagNS_RoleDead* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SetRoleState( tagNS_SetRoleState* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_UnSetRoleState( tagNS_UnSetRoleState* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SetState( tagNS_SetState* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_UnSetState( tagNS_UnSetState* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_RolePKStateChange( tagNS_RolePKStateChange* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_RoleRevive( tagNS_RoleRevive* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_AddRoleBuff( tagNS_AddRoleBuff* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_RemoveRoleBuff( tagNS_RemoveRoleBuff* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_HitTarget( tagNS_HitTarget* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SyncGroundItem( tagNS_SyncGroundItem* pNetCmd, DWORD dwParam )
{
	if(GT_INVALID==pNetCmd->dwOwnerID)//无所有者的物品
	{
		GroundItemMgr::Inst()->RoleSyncGroundItem(pNetCmd,dwParam);
	}
	else
	{
		Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwOwnerID);
		if(pRole!=NULL)
			pRole->OnNetCmd(pNetCmd);
	}
	return 0;
}

DWORD CombatSys::OnNS_RoleExpChange( tagNS_RoleExpChange* pNetCmd, DWORD dwParam )
{
	RoleMgr::Inst()->GetLocalPlayer()->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_RoleLevelChange( tagNS_RoleLevelChange* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_UseItem( tagNS_UseItem* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwSrcRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_RemoveRemote( tagNS_RemoveRemote* pNetCmd, DWORD )
{
	RoleMgr::Inst()->GetLocalPlayer()->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_MonsterEnterCombat( tagNS_MonsterEnterCombat* pNetCmd, DWORD )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_OpenDoor( tagNS_OpenDoor* pNetCmd, DWORD )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_CloseDoor( tagNS_CloseDoor* pNetCmd, DWORD )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_SpecialMove(tagNS_SpecialMove* pNetCmd, DWORD)
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_RoleStyleAction(tagNS_RoleStyleAction* pNetCmd, DWORD)
{
	Role* pSrcRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	Role* pDestRole = RoleMgr::Inst()->FindRole(pNetCmd->dwDstRoleID);
	if(pSrcRole!=NULL)
		pSrcRole->OnNetCmd(pNetCmd);
	if(pDestRole!=NULL)
		pDestRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_MoveSpeedChange( tagNS_MoveSpeedChange* pNetCmd, DWORD )
{
	Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

BOOL CombatSys::EscCancel()
{
	tagESCCancelEvent event;
	RoleMgr::Inst()->GetLocalPlayer()->OnGameEvent(&event);
	return event.bCanceled;
}

DWORD CombatSys::OnCon_FindPath( LPCTSTR szDestMap, LPVOID )
{
	const TCHAR* szCurMap=MapMgr::Inst()->GetCurMapName().c_str();
	if(_tcsicmp(szCurMap,szDestMap)==0)
		return 0;

	Vector3 switchPos;
	if(!MapMgr::Inst()->GetNavMap()->GetPlayerNavMap()->FindPathByMap(szCurMap,szDestMap,switchPos))
		return 0;

	tagMouseMoveEvent event;
	event.start=RoleMgr::Inst()->GetLocalPlayer()->GetPos();
	event.end=switchPos;
	event.bUserCtrl=true;
	
	RoleMgr::Inst()->GetLocalPlayer()->OnGameEvent(&event);

	return 1;
}

DWORD CombatSys::OnNS_PlaySceneEffect( tagNS_PlaySceneEffect* pNetCmd, DWORD )
{
    if (pNetCmd->eType == ESET_ByObjID)
    {
        // 取当前触发obj信息
        const tagMapTriggerEffect* triEffect = MapMgr::Inst()->GetGameMap().FindTriggerEffect(pNetCmd->dwObjID);
        if (P_VALID(triEffect))
        {
            DWORD effectId = EffectMgr::Inst()->PlayEffect(triEffect->szMdlPath,
                Vector3(triEffect->fPos[0], triEffect->fPos[1], triEffect->fPos[2]),
                Vector3(triEffect->fScale[0], triEffect->fScale[1], triEffect->fScale[2]),
                Vector3(triEffect->fRotate[0], triEffect->fRotate[1], triEffect->fRotate[2]));

            m_multiObjId2EffectId.insert(make_pair(pNetCmd->dwObjID, effectId));
        }
    }
    else if (pNetCmd->eType == ESET_ByPos)
    {
        // 从指定位置播特效
        EffectMgr::Inst()->PlayEffect(SceneEffectData::Inst()->FindSfxPath(pNetCmd->dwEffectID).c_str(),
            pNetCmd->vPos, Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
    }

    return 0;
}

DWORD CombatSys::OnNS_StopSceneEffect( tagNS_StopSceneEffect* pNetCmd, DWORD )
{
    multimap<DWORD, DWORD>::iterator itBegin, itEnd;
    itBegin = m_multiObjId2EffectId.lower_bound(pNetCmd->dwObjID);
    itEnd   = m_multiObjId2EffectId.upper_bound(pNetCmd->dwObjID);
    while(itBegin != itEnd)
    {
        EffectMgr::Inst()->StopEffect(itBegin->second);
        ++itBegin;
    }

    m_multiObjId2EffectId.erase(pNetCmd->dwObjID);

    return 0;
}

DWORD CombatSys::OnNS_MonsterPlayAction(tagNS_MonsterPlayAction *pNetCmd, DWORD)
{
	Role *pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if (P_VALID(pRole))
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_MonsterSay(tagNS_MonsterSay *pNetCmd, DWORD)
{
	Role *pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
	if (P_VALID(pRole))
		pRole->OnNetCmd(pNetCmd);
	return 0;
}

DWORD CombatSys::OnNS_PlaySceneMusic(tagNS_PlaySceneMusic *pNetCmd, DWORD)
{
	tagNS_PlaySceneMusic *pCmd = pNetCmd;
	tstring strName = _T("SCN");
	TCHAR buffer[12];
	_sntprintf(buffer, 12, _T("%d"), pCmd->dwMusicID);
	strName += buffer;
	switch (pCmd->eType)
	{
	case ESMT_2D:
		{
			TObjRef<AudioSys>()->Play2DSound(strName.c_str());
		} break;
	case ESMT_3D:
		{
			TObjRef<AudioSys>()->Play3DSound(strName.c_str(), 100.0f, 100.0f*50.0f, pCmd->vPos);
		} break;
	default: break;
	}
	return 0;
}

DWORD CombatSys::OnNS_Mount( tagNS_Mount *pNetCmd, DWORD )
{
	if( pNetCmd->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		RoleMgr::Inst()->GetLocalPlayer()->OnNetCmd(pNetCmd);
	}
	else
	{
		Role *pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
		if (P_VALID(pRole))
			pRole->OnNetCmd(pNetCmd);
	}

	return 0;
}

DWORD CombatSys::OnNS_Mount2( tagNS_Mount2 *pNetCmd, DWORD )
{
	if( pNetCmd->dwRoleID == RoleMgr::Inst()->GetLocalPlayerID() )
	{
		RoleMgr::Inst()->GetLocalPlayer()->OnNetCmd(pNetCmd);
	}
	else
	{
		Role *pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
		if (P_VALID(pRole))
			pRole->OnNetCmd(pNetCmd);
	}

	return 0;
}

DWORD CombatSys::OnNS_GetRemoteRoleState( tagNS_GetRemoteRoleState *pNetCmd, DWORD )
{
	Role *pRole = RoleMgr::Inst()->FindRole(pNetCmd->RoleData.dwID);
	if (P_VALID(pRole))
		pRole->OnNetCmd(pNetCmd);

	return 0;
}

DWORD CombatSys::OnNS_MountInvite( tagNS_MountInvite *pNetCmd, DWORD )
{
	RoleMgr::Inst()->GetLocalPlayer()->OnNetCmd(pNetCmd);

	return 0;
}

DWORD CombatSys::OnNS_HitFly( tagNS_HitFly* pNetCmd, DWORD dwParam )
{
    Role* pRole = RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID, true);
    if(pRole!=NULL)
        pRole->OnNetCmd(pNetCmd);
    return 0;
}

DWORD CombatSys::OnNS_Fishing( tagNS_Fishing* pNetCmd, DWORD dwParam )
{
	Role* pRole = RoleMgr::Inst()->GetLocalPlayer();
	if(pRole!=NULL)
		pRole->OnNetCmd(pNetCmd);
	return 0;
}