/********************************************************************
	created:	2008/11/04
	created:	4:11:2008   13:55
	filename: 	d:\loong_client\Client\QuestTrackFrame.h
	file path:	d:\loong_client\Client
	file base:	QuestTrackFrame
	file ext:	h
	author:		leili
	
	purpose:	任务追踪界面声明
*********************************************************************/
#pragma once

class QuestTrackFrame : public GameFrame
{
public:
	QuestTrackFrame(void);
	~QuestTrackFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();

	VOID		 Show(void);
	VOID		 Hide(void);
	BOOL		 IsVisible(void) const;

	VOID		 UpdateQuestTrack(void);
	BOOL		 CanTrackQuest(UINT16 questID);

private:
	BOOL		 EventHandler(tagGUIEvent* pEvent);
	BOOL		 AddQuestTrack(UINT16 questID);

private:
	TSFPTrunk<QuestTrackFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;

	GUIWnd						*m_pWnd;				//主界面
	GUIStaticEx					*m_pQuest;				//跟踪描述
	GUIButton					*m_pBtnCloseWnd;		//关闭任务跟踪界面按钮

	int							m_trackCount;			//跟踪的任务数量
};