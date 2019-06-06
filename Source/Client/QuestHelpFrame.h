#pragma once 

struct tagQuestHelpInfo
{
	DWORD dwQuestId;
	TCHAR szStartPicName[X_LONG_NAME];
	TCHAR szFinishPicName[X_LONG_NAME];
	DWORD dwStartPicPage;
	DWORD dwFinishPicPage;
	tagQuestHelpInfo(){ZeroMemory(this, sizeof(*this));}
};

class IconStatic;
class QuestHelpFrame : public GameFrame
{
public:
	QuestHelpFrame();
	~QuestHelpFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

public:
	bool GetShowFrameFlag() { return m_bShowFrame; }
	VOID SetPicName( const tstring &strName ) { m_strPicName = strName; }
	VOID SetPicName( const DWORD dwPage );
	VOID SetCurPage( const DWORD dwPage ) { m_dwCurPage = dwPage; }
	VOID ShowFrame();
	VOID HideFrame();
	const vector<tagQuestHelpInfo> &GetQuestHelpInfo() { return m_vecQuestHelpInfo; }

private:
	DWORD EventHandler( tagGUIEvent *pEvent );
	void LoadQuestHelpInfoFile();

private:
	TSFPTrunk<QuestHelpFrame> m_Trunk;
	TObjRef<VarContainer> m_pVarContainer;
	TObjRef<GUISystem> m_pGUI;

	GUIWnd *m_pWnd;
	GUIWnd *m_pWndCaption;

	GUIFrameMutex *m_pShowFrame;
	GUIButton *m_pButShowFrame;
	GUIButton *m_pButHideFrame;

	GUIButton *m_pButNext;
	GUIButton *m_pButPre;
	GUIStatic *m_pstcPage;

	GUIButton *m_pButClose;
	GUIButton *m_pButExit;

	IconStatic *m_pStcQuestHelpPic;

	vector<tagQuestHelpInfo> m_vecQuestHelpInfo;
	tstring m_strPicName;
	DWORD m_dwCurPage;
	DWORD m_dwTotalPicNum;
	bool m_bShowFrame;
};