/********************************************************************
	created:	2008/08/16
	created:	16:8:2008   18:02
	filename: 	d:\loong_client\Client\QuestNPCFrame.h
	file path:	d:\loong_client\Client
	file base:	QuestNPCFrame
	file ext:	h
	author:		leili
	
	purpose:	可接任务NPC界面声明
*********************************************************************/
#pragma once

class QuestNPCFrame : public GameFrame
{
public:
	QuestNPCFrame(void);
	~QuestNPCFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	VOID		 Show(void);
	VOID		 Hide(void);

	BOOL		 IsVisible(void) const;
	BOOL		 ReloadUI(LPCSTR alignCtrl, LPCSTR alignType, LPCSTR szNewName = NULL );
	VOID		 Align(void);

	VOID		 UpdateNPCList(void);
	VOID         UpdateUI(void);

private:
	BOOL		 EventHandler(tagGUIEvent* pEvent);
	DWORD		 OnEventQuestFilterConfigChange(tagGameEvent *pEvent);

private:
	TSFPTrunk<QuestNPCFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;

	GUIWnd						*m_pWnd;				//主界面
	GUIListBox					*m_pListNPCs;			//NPC列表
	GUIButton					*m_pBtnCloseWnd;		//关闭窗口按钮
	GUIPushButton				*m_pPbnFilterByLevel;	//过滤低等级任务
	GUIPushButton				*m_pPbnFilterByType;	//按类型过滤任务
	GUIPushButton				*m_pPbnType1;			//方案一
	GUIPushButton				*m_pPbnType2;			//方案二
	GUIStatic					*m_pStcType1;			//方案一说明
	GUIStatic					*m_pStcType2;			//方案二说明
	GUIButton					*m_pBtnConfig;			//手动配置任务过滤
	

	
	DWORD						m_curRow;
	vector<DWORD>				m_optflags;
};