/********************************************************************
	created:	2008/04/30
	created:	30:4:2008   12:01
	filename: 	d:\loong_client\Client\QuestMainFrame.h
	file path:	d:\loong_client\Client
	file base:	QuestMainFrame
	file ext:	h
	author:		leili
	
	purpose:	任务主界面声明
*********************************************************************/
#pragma once
#include "AppliedTree.h"
#include "IconStatic.h"
#include "QuestMgr.h"

class QuestMainFrame : public GameFrame
{
public:
	QuestMainFrame(void);
	~QuestMainFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	VOID		 InitFocus(void);
	VOID		 Show(bool bReset=false);
	VOID		 Hide(void);
	BOOL		 IsVisible(void) const;

	VOID		 DeleteQuest(UINT16 questID);
	VOID		 AddQuest(UINT16 questID,bool sel=false);
	VOID		 SetQuestCompleteFlag(UINT16 questID);
	VOID		 SetQuestTrackFlag(UINT16 questID);
	VOID		 UpdateQuestDesc(UINT16 questID);
	VOID		 ChangeColor(void);

private:
	BOOL		 EventHandler(tagGUIEvent* pEvent);
	VOID		 SetQuestDesc(UINT16 questID);

	DWORD		 OnEvent_CloseTrack(tagGameEvent* pEvent);
	DWORD		 OnEvent_CloseNPC(tagGameEvent* pEvent);
	DWORD		 OnEvent_OpenNPC(tagGameEvent* pEvent);
	DWORD		 OnEvent_CancelQuest(tagMsgBoxEvent* pEvent);

	VOID		 _ChangeColor(tagGUITreeItem* pItem);

private:
	TSFPTrunk<QuestMainFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<InputDX8>				m_pInput;
	TObjRef<MsgBox>					m_pMsgBox;

	GUIWnd							*m_pWnd;				//主界面
	AppliedTree						*m_pTreeTaskList;		//任务列表Tree
	GUIButton						*m_pBtnQuestNpc;		//显示/隐藏可接任务NPCTree按钮
	GUIButton						*m_pBtnActiveQuest;		//显示/隐藏活动任务窗口按钮
	GUIButton						*m_pBtnQuestTrack;		//显示/隐藏任务追踪窗口按钮
	GUIButton						*m_pBtnDelQuest;		//删除任务按钮
	GUIButton						*m_pBtnCloseWnd1;		//关闭任务主界面按钮，上
	GUIButton						*m_pBtnCloseWnd2;		//关闭任务主界面按钮，下

	GUIStaticEx						*m_pStcCondition;		//显示任务条件窗口
	GUIStatic						*m_pStcBonusExp;		//经验奖励
	GUIStatic						*m_pStcBonusMoney1;		//金钱奖励1
	GUIStatic						*m_pStcBonusMoney2;		//金钱奖励2
	vector<GUIStatic*>				m_pStcBonusSpecT;
	vector<GUIStatic*>				m_pStcBonusSpecV;
	GUIScrollBar					*m_pSrlCondition;

	typedef std::vector<IconStatic*> GUIStaticList;
	GUIStaticList					m_stcBonusGoods;		//物品奖励
	GUIStaticList					m_stcBonusSels;			//可选奖励
	DWORD							m_BonusGoodsID[QUEST_REW_ITEM];
	DWORD							m_BonusSelsID[QUEST_REW_ITEM];

	typedef std::map<DWORD, UINT16> TreeQuestMap;
	TreeQuestMap					m_tree2quest;

	bool							m_showNPC;
	bool							m_showTrack;

	vector<tagIDLink>				m_idLinks;
	vector<tagPosLink>				m_posLinks;
};
