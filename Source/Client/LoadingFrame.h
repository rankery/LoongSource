//-----------------------------------------------------------------------------
//!\file LoadingFrame.h
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief loading frame
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
//!\brief loading frame
//!
//!
//-----------------------------------------------------------------------------
class LoadingFrame : public GameFrame
{
public:
	enum ELoadingType
	{
		ELT_SelectRole = 1,
		ELT_CreateRole,
		ELT_World,
	};
public:
	LoadingFrame();
	~LoadingFrame();

	// 外部需要将图片文件名字符串指针作为Param传入
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	void SetLoadingType( ELoadingType type=ELT_World ) { m_eLoadingType = type; }


	// 设置文字
	VOID SetText(LPCTSTR szText) { if(P_VALID(m_pWndText)) m_pWndText->SetText(szText); }

private:
	TSFPTrunk<LoadingFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<VarContainer>		m_pTips;
	TObjRef<GameInputMap>		m_pInput;

	GUIWnd*						m_pWnd;
	GUIStatic*					m_pWndText;
	GUIProgress*				m_pProgress;

	tstring						m_strBackPicFileName;
	DWORD						m_dwCurrent;
	DWORD						m_dwQuitTime;
	ELoadingType				m_eLoadingType;

	BOOL EventHandler(tagGUIEvent* pEvent);
};

