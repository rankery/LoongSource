//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role.h
// author: Aslan
// actor:
// data: 2008-7-11
// last:
// brief: 人物数据结构
//-----------------------------------------------------------------------------
#pragma once

#include "stdafx.h"
#include "../WorldDefine/base_define.h"
#include "../WorldDefine/RoleDefine.h"
#include "../WorldDefine/move_define.h"
#include "../WorldDefine/talent_define.h"
#include "../WorldDefine/QuestDef.h"
#include "../WorldDefine/pk_define.h"
#include "../WorldDefine/SocialDef.h"
#include "../WorldDefine/skill_define.h"
#include "../WorldDefine/compose_define.h"
#include "../WorldDefine/ScriptMsgInfo.h"

#include "../WorldDefine/msg_compose.h"
#include "../WorldDefine/msg_pk.h"
#include "../WorldDefine/msg_talent.h"
#include "../WorldDefine/action.h"
#include "../WorldDefine/msg_combat.h"
#include "../WorldDefine/msg_gm_cmd.h"

#include "../WorldDefine/msg_guild.h"
#include "../WorldDefine/TreasureChest_define.h"
#include "../WorldDefine/penalty_define.h"

#include "../ServerDefine/role_data_define.h"
#include "../ServerDefine/msg_skill.h"

#include "unit.h"
#include "item_mgr.h"
#include "currency.h"
#include "db_session.h"
#include "player_session.h"
#include "exchange.h"
#include "pet_exchange.h"
#include "suit.h"
#include "quest.h"
#include "stall.h"
#include "group_mgr.h"
#include "team.h"
#include "vcard.h"
#include "clandata.h"
#include "state_mgr.h"
#include "mutex.h"
#include "longhun.h"


class PlayerSession;
class Map;
class Quest;
class Skill;
class Creature;
class Stall;
class Team;
class RoleScript;
class TitleMgr;
class PetPocket;

struct tagRoleDataLoad;
struct tagQuestDoneSave;
struct tagRoleTalent;
struct tagNDBC_SaveRole;
struct tagIMEffect;

//--------------------------------------------------------------------------------
// 被动技能和装备的触发器类型
//--------------------------------------------------------------------------------
enum EPassiveSkillAndEquipTrigger
{
	EPSAET_Null				=	-1,
	EPSAET_BeAttacked		=	0,	// 被攻击
	EPSAET_Hit				=	1,	// 命中
	EPSAET_Hited			=	2,	// 被命中
	EPSAET_Dodged			=	3,	// 被闪避
	EPSAET_Dodge			=	4,	// 闪避
	EPSAET_Blocked			=	5,	// 被格挡
	EPSAET_Block			=	6,	// 格挡
	EPSAET_Crit				=	7,	// 暴击
	EPSAET_Crited			=	8,	// 被暴击
	EPSAET_Die				=	9,	// 死亡
	EPSAET_Random			=	10,	// 随机
	EPSAET_Attack			=	11,	// 攻击
	EPSAET_Kill				=	12,	// 击杀
	EPSAET_End				=	13
};

typedef VOID (Role::*pFun_RegTriggerEquipSet)(DWORD, DWORD, INT16);
//------------------------------------------------------------------------------
// 角色类
//------------------------------------------------------------------------------
class Role : public Unit, public ScriptData<ESD_Role>
{
public:
	friend class PlayerSession;
	friend class GMCommandMgr;

public:
	typedef TMap<UINT16, tagQuestDoneSave*>			QuestDoneMap;
	typedef TMap<UINT16, Quest*>					QuestMap;

	typedef State<DWORD, ERoleState>				RoleState;
	typedef State<DWORD, ERoleStateEx>				RoleStateEx;

protected:
	//---------------------------------------------------------------------------
	// Constructor&Destructor
	//---------------------------------------------------------------------------
	Role(DWORD dwRoleID, const tagRoleDataLoad* pData, PlayerSession* pSession);
	virtual ~Role();
public:
	static Role* Create(DWORD dwRoleID, const tagRoleDataLoad* pData, PlayerSession* pSession);
	static VOID	Delete(Role* &pRole);

	//---------------------------------------------------------------------------
	// 初始化
	//---------------------------------------------------------------------------
	VOID Init(const tagRoleDataLoad* pData);

	//---------------------------------------------------------------------------
	// 上线
	//---------------------------------------------------------------------------
	VOID Online(BOOL bFirst);

	//----------------------------------------------------------------------------
	// 第一次上线
	//----------------------------------------------------------------------------
	VOID VirginOnline();

	//----------------------------------------------------------------------------
	// 各种Update函数
	//----------------------------------------------------------------------------
	virtual VOID Update();

	//-----------------------------------------------------------------------------
	// 保存数据库
	//-----------------------------------------------------------------------------
	VOID SaveToDB();

	//-----------------------------------------------------------------------------
	// 加入到游戏世界中
	//-----------------------------------------------------------------------------
	BOOL AddToWorld(BOOL bFirst);

	//-----------------------------------------------------------------------------
	// 传送到到某个地图中
	//-----------------------------------------------------------------------------
	BOOL GotoNewMap(DWORD dwDestMapID, FLOAT fX, FLOAT fY, FLOAT fZ, DWORD dwMisc=0, BOOL bSameInstance=TRUE);

	//------------------------------------------------------------------------------
	// 回城
	//------------------------------------------------------------------------------
	VOID ReturnCity();

	//------------------------------------------------------------------------------
	// 改名
	//------------------------------------------------------------------------------
	VOID ReName(LPCTSTR szNewName);

	//------------------------------------------------------------------------------
	// 物品是否需要冷却
	//------------------------------------------------------------------------------
	BOOL ObjectCoolOff()
	{
		return m_bObjectCoolOff;
	}

	VOID SetObjectCoolMode(BOOL bMode)
	{
		m_bObjectCoolOff = bMode;
	}

	//------------------------------------------------------------------------------
	// 禁言
	//------------------------------------------------------------------------------
	BOOL SpeakOff() { return m_bSpeakOff; }
	VOID SetSpeakOff(BOOL bFlag) { m_bSpeakOff = bFlag; }

	//-----------------------------------------------------------------------
	// 检查是否在对应职能NPC附近
	//-----------------------------------------------------------------------
	DWORD CheckFuncNPC(DWORD dwNPCID, EFunctionNPCType eNPCType, OUT Creature **ppNPC = NULL, OUT Map **ppMap = NULL);

	//------------------------------------------------------------------------------
	// 当前状态 -- 各状态间的转换，需要手动完成。即SetState()不会自动清除其他状态位。
	//------------------------------------------------------------------------------
	DWORD GetRoleState() const { return m_RoleState.GetState(); }

	BOOL IsInRoleState(ERoleState eState) const
	{
		return m_RoleState.IsInState(eState);
	}

	BOOL IsInRoleStateAll(DWORD dwState) const
	{
		return m_RoleState.IsInStateAll(dwState);
	}

	BOOL IsInRoleStateAny(DWORD dwState) const
	{
		return m_RoleState.IsInStateAny(dwState);
	}

	VOID SetRoleState(ERoleState eState, BOOL bSendMsg=TRUE)
	{
		m_RoleState.SetState(eState);

		if( bSendMsg )
		{
			tagNS_SetRoleState send;
			send.dwRoleID = GetID();
			send.eState = eState;
			if( P_VALID(GetMap()) )
			{
				GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
			}
		}
	}

	VOID UnsetRoleState(ERoleState eState, BOOL bSendMsg=TRUE)
	{
		if( FALSE == IsInRoleState(eState) ) return;

		m_RoleState.UnsetState(eState);

		if( bSendMsg )
		{
			tagNS_UnSetRoleState send;
			send.dwRoleID = GetID();
			send.eState = eState;
			if( P_VALID(GetMap()) )
			{
				GetMap()->SendBigVisTileMsg(this, &send, send.dwSize);
			}
		}
	}

	//------------------------------------------------------------------------------
	// 扩展状态 -- 该状态只需玩家本人知道就可以
	//------------------------------------------------------------------------------
	const RoleStateEx& GetRoleStateEx() const
	{
		return m_RoleStateEx;
	}
	
	VOID SetRoleStateEx(ERoleStateEx eState, BOOL bSendMsg=TRUE)
	{
		if( TRUE == m_RoleStateEx.IsInState(eState) ) return;

		m_RoleStateEx.SetState(eState);

		if( bSendMsg )
		{
			tagNS_SetRoleStateEx send;
			send.eState = eState;
			SendMessage(&send, send.dwSize);
		}
	}

	VOID UnsetRoleStateEx(ERoleStateEx eState, BOOL bSendMsg=TRUE)
	{
		if( FALSE == m_RoleStateEx.IsInState(eState) ) return;

		m_RoleStateEx.UnsetState(eState);

		if( bSendMsg )
		{
			tagNS_UnSetRoleStateEx send;
			send.eState = eState;
			SendMessage(&send, send.dwSize);
		}
	}

	//------------------------------------------------------------------------------
	// 是否处在不能移动的状态
	//------------------------------------------------------------------------------
	virtual BOOL IsInStateCantMove()
	{
		return Unit::IsInStateCantMove() || IsInRoleState(ERS_Stall) ;
	}

	//------------------------------------------------------------------------------
	// 是否处在不能使用技能的状态
	//------------------------------------------------------------------------------
	virtual BOOL IsInStateCantCastSkill()
	{
		return Unit::IsInStateCantCastSkill() || IsInRoleState(ERS_Stall) || IsInRoleState(ERS_Wedding);
	}

	//------------------------------------------------------------------------------
	// 是否处在不可被攻击状态
	//------------------------------------------------------------------------------
	virtual BOOL IsInStateCantBeSkill()
	{
		return Unit::IsInStateCantBeSkill() || IsInRoleState(ERS_Stall) || IsInRoleState(ERS_Wedding);
	}
	//------------------------------------------------------------------------------
	//是否处在不可以被邀请组队的状态
	//------------------------------------------------------------------------------
	virtual BOOL IsInStateCantBeInviteJoinTeam()
	{ 
		return Unit::IsInStateCantBeInviteJoinTeam() || IsInRoleState(ERS_Wedding);
	} 
	//------------------------------------------------------------------------------
	// 是否处在不可见的状态
	//------------------------------------------------------------------------------
	virtual BOOL IsInStateInvisible()
	{
		return Unit::IsInStateInvisible();
	}

	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	// 检测所在地图区域
	//------------------------------------------------------------------------------
	VOID CheckMapArea();
	
	//------------------------------------------------------------------------------
	// 金钱和物品
	//------------------------------------------------------------------------------
	CurrencyMgr&	GetCurMgr()		{ return m_CurMgr; }
	ItemMgr&		GetItemMgr()	{ return m_ItemMgr; }

	//-------------------------------------------------------------------------------
	// 人物数据获取
	//-------------------------------------------------------------------------------
	EClassType				GetClass()		const { return m_eClass; }
	EClassTypeEx			GetClassEx()	const { return m_eClassEx; }
	INT						GetCredit()		const { return m_nCredit; }
	INT						GetIdentity()	const { return m_nIdentity; }
	INT						GetVIPPoint()	const { return m_nVIPPoint; }
	PlayerSession*			GetSession()	const { return m_pSession; }
	const tagAvatarAtt*		GetAvatar()		const { return &m_Avatar; }
	const tagDisplaySet&	GetDisplaySet()	const { return m_DisplaySet; }
	ERolePKState			GetPKState()	const { return m_ePKState; }
	DWORD					GetRebornMapID()const { return m_dwRebornMapID; }
	DWORD					GetNameID()		const { return GetID(); }			//?? 该接口需进一步处理
	DWORD					GetTeamID()		const { return m_dwTeamID; }
	DWORD					GetGroupID()	const { return m_dwGroupID; }
	DWORD					GetTeamInvite()	const { return m_dwTeamInvite; }
	DWORD					GetEarnRate()	const { return GetSession()->GetFatigueGuarder().GetEarnRate();}
	virtual BYTE			GetSex()		const { return m_Avatar.bySex; }
	INT						GetOnlineTime() const { return m_nOnlineTime; }
	tagDWORDTime			GetLogoutTime() const { return m_LogoutTime; }

	DWORD					GetStrength()	const { return m_Strength.dwValue; }
	DWORD					GetErrantry()	const { return m_Strength.m_dwErrantry; }
	DWORD					GetHeroism()	const { return m_Strength.m_dwHeroism; }
	DWORD					GetValor()		const { return m_Strength.m_dwValor; }
	DWORD					GetWit()		const { return m_Strength.m_dwWit; }
	DWORD					GetStrengthNum() const {return m_nStrengthNum; }
	

	const tagRemoteOpenSet& GetRemoteOpenSet() const { return m_sRemoteOpenSet; }

	LPCTSTR					GetVNBName() const { return GetSession()->GetVNBName(); }

	const tagAvatarEquip&	GetAvatarEquip() const
	{
		return GetDisplaySet().bFashionDisplay ? m_AvatarEquipFashion : m_AvatarEquipEquip; 
	}

	const tagChestItem&		GetChestItem()	const	{return m_TreasureState.ChestItem;}
	INT64					GetChestSerial() const	{return m_TreasureState.nChestSerial;}
	INT64					GetKeySerial() const	{return m_TreasureState.nKeySerial;}
	DWORD					GetChestTypeID() const	{return m_TreasureState.dwChestTypeID;}
	DWORD					GetKeyTypeID() const	{return m_TreasureState.dwKeyTypeID;}

	//-------------------------------------------------------------------------------
	// 人物数据设置
	//-------------------------------------------------------------------------------

	VOID SetClass(EClassType eVocation)		
	{
		m_eClass = eVocation;
	}
	
	VOID SetClassEx(EClassTypeEx eHVocation)	
	{
		m_eClassEx = eHVocation;
	}

	VOID SetStrength(DWORD dwStrength, DWORD dwHeroism, DWORD dwWit, DWORD dwErrantry, DWORD dwValor)
	{
		m_Strength.dwValue = dwStrength;			// 实力值
		m_Strength.m_dwHeroism = dwHeroism;			// 英雄值
		m_Strength.m_dwWit = dwWit;					// 谋略值
		m_Strength.m_dwErrantry = dwErrantry;		// 侠义值
		m_Strength.m_dwValor = dwValor;				// 征战值
	}

	VOID SetChestTypeID(DWORD dwTypeID)
	{
		m_TreasureState.dwChestTypeID = dwTypeID;
	}

	VOID SetKeyTypeID(DWORD dwTypeID)
	{
		m_TreasureState.dwKeyTypeID = dwTypeID;
	}

	VOID SetChestSerial(INT64 nSerial)
	{
		m_TreasureState.nChestSerial = nSerial;
	}

	VOID SetKeySerial(INT64 nSerial)
	{
		m_TreasureState.nKeySerial = nSerial;
	}

	VOID SetChestItem(tagChestItem item)
	{
		m_TreasureState.ChestItem = item;
	}

	VOID SetFashionMode(bool bDisplay)
	{
		m_DisplaySet.bFashionDisplay = bDisplay;
	}

	VOID SetDisplaySet(bool bHead, bool bFace, bool bBack)
	{
		m_DisplaySet.Set(bHead, bFace, bBack);
	}

	VOID SetEquipDisplay(BOOL bFashion, INT nDisplayPos, DWORD dwTypeID, BYTE byFlareVal, INT8 n8ColorID)
	{
		if(bFashion)	// 时装
		{
			m_AvatarEquipFashion.Set(nDisplayPos, dwTypeID, byFlareVal, n8ColorID);
		}
		else	// 武器或防具
		{
			m_AvatarEquipEquip.Set(nDisplayPos, dwTypeID, byFlareVal, n8ColorID);

			// 如果是武器，则做特殊处理
			if(MIsWeaponByDisplayPos(nDisplayPos))
			{
				m_AvatarEquipFashion.Set(nDisplayPos, dwTypeID, byFlareVal, n8ColorID);
			}
		}
	}

	// 龙魂能力特效需要单独设置
	VOID SetEquipEffect(INT nDisplayPos, BYTE byEquipEffect)
	{
		m_AvatarEquipEquip.Set(nDisplayPos, byEquipEffect);
		m_AvatarEquipFashion.Set(nDisplayPos, byEquipEffect);
	}

	// 取得可激发龙魂ID
	LongHun& GetLongHun()				{ return m_LongHun; }

	// 设置记录式传送符的坐标
	VOID SetItemTransportPoint(tagNetCmd* pCmd);

	// 使用记录式传送符
	VOID UseNoteTakingItemTransport();
	//-------------------------------------------------------------------------------
	// 帮派相关
	//-------------------------------------------------------------------------------
	BOOL	IsInGuild()	const			{ return m_dwGuildID != GT_INVALID; }
	DWORD	GetGuildID() const			{ return m_dwGuildID; }
	VOID	SetGuildID(DWORD dwGuildID);

	//-------------------------------------------------------------------------------
	// 是否需要保存到数据库
	//-------------------------------------------------------------------------------
	BOOL IsNeedSave2DB() const { return m_nNeedSave2DBCountDown <= 0; }

	//-------------------------------------------------------------------------------
	// 发送消息
	//-------------------------------------------------------------------------------
	VOID SendMessage(LPVOID pMsg, DWORD dwSize) { if( P_VALID(GetSession()) ) { GetSession()->SendMessage(pMsg, dwSize); } }

	//---------------------------------------------------------------------------------
	// 投属性点
	//---------------------------------------------------------------------------------
	INT BidAttPoint(const INT nAttPointAdd[X_ERA_ATTA_NUM]);

	//---------------------------------------------------------------------------------
	// 洗属性点
	//---------------------------------------------------------------------------------
	INT ClearAttPoint(INT64 n64ItemID, BOOL bCheckItem=TRUE);

	//---------------------------------------------------------------------------------
	// 学技能
	//---------------------------------------------------------------------------------
	INT	LearnSkill(DWORD dwSkillID, DWORD dwNPCID=GT_INVALID, INT64 n64ItemID=GT_INVALID);

	//---------------------------------------------------------------------------------
	// 职业判断
	//---------------------------------------------------------------------------------
	DWORD VocationCheck(DWORD dwVocationLimit);

	//---------------------------------------------------------------------------------
	// 升级技能
	//---------------------------------------------------------------------------------
	INT LevelUpSkill(DWORD dwSkillID, DWORD dwNPCID=GT_INVALID, INT64 n64ItemID=GT_INVALID);

	//---------------------------------------------------------------------------------
	// 遗忘技能
	//---------------------------------------------------------------------------------
	INT ForgetSkill(DWORD dwSkillID, DWORD dwNPCID);

	//---------------------------------------------------------------------------------
	// 洗点
	//---------------------------------------------------------------------------------
	INT	ClearTalent(INT64 n64ItemID, BOOL bCheckItem=TRUE);

	//---------------------------------------------------------------------------------
	// 开启/关闭PK保护
	//---------------------------------------------------------------------------------
	INT OpenSafeGuard(INT64 n64Item);
	INT CloseSafeGuard();

	//---------------------------------------------------------------------------------
	// 开启/更新行凶状态
	//---------------------------------------------------------------------------------
	INT SetPK(BOOL bOnlyPK);
	VOID ResetPK();
	VOID UpdatePK();

	//---------------------------------------------------------------------------------
	// 开启/更新PVP状态
	//---------------------------------------------------------------------------------
	VOID SetPVP();
	VOID UpdatePVP();

	//---------------------------------------------------------------------------------
	// 计算PK状态
	//---------------------------------------------------------------------------------
	VOID CalPKState(BOOL bSendMsg=TRUE);

	//---------------------------------------------------------------------------------
	// 设置戾气为0时回城复活开启pk保护
	//---------------------------------------------------------------------------------
	VOID SetHostilitySafeGuard();
	VOID UpdateHostilitySafeGuard();

	//---------------------------------------------------------------------------------
	// 复活
	//---------------------------------------------------------------------------------
	VOID BeRevived(INT nHP, INT nMP, Unit* pSrc);

	//---------------------------------------------------------------------------------
	// 同步单向好友是否在线
	//---------------------------------------------------------------------------------
	DWORD UpdateFriOnline();

	//---------------------------------------------------------------------------------
	// 目标是否完全可见//??
	//---------------------------------------------------------------------------------
	BOOL CanSeeTargetEntirely(Unit *pUnit)
	{
		return (FriendEnemy(pUnit) & ETFE_Friendly) == ETFE_Friendly;
	}

	//---------------------------------------------------------------------------------
	// 任务设置复活点
	//---------------------------------------------------------------------------------
	DWORD QuestSetRebornMap(UINT16 u16QuestID, DWORD &dwBornMapID, Vector3 &vBornPos);

	//---------------------------------------------------------------------------------
	// 添加，删除和改变技能技能
	//---------------------------------------------------------------------------------
	virtual VOID AddSkill(Skill* pSkill, BOOL bSendMsg=TRUE)
	{
		Unit::AddSkill(pSkill);

		if( bSendMsg )
		{
			// 发送给自己添加一个技能
			tagNS_AddSkill send;
			pSkill->GenerateMsgInfo(&send.Skill);
			SendMessage(&send, send.dwSize);

			// 保存到数据库
			tagNDBC_AddSkill send1;
			send1.dwRoleID = GetID();
			send1.Skill.dwID = pSkill->GetID();
			send1.Skill.nSelfLevel = pSkill->GetSelfLevel();
			send1.Skill.nLearnLevel = pSkill->GetLearnLevel();
			send1.Skill.nProficiency = pSkill->GetProficiency();
			send1.Skill.nCoolDown = pSkill->GetCoolDownCountDown();
			g_dbSession.Send(&send1, send1.dwSize);
		}
	};

	virtual VOID RemoveSkill(DWORD dwSkillID)
	{
		Unit::RemoveSkill(dwSkillID);

		// 发送给自己删除一个技能
		tagNS_RemoveSkill send;
		send.dwSkillID = dwSkillID;
		SendMessage(&send, send.dwSize);

		// 发送到数据库删除
		tagNDBC_RemoveSkill send1;
		send1.dwRoleID = GetID();
		send1.dwSkillID = dwSkillID;
		g_dbSession.Send(&send1, send1.dwSize);
	}

	virtual VOID ChangeSkillLevel(Skill* pSkill, ESkillLevelChange eType, INT nChange=1)
	{
		Unit::ChangeSkillLevel(pSkill, eType, nChange);

		// 发送给自己更新一个技能
		tagNS_UpdateSkill send;
		pSkill->GenerateMsgInfo(&send.Skill);
		SendMessage(&send, send.dwSize);

		tagNDBC_UpdateSkill send1;
		send1.dwRoleID = GetID();
		send1.Skill.dwID = pSkill->GetID();
		send1.Skill.nSelfLevel = pSkill->GetSelfLevel();
		send1.Skill.nLearnLevel = pSkill->GetLearnLevel();
		send1.Skill.nProficiency = pSkill->GetProficiency();
		send1.Skill.nCoolDown = pSkill->GetCoolDownCountDown();
		g_dbSession.Send(&send1, send1.dwSize);
	}

	virtual VOID ChangeSkillExp(Skill *pSkill, INT nValue)
	{
		Unit::ChangeSkillExp(pSkill, nValue);
		
		// 发送给自己更新一个技能
		tagNS_UpdateSkill send;
		pSkill->GenerateMsgInfo(&send.Skill);
		SendMessage(&send, send.dwSize);
	}

	virtual VOID StartSkillCoolDown(Skill* pSkill)
	{
		Unit::StartSkillCoolDown(pSkill);

		// 发送给自己更新技能CD
		tagNS_UpdateSkillCoolDown send;
		send.dwSkillID = pSkill->GetID();
		send.nCoolDown = pSkill->GetCoolDownCountDown();
		SendMessage(&send, send.dwSize);
		
	}

	virtual VOID ClearSkillCoolDown(DWORD dwSkillID)
	{
		Unit::ClearSkillCoolDown(dwSkillID);

		// 发送给自己更新技能CD
		tagNS_UpdateSkillCoolDown send;
		send.dwSkillID = dwSkillID;
		send.nCoolDown = 0;
		SendMessage(&send, send.dwSize);
	}

	//--------------------------------------------------------------------------------
	// 属性改变引起的其它内容改变
	//--------------------------------------------------------------------------------
	virtual VOID OnAttChange(INT nIndex);
	virtual VOID OnAttChange(bool bRecalFlag[ERA_End]);

	//--------------------------------------------------------------------------------
	// 获得或减少经验
	//--------------------------------------------------------------------------------
	VOID ExpChange(INT nValue, BOOL bKill=FALSE, BOOL bForce=FALSE);

	//--------------------------------------------------------------------------------
	// 改变等级
	//--------------------------------------------------------------------------------
	VOID LevelChange(INT nValue, BOOL bKill=FALSE, BOOL bForce=FALSE);

	//--------------------------------------------------------------------------------
	// 设置套装特效
	//--------------------------------------------------------------------------------
	VOID SetSuitEffect(DWORD dwSuitEffect)
	{ 
		m_AvatarEquipEquip.dwSuitEffectID = dwSuitEffect;
		m_AvatarEquipFashion.dwSuitEffectID = dwSuitEffect;
	}

	//--------------------------------------------------------------------------------
	// 换装相关
	//--------------------------------------------------------------------------------
	VOID  ProcEquipEffect(tagEquip* pNewEquip, tagEquip* pOldEquip, INT16 n16IndexOld, BOOL bDiscard = FALSE);
	INT32 GetEquipDisplayPos(INT16 n16EquipPos);
	DWORD Equip(INT64 n64Serial, EEquipPos ePosDst);
	VOID  ResetWeaponDmg(tagEquip& Equip);				// 崭新度变化时调用

	//--------------------------------------------------------------------------------
	// 角色采集技能加成
	//--------------------------------------------------------------------------------
	INT CalGatherRate( Creature* pCreature );

	//--------------------------------------------------------------------------------
	// 角色实际打怪获得经验计算
	//--------------------------------------------------------------------------------
	INT CalRealGainExp( INT nSrcExp );

	//----------------------------------------------------------------------------------
	// 根据索引得到好友列表的相应位置
	//----------------------------------------------------------------------------------
	tagFriend GetFriend(INT nIndex)
	{
		ASSERT(nIndex < MAX_FILENAME && nIndex >= 0);
		return m_Friend[nIndex];
	}

	VOID SetFriend(INT nIndex, DWORD dwFriendID, DWORD dwFriVal = 0, BYTE byGroup = 1)
	{
		ASSERT(nIndex < MAX_FRIENDNUM && nIndex >= 0);
		ASSERT(byGroup < 5 && byGroup > 0);

		if(m_Friend[nIndex].dwFriendID == GT_INVALID && dwFriendID != GT_INVALID)
		{
			m_mapFriend.Add(dwFriendID, &m_Friend[nIndex]);
		}
		else if(m_Friend[nIndex].dwFriendID != GT_INVALID && dwFriendID == GT_INVALID)
		{
			m_mapFriend.Erase(m_Friend[nIndex].dwFriendID);
		}

		m_Friend[nIndex].dwFriendID = dwFriendID; 
		if(dwFriendID == GT_INVALID)
			m_Friend[nIndex].dwFriVal = 0;
		else
			m_Friend[nIndex].dwFriVal += dwFriVal;	
		m_Friend[nIndex].byGroup = byGroup;

		m_Friend[nIndex].dwFriVal = (m_Friend[nIndex].dwFriVal > MAX_FRIENDVAL) ? MAX_FRIENDVAL : m_Friend[nIndex].dwFriVal;
	}

	tagFriend* GetFriendPtr(DWORD dwFriendID) { return m_mapFriend.Peek(dwFriendID); }

	//----------------------------------------------------------------------------------
	// 根据索引得到黑名单的相应位置
	//----------------------------------------------------------------------------------
	DWORD GetBlackList(INT nIndex)
	{
		ASSERT(nIndex < MAX_BLACKLIST && nIndex >= 0);
		return m_dwBlackList[nIndex];
	}

	VOID SetBlackList(INT nIndex, DWORD dwRoleID)
	{
		ASSERT(nIndex < MAX_BLACKLIST && nIndex >= 0);
		m_dwBlackList[nIndex] = dwRoleID;
	}

	//----------------------------------------------------------------------------------
	// 设置小队和团队ID
	//----------------------------------------------------------------------------------
	VOID SetTeamID(DWORD dwTeamID)		{ m_dwTeamID = dwTeamID; }
	VOID SetGroupID(DWORD dwGroupID)	{ m_dwGroupID = dwGroupID; }
	VOID SetTeamInvite(DWORD dwRoleID)	{ m_dwTeamInvite = dwRoleID; }

	BOOL IsTeamMate(Role* pRole)		{ return GetTeamID() != GT_INVALID && GetTeamID() == pRole->GetTeamID(); }
	BOOL IsGuildMate(Role* pRole)		{ return GetGuildID() != GT_INVALID && GetGuildID() == pRole->GetGuildID(); }
	BOOL IsLover(Role* pRole)			{ return GetLoverID() != GT_INVALID && GetLoverID() == pRole->GetID(); }

	//----------------------------------------------------------------------------------
	// 与某个单位是不是友好的社会关系（队友，结拜，师徒，夫妻，同门等）
	//----------------------------------------------------------------------------------
	BOOL IsFriendlySocial(Role* pRole)	{ return IsTeamMate(pRole) || IsGuildMate(pRole) || IsLover(pRole); }

	//----------------------------------------------------------------------------------
	// 副本
	//----------------------------------------------------------------------------------
	DWORD		GetOwnInstanceID()	const;
	DWORD		GetOwnInstanceMapID() const;
	DWORD		GetMyOwnInstanceID() const			{ return m_dwOwnInstanceID; }
	DWORD		GetMyOwnInstanceMapID() const		{ return m_dwOwnInstanceMapID; }
	DWORD		GetExportMapID() const				{ return m_dwExportMapID; }
	Vector3&	GetExportPoint()					{ return m_vExport; }
	VOID		SetMyOwnInstanceID(DWORD dwID)		{ m_dwOwnInstanceID = dwID; }
	VOID		SetMyOwnInstanceMapID(DWORD dwID)	{ m_dwOwnInstanceMapID = dwID; }
	VOID		SetExportMapID(DWORD dwMapID)		{ m_dwExportMapID = dwMapID; }
	VOID		SetExportPoint(Vector3 &vExport)	{ m_vExport = vExport; }

	//----------------------------------------------------------------------------------
	// 商城相关
	//----------------------------------------------------------------------------------
	tagDWORDTime GetLastMallFreeTime() const { return m_LastGetMallFreeTime; }

	VOID SetLastGetMallFreeTime(DWORD dwNewLastGetTime) { m_LastGetMallFreeTime = dwNewLastGetTime; }
	
	//----------------------------------------------------------------------------------
	// 主动技能状态触发
	//----------------------------------------------------------------------------------
	BOOL OnActiveItemBuffTrigger(tagItem* pItem, ETriggerEventType eEvent, DWORD dwEventMisc1=GT_INVALID, DWORD dwEventMisc2=GT_INVALID);

	//----------------------------------------------------------------------------------
	// 被动技能状态触发
	//----------------------------------------------------------------------------------
	BOOL OnPassiveSkillBuffTrigger(Unit* pTarget, ETriggerEventType eEvent, DWORD dwEventMisc1=GT_INVALID, DWORD dwEventMisc2=GT_INVALID);

	//----------------------------------------------------------------------------------
	// 装备被动触发
	//----------------------------------------------------------------------------------
	BOOL OnEquipmentBuffTrigger(Unit* pTarget, ETriggerEventType eEvent, DWORD dwEventMisc1=GT_INVALID, DWORD dwEventMisc2=GT_INVALID);

	//----------------------------------------------------------------------------------
	// 注册触发器技能关联表
	//----------------------------------------------------------------------------------
	VOID RegisterTriggerSkillSet(ETriggerEventType eEvent, DWORD dwSkillID);

	//----------------------------------------------------------------------------------
	// 反注册触发器技能关联表
	//----------------------------------------------------------------------------------
	VOID UnRegisterTriggerSkillSet(ETriggerEventType eType, DWORD dwSkillID);

	//----------------------------------------------------------------------------
	// 注册触发器装备关联表
	//----------------------------------------------------------------------------
	VOID RegisterTriggerEquipSet(DWORD dwTriggerID, DWORD dwBuffID, INT16 n16EquipPos)
	{
		ASSERT(MIsInEquipPos(n16EquipPos));

		if( GT_INVALID == dwTriggerID || GT_INVALID == dwBuffID )
		{
			return;
		}

		EPassiveSkillAndEquipTrigger eTriggerType = PreRegisterTriggerEquipSet(dwTriggerID, dwBuffID, true);
		if( EPSAET_Null == eTriggerType ) return;

		m_bitsetEquipTrigger[eTriggerType][n16EquipPos] = true;
	}

	//----------------------------------------------------------------------------
	// 反注册触发器装备关联表
	//----------------------------------------------------------------------------
	VOID UnRegisterTriggerEquipSet(DWORD dwTriggerID, DWORD dwBuffID, INT16 n16EquipPos)
	{
		ASSERT(MIsInEquipPos(n16EquipPos));

		if( GT_INVALID == dwTriggerID || GT_INVALID == dwBuffID )
		{
			return;
		}

		EPassiveSkillAndEquipTrigger eTriggerType = PreRegisterTriggerEquipSet(dwTriggerID, dwBuffID, false);
		if( EPSAET_Null == eTriggerType ) return;

		m_bitsetEquipTrigger[eTriggerType][n16EquipPos] = false;
	}

	//----------------------------------------------------------------------------
	// 注册触发器龙魂关联表
	//----------------------------------------------------------------------------
	VOID RegisterTriggerLongHunSet(DWORD dwTriggerID, DWORD dwBuffID, INT16 n16EquipPos)
	{
		ASSERT(MIsInEquipPos(n16EquipPos));

		if( GT_INVALID == dwTriggerID || GT_INVALID == dwBuffID )
		{
			return;
		}

		EPassiveSkillAndEquipTrigger eTriggerType = PreRegisterTriggerEquipSet(dwTriggerID, dwBuffID, true);
		if( EPSAET_Null == eTriggerType ) return;

		m_bitsetLongHunTrigger[eTriggerType][n16EquipPos] = true;
	}

	//----------------------------------------------------------------------------
	// 反注册触发器龙魂关联表
	//----------------------------------------------------------------------------
	VOID UnRegisterTriggerLongHunSet(DWORD dwTriggerID, DWORD dwBuffID, INT16 n16EquipPos)
	{
		ASSERT(MIsInEquipPos(n16EquipPos));

		if( GT_INVALID == dwTriggerID || GT_INVALID == dwBuffID )
		{
			return;
		}

		EPassiveSkillAndEquipTrigger eTriggerType = PreRegisterTriggerEquipSet(dwTriggerID, dwBuffID, false);
		if( EPSAET_Null == eTriggerType ) return;

		m_bitsetLongHunTrigger[eTriggerType][n16EquipPos] = false;
	}

	//----------------------------------------------------------------------------------
	// 注册触发器套装关联表
	//----------------------------------------------------------------------------------
	VOID RegisterTriggerSuitSet(DWORD dwTriggerID, DWORD dwBuffID, DWORD dwSuitID)
	{
		if( GT_INVALID == dwTriggerID || GT_INVALID == dwBuffID )
		{
			return;
		}

		EPassiveSkillAndEquipTrigger eTriggerType = PreRegisterTriggerEquipSet(dwTriggerID, dwBuffID, true);
		if( EPSAET_Null == eTriggerType ) return;

		m_setSuitTrigger[eTriggerType].insert(dwSuitID);
	}

	//----------------------------------------------------------------------------------
	// 反注册触发器套装关联表
	//----------------------------------------------------------------------------------
	VOID UnRegisterTriggerSuitSet(DWORD dwTriggerID, DWORD dwBuffID, DWORD dwSuitID)
	{
		if( GT_INVALID == dwTriggerID || GT_INVALID == dwBuffID )
		{
			return;
		}

		EPassiveSkillAndEquipTrigger eTriggerType = PreRegisterTriggerEquipSet(dwTriggerID, dwBuffID, false);
		if( EPSAET_Null == eTriggerType ) return;

		m_setSuitTrigger[eTriggerType].erase(dwSuitID);
	}
	
	//----------------------------------------------------------------------------------
	// 夫妻相关
	//----------------------------------------------------------------------------------
	VOID	UpdateLoverID(DWORD dwLoverID)	{ m_dwLoverID = dwLoverID; }
	DWORD	GetLoverID()					{ return m_dwLoverID; }

	BOOL IsRoleMarried() { return (m_dwLoverID != GT_INVALID) ? TRUE:FALSE ; } //是否已婚
	BOOL IsRoleWedded() { return m_bHaveWedding; } //是否举行过婚礼


private:
	//----------------------------------------------------------------------------------
	// 装备相关buff预处理，并获取tigger类型
	//----------------------------------------------------------------------------------
	EPassiveSkillAndEquipTrigger PreRegisterTriggerEquipSet(DWORD dwTriggerID, DWORD dwBuffID, BOOL bEquip);

	//----------------------------------------------------------------------------------
	// 装备相关被动trigger触发处理
	//----------------------------------------------------------------------------------
	VOID OnEquipmentBuffTriggerCommon(ETriggerEventType eEvent, EPassiveSkillAndEquipTrigger eTriggerType);
	VOID OnEquipmentBuffTriggerLongHun(ETriggerEventType eEvent, EPassiveSkillAndEquipTrigger eTriggerType);
	VOID OnEquipmentBuffTriggerSuit(ETriggerEventType eEvent, EPassiveSkillAndEquipTrigger eTriggerType);

private:
	//------------------------------------------------------------------------------
	// 发送客户端初始属性
	//------------------------------------------------------------------------------
	VOID SendInitStateAtt();
	VOID SendInitStateSkill();
	VOID SendInitStateItem();	// 物品&装备
	VOID SendInitStateSuit();	// 套装
	VOID SendInitStateLongHun();
	VOID SendInitStateCompleteQuest();
	VOID SendInitStateIncompleteQuest();
	VOID SendInitStateMoney();
	VOID SendInitStateReputation();
	VOID SendFriendAndEnemy();
	VOID SendInitStateGuild();				// 帮派
	VOID SendFatigueGuardInfo(BYTE byCode);	// 防沉迷

	//------------------------------------------------------------------------------
	// 初始化从数据库中读取的列表数据
	//------------------------------------------------------------------------------
	VOID InitAtt(const tagRoleDataSave* pData);
	VOID InitSkill(const BYTE* &pData, INT32 nNum);
	VOID InitBuff(const BYTE* &pData, INT32 nNum);
	VOID InitItem(const BYTE* &pData, INT32 nNum);
	VOID InitItemCDTime(const BYTE* &pData, INT32 nNum);
	VOID InitFriend(const BYTE* &pData, INT32 nNum);
	VOID InitFriendValue(const BYTE* &pData, INT32 nNum);
	VOID InitBlackList(const BYTE* &pData, INT32 nNum);

	DWORD Put2Container(tagItem* pItem);

	VOID CalInitAtt();

	VOID CalInitState();

	//------------------------------------------------------------------------------
	// 初始化需保存数据库的数据
	//------------------------------------------------------------------------------
	VOID SaveSkill2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum);
	VOID SaveBuff2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum);
	VOID SaveQuest2DB(OUT LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum);

	//------------------------------------------------------------------------------
	// 重置保存到数据库时间倒计时
	//------------------------------------------------------------------------------
	VOID ResetNeedSave2DBCD() { m_nNeedSave2DBCountDown = MIN_ROLE_SAVE2DB_INTERVAL; }

	//------------------------------------------------------------------------------
	// 复活
	//------------------------------------------------------------------------------
	DWORD SetRebornMap(DWORD dwNPCID, DWORD &dwBornMapID, Vector3 &vBornPos);
	DWORD Revive(ERoleReviveType eType, INT64 n64ItemID, BOOL bNeedItem=TRUE);

	DWORD CityRevive();
	DWORD LocusRevive(INT64 n64ItemID, BOOL bNeedItem=TRUE);
	DWORD AcceptRevive();
	DWORD PrisonRevive();
	//------------------------------------------------------------------------------
	// 换装
	//------------------------------------------------------------------------------
	DWORD	CanEquip(tagEquip* pEquip, EEquipPos ePosDst);
	DWORD	Unequip(INT64 n64Serial, INT16 n16IndexDst);
	DWORD	SwapWeapon();
	DWORD	MoveRing(INT64 n64SerialSrc, INT16 n16PosDst);
	VOID	ProcEquipEffectPos(tagEquip* pNewEquip, tagEquip* pOldEquip, EItemConType eConTypeNewDst, EItemConType eConTypeNewSrc);
	VOID	ProcEquipEffectAtt(tagEquip* pEquip, bool bEquip, const INT16 n16Index);
	VOID	ProcEquipEffectAvatar(tagEquip* pNewEquip, INT16 n16IndexOld);
	INT32	ResetOneEquipDisplay(tagEquip* pEquip, INT16 n16Index);
	VOID	ChangeRoleAtt(const tagRoleAttEffect Effect[], INT32 nArraySz, INT32 nFactor);
	VOID	ChangeRoleAtt(const INT32 nValue[], INT32 nArraySz, INT32 nAttStart, INT32 nFactor);
	VOID	ChangeRoleAtt(const DWORD dwValue[], INT32 nArraySz, INT32 nFactor);
	VOID	ProcEquipBuffTrigger(DWORD dwBuffID, BOOL bEquip);

	//------------------------------------------------------------------------------
	// 武器伤害相关计算
	//------------------------------------------------------------------------------
	/* 计算磨损对武器伤害影响.返回值为对武器伤害影响的百分比值.*/
	FLOAT	CalAbrasionEffect(const tagEquip& Equip);
	VOID	ResetWeaponDmg(const tagEquip& Equip, BOOL bEquip);	// 换装时调用

	//------------------------------------------------------------------------------
	// 玩家间交易相关
	//------------------------------------------------------------------------------
	ExchangeMgr& GetExchMgr() { return m_ExchMgr; }
	
	BOOL	IsExchanging()					{ return IsInRoleState(ERS_Exchange); }
	BOOL	CanExchange()
	{
		return !IsInRoleStateAny(ERS_Exchange | ERS_Shop | ERS_Stall | ERS_Wedding) 
				&& (GetExchMgr().GetTgtRoleID() == GT_INVALID);
	}
	
	VOID	BeginExchange(DWORD dwTgtRoleID)
	{
		SetRoleState(ERS_Exchange);
		GetExchMgr().CreateData();
		GetExchMgr().SetTgtRoleID(dwTgtRoleID);
	}

	VOID	EndExchange()
	{
		UnsetRoleState(ERS_Exchange);
		GetExchMgr().DeleteData();
		GetExchMgr().SetTgtRoleID(GT_INVALID);
	}

	DWORD	ProcExchangeReq(OUT Role* &pTarget, DWORD dwTgtRoleID);
	DWORD	ProcExchangeReqRes(OUT Role* &pApplicant, DWORD dwTgtRoleID, DWORD dwErrorCode);
	DWORD	ProcExchangeAdd(OUT Role* &pTarget, OUT tagItem* &pItem, INT32 &nInsIndex, INT64 n64Serial);
	DWORD	ProcExchangeDec(OUT Role* &pTarget, INT64 n64Serial);
	DWORD	ProcExchangeMoney(OUT Role* &pTarget, INT64 n64Silver);
	DWORD	ProcExchangeLock(OUT Role* &pTarget);
	DWORD	ProcExchangeCancel(OUT Role* &pTarget);
	DWORD	ProcExchangeVerify(OUT Role* &pTarget, OUT DWORD &dwFailedRoleID);

	DWORD	VerifyExchangeData(OUT tagItem* pItem[]);
	DWORD	ProcExchange();

	DWORD	ProcPrepareExchange(OUT Role* &pTarget);

	//---------------------------------------------------------------------------------
	// 商店相关
	//---------------------------------------------------------------------------------
	DWORD	GetShopItems(DWORD dwNPCID, BYTE byShelf);
	DWORD	GetShopEquips(DWORD dwNPCID, BYTE byShelf);
	DWORD	BuyShopItem(DWORD dwNPCID, BYTE byShelf, DWORD dwTypeID, INT16 n16ItemNum);
	DWORD	BuyShopEquip(DWORD dwNPCID, BYTE byShelf, DWORD dwTypeID, INT64 n64Serial);
	DWORD	SellToShop(DWORD dwNPCID, INT64 n64Serial);

	VOID	SendShopFeedbackMsg(DWORD dwErrorCode, DWORD dwNPCID);

	//-----------------------------------------------------------------------------
	// 商城相关
	//-----------------------------------------------------------------------------
	DWORD	GetMallAll(OUT DWORD &dwMallTime);
	DWORD	UpdateMallAll(OUT DWORD &dwNewMallTime, IN DWORD dwOldMallTime);
	DWORD	BuyMallItem(DWORD dwID, INT nUnitPrice, INT16 n16BuyNum, INT nIndex);
	DWORD	BuyMallPack(DWORD dwID, INT nUnitPrice, BYTE byIndex);
	DWORD	BuyMallItem(DWORD dwTgtRoleID, LPCTSTR szLeaveWord, 
							DWORD dwID, INT nUnitPrice, INT16 n16BuyNum, INT nIndex);
	DWORD	BuyMallPack(DWORD dwTgtRoleID, LPCTSTR szLeaveWord,
							DWORD dwID, INT nUnitPrice, BYTE byIndex);
	DWORD	GetMallFreeItem(DWORD dwID);

	//-----------------------------------------------------------------------------
	// 婚姻相关
	//-----------------------------------------------------------------------------
	//结婚
	DWORD ProcGetMarriageReq(OUT Role* &pTarget, DWORD dwTgtRoleID, DWORD dwNpcID);
	DWORD ProcGetMarriageReqRes(OUT Role* &pApplicant, DWORD dwApplicantRoleID, DWORD dwErrorCode);

	//离婚
	DWORD ProcBreakMarriageReq(OUT Role* &pTarget, DWORD dwTgtRoleID);
	DWORD ProcBreakMarriageReqRes(OUT Role* &pApplicant, DWORD dwApplicantRoleID, DWORD dwErrorCode);

	//婚礼
	DWORD ProcCelebrateWeddingReq(OUT Role* &pTarget,OUT Role* &pOfficiatorRole,DWORD dwOfficiatorNameCrc,DWORD dwNpcID);
	DWORD ProcCelebrateWeddingReqRes(OUT Role* &pApplicant, DWORD dwErrorCode);
public:
	DWORD	MallItemExchange(DWORD dwMallID, INT nPrice, INT16 n16BuyNum, BYTE byIndex);
	DWORD	MallPackExchange(DWORD dwMallID, INT nPrice, BYTE byIndex);

public:
	DWORD	RoleSetVocation(DWORD dwNPCID, EClassType eVocation);
	DWORD	RoleSetHeroVocation(DWORD dwNPCID, EClassTypeEx eHVocation);
	DWORD	ChangeVocation(DWORD dwNPCID, EClassType eVocation);

private:

	//-----------------------------------------------------------------------------
	// 商城元宝交易相关
	//-----------------------------------------------------------------------------
	DWORD	SaveYB2Account(DWORD dwID, INT nNum);
	DWORD 	SaveSilver2Account(DWORD dwID, INT64 nNum);
	DWORD 	DepositYBAccout(DWORD dwID, INT nNum);
	DWORD 	DepositSilverAccount(DWORD dwID, INT64 nNum);
	DWORD 	GetYBTradeInfo();
	DWORD 	SubmitSellOrder(DWORD dwRoleID, INT nNum, INT nPrice);
	DWORD 	SubmitBuyOrder(DWORD dwRole, INT nNum, INT nPrice);
	DWORD 	DeleteOrder(DWORD dwRoleID, DWORD dwOrderID, EYBOTYPE eYBOType);
	DWORD 	GetYBOrder(DWORD dwRoleID);

public:
	//------------------------------------------------------------------------
	// 摆摊相关
	//------------------------------------------------------------------------
	DWORD	StartStall()
	{
		DWORD dwRtv = m_pStall->Init(0);
		if (E_Success == dwRtv)
		{
			StopMount();
		}
		return dwRtv;//?? 城镇税率
	}

	DWORD	SetStallGoods(INT64 n64Serial, INT64 n64UnitPrice, BYTE byIndex)
	{
		return m_pStall->SetGoods(n64Serial, n64UnitPrice, byIndex);
	}

	DWORD	UnsetStallGoods(const BYTE byIndex)
	{
		return m_pStall->UnSetGoods(byIndex);
	}

	DWORD	SetStallTitle(LPCTSTR strTitle)
	{
		return m_pStall->SetTitle(strTitle);
	}

	DWORD	SetStallAdText(LPCTSTR strAdText)
	{
		return m_pStall->SetAdText(strAdText);
	}

	DWORD	SetStallAdFlag(bool bAdFlag)
	{
		return m_pStall->SetAdFlag(bAdFlag);
	}

	DWORD	SetStallFinish()
	{
		return m_pStall->SetFinish();
	}

	DWORD	CloseStall()
	{
		return m_pStall->Destroy();
	}

	DWORD	GetStallTitle(OUT LPTSTR pSzTitle)
	{
		return m_pStall->GetStallTitle(pSzTitle);
	}

	DWORD	GetStallSpecGoods(BYTE byIndex, OUT LPVOID pData, OUT INT &nGoodsSz)
	{
		return m_pStall->GetSpecGoods(byIndex, pData, nGoodsSz);
	}
	
	DWORD	GetStallGoods(OUT LPVOID pData, OUT BYTE &byGoodsNum, OUT INT &nGoodsSz)
	{
		return m_pStall->GetGoods(pData, byGoodsNum, nGoodsSz);
	}

	DWORD	BuyStallGoods(Role *pBuyer, INT64 n64Serial, INT64 n64UnitPrice, 
							INT16 n16Num, BYTE byIndex, OUT INT16 &n16RemainNum)
	{
		return m_pStall->Sell(pBuyer, n64UnitPrice, n64Serial, byIndex, n16Num, n16RemainNum);
	}

	BYTE	GetStallModeLevel() const
	{
		return m_pStall->GetModeLevel();
	}

	//INT8	GetStallGoodsNum() const
	//{
	//	return m_pStall->GetGoodsNum();
	//}

	BOOL	IsNoGoodsInStall() const
	{
		return m_pStall->IsEmpty();
	}

	BOOL	IsSetGoodsFinish() const
	{
		return IsInRoleState(ERS_Stall);
	}

	INT32	CalStallGoodsMemUsed() const	// 所有摊位上商品存入结构tagMsgStallGoods时的大小
	{
		return m_pStall->CalMemSzGoodsUsed();
	}

	DWORD	SendCloseStall();

	DWORD	GainStallExp(INT32 nExp)
	{
		return m_pStall->GainExp(nExp);
	}

private:
	//---------------------------------------------------------------------------------
	// 部分职能NPC功能
	//---------------------------------------------------------------------------------
	DWORD	ProcDak(DWORD dwNPCID, INT32 nIndex, DWORD dwMapID);

	//---------------------------------------------------------------------------------
	// 使用磨石
	//---------------------------------------------------------------------------------
	DWORD	AbraseWeapon(INT64 n64AbraserSerial, INT64 n64WeaponSerial, OUT INT32 &nAttackTimes);

	//---------------------------------------------------------------------------------
	// 地图触发器
	//---------------------------------------------------------------------------------
	BOOL	MapTrigger(DWORD dwTriggerID, DWORD dwMisc);

	//---------------------------------------------------------------------------------
	// 通知队友进入副本
	//---------------------------------------------------------------------------------
	DWORD	InstanceNotify(BOOL bNotify);

	//---------------------------------------------------------------------------------
	// 玩家是否同意其它玩家进入副本的邀请
	//---------------------------------------------------------------------------------
	DWORD	InstanceAgree(BOOL bAgree);

	//---------------------------------------------------------------------------------
	// 玩家请求离开副本
	//---------------------------------------------------------------------------------
	DWORD	LeaveInstance();

	//---------------------------------------------------------------------------------
	// 强化装备
	//---------------------------------------------------------------------------------
	// 铭纹
	DWORD	PosyEquip(DWORD dwNPCID, DWORD dwFormulaID, INT64 n64ItemID, INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID);	
	DWORD	GMPosyEquip(DWORD dwFormulaID, INT16 n16ItemIndex);	
	// 镌刻
	DWORD	EngraveEquip(DWORD dwNPCID, DWORD dwFormulaID, INT64 n64ItemID, INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID);
	DWORD	GMEngraveEquip(DWORD dwFormulaID, INT16 n16ItemIndex);
	// 镶嵌
	DWORD	InlayEquip(INT64 n64SrcItemID, INT64 n64DstItemID, DWORD dwCmdID);
	// 烙印
	DWORD	BrandEquip(INT64 n64SrcItemID, INT64 n64DstItemID, INT64 n64IMID, DWORD dwCmdID);
	// 龙附
	DWORD	LoongSoulEquip(INT64 n64SrcItemID, INT64 n64DstItemID, DWORD dwCmdID);
	DWORD	GMLoongSoulEquip(INT64 n64SrcItemID, INT16 n16ItemIndex);
	// 淬火
	DWORD	QuenchEquip(DWORD dwNPCID, DWORD dwFormulaID, INT64 n64ItemID, INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID);
	// 开凿
	DWORD	ChiselEquip(INT64 n64SrcItemID, INT64 n64SuffID, DWORD dwCmdID);
	// 时装染色
	DWORD	DyeFashion(INT64 n64DyeSN, INT64 n64EquipSerial, DWORD dwCmdID);
	// 计算B类属性对强化成功率的影响
	FLOAT	CalSpecAttEffectSuc(EEquipSpecAtt eSpecAtt, FLOAT fProp, EEquipSpecAtt eSpecType);
	// 计算B类属性对强化完美率的影响
	FLOAT	CalSpecAttEffectPef(EEquipSpecAtt eSpecAtt, FLOAT fPerfect);
	// 打开随身仓库
	DWORD	OpenSideWare(INT64 n64ItemSerial);
	
	//---------------------------------------------------------------------------------
	// 生产合成物品
	//---------------------------------------------------------------------------------
	DWORD	ProduceItem(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID);
	DWORD	ProduceEquip(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, INT64 n64IMID, INT64 n64StuffID[], INT32 nArraySz, DWORD dwCmdID);
	DWORD	CheckProduceLimit(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, const tagProduceProtoSer* &pProduceProto, INT64 n64StuffID[], INT32 nArraySz);
	DWORD	DeComposeItem(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, INT64 n64IMID, INT64 n64Item, DWORD dwCmdID);
	DWORD	CheckDeComposeLimit(DWORD dwNPCID, DWORD dwSkillID, INT64 n64ItemID, DWORD dwFormulaID, const tagDeComposeProtoSer* &pDeComposeProto, INT64 n64Item, tagEquip *pEquip);
	EProduceType Skill2ProduceType(ESkillTypeEx2 eSkillType);	
	tagItem*	 CreateItem(EItemCreateMode eCreateMode, DWORD dwCreateID, DWORD dwTypeID, INT16 n16Num, DWORD dwCreator);
	VOID	CalIMEffect(EConsolidateTypeSer eConType, tagIMEffect &IMEffect, INT64 n64IMID, const LPVOID pProto);
	VOID	CalEquipFlare(tagEquip* pEquip);
	// Jason 2009-12-6 提升装备潜力值道具处理
	DWORD RaiseEquipPotVal(INT64 n64SrcItemID, INT64 n64DstItemID, INT64 n64IMID, DWORD & dwPotValAdded);

	// Jason v1.3.1 date:2009-12-21
	DWORD GetGemRemovalInfo(DWORD dwNPCID,INT64 equipID,INT8 & num,DWORD dwGemIDs[MAX_EQUIPHOLE_NUM]);
	DWORD RemoveGemFromEquip(DWORD dwNPCID,INT64 equipID,INT8 num,INT64 SignIDs[MAX_EQUIPHOLE_NUM]);

	//---------------------------------------------------------------------------------
	// 查询某个天资所在的位置（如果没有，返回GT_INVALD）
	//---------------------------------------------------------------------------------
	INT FindTalentIndex(ETalentType eType)
	{
		INT nBegin = X_MAIN_TALENT_START, nEnd = X_ASS_TALENT_START;	// 设置起始点和终点

		// 如果是辅助天资，则取后半部分
		if( ETT_Action == eType || ETT_Jugglery == eType )
		{
			nBegin	=	X_ASS_TALENT_START;
			nEnd	=	X_MAX_TALENT_PER_ROLE;
		}

		// 如果该天资是主天资，则
		for(INT n = nBegin; n < nEnd; n++)
		{
			if( m_Talent[n].eType == eType )
				return n;
		}

		return GT_INVALID;
	}

	//---------------------------------------------------------------------------------
	// 找到某个可插入的天资位置（如果没有，返回GT_INVALID）
	//---------------------------------------------------------------------------------
	INT FindBlankTalentIndex(ETalentType eType)
	{
		INT nBegin = X_MAIN_TALENT_START, nEnd = X_ASS_TALENT_START;	// 设置起始点和终点

		// 如果是辅助天资，则取后半部分
		if( ETT_Action == eType || ETT_Jugglery == eType )
		{
			nBegin	=	X_ASS_TALENT_START;
			nEnd	=	X_MAX_TALENT_PER_ROLE;
		}

		for(INT n = nBegin; n < nEnd; n++)
		{
			if( ETT_Null == m_Talent[n].eType )
				return n;
		}

		return GT_INVALID;
	}

	//---------------------------------------------------------------------------------
	// 增加一个天资
	//---------------------------------------------------------------------------------
	VOID AddTalent(INT nIndex, ETalentType eType, INT nPoint=1)
	{
		ASSERT( nIndex >= 0 && nIndex < X_MAX_TALENT_PER_ROLE );
		ASSERT( ETT_Null == m_Talent[nIndex].eType );
		ASSERT( nPoint > 0 );

		m_Talent[nIndex].eType = eType;
		m_Talent[nIndex].nPoint = nPoint;

		// 同步
		tagNS_AddTalent send;
		send.eType = eType;
		send.nPoint = nPoint;
		SendMessage(&send, send.dwSize);
	}

	//---------------------------------------------------------------------------------
	// 删除一个天资
	//---------------------------------------------------------------------------------
	VOID RemoveTalent(INT nIndex)
	{
		ASSERT( nIndex >= 0 && nIndex < X_MAX_TALENT_PER_ROLE );
		ASSERT( ETT_Null != m_Talent[nIndex].eType );
		
		ETalentType eType = m_Talent[nIndex].eType;

		m_Talent[nIndex].eType = ETT_Null;
		m_Talent[nIndex].nPoint = 0;

		// 同步
		tagNS_RemoveTalent send;
		send.eType = eType;
		SendMessage(&send, send.dwSize);
	}


	//---------------------------------------------------------------------------------
	// 得到某个索引所对应的天资当前投点数
	//---------------------------------------------------------------------------------
	INT GetTalentPoint(INT nIndex)
	{
		ASSERT( nIndex >= 0 && nIndex < X_MAX_TALENT_PER_ROLE );
		ASSERT( ETT_Null != m_Talent[nIndex].eType );

		return m_Talent[nIndex].nPoint;
	}

	//---------------------------------------------------------------------------------
	// 给天资加点
	//---------------------------------------------------------------------------------
	VOID AddTalentPoint(INT nIndex, INT nPoint=1)
	{
		ASSERT( nIndex >= 0 && nIndex < X_MAX_TALENT_PER_ROLE );
		ASSERT( ETT_Null != m_Talent[nIndex].eType );
		ASSERT( nPoint > 0 );

		m_Talent[nIndex].nPoint += nPoint;

		// 同步
		tagNS_UpdateTalent send;
		send.eType = m_Talent[nIndex].eType;
		send.nPoint = m_Talent[nIndex].nPoint;
		SendMessage(&send, send.dwSize);
	}

	//---------------------------------------------------------------------------------
	// 被攻击
	//---------------------------------------------------------------------------------
	virtual VOID OnBeAttacked(Unit* pSrc, Skill* pSkill, BOOL bHited, BOOL bBlock, BOOL bCrited);

	//---------------------------------------------------------------------------------
	// 死亡
	//---------------------------------------------------------------------------------
	virtual VOID OnDead(Unit* pSrc, Skill* pSkill=NULL, const tagBuffProto* pBuff=NULL, DWORD dwSerial=GT_INVALID, DWORD dwMisc=0);

	//---------------------------------------------------------------------------------
	// 击杀
	//---------------------------------------------------------------------------------
	virtual VOID OnKill(Unit* pSrc);

	//---------------------------------------------------------------------------------
	// 是否进行死亡惩罚
	//---------------------------------------------------------------------------------
	BOOL IsDeadPenalty(EDeadPenaltyMode eMode);

	//---------------------------------------------------------------------------------
	// 死亡惩罚
	//---------------------------------------------------------------------------------
	VOID DeadPenalty(Unit* pSrc, EDeadPenaltyMode eMode);

	//---------------------------------------------------------------------------------
	// 是否能免除死亡惩罚
	//---------------------------------------------------------------------------------
	VOID CanCancelPenalty(Unit* pSrc, BOOL& bEquip, BOOL& bExp, BOOL& bInjury, BOOL& bHostility, BOOL& bCommercePenalty);

	//---------------------------------------------------------------------------------
	// 目标的类型标志
	//---------------------------------------------------------------------------------
	virtual DWORD TargetTypeFlag(Unit* pTarget);

	//---------------------------------------------------------------------------------
	// 状态标志
	//---------------------------------------------------------------------------------
	virtual DWORD GetStateFlag()
	{
		DWORD dwStateFlag = Unit::GetStateFlag();

		dwStateFlag |= ( IsInRoleState(ERS_Mount)		?	ESF_Mount	:	ESF_NoMount );
		dwStateFlag |= ( IsInRoleState(ERS_PrisonArea)	?	ESF_Prison	:	ESF_NoPrison );
		dwStateFlag |= ( IsInRoleState(ERS_Commerce)	?	ESF_Commerce:	ESF_NoCommerce );

		return dwStateFlag;
	}

	//---------------------------------------------------------------------------------
	// 与目标的敌友中立判断
	//---------------------------------------------------------------------------------
	virtual DWORD FriendEnemy(Unit* pTarget);

	//----------------------------------------------------------------------------------
	// 触发器类型对应被动技能和装备触发器类型
	//----------------------------------------------------------------------------------
	EPassiveSkillAndEquipTrigger TriggerTypeToPassiveSkillAndEquipTriggerType(ETriggerEventType eType)
	{   
		switch(eType)
		{
		case ETEE_BeAttacked:
			return EPSAET_BeAttacked;
			break;

		case ETEE_Hit:
			return EPSAET_Hit;
			break;

		case ETEE_Hited:
			return EPSAET_Hited;
			break;

		case ETEE_Dodged:
			return EPSAET_Dodged;
			break;

		case ETEE_Dodge:
			return EPSAET_Dodge;
			break;

		case ETEE_Blocked:
			return EPSAET_Blocked;
			break;

		case ETEE_Block:
			return EPSAET_Block;

		case ETEE_Crit:
			return EPSAET_Crit;
			break;

		case ETEE_Crited:
			return EPSAET_Crited;
			break;

		case ETEE_Die:
			return EPSAET_Die;
			break;

		case ETEE_Random:
			return EPSAET_Random;
			break;

		case ETEE_Attack:
			return EPSAET_Attack;
			break;

		case ETEE_Kill:
			return EPSAET_Kill;

		default:
			return EPSAET_Null;
			break;
		}
	}
	
private:
	DWORD TargetTypeFlag(Role* pTarget);
	DWORD TargetTypeFlag(Creature* pTarget);
	DWORD FriendEnemy(Role* pTarget);
	DWORD FriendEnemy(Creature* pCreature);

private:
	//---------------------------------------------------------------------------------
	// 将角色数据保存到数据库相关
	//---------------------------------------------------------------------------------
	class SaveRole
	{
	public:
		SaveRole();
		~SaveRole();

		VOID Init();
		operator tagNDBC_SaveRole*() { return m_pSaveRole; }

	private:
		tagNDBC_SaveRole*	m_pSaveRole;
	};

	static SaveRole	m_SaveRole;
	static Mutex	m_SaveRoleLock;
	
	//---------------------------------------------------------------------------------
	// 复活相关
	//---------------------------------------------------------------------------------
	struct tagRevive
	{
		INT		nHP;
		INT		nMP;

		DWORD	dwMapID;
		FLOAT	fX;
		FLOAT	fY;
		FLOAT	fZ;

		tagRevive() { ZeroMemory(this, sizeof(*this)); }
	};

	tagRevive m_Revive;

private:
	//---------------------------------------------------------------------------------
	// 对应的session
	//---------------------------------------------------------------------------------
	PlayerSession*				m_pSession;

protected:
	//-------------------------------------------------------------------------------------
	// 时间相关
	//-------------------------------------------------------------------------------------
	tagDWORDTime		m_CreatTime;						// 创建时间
	tagDWORDTime		m_LoginTime;						// 登陆时间
	tagDWORDTime		m_LogoutTime;						// 登出时间
	INT					m_nOnlineTime;						// 累计在线时间（秒）
	INT					m_nCurOnlineTime;					// 本次在线时间（秒）

	tagDWORDTime		m_CloseSafeGuardTime;				// 上一次关闭PK保护的时间
	INT					m_nUnSetPKTickCountDown;			// 关闭玩家行凶状态的Tick倒计时
	INT					m_nUnSetPVPTickCountDown;			// 关闭玩家PVP状态的Tick倒计时
	INT					m_nNeedSave2DBCountDown;			// 可以保存数据库倒计时
	INT					m_nDeadUnSetSafeGuardCountDown;		// 关闭玩家戾气为0时回城复活所加上的pk保护倒计tick

	DWORD				m_dwItemTransportMapID;				//当前记录传送符绑定地图ID
	FLOAT				m_fItemTransportX;					//当前记录传送符绑定的x坐标
	FLOAT				m_fItemTransportZ;					//当前记录传送符绑定的z坐标
	FLOAT				m_fItemTransportY;					//当前记录传送符绑定的y坐标

	tagDWORDTime		m_LastGetMallFreeTime;				// 上一次从商城获取免费物品时间

	//-------------------------------------------------------------------------------------
	// 玩家当前状态
	//-------------------------------------------------------------------------------------
	RoleState			m_RoleState;						// 玩家状态	-- 变化后，需通知周围玩家
	RoleStateEx			m_RoleStateEx;						// 玩家状态 -- 变化后，只需要通知自己
	ERolePKState		m_ePKState;							// 当前PK状态

	//-------------------------------------------------------------------------------------
	// 人物属性
	//-------------------------------------------------------------------------------------
	tagAvatarAtt		m_Avatar;							// 外观属性
	tagAvatarEquip		m_AvatarEquipEquip;					// 装备外观
	tagAvatarEquip		m_AvatarEquipFashion;				// 时装外观
	tagDisplaySet		m_DisplaySet;						// 装备显示设置
	EClassType			m_eClass;							// 职业
	EClassTypeEx		m_eClassEx;							// 扩展职业
	INT					m_nCredit;							// 信用度
	INT					m_nIdentity;						// 身份
	INT					m_nVIPPoint;						// 会员积分
	DWORD				m_dwGuildID;						// 所属帮派ID
	DWORD				m_dwLoverID;						// 爱人对应的roleid 没结婚就是GT_INVALID
	BOOL				m_bHaveWedding;						// 是否举行过婚礼
	BOOL				m_bNeedPrisonRevive;				// 是否需要牢狱复活
	//-------------------------------------------------------------------------------------
	// 物品冷却
	//-------------------------------------------------------------------------------------
	BOOL				m_bObjectCoolOff;					// 行囊内的物品是否冷却

	//-------------------------------------------------------------------------------------
	// 禁言
	//-------------------------------------------------------------------------------------
	BOOL				m_bSpeakOff;						// 玩家是否被禁言

	//-------------------------------------------------------------------------------------
	// 属性点和技能点
	//-------------------------------------------------------------------------------------
	INT					m_nAttPointAdd[X_ERA_ATTA_NUM];		// 投入各种一级属性的属性点
	tagRoleTalent		m_Talent[X_MAX_TALENT_PER_ROLE];	// 天资及各自天资点

	//-------------------------------------------------------------------------------------
	// 复活相关
	//-------------------------------------------------------------------------------------
	DWORD				m_dwRebornMapID;					// 复活地图ID

	//-------------------------------------------------------------------------------------
	// 被动技能的触发器索引表
	//-------------------------------------------------------------------------------------
	std::set<DWORD>		m_setPassiveSkillTrigger[EPSAET_End];

	//-------------------------------------------------------------------------------------
	// 装备的触发器索引表
	//-------------------------------------------------------------------------------------
	typedef std::bitset<X_EQUIP_BAR_SIZE>	BitSetEquipPos;		/*n16EquipPos*/
	typedef std::set<DWORD>					SetSuitTrigger;		/*dwSuitID*/

	BitSetEquipPos		m_bitsetEquipTrigger[EPSAET_End];
	BitSetEquipPos		m_bitsetLongHunTrigger[EPSAET_End];
	SetSuitTrigger		m_setSuitTrigger[EPSAET_End];

	//-------------------------------------------------------------------------------------
	// 金钱
	//-------------------------------------------------------------------------------------
	CurrencyMgr			m_CurMgr;

	//-------------------------------------------------------------------------------------
	// 物品管理器 -- 管理行囊、任务页、装备栏、角色仓库和百宝袋等
	//-------------------------------------------------------------------------------------
	ItemMgr				m_ItemMgr;							// 物品管理器

	//-------------------------------------------------------------------------------------
	// 套装管理
	//-------------------------------------------------------------------------------------
	Suit				m_Suit;

	//-------------------------------------------------------------------------------------
	// 龙魂能力相关
	//-------------------------------------------------------------------------------------
	LongHun				m_LongHun;

	//-------------------------------------------------------------------------------------
	// 玩家间交易相关
	//-------------------------------------------------------------------------------------
	ExchangeMgr			m_ExchMgr;

	//-------------------------------------------------------------------------------------
	// 摆摊
	//-------------------------------------------------------------------------------------
	Stall				*m_pStall;

	//-------------------------------------------------------------------------------------
	// 好友相关
	//-------------------------------------------------------------------------------------
	tagFriend						m_Friend[MAX_FRIENDNUM];				// 好友列表	
	TMap<DWORD, tagFriend*>			m_mapFriend;
	DWORD							m_dwBlackList[MAX_BLACKLIST];			// 黑名单

	//-------------------------------------------------------------------------------------
	// 团队相关
	//-------------------------------------------------------------------------------------
	DWORD				m_dwTeamID;											// 小队ID
	DWORD				m_dwGroupID;										// 团队ID
	DWORD				m_dwTeamInvite;										// 邀请人ID
	BOOL				m_bTeamSyn;											// 小队玩家状态同步标志

	//-------------------------------------------------------------------------------------
	// 副本相关
	//-------------------------------------------------------------------------------------
	DWORD				m_dwOwnInstanceMapID;								// 玩家所创建的副本的地图ID
	DWORD				m_dwOwnInstanceID;									// 玩家创建的副本ID
	DWORD				m_dwExportMapID;									// 传出副本时的地图ID
	Vector3				m_vExport;											// 传出副本时地图的坐标		

	//-------------------------------------------------------------------------------------
	// 对远端玩家公开信息设置
	//-------------------------------------------------------------------------------------
	tagRemoteOpenSet	m_sRemoteOpenSet;

	//-------------------------------------------------------------------------------------
	// 玩家脚本
	//-------------------------------------------------------------------------------------
	const RoleScript*	m_pScript;											// 玩家脚本

	//-------------------------------------------------------------------------------------
	// 角色开启宝箱计数
	//-------------------------------------------------------------------------------------
	INT					m_nTreasureSum;										// 玩家已开启的宝箱数
	
	//-------------------------------------------------------------------------------------
	// 开宝箱状态
	//-------------------------------------------------------------------------------------
	tagChestState		m_TreasureState;

	//-------------------------------------------------------------------------------------
	// 是否升30级已关闭pk保护
	//-------------------------------------------------------------------------------------
	BOOL				m_bLevelUpClosedSafeGuard;

	// 实力值
	tagStrength				m_Strength;											// 玩家实力值
	INT						m_nStrengthNum;										// 在实力排行榜中的名次

public:
	//-------------------------------------------------------------------------------------
	// 初始化当前任务
	//-------------------------------------------------------------------------------------
	VOID InitCurrentQuest(const BYTE* &pData, INT32 nNum);

	//-------------------------------------------------------------------------------------
	// 初始化所有已完成任务
	//-------------------------------------------------------------------------------------
	VOID InitCompleteQuest(const BYTE* &pData, INT32 nNum);

	//-------------------------------------------------------------------------------------
	// 通过NPC处接取任务
	//-------------------------------------------------------------------------------------
	INT AcceptQuestFromNPC(UINT16 u16QuestID, DWORD dwNPCID);

	//-------------------------------------------------------------------------------------
	// 通过触发器接取任务
	//-------------------------------------------------------------------------------------
	INT AcceptQuestFromMapTrigger(UINT16 u16QuestID, DWORD dwMapTriggerID);

	//-------------------------------------------------------------------------------------
	// 是否可以接取任务，返回参数为如果可参加，对应的索引
	//-------------------------------------------------------------------------------------
	INT CanAcceptQuest(UINT16 u16QuestID, INT& nIndex, Creature* pNPC=NULL);

	//-------------------------------------------------------------------------------------
	// 增加任务
	//-------------------------------------------------------------------------------------
	BOOL AddQuest(const tagQuestProto* pProto, INT nIndex);

	//-------------------------------------------------------------------------------------
	// 完成任务
	//-------------------------------------------------------------------------------------
	INT CompleteQuest(UINT16 u16QuestID, DWORD dwNPCID, INT nChoiceItemIndex, UINT16& u16NexitQuestID);

	//-------------------------------------------------------------------------------------
	// 是否可以交任务
	//-------------------------------------------------------------------------------------
	INT CanCompleteQuest(Quest* pQuest, DWORD dwNPCID, INT nChoiceItemIndex);

	//-------------------------------------------------------------------------------------
	// 任务奖励
	//-------------------------------------------------------------------------------------
	VOID RewardQuest(Quest* pQuest, INT32 nChoiceItemIndex);

	//-------------------------------------------------------------------------------------
	// 删除任务
	//-------------------------------------------------------------------------------------
	INT DeleteQuest(UINT16 u16QuestID);

	//-------------------------------------------------------------------------------------
	// 删除一个任务
	//-------------------------------------------------------------------------------------
	VOID RemoveQuest(UINT16 u16QuestID, BOOL bComplete);

	//-------------------------------------------------------------------------------------
	// 更新NPC对话任务状态
	//-------------------------------------------------------------------------------------
	VOID UpdateQuestNPCTalk(UINT16 u16QuestID, DWORD dwNPCID);

	//-------------------------------------------------------------------------------------
	// 更新Trigger任务状态
	//-------------------------------------------------------------------------------------
	VOID UpdateQuestTrigger(UINT16 u16QuestID, DWORD dwTriggerID);

	//-------------------------------------------------------------------------------------
	// 触发任务事件
	//-------------------------------------------------------------------------------------
	VOID OnQuestEvent(EQuestEvent eQuestType, DWORD dwEventMisc1=0, DWORD dwEventMisc2=0, DWORD dwEventMisc3=0);

	//------------------------------------------------------------------------------------
	// 得到完成任务的个数
	//------------------------------------------------------------------------------------
	INT GetCompleteQuestCount() { return m_mapCompleteQuests.Size(); }

	//------------------------------------------------------------------------------------
	// 清空所有已完成任务
	//------------------------------------------------------------------------------------
	VOID ClearCompleteQuest()
	{
		tagQuestDoneSave* pDoneQuest = NULL;
		QuestDoneMap::TMapIterator it = m_mapCompleteQuests.Begin();
		while( m_mapCompleteQuests.PeekNext(it, pDoneQuest) )
		{
			// 向客户端发送消息
			tagNS_GMQuestState send;
			send.u16QuestID = pDoneQuest->u16QuestID;
			send.bDone = FALSE;
			SendMessage(&send, send.dwSize);

			SAFE_DEL(pDoneQuest);
		}
		m_mapCompleteQuests.Clear();
	}

	//------------------------------------------------------------------------------------
	// 添加或删除一个已完成任务
	//------------------------------------------------------------------------------------
	VOID AddDelCompleteQuest(UINT16 u16QuestID, BOOL bAddOrDel)
	{
		// 将指定任务加入已完成任务
		if( bAddOrDel )
		{
			if( m_mapCompleteQuests.IsExist(u16QuestID) )
				return;

			tagQuestDoneSave* pQuestDone = new tagQuestDoneSave;
			pQuestDone->u16QuestID = u16QuestID;
			pQuestDone->nTimes = 1;
			pQuestDone->dwStartTime = g_world.GetWorldTime();

			m_mapCompleteQuests.Add(u16QuestID, pQuestDone);

			// 向客户端发送消息
			tagNS_GMQuestState send;
			send.u16QuestID = u16QuestID;
			send.bDone = TRUE;
			SendMessage(&send, send.dwSize);
		}
		else // 令指定任务为未完成任务
		{		
			tagQuestDoneSave* pQuestDone = m_mapCompleteQuests.Peek(u16QuestID);
			if( P_VALID(pQuestDone) )
			{
				m_mapCompleteQuests.Erase(u16QuestID);
				SAFE_DEL(pQuestDone);
			}

			// 向客户端发送消息
			tagNS_GMQuestState send;
			send.u16QuestID = u16QuestID;
			send.bDone = FALSE;
			SendMessage(&send, send.dwSize);
		}
	}

	//------------------------------------------------------------------------------------
	// 得到完成任务的个数
	//------------------------------------------------------------------------------------
	INT GetCurrentQuestCount() { return m_mapCurrentQuests.Size(); }

	//-------------------------------------------------------------------------------------
	// 得到指定任务完成的次数
	//-------------------------------------------------------------------------------------
	INT GetQuestCompleteTimes(UINT16 u16QuestID)
	{
		tagQuestDoneSave* pDoneQuest = m_mapCompleteQuests.Peek(u16QuestID);
		if( !P_VALID(pDoneQuest) ) return GT_INVALID;

		return pDoneQuest->nTimes;
	}

	//-------------------------------------------------------------------------------------
	// 得到指定任务完成的接取时间
	//-------------------------------------------------------------------------------------
	DWORD GetQuestCompleteAcceptTime(UINT16 u16QuestID)
	{
		tagQuestDoneSave* pDoneQuest = m_mapCompleteQuests.Peek(u16QuestID);
		if( !P_VALID(pDoneQuest) ) return GT_INVALID;

		return pDoneQuest->dwStartTime;
	}

	//-------------------------------------------------------------------------------------
	// 得到指定任务
	//-------------------------------------------------------------------------------------
	Quest* GetQuest(UINT16 u16QuestID) { return m_mapCurrentQuests.Peek(u16QuestID); }

	//-------------------------------------------------------------------------------------
	// 得到指定任务是否存在
	//-------------------------------------------------------------------------------------
	BOOL IsHaveQuest(UINT16 u16QuestID) { return m_mapCurrentQuests.IsExist(u16QuestID); }

	//-------------------------------------------------------------------------------------
	// 检测是否做过某任务
	//-------------------------------------------------------------------------------------
	BOOL IsHaveDoneQuest(UINT16 u16QuestID) { return m_mapCompleteQuests.IsExist(u16QuestID); }

	//-------------------------------------------------------------------------------------
	// 得到好友个数
	//-------------------------------------------------------------------------------------
	INT  GetFriendCount() { return m_mapFriend.Size(); }

	//-------------------------------------------------------------------------------------
	// 服务器可控对话框缺省事件
	//-------------------------------------------------------------------------------------
	VOID OnDlgDefaultEvent(EMsgUnitType	eDlgTarget, DWORD dwTargetID, EDlgOption eDlgOption);

private:
	Quest			m_Quests[QUEST_MAX_COUNT];		// 玩家当前的任务
public:
	BOOL			QuestValid(INT nIndex){return m_Quests[nIndex].IsValid();}
private:
	QuestMap		m_mapCurrentQuests;				// 玩家当前的任务
	QuestDoneMap	m_mapCompleteQuests;			// 已经完成的任务集合
// 称号相关
public:
	TitleMgr*		GetTitleMgr() { return m_pTitleMgr; }
private:
	TitleMgr*		m_pTitleMgr;
//角色名贴相关
public:
	tagRoleVCard&	GetVCard() { return m_VCard; }
private:
	tagRoleVCard	m_VCard;
//氏族信息
public:
	ClanData& GetClanData(){	return m_ClanData;}
private:
	ClanData	m_ClanData;
// 宠物带
public:
	PetPocket*	GetPetPocket() { return m_pPetPocket; }
private:
	PetPocket*	m_pPetPocket;
public:
	VOID OnLeaveMap(BOOL bChgMap = TRUE);

	void BreakMount();
	VOID OnEnterMap();
public:
	// 世界喊话，并重置时间
	BOOL TalkToWorld();
private:
	INT m_nWorldTalkCounter;
	VOID UpdateTalkToWorld();

public:
	DWORD CanPickUpItem(INT64 n64GroundID, BOOL bRole = TRUE);
	// 拾取物品
	DWORD PickUpItem(INT64 n64GroundID, BOOL bRole = TRUE);
	// 放下物品
	DWORD PutDownItem(INT64 n64Serial);
public:
	// 设置骑乘状态
	VOID SetMount(BOOL bSet, INT nMountSpeed, const tagPetProto* pPetProto);

	// 设置骑乘速度
	VOID ModMountSpeed(INT nMod);

	// 邀请骑乘
	VOID SetPassenger(Role* pHost);

	typedef MoveData::EMoveRet (MoveData::*PFMoveAction2P)(const Vector3& v1, const Vector3& v2);
	typedef MoveData::EMoveRet (MoveData::*PFMoveAction1P)(const Vector3& v);

	MoveData::EMoveRet	MoveAction(PFMoveAction2P pAction, Vector3& v1, Vector3& v2);
	MoveData::EMoveRet	MoveAction(PFMoveAction1P pAction, Vector3& v);

	//------------------------------------------------------------------------------
	// 玩家间宠物交易相关
	//------------------------------------------------------------------------------
	PetExchangeMgr& GetPetExchMgr() { return m_PetExchMgr; }

	BOOL	IsPetExchanging()					{ return IsInRoleState(ERS_PetExchange); }
	BOOL	CanPetExchange()
	{
		return !IsInRoleStateAny(ERS_Exchange | ERS_Shop | ERS_Stall | ERS_PetExchange) 
			&& (GetPetExchMgr().GetTgtRoleID() == GT_INVALID);
	}

	VOID	BeginPetExchange(DWORD dwTgtRoleID)
	{
		SetRoleState(ERS_PetExchange);
		GetPetExchMgr().CreateData();
		GetPetExchMgr().SetTgtRoleID(dwTgtRoleID);
	}

	VOID	EndPetExchange()
	{
		UnsetRoleState(ERS_PetExchange);
		GetPetExchMgr().DeleteData();
		GetPetExchMgr().SetTgtRoleID(GT_INVALID);
	}

	DWORD	ProcPetExchangeReq(OUT Role* &pTarget, DWORD dwTgtRoleID);
	DWORD	ProcPetExchangeReqRes(OUT Role* &pApplicant, DWORD dwTgtRoleID, DWORD dwErrorCode);
	DWORD ProcPetExchangeAdd(OUT Role* &pTarget, DWORD dwPetID);
	DWORD ProcPetExchangeDec(OUT Role* &pTarget, DWORD dwPetID);
	DWORD	ProcPetExchangeMoney(OUT Role* &pTarget, INT64 n64Silver);
	DWORD	ProcPetExchangeLock(OUT Role* &pTarget);
	DWORD	ProcPetExchangeCancel(OUT Role* &pTarget);
	DWORD	ProcPetExchangeVerify(OUT Role* &pTarget, OUT DWORD &dwFailedRoleID);

	DWORD VerifyPetExchangeData();
	DWORD	ProcPetExchange();

	DWORD	ProcPreparePetExchange(OUT Role* &pTarget);

private:
	PetExchangeMgr m_PetExchMgr;
	//	角色开启宝箱计数相关
public:

	//	获取角色开启宝箱计数
	INT GetTreasureSum()		{ return m_nTreasureSum; }
	//	增加宝箱计数
	VOID IncTreasureSum();
	//	初始化宝箱相关
	VOID InitChestState();
	//	设置宝箱开启数
	VOID SetTreasureSum(INT nSum);

	VOID StopMount();
	VOID ContinueMount();

public:
	// 金牌网吧经验加成
	FLOAT	GetVNBExpRate();
	// 金牌网吧掉率加成
	FLOAT	GetVNBLootRate();
	
public:
	// GM命令调用的几个函数
	DWORD AddSuit(DWORD dwSuitID, INT nQlty);
	DWORD AddEquip(INT nType, INT nLevel, INT nQlty);
public:
	//重置是否有留言消息
	VOID	ResetHasLeftMsg()
	{
		m_bHasLeftMsg = FALSE;
	}
private:
	BOOL	m_bHasLeftMsg;

public:
	// 是否可以采集
	INT CanGather(Creature* pRes);

public:
	//是否可以播放个性动作
	DWORD	CanCastMotion(Unit* pDest, DWORD dwActionID);
	// 播放个性动作
	DWORD	CastMotion(Unit* pDest, DWORD dwActionID);

	// 判断dwRoleID有没有权利进行交互
	BOOL	GetMotionInviteStatus(DWORD dwRoleID);
	// 玩家是否可以接受或者邀请标记
	VOID	SetMotionInviteStatus(BOOL bSet, DWORD dwRoleID);
	// 更新状态
	VOID	UpdateMotionInviteState();
private:
	INT		m_nMotionCounter;
	DWORD	m_dwPartnerID;
public:
	//保存经验到db
	VOID	SaveExp2DB();
	//保存属性点到db
	VOID SaveAttPoint2DB();
	//保存天资点到db
	VOID	SaveTalentPoint2DB(INT nIndex);
	//保存等级到db
	VOID	SaveLevel2DB();
	//保存名字到db
	VOID	SaveNewName2DB();

private:
	// Jason 2009-12-7 神龙赐福
	VOID	UpdateBlessingOfLoong(VOID);

	// Jason 2009-12-7 神龙赐福
	DWORD	m_timeLastLessing;			// 上次神龙赐福时间，注意，该值与总在线时间单位一致，单位秒
	INT				m_nLessingLevel;				// 赐福级别，最高ROLE_MAX_LESSLEVEL_OF_LOONG级
	BOOL			m_bFiredOnceOfLessingLoong;

	BOOL IsValidOfLessingLoong(VOID);

public:
	VOID NotifyClientToReceiveLessingOfLoong(VOID); // 通知玩家领下一次取神龙赐福
	VOID NotifyClientIniLessingOfLoong(VOID);		// 通知玩家神龙赐福，开启客户端相关功能界面

	DWORD ReceiveLessingOfLoong(VOID);				// 领取神龙赐福奖励。

	// 这个函数返回的是累计的秒数，
	DWORD GetCurrentOnlineTime(VOID);
	DWORD ProcessGoldStoneMsg( INT64 GoldStoneItemID, INT64 destItemID);
};
