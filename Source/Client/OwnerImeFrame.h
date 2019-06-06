#pragma once
#include "Render2D.h"

struct tagOwnerImeShow;
struct tagOwnerInputWordEvent;
struct tagOwnerInputWordClear;
struct tagOwnerImeWordEvent;

/*
	class OwnerImeFrame
	brief 真全屏输入法界面
*/
class OwnerImeFrame : public GameFrame
{
public:
	OwnerImeFrame(void);
	~OwnerImeFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

private:
	DWORD EventHandler(tagGUIEvent* pEvent);

	DWORD OntOwnerImeShow(tagOwnerImeShow* pGameEvent);
	DWORD OnOwnerInputWordEvent(tagOwnerInputWordEvent* pGameEvent);
	DWORD OnOwnerInputWordClear(tagOwnerInputWordClear* pGameEvent);
	DWORD OnOwnerImeWordEvent(tagOwnerImeWordEvent* pGameEvent);

private:
	TSFPTrunk<OwnerImeFrame>				m_Trunk;
	TObjRef<Util>							m_pUtil;
	TObjRef<NetCmdMgr>						m_pCmdMgr;
	TObjRef<NetSession>						m_pSession;
	TObjRef<GUISystem>						m_pGUI;
	TObjRef<GameFrameMgr>					m_pFrameMgr;
	TObjRef<GUIRender>						m_pRender;

	GUIWnd*									m_pWndIme;
	GUIStatic*								m_pWndInput;
	GUIListBox*								m_pLWord;
	GUIButton*								m_pBtnUp;
	GUIButton*								m_pBtnDown;
	GUIWnd*									m_pWndActive;

	INT										m_nPageMax;
	INT										m_nCurPage;

};