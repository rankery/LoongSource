#pragma once

class ActivityPvPFrame : public GameFrame
{
public:
	enum 
	{
		E_PvP			= 0,
		E_PvPMulti,
	};

	enum 
	{
		E_Sort_TeamID	= 2000,
		E_Sort_Name,
		E_Sort_Score,
	};

public:
	ActivityPvPFrame(void);
	~ActivityPvPFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

	void RefreshScoreUI();
	void RefreshSelfUI();

private:
	//控件事件处理函数
	BOOL EventHandler(tagGUIEvent* pEvent);

	//发送查询排行消息
	void SendQueryScore();

	//发送离别活动消息
	void SendQuitActivity();

private:
	TSFPTrunk<ActivityPvPFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<NetSession>				m_pSession;
	TObjRef<NetCmdMgr>				m_pCmdMgr;


	GUIWnd*							m_pWnd_Rank;
	GUIListBox*						m_pLB_RankInfo;		//排行信息
	GUIButton*						m_pBtn_TeamID;		//队伍ID
	GUIButton*						m_pBtn_NameSort;	//姓名排行
	GUIButton*						m_pBtn_Score;		//积分
	GUIButton*						m_pBtn_Cancel;		//关闭排行信息

	GUIWnd*							m_pWnd_Self;
	GUIStatic*						m_pStc_Info;		//个人信息
	GUIButton*						m_pBtn_Rank;		//排行按钮
	GUIButton*						m_pBtn_Exit;		//离开按钮

	DWORD							m_dwActivityID;
};