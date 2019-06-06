#pragma once
#include "gameframe.h"

class GuildMemberPosFrame :
	public GameFrame
{
public:
	typedef vector<GUIButton*> GUIButtonList;

public:
	GuildMemberPosFrame(void);
	~GuildMemberPosFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();

	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	VOID ReflashPosButton(INT nIndex);

private:
	DWORD EventHandler(tagGUIEvent* pEvent);

private:
	TSFPTrunk<GuildMemberPosFrame>		m_Trunk;
	TObjRef<GUISystem>					m_pGUI;

	GUIWnd*								m_pWnd;
	GUIButton*							m_pBtnClose;
	GUIButtonList						m_pBtnPosAppoint;

	DWORD								m_dwDstRoleID;
};
