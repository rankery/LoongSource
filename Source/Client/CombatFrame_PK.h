#pragma once
#include "gameframe.h"

struct tagOpenSafeguardEvent;
struct tagOpenPKEvent;
struct tagNS_SafeGuard;
struct tagNS_PK;
struct tagMsgBoxEvent;
struct tagNS_RoleBeAttacked;

/** \class CombatFrame_PK
	\brief PK处理相关
*/
class CombatFrame_PK :
	public GameFrame
{
public:
	CombatFrame_PK(void);
	virtual ~CombatFrame_PK(void);

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();

	//--

private:
	DWORD OnOpenSafeguardEvent(tagOpenSafeguardEvent* pEvent);
	DWORD OnOpenPKEvent(tagOpenPKEvent* pEvent);

	DWORD OnNS_SafeGuard(tagNS_SafeGuard* pNetCmd, DWORD dwParam);
	DWORD OnNS_PK(tagNS_PK* pNetCmd, DWORD dwParam);
	DWORD OnNS_RoleBeAttacked( tagNS_RoleBeAttacked* pNetCmd, DWORD dwParam );

	DWORD OnFreeModeMsgBoxEvent(tagMsgBoxEvent* pEvent);

private:
	TSFPTrunk<CombatFrame_PK>		m_trunk;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<NetSession>			m_pSession;
};
