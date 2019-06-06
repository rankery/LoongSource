#pragma once

class CombatFrame_Cursor;
class CombatFrame_PK;
class CombatFrame_MsgShow;
class CombatFrame_EventCreater;
class CombatFrame_DeadMenu;
struct tagUseSkillEvent;
struct tagUseItemEvent;
struct tagRotateCameraEvent;
struct tagHitTargetEvent;
struct tagFollowRoleEvent;
struct tagShowDeadMenuEvent;
struct tagWeaponChangeEvent;
struct tagLPRoleStateChangeEvent;
struct tagRPRoleStateChangeEvent;
struct tagRoleBuffChangeEvent;
struct tagPlayPersonalityActEvent;
struct tagNS_SyncWalk;
struct tagNS_SyncJump;
struct tagNS_SyncDrop;
struct tagNS_SyncVDrop;
struct tagNS_SyncSlide;
struct tagNS_SyncStand;
struct tagNS_MoveFailed;
struct tagNS_Skill;
struct tagNS_SkillInterrupt;
struct tagNS_UseItem;
struct tagNS_UseItemInterrupt;
struct tagNS_RoleHPChange;
struct tagNS_RoleDead;
struct tagNS_SetRoleState;
struct tagNS_UnSetRoleState;
struct tagNS_SetState;
struct tagNS_UnSetState;
struct tagNS_RolePKStateChange;
struct tagNS_RoleRevive;
struct tagNS_AddRoleBuff;
struct tagNS_RemoveRoleBuff;
struct tagNS_HitTarget;
struct tagNS_PutDownItem;
struct tagNS_RoleExpChange;
struct tagNS_RoleLevelChange;
struct tagNS_RemoveRemote;
struct tagNS_SyncGroundItem;
struct tagNS_MonsterEnterCombat;
struct tagNS_OpenDoor;
struct tagNS_CloseDoor;
struct tagNS_SpecialMove;
struct tagNS_RoleStyleAction;
struct tagNS_MoveSpeedChange;
struct tagNS_PlaySceneEffect;
struct tagNS_StopSceneEffect;
struct tagNS_MonsterPlayAction;
struct tagNS_MonsterSay;
struct tagNS_PlaySceneMusic;
struct tagNS_Mount;
struct tagNS_Mount2;
struct tagNS_GetRemoteRoleState;
struct tagNS_MountInvite;
struct tagNS_HitFly;
struct tagNS_Fishing;

/** \class CombatSys
	\brief 战斗系统
	\remark 负责控制游戏中所有角色的行为
*/
class CombatSys :
	public GameFrame
{
public:
	CombatSys(void);
	virtual ~CombatSys(void);

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();
	virtual BOOL EscCancel();

	//--
	bool IsSelectEnemy();
	DWORD GetCurTargetID();

	/** 注册子Frame
	*/
	static void RegisterFrames(GameFrameMgr* pMgr);

private:
	DWORD OnRotateCameraEvent(tagRotateCameraEvent* pEvent);
	DWORD OnHitTargetEvent(tagHitTargetEvent* pEvent);
	DWORD OnFollowRoleEvent(tagFollowRoleEvent* pEvent);
	DWORD OnShowDeadMenuEvent(tagShowDeadMenuEvent* pEvent);
	DWORD OnUseSkillEvent(tagUseSkillEvent* pEvent);
	DWORD OnUseItemEvent(tagUseItemEvent* pEvent);
	DWORD OnWeaponChangeEvent(tagWeaponChangeEvent* pEvent);
	DWORD OnLPRoleStateChangeEvent(tagLPRoleStateChangeEvent* pEvent);
	DWORD OnRPRoleStateChangeEvent(tagRPRoleStateChangeEvent* pEvent);
	DWORD OnRoleBuffChangeEvent(tagRoleBuffChangeEvent* pEvent);
	DWORD OnPlayPersonalityActEvent(tagPlayPersonalityActEvent* pEvent);

	DWORD OnNS_SyncWalk(tagNS_SyncWalk* pNetCmd, DWORD dwParam);
	DWORD OnNS_SyncJump(tagNS_SyncJump* pNetCmd, DWORD dwParam);
	DWORD OnNS_SyncDrop(tagNS_SyncDrop* pNetCmd, DWORD dwParam);
	DWORD OnNS_SyncVDrop(tagNS_SyncVDrop* pNetCmd, DWORD dwParam);
	DWORD OnNS_SyncSlide(tagNS_SyncSlide* pNetCmd, DWORD dwParam);
	DWORD OnNS_SyncStand(tagNS_SyncStand* pNetCmd, DWORD dwParam);
	DWORD OnNS_MoveFailed(tagNS_MoveFailed* pNetCmd, DWORD dwParam);
	DWORD OnNS_Skill(tagNS_Skill* pNetCmd, DWORD dwParam);
	DWORD OnNS_SkillInterrupt(tagNS_SkillInterrupt* pNetCmd, DWORD dwParam);
	DWORD OnNS_UseItem(tagNS_UseItem* pNetCmd, DWORD dwParam);
	DWORD OnNS_UseItemInterrupt(tagNS_UseItemInterrupt* pNetCmd, DWORD dwParam);
	DWORD OnNS_RoleHPChange(tagNS_RoleHPChange* pNetCmd, DWORD dwParam);
	DWORD OnNS_RoleDead(tagNS_RoleDead* pNetCmd, DWORD dwParam);
	DWORD OnNS_RoleRevive(tagNS_RoleRevive* pNetCmd, DWORD dwParam);
	DWORD OnNS_SetRoleState(tagNS_SetRoleState* pNetCmd, DWORD dwParam);
	DWORD OnNS_UnSetRoleState(tagNS_UnSetRoleState* pNetCmd, DWORD dwParam);
	DWORD OnNS_SetState(tagNS_SetState* pNetCmd, DWORD dwParam);
	DWORD OnNS_UnSetState(tagNS_UnSetState* pNetCmd, DWORD dwParam);
	DWORD OnNS_RolePKStateChange(tagNS_RolePKStateChange* pNetCmd, DWORD dwParam);
	DWORD OnNS_AddRoleBuff(tagNS_AddRoleBuff* pNetCmd, DWORD dwParam);
	DWORD OnNS_RemoveRoleBuff(tagNS_RemoveRoleBuff* pNetCmd, DWORD dwParam);
	DWORD OnNS_HitTarget(tagNS_HitTarget* pNetCmd, DWORD dwParam);
	DWORD OnNS_SyncGroundItem(tagNS_SyncGroundItem* pNetCmd, DWORD dwParam);
	DWORD OnNS_RoleExpChange(tagNS_RoleExpChange* pNetCmd, DWORD dwParam);
	DWORD OnNS_RoleLevelChange(tagNS_RoleLevelChange* pNetCmd, DWORD dwParam);
	DWORD OnNS_RemoveRemote(tagNS_RemoveRemote* pNetCmd, DWORD);
	DWORD OnNS_MonsterEnterCombat(tagNS_MonsterEnterCombat* pNetCmd, DWORD);
	DWORD OnNS_OpenDoor(tagNS_OpenDoor* pNetCmd, DWORD);
	DWORD OnNS_CloseDoor(tagNS_CloseDoor* pNetCmd, DWORD);
	DWORD OnNS_SpecialMove(tagNS_SpecialMove* pNetCmd, DWORD);
	DWORD OnNS_RoleStyleAction(tagNS_RoleStyleAction* pNetCmd, DWORD);
	DWORD OnNS_MoveSpeedChange(tagNS_MoveSpeedChange* pNetCmd, DWORD);
    DWORD OnNS_PlaySceneEffect(tagNS_PlaySceneEffect* pNetCmd, DWORD);
    DWORD OnNS_StopSceneEffect(tagNS_StopSceneEffect* pNetCmd, DWORD);
	DWORD OnNS_MonsterPlayAction(tagNS_MonsterPlayAction *pNetCmd, DWORD);
	DWORD OnNS_MonsterSay(tagNS_MonsterSay *pNetCmd, DWORD);
	DWORD OnNS_PlaySceneMusic(tagNS_PlaySceneMusic *pNetCmd, DWORD);
	DWORD OnNS_Mount(tagNS_Mount *pNetCmd, DWORD);
	DWORD OnNS_Mount2(tagNS_Mount2 *pNetCmd, DWORD);
	DWORD OnNS_GetRemoteRoleState(tagNS_GetRemoteRoleState *pNetCmd, DWORD);
	DWORD OnNS_MountInvite(tagNS_MountInvite *pNetCmd, DWORD);
    DWORD OnNS_HitFly( tagNS_HitFly* pNetCmd, DWORD dwParam );
	DWORD OnNS_Fishing( tagNS_Fishing* pNetCmd, DWORD dwParam );
	DWORD OnCon_FindPath(LPCTSTR szDestMap, LPVOID);

private:
	TSFPTrunk<CombatSys>		m_trunk;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<Console>			m_pConsole;

	CombatFrame_EventCreater*	m_pEventCreaterFrame;
	CombatFrame_PK*				m_pPKFrame;
	CombatFrame_MsgShow*		m_pMsgShowFrame;
	CombatFrame_Cursor*			m_pCursorFrame;
	CombatFrame_DeadMenu*		m_pDeadMenuFrame;

    multimap<DWORD, DWORD>      m_multiObjId2EffectId;
};

inline CombatSys* GetCombatSys()
{
	return (CombatSys*)TObjRef<GameFrameMgr>()->GetFrame(_T("CombatSys"));
}

inline DWORD GetCurTargetID()
{
	CombatSys* pCombatSys = GetCombatSys();
	if( P_VALID( pCombatSys ) )
		return pCombatSys->GetCurTargetID();
	else
		return GT_INVALID;
}