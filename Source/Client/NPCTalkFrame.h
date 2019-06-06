/********************************************************************
	created:	2008/08/16
	created:	16:8:2008   17:18
	filename: 	d:\loong_client\Client\NPCTalkFrame.h
	file path:	d:\loong_client\Client
	file base:	NPCTalkFrame
	file ext:	h
	author:		leili
	
	purpose:	普通NPC对话界面声明
*********************************************************************/
#include "Item.h"

#pragma once

class NPCTalkFrame : public GameFrame
{
public:
	enum EOptionType
	{
		EOT_None			= -1,		//
		EOT_AcceptQuest		= 0,		// 可接任务选项
		EOT_CompleteQuest,				// 可交任务选项
		EOT_ScenarioTalk,				// 情景对话选项
		EOT_ProduceSkillTalk,			//
		EOT_LearnProduceSkill,			//
		EOT_LeaveProduceSkill,			//
		EOT_CancelProduceSkill,			//
		EOT_ServerCtrl,					//	服务器控制选项
		EOT_GuildCommerce,				// 帮派跑商
        EOT_SetTransportPoint,          // 设置可记录传送点(added by hyu on 2009-11-27)
	};

public:
	NPCTalkFrame(void);
	~NPCTalkFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	VOID		 Show(void);
	VOID		 Hide(void);

	VOID		 SetCaption(LPCTSTR szCaption);
	VOID		 SetContent(LPCTSTR szContent);
	VOID		 ClearOptions(void);
	VOID		 AddTalkOption(LPCTSTR szOption, DWORD param1, DWORD param2, EOptionType optionType);
	VOID		 AddAcceptOptions(DWORD npcID);
	VOID		 AddCompleteOptions(DWORD npcID);

	VOID		 SetTarget(INT targetType, DWORD targetID) { m_targetType=targetType;m_targetID=targetID; }

    void SetTransItem(Item* pItem) { m_pTransportItem = pItem; }

    void SetTransportItemEnable(bool bEnabled) { m_isUsingTransportItem = bEnabled; }

private:
	BOOL		 EventHandler(tagGUIEvent* pEvent);

private:
	TSFPTrunk<NPCTalkFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;

	GUIWnd						*m_pWnd;				//主界面
	GUIPatch					*m_pCaption;			//标题
	GUIStaticEx					*m_pStcCondition;		//对话内容窗口
	GUIListBox					*m_pListOptions;		//对话选项
	GUIButton					*m_pBtnCloseWnd;		//关闭界面按钮，上

	DWORD						m_curRow;
	DWORD						m_escapeTime;			//毫秒

	struct tagOptAttr 
	{
		DWORD		id;
		DWORD		step;
		EOptionType	type;
	};
	vector<tagOptAttr>			m_optAttrs;

	INT							m_targetType;
	DWORD						m_targetID;

    Item*                       m_pTransportItem;       // 可记录传送符item
    bool                        m_isUsingTransportItem; // 是否正在使用TransportItem
};