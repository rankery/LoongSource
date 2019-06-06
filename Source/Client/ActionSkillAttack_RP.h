#pragma once
#include "CombatEvent.h"

class Player;
class Role;
/** \class ActionSkillAttack_RP
	\brief 远程玩家技能攻击行为
*/
class ActionSkillAttack_RP
{
public:
	ActionSkillAttack_RP(void);
	virtual ~ActionSkillAttack_RP(void);

	void Active(Player* pRole,Role* pTarget,DWORD skillID,DWORD dwSerial);
	void Update(Player* pRole,Role* pTarget);
	bool IsClosed(){ return m_bClosed;}
	const TCHAR* GetTrackName(){ return m_trackName.c_str();}

	void Stop(Player* pRole);

private:
	void StopEffect(bool bAtkEffect);
	void FaceToTarget(Player* pSelf,Role* pTarget);
	void RestoreYawAndLowerAni(Player* pSelf);
	void OnMsgCode(Player* pRole,const DWORD dwMsgCode);
	void PlayAtkSfx(DWORD dwRoleID);
	void StopAtkSfx();
private:
	DWORD					m_skillID;
	DWORD					m_dwSerial;
	DWORD					m_targetID;
	bool					m_bClosed;
	tstring					m_trackName;
	bool					m_bSingle;
	int						m_dmgTimes;
	bool					m_bMoveable;
	bool					m_bAniBlend;
	bool                    m_bHostile;

	tstring                 m_strAtkSfx;          // 攻击特效编号
	vector<tstring>			m_vecAtkSfxTagNode;	  // 攻击特效挂载点或骨骼名称
	tstring					m_strLchSfx;          // 发射特效编号
	tstring					m_strLchSfxTagNode;   // 发射特效挂载点或骨骼名称
	vector<DWORD>           m_vecAttackEffectID;  // 当前播放的攻击特效的ID
	tstring					m_strAtkSound;		  // 攻击音效编号
	DWORD                   m_dwMoveEffectID[2];  // 移动攻击特效的ID（2个）
	Vector3                 m_vPlayerPos;         // 保存一个角色的坐标，用来判断角色是否有位移
};
