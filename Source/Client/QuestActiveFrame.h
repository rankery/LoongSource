/********************************************************************
	created:	2008/08/16
	created:	16:8:2008   17:36
	filename: 	d:\loong_client\Client\QuestActiveFrame.h
	file path:	d:\loong_client\Client
	file base:	QuestActiveFrame
	file ext:	h
	author:		leili
	
	purpose:	固定活动任务界面声明
*********************************************************************/
#pragma once

#include "QuestQuery.h"
#include "ActivityMgr.h"

class QuestActiveFrame : public GameFrame
{
public:
	QuestActiveFrame(void);
	~QuestActiveFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
	VOID		 Show(void);
	VOID		 Hide(void)		{ if( P_VALID(m_pWnd) ) m_pWnd->SetInvisible(TRUE); }

private:
	BOOL		 EventHandler(tagGUIEvent* pEvent);

	VOID		 ShowQuest(void);
	VOID		 ShowDesc(UINT16 questID);
	bool		 CompareTime(int SrcHour, int SrcMinute, int DestHour, int DestMinute)
	{
		if (((SrcHour * 60 + SrcMinute) - (DestHour * 60 + DestMinute)) > 0)
			return true;
		return false;
	}

private:
	TSFPTrunk<QuestActiveFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;

	GUIWnd						*m_pWnd;				//主界面
	GUIStaticEx					*m_pStcDesc;			//任务描述窗口
	GUIListBox					*m_pListQuests;			//任务列表
	GUIButton					*m_pBtnTime;			//接取时间按钮
	GUIButton					*m_pBtnName;			//任务名称按钮
	GUIButton					*m_pBtnLimit;			//等级限制按钮
	GUIButton					*m_pBtnNPC;				//接取NPC按钮
	GUIButton					*m_pBtnCloseWnd;		//关闭窗口按钮
	GUIPushButton				*m_pPshSwitchCan;		//过滤可做活动

	ActivityMgr::ESortType		m_curSortType;
	DWORD						m_curRow;
	bool						m_showCan;
	ActivityMgr::ESortType		m_sortType;
	std::vector<int>			m_curShowQuestIDs; 
};