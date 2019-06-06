//-----------------------------------------------------------------------------
//!\file QuitFrame.h
//!\author Lyp
//!
//!\date 2008-08-17
//! last 2008-08-26
//!
//!\brief quit game frame
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

struct tagNS_ReturnRoleSelect;
//-----------------------------------------------------------------------------
//!\brief loading frame
//!
//!
//-----------------------------------------------------------------------------
class QuitFrame : public GameFrame
{
public:
	/** \登出类型
	*/
	enum ELogoutType
	{
		ELOT_ResumeRoleSelect,
		ELOT_ResumLogin,
		ELOT_QuitGame,
	};
public:
	QuitFrame();
	~QuitFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	// 设置文字
	VOID SetText(LPCTSTR szText) { if(P_VALID(m_pWndText)) m_pWndText->SetText(szText); }

private:
	DWORD OnNS_ReturnRoleSelect(tagNS_ReturnRoleSelect* pMsg, DWORD dwParam);

private:
	TSFPTrunk<QuitFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	GUIWnd*						m_pWnd;
	GUIPatch*					m_pWndBack;
	GUIStaticEx*				m_pWndText;
	GUIButton*					m_pButtonQuit;
	GUIButton*					m_pButtonCancel;

	ELogoutType                 m_eLogoutType;                // 登出类型
	DWORD						m_dwStartTime;
	BOOL						m_bSendMsg;

	BOOL EventHandler(tagGUIEvent* pEvent);
	VOID Logout( const ELogoutType eLogoutTye );
};

