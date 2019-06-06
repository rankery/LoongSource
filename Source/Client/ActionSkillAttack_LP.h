#pragma once
#include "CombatEvent.h"

class LocalPlayer;
class Role;
/** \class ActionSkillAttack_LP
	\brief 本地玩家技能攻击行为
*/
class ActionSkillAttack_LP
{
public:
	ActionSkillAttack_LP(void);
	virtual ~ActionSkillAttack_LP(void);

	void Active(LocalPlayer* pRole,Role* pTarget,DWORD skillID,DWORD dwSerial,bool bSendNetMsg);
	void Update(LocalPlayer* pRole,Role* pTarget);
	bool IsClosed(){ return m_bClosed;}
	const TCHAR* GetTrackName(){ return m_trackName.c_str();}

	/** 停止
	*/
	void Stop(LocalPlayer* pRole,bool bSendStopCmd);

private:
	void StopEffect(bool bAtkEffect);
	void FaceToTarget(LocalPlayer* pSelf,Role* pTarget);
	void RestoreYawAndLowerAni(LocalPlayer* pSelf);
	void OnMsgCode(LocalPlayer* pRole,const DWORD dwMsgCode);
	void PlayAtkSfx(DWORD dwRoleID);
	void StopAtkSfx();
private:
	tstring					m_trackName;
	DWORD					m_skillID;
	DWORD					m_targetID;
	bool					m_bSingle;//是否单体攻击
	int						m_dmgTimes;//伤害次数
	DWORD					m_dwSerial;//消息序列号
	DWORD					m_attackEffectID;
	bool					m_bClosed;

	bool					m_bMoveable;
	bool					m_bAniBlend;
	bool                    m_bHostile;

	tstring                 m_strAtkSfx;				// 攻击特效编号
	vector<tstring>			m_vecAtkSfxTagNode;			// 攻击特效挂载点或骨骼名称
	tstring					m_strLchSfx;				// 发射特效编号
	tstring					m_strLchSfxTagNode;			// 发射特效挂载点或骨骼名称
	vector<DWORD>           m_vecAttackEffectID;		// 当前播放的攻击特效的ID
	tstring					m_strAtkSound;				// 攻击音效编号
	DWORD                   m_dwMoveEffectID[2];		// 移动攻击特效的ID（2个）
	Vector3                 m_vPlayerPos;               // 保存一个角色的坐标，用来判断角色是否有位移
};
