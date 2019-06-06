#pragma once

class LocalPlayer;
class Role;
/** \class ActionSkillSpell_LP
	\brief 本地玩家技能吟唱行为
*/
class ActionSkillSpell_LP
{
	enum EStep
	{
		EStep_Action1,	//第一个动作
		EStep_Action2,	//第二个动作
	};
public:
	ActionSkillSpell_LP(void);
	virtual ~ActionSkillSpell_LP(void);

	void Active(LocalPlayer* pRole,Role* pTarget,DWORD skillID,DWORD dwSerial);
	void Update(LocalPlayer* pRole,Role* pTarget);

	bool IsClosed(){ return m_bClosed;}
	void OnRecvSpellTime(DWORD spellTime);
	void Stop(LocalPlayer* pRole,bool bSendStopCmd);

private:
	void PlayEffect(DWORD dwRoleID);
	void StopEffect();
	void FacetoTarget(LocalPlayer* pSelf,Role* pTarget);

private:
	bool					m_bClosed;
	DWORD					m_skillID;
	DWORD					m_dwSerial;
	DWORD					m_targetID;
	DWORD					m_startSpellTime;
	DWORD					m_spellTime;
	tstring					m_szTrackName1;
	tstring					m_szTrackName2;
	bool					m_bRecvSpellTime;
	bool					m_bMoveSpell;			// 是否可以移动起手

	EStep					m_step;

	tstring                 m_strPreSfx;			// 吟唱特效
	vector<tstring>         m_vecPreSfxTagNode;		// 吟唱特效挂载点或骨骼名称
	vector<DWORD>			m_vecPreSfxID;          // 吟唱特效ID

	DWORD					m_dwChopSoundID;		// 采集音效ID
};
