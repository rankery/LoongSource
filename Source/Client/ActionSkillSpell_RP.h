#pragma once

class Player;
class Role;
struct tagTargetSelectEvent;

/** \class ActionSkillSpell_RP
	\brief 远程玩家技能吟唱行为
*/
class ActionSkillSpell_RP
{
	enum EStep
	{
		EStep_Action1,	//第一个动作
		EStep_Action2,	//第二个动作
	};
public:
	ActionSkillSpell_RP(void);
	virtual ~ActionSkillSpell_RP(void);

	void Active(Player* pRole,Role* pTarget,DWORD skillID,DWORD spellTime);
	void Update(Player* pRole,Role* pTarget);
	bool IsClosed(){ return m_bClosed;}
	void Stop(Player* pRole);
private:
	void PlayEffect(DWORD dwRoleID);
	void StopEffect();
	void FacetoTarget(Player* pSelf,Role* pTarget);

private:
	DWORD					m_skillID;
	DWORD					m_spellTime;
	DWORD					m_startSpellTime;
	bool					m_bClosed;
	EStep					m_step;
	tstring					m_szTrackName1;
	tstring					m_szTrackName2;
	bool					m_bMoveSpell;			// 是否可以移动起手

	tstring                 m_strPreSfx;			// 吟唱特效
	vector<tstring>         m_vecPreSfxTagNode;		// 吟唱特效挂载点或骨骼名称
	vector<DWORD>			m_vecPreSfxID;          // 吟唱特效ID
};
