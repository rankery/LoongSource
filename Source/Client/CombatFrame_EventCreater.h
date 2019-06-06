#pragma once
#include "gameframe.h"

struct tagRolePickEvent;
struct tagScenePickEvent;
struct tagGroundItemPickEvent;
struct tagMouseMoveEvent;
class Role;

/** \class CombatFrame_EventCreater
	\brief 战斗相关游戏事件生成器
*/
class CombatFrame_EventCreater :
	public GameFrame
{
public:
	CombatFrame_EventCreater(void);
	virtual ~CombatFrame_EventCreater(void);

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();

	//--
	bool IsSelectEnemy(){ return m_bSelectEnemy;}
	DWORD GetCurTargetID(){ return m_curTargetRoleID;}

private:
	DWORD OnKey(DWORD dwID, BOOL bDown);
	DWORD OnRolePickEvent(tagRolePickEvent* pEvent);
	DWORD OnScenePickEvent(tagScenePickEvent* pEvent);
	DWORD OnGroundItemPickEvent(tagGroundItemPickEvent* pEvent);
	DWORD OnMouseMoveEvent(tagMouseMoveEvent* pEvent);
	DWORD GetNextTarget();
	bool  CanSwitchTargetRole(Role* pRole,const Frustum* pFrustum);

private:
	TSFPTrunk<CombatFrame_EventCreater>	m_trunk;
	TObjRef<GameInputMap>	m_pKeyMap;

	DWORD					m_curTargetRoleID;
	bool					m_bSelectEnemy;

	BOOL					m_bFrontKeyDown;
	BOOL					m_bBackKeyDown;
	BOOL					m_bLeftKeyDown;
	BOOL					m_bRightKeyDown;
};
