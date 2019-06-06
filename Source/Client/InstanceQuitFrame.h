#pragma once

class InstanceQuitFrame : public GameFrame
{
public:
	InstanceQuitFrame();
	~InstanceQuitFrame();
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();
	VOID SetText(LPCTSTR szText);

private:
	TSFPTrunk<InstanceQuitFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<Util> m_pUtil;
	TObjRef<GameFrameMgr> m_pFrameMgr;
	GUIWnd *m_pWnd;
	GUIPatch *m_pWndBack;
	GUIStaticEx *m_pWndText;
	GUIButton *m_pButtonQuit;
	GUIButton *m_pButtonCancel;
	DWORD m_dwStartTime;
	static const DWORD QUIT_INSTANCE_TIME = 60 * 1000; 

private:
	BOOL EventHandler(tagGUIEvent* pEvent);
};