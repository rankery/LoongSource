#pragma once

struct tagGuildAffairInfo;

class GuildAffairFrame : public GameFrame
{
public:
	GuildAffairFrame();
	~GuildAffairFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

private:
	DWORD EventHandler(tagGUIEvent *pEvent);
	DWORD OnEventRefresh(tagGameEvent *pEvent);
	void ShowQuest();
	bool IsConditionSatisfaction(tagGuildAffairInfo *pInfo);
	void DestroyMsgBoxes();
	
private:
	TSFPTrunk<GuildAffairFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;

	GUIWnd *m_pWnd;					// 主界面
	GUIListBox *m_pListAffaires;	// 任务列表
	GUIStatic *m_StcpTimes;			// 可发布次数
	GUIButton *m_pBtnIssueQuest;	// 发布帮务
	GUIButton *m_pBtnClose1;		// 右上角X
	GUIButton *m_pBtnClose2;		// 关闭	
	
	DWORD m_dwCurRow;
	DWORD m_dwCurChooseRow;
	std::vector<tagGuildAffairInfo*> m_vecAffairInfo;	// 满足显示条件的帮务
	BYTE m_byGuildLv;
};



