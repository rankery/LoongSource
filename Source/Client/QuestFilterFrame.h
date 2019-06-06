#pragma once

#define FILTER_NUM 10

struct FilterCache
{
	INT16 n16Category;
	BOOL bSet;
};

class QuestFilterFrame : public GameFrame
{
public:
	QuestFilterFrame();
	~QuestFilterFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
	VOID Show(BOOL bShow);

private:
	BOOL EventHandler(tagGUIEvent *pEvent);
	VOID FlushFilterCache();
	VOID UpdateData();
	VOID RestorDefaultFilterConfig();
	VOID UpdateUI();

private:
	TSFPTrunk<QuestFilterFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;


	GUIWnd *m_pWnd;
	GUIButton *m_pBtnClose;						// 关闭
	GUIButton *m_pBtnDefault;					// 恢复默认设置
	GUIButton *m_pBtnOK;						// 确定
	GUIButton *m_pBtnCancel;					// 取消
	GUIStatic *m_pStcFilterDesc[FILTER_NUM];	// 任务类型
	GUIPushButton *m_pPbnFilter1[FILTER_NUM];	// 方案一
	GUIPushButton *m_pPbnFilter2[FILTER_NUM];	// 方案二


	std::vector<FilterCache> m_vecFilterCache1;
	std::vector<FilterCache> m_vecFilterCache2;
};


