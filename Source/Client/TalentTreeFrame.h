#pragma once
#include "TalentTree.h"
#include "..\WorldDefine\RoleDefine.h"

struct tagUpdateLPSkillEvent;
struct tagUpdateLPLevel;
struct tagUpdateLPAttEvent;
struct tagUpdateLPTalentPoint;

/** class	TalentTreeFrame
	brief	天赋树界面
*/
class TalentTreeFrame :
	public GameFrame
{
public:
	TalentTreeFrame();
	~TalentTreeFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();

	// 处理天资打开
	VOID OnTalentTreeOpen();

private:
	// 处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);
	
	// 处理游戏事件
	DWORD OnUpdateLPSkillEvent(tagUpdateLPSkillEvent* pGameEvent);
	DWORD OnUpdateLPLevel(tagUpdateLPLevel* pGameEvent);
	DWORD OnUpdateLPAttEvent(tagUpdateLPAttEvent* pGameEvent);
	DWORD OnUpdateLPTalentPoint(tagUpdateLPTalentPoint* pGameEvent);
	DWORD OnEventClearTalent(tagGameEvent* pGameEvent);

	// 设置当前可投点
	VOID SetBidden(INT nValue);
	// 设置当前已投点
	VOID SetAvail(INT nValue);
private:

	TSFPTrunk<TalentTreeFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>		m_pSession;

	GUIWnd*						m_pWnd;
	GUIPushButton*				m_pPbnTalent[ETT_End];
	GUIStatic*					m_pStcAvail;
	GUIStatic*					m_pStcBidden;

	TalentTree					m_TalentTree;

	INT							m_nCurBidden;
	ETalentType					m_eCurTalent;
	INT							m_nCurAvail;
	INT							m_nCurTalentNum;

};
