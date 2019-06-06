#pragma once
#include "gameframe.h"
#include "..\WorldDefine\msg_combat.h"

/** \class CombatFrame_DeadMenu
	\brief 死亡菜单
*/
class CombatFrame_DeadMenu :
	public GameFrame
{
public:
	CombatFrame_DeadMenu(void);
	virtual ~CombatFrame_DeadMenu(void);

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();
	virtual BOOL ReloadUI();

	DWORD NetRecvNS_RoleRevive(tagNS_RoleRevive* pMsg, DWORD dwParam);
	DWORD NetRecvNS_RoleReviveNotify(tagNS_RoleReviveNotify* pMsg, DWORD dwParam);

private:
	BOOL EventHandler(tagGUIEvent* pEvent);

private:
	TSFPTrunk<CombatFrame_DeadMenu>			m_Trunk;
	TObjRef<GUISystem>						m_pGUI;
	TObjRef<GameFrameMgr>					m_pFrameMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<NetCmdMgr>						m_pCmdMgr;

	GUIWnd*						m_pWnd;				//主界面
	GUIButton*					m_pBtnLocalRise;	//原地复活
	GUIButton*					m_pBtnAcceptRise;	//接受复活
	GUIButton*					m_pBtnBackRise;		//回城复活
	GUIStatic*					m_pStcHint;			//提示

};
