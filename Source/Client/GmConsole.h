//-----------------------------------------------------------------------------
//!\file GmConsole.h
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief gm console frame
//-----------------------------------------------------------------------------
#pragma once

struct tagRoleGetNameEvent;
struct tagRolePickEvent;
//-----------------------------------------------------------------------------
//!\brief gm console frame
//!
//!
//-----------------------------------------------------------------------------
class GmConsoleFrame : public GameFrame
{
public:
	GmConsoleFrame();
	~GmConsoleFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();


	// 设置文字
	VOID SetText(LPCTSTR szText) { if(P_VALID(m_pWndText)) m_pWndText->SetText(szText); }

private:
	TSFPTrunk<GmConsoleFrame>	m_Trunk;
	TObjRef<GameInputMap>		m_pInput;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	ConsoleCommand*				m_pCommander;

	GUIWnd*						m_pWnd;
	GUIPatch*					m_pWndCaption;
	GUIEditBox*					m_pWndCommand;
	GUIStatic*					m_pWndText;
	GUIWnd*						m_pWndSubmit;

	GUIEditBox*					m_pWndEditKick;
	GUIEditBox*					m_pWndEditMoveRole;
	GUIEditBox*					m_pWndEditMoveToRole;
	GUIEditBox*					m_pWndEditKill;

	GUIWnd*						m_pWndKick;
	GUIWnd*						m_pWndMoveRole;
	GUIWnd*						m_pWndMoveToRole;
	GUIWnd*						m_pWndKill;

	GUIWnd*						m_pWndClearBag;
	GUIWnd*						m_pWndReDrop;
	GUIWnd*						m_pWndInit;

	DWORD						m_dwCurShowRoleID;

	BOOL EventHandler(tagGUIEvent* pEvent);

	// 控制台开关
	DWORD Switch(DWORD dwID, BOOL bDown);

	VOID OnRolePickEvent(tagRolePickEvent* pEvent);
	VOID OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent);
};
