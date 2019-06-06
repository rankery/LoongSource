#pragma once

class RenameFrame: public GameFrame
{
public:
	RenameFrame();
	~RenameFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancle();
	void ShowFrame(bool bShow, INT64 n64SrcID = GT_INVALID );

private:
	DWORD EventHandler( tagGUIEvent *pEvent );
	VOID FliterStr( tstring &str );
	bool CheckName( tstring &strNewName );

private:
	TSFPTrunk<RenameFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<NetSession> m_pSession;

	GUIWnd *m_pWnd;
	GUIWnd *m_pWndBackPic;

	GUIStaticEx *m_pStcTips;
	GUIButton *m_pBtnRename;
	GUIButton *m_pBtnCancle;
	GUIEditBox *m_pEditNewName;

	INT64 m_n64SrcID;
};