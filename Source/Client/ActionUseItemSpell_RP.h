#pragma once

class Player;
class Role;
struct tagTargetSelectEvent;
/** \class ActionUseItemSpell_RP
	\brief 远程玩家使用物品吟唱行为
*/
class ActionUseItemSpell_RP
{
public:
	ActionUseItemSpell_RP(void);
	virtual ~ActionUseItemSpell_RP(void);

	void Active(Player* pRole,Role* pTarget,DWORD itemID,DWORD spellTime);
	void Update(Player* pRole,Role* pTarget);
	bool IsClosed(){ return m_bClosed;}
	void Stop(Player* pRole);
	void OnTargetSelectEvent(tagTargetSelectEvent* pEvent,Role* pRole);
private:
	DWORD                   m_itemID;
	DWORD					m_spellTime;
	DWORD					m_startSpellTime;
	bool					m_bClosed;

	DWORD					m_dwEffectID;			//起手特效ID
};
