//-----------------------------------------------------------------------------
//!\file GameInputFrame.h
//!\author Lyp
//!
//!\date 2008-10-09
//! last 2008-10-09
//!
//!\brief game input frame
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
//!\brief 
//!
//-----------------------------------------------------------------------------
class GameInputFrame : public GameFrame
{
public:
	GameInputFrame():m_Trunk(this){};
	~GameInputFrame(){};

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();

	virtual BOOL EscCancel();

private:
	TSFPTrunk<GameInputFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	
	TObjRef<GameInputMap>			m_pInputMap;

	VOID OnEvent(tagGameEvent* pEvent);

};