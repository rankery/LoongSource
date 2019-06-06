/********************************************************************
	created:	2008/08/15
	created:	15:8:2008   17:16
	filename: 	d:\loong_client\Client\QuestTalkFrame.h
	file path:	d:\loong_client\Client
	file base:	QuestTalkFrame
	file ext:	h
	author:		leili
	
	purpose:	任务对话界面声明
*********************************************************************/
#pragma once

#include "IconStatic.h"

class QuestTalkFrame : public GameFrame
{
public:
	QuestTalkFrame(void);
	~QuestTalkFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	VOID		 Show(void);
	VOID		 Hide(void);
	BOOL		 IsVisible(void) const;

	VOID		 SetCaption(LPCTSTR szCaption);
	VOID		 SetContent(UINT16 questID, LPCTSTR szContent, bool accept);

	VOID		 SetCanSelect(bool bCanSel) { m_canSelect = bCanSel; }
	VOID		 SetTrigger(bool bIsTrigger){ m_isTrigger = bIsTrigger; }

private:
	BOOL		 EventHandler(tagGUIEvent* pEvent);
	bool		 AlreadySelRewItem(int& index);

	VOID		 SetSpeRew(const tagQuestProto* pQuestProto);

private:
	TSFPTrunk<QuestTalkFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;

	GUIWnd						*m_pWnd;				//主界面
	GUIPatch					*m_pCaption;			//标题
	GUIStaticEx					*m_pStcCondition;		//对话内容窗口

	GUIStatic					*m_pStcBonusExp;		//经验奖励
	GUIStatic					*m_pStcBonusMoney1;		//金钱奖励1
	GUIStatic					*m_pStcBonusMoney2;		//金钱奖励2
	vector<GUIStatic*>			m_pStcBonusSpecT;
	vector<GUIStatic*>			m_pStcBonusSpecV;

	typedef std::vector<IconStatic*> GUIStaticList;
	GUIStaticList				m_stcBonusGoods;		//物品奖励
	GUIStaticList				m_stcBonusSels;			//可选奖励
	GUIStaticList				m_stcBonusGoodOuters;	//物品奖励选择框
	GUIStaticList				m_stcBonusSelOuters;	//可选奖励选择框
	DWORD						m_BonusGoodsID[QUEST_REW_ITEM];
	DWORD						m_BonusSelsID[QUEST_REW_ITEM];

	GUIButton					*m_pBtnCloseWnd;		//关闭任务主界面按钮
	GUIButton					*m_pBtnOK;				//确定按钮
	GUIButton					*m_pBtnCancel;			//取消按钮

	UINT16						m_questID;
	bool						m_isAccept;

	bool						m_selIndex[QUEST_REW_ITEM];
	bool						m_canSelect;			//是否可以选择可选奖励
	bool						m_isDynamic;			//是否动态任务
	bool						m_hasRewChoicesItem;
	bool						m_isTrigger;			//是否由触发器引起

	DWORD						m_escapeTime;			//毫秒
};