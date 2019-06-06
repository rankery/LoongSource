#pragma once
#include "player.h"

class FSM_LP;
struct tagNS_GetRoleInitState_Att;

/** \class LocalPlayer
	\brief 本地玩家角色
*/
class LocalPlayer : public Player
{
public:
	LocalPlayer(void);
	virtual ~LocalPlayer(void);

	//--Role
	virtual void InitFSM();
	virtual void Update();
	virtual void OnNetCmd(tagNetCmd* pNetCmd);
	virtual void OnGameEvent(tagGameEvent* pGameEvent);
	virtual bool IsDead();

	//--Player
	virtual bool IsReadyState();
	virtual bool IsStandState();
	virtual bool IsStyleActionState();
	virtual bool IsIdleState();
	virtual EActionState GetActionState();
	virtual void ReplayLowerAni();

	//--角色属性相关
	/** \读取和写入玩家投点
	*/
	INT GetAttPointAdd(ERoleAttribute eType){return m_rolepointadd[MTransERAAttA2Index(eType)];}
	VOID SetAttPointAdd(ERoleAttribute eType, INT nValue);
	/** \读取玩家原始一级属性
	*/
	INT GetOriginAtt(ERoleAttribute eType);
	/** \处理本地玩家属性初始化
	*/
	VOID InitAttribute(tagNS_GetRoleInitState_Att* pCmd);
	/** \读取和写入玩家当前经验
	*/
	INT GetRoleLevelExp(){return m_rolecurlevelexp;}
	VOID SetRoleLevelExp(INT nValue, INT nInc = 0);

	//--
	/** \返回角色是否处于战斗状态
	*/
	bool IsCombatState();
	/** \返回角色是否处于使用技能状态
	*/
	bool IsSkillState();
	/** \返回角色是否处于游泳状态
	*/
	bool IsSwimState();
	/** \返回角色是否处于水上行走状态
	*/
	bool IsOnWaterState();
	/** \设置角色自身状态
	*/
	void SetRoleStateEx(DWORD dwValue);
	/** \设置角色自身状态
	*/
	void SetRoleStateEx(ERoleStateEx eType, bool bFlag);
	/** \获取角色自身状态
	*/
	bool GetRoleStateEx(ERoleStateEx eType){return (m_roleStateEx&eType) != 0;}
	/** \切换地图或传送后，设置坐标、朝向等
	*/
	void OnGotoMap(const Vector3& pos,const Vector3 faceTo);

	/** \玩家的个人设置
	*/
	const tagDWPlayerPersonalSet& GetPersonalSet() const { return m_personalSet; }
	void SetPersonalSet( const tagDWPlayerPersonalSet& pps ) { m_personalSet = pps; }

	/** \钓鱼,忽略一次悠闲动作
	*/
	void DontPlayEaseSkeleton();

	/** \钓鱼,强制转换到idle状态, 为钓鱼系统设计
	*/
	void ChangeUpperToIdle();

	/** \当前状态是否为idle
	*/
	bool IsUpperIdleState();

	/** 
		\ 得到正在使用的物品的 typeid，如果当前不是使用物品状态返回0
	*/
	DWORD GetUsingItemTypeID();

protected:
	FSM_LP*					m_pFSM;
	INT						m_rolepointadd[X_ERA_ATTA_NUM];
	INT						m_rolecurlevelexp;
	DWORD					m_roleStateEx;

	tagDWPlayerPersonalSet  m_personalSet;

	DECL_RTTI(LocalPlayer);
};

