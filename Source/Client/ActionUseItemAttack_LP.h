#pragma once

class LocalPlayer;
class Role;
/** \class ActionUseItemAttack_LP
	\brief 本地玩家使用物品行为
*/
class ActionUseItemAttack_LP
{
public:
	ActionUseItemAttack_LP(void);
	virtual ~ActionUseItemAttack_LP(void);

	void Active(LocalPlayer* pRole,Role* pTarget,INT64 itemID,DWORD itemTypeID,DWORD dwSerial,bool bSendNetMsg);
	void Update(LocalPlayer* pRole,Role* pTarget);
	bool IsClosed(){ return m_bClosed;}
	const TCHAR* GetTrackName(){ return m_trackName.c_str();}

	void Stop(LocalPlayer* pRole);

private:
	tstring					m_trackName;
	DWORD					m_itemID;
	DWORD					m_dwSerial;
	DWORD					m_targetID;
	bool					m_bSingle;//是否单体攻击
	bool					m_bClosed;
	bool					m_bMoveable;//是否可以移动使用
	bool					m_bAniBlend;//是否上下身分开
};
