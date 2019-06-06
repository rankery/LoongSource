/********************************************************************
	created:	2009/08/07
	created:	7:8:2009   18:12
	filename: 	d:\loong_client\Client\QuizFrame.h
	file path:	d:\loong_client\Client
	file base:	QuizFrame
	file ext:	h
	author:		leili
	
	purpose:	答题界面声明
*********************************************************************/
#pragma once

class QuizFrame : public GameFrame
{
public:
	QuizFrame(void);
	~QuizFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	VOID		 Show(void);
	VOID		 Hide(void);

	VOID		 SetContent(LPCTSTR szContent);
	VOID		 SetOption(LPCTSTR szOption, DWORD param1, DWORD param2);
	VOID		 SetCD(DWORD cd);
	VOID		 SetNum(DWORD num);
	VOID		 Clear(void);

	VOID		 SetTarget(INT targetType, DWORD targetID) { m_targetType=targetType;m_targetID=targetID; }

private:
	BOOL		 EventHandler(tagGUIEvent* pEvent);

private:
	TSFPTrunk<QuizFrame>	m_Trunk;
	TObjRef<GUISystem>		m_pGUI;

	GUIWnd					*m_pWnd;				// 主界面
	GUIStatic				*m_pStcDesc;			// 题目描述
	GUIStatic				*m_pStcOptA;			// 选项A
	GUIStatic				*m_pStcOptB;			// 选项B
	GUIStatic				*m_pStcOptC;			// 选项C
	GUIStatic				*m_pStcOptD;			// 选项D
	GUIPushButton			*m_pPshA;				// 选择框A
	GUIPushButton			*m_pPshB;				// 选择框B
	GUIPushButton			*m_pPshC;				// 选择框C
	GUIPushButton			*m_pPshD;				// 选择框D
	GUIStatic				*m_pStcCD;				// 倒计时
	GUIStatic				*m_pStcNum;				// 答题数目
	GUIButton				*m_pBtnSubmit;			// 提交
	GUIButton				*m_pBtnCancel;			// 放弃
	GUIButton				*m_pBtnCloseWnd;		// 关闭界面按钮，上

	DWORD					m_curOpt;				// 当前选项索引
	DWORD					m_questTime;			// 答题时间
	DWORD					m_escapeTime;			// 答题逝去时间

	struct tagOptAttr 
	{
		DWORD		id;
		DWORD		step;
	};
	vector<tagOptAttr>		m_optAttrs;

	INT						m_targetType;
	DWORD					m_targetID;
};