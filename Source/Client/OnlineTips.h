#pragma once

/*************
\!brief (40级以下)角色登陆提示界面
*************/


class OnlineTips : public GameFrame
{
public:
	OnlineTips();
	~OnlineTips();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
	BOOL IsShow() const { return m_bShow; }
	void ShowWnd(BOOL bShow);
private:
	DWORD EventHandler(tagGUIEvent *pEvent);
	DWORD OnRefreshTips(tagGUIEvent *pEvent);
	
	void RefreshCanAcceptQuestsNum();
	void RefreshNotCompleteQuestsNum();
	void RefreshCanAssignAttNum();
	void RefreshCanAssignTalentNum();
	void RefreshBBItemNum();
	void RefreshBBYuanBaoNum();

private:
	TSFPTrunk<OnlineTips> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<NetSession> m_pSession;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;

	GUIWnd *m_pWnd;
	GUIButton *m_pBtnCanAcceptQuestsNum;	// 可接任务数量
	GUIButton *m_pBtnNotCompleteQuestsNum;	// 未完成任务数量
	GUIButton *m_pBtnCanAssignAttNum;		// 可分配属性点
	GUIButton *m_pBtnCanAssignTalentNum;	// 可分配天资点
	GUIButton *m_pBtnBBItemNum;				// 百宝袋物品数量
	GUIButton *m_pBtnBBYuanBaoNum;			// 百宝袋元宝数量
	GUIButton *m_pBtnClose;					// 关闭

	BOOL m_bShow;
};

