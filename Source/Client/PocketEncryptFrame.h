#pragma once
#include "gameframe.h"

/** \class PocketEncryptFrame
	\brief 行囊加密界面
*/
class PocketEncryptFrame :
	public GameFrame
{
public:
	PocketEncryptFrame(void);
	~PocketEncryptFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();

	virtual BOOL ReloadUI();

	virtual BOOL EscCancel();

private:

	DWORD EventHandler(tagGUIEvent* pEvent);

private:
	TSFPTrunk<PocketEncryptFrame>	m_Trunk;
	TObjRef<NetCmdMgr>				m_pNetCmd;
	TObjRef<NetSession>				m_pSession;
	TObjRef<GUISystem>				m_pGUI;
	
	GUIWnd*							m_pWnd;
	GUIWnd*							m_pWndCaption;
	GUIButton*						m_pBtnSetup;		// 设置
	GUIButton*						m_pBtnModify;		// 修改
	GUIButton*						m_pBtnCancel;		// 删除
	GUIButton*						m_pBtnExit;			// 离开

};
