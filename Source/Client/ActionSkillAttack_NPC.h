#pragma once
#include "CombatEvent.h"

class NPC;
class Role;
/** \class ActionSkillAttack_NPC
	\brief NPC技能攻击行为
*/
class ActionSkillAttack_NPC
{
public:
	ActionSkillAttack_NPC(void);
	virtual ~ActionSkillAttack_NPC(void);

	void Active(NPC* pRole,Role* pTarget,DWORD skillID,DWORD dwSerial);
	void Update(NPC* pRole,Role* pTarget);
	void Stop(NPC* pRole);
	bool IsClosed(){ return m_bClosed;}
	const TCHAR* GetTrackName(){ return m_trackName.c_str();}

private:
	void StopEffect(bool bAtkEffect);
	void OnMsgCode(NPC* pRole,const DWORD dwMsgCode);
	void PlayAtkSfx(DWORD dwRoleID);
	void StopAtkSfx();
private:
	DWORD					m_skillID;
	DWORD					m_targetID;
	DWORD					m_dwSerial;
	bool					m_bSingle;//是否单体攻击
	int						m_dmgTimes;
	bool					m_bClosed;
	tstring					m_trackName;
	TObjRef<GameFrameMgr>   m_pGameFrameMgr;

	tstring                 m_strAtkSfx;				// 攻击特效编号
	vector<tstring>			m_vecAtkSfxTagNode;			// 攻击特效挂载点或骨骼名称
	tstring					m_strLchSfx;				// 发射特效编号
	tstring					m_strLchSfxTagNode;			// 发射特效挂载点或骨骼名称
	vector<DWORD>           m_vecAttackEffectID;		// 当前播放的攻击特效的ID
	tstring					m_strAtkSound;				// 攻击音效编号
};
