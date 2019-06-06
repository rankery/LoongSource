//-----------------------------------------------------------------------------
//!\file gui_msgbox.h
//!\author Lyp
//!
//!\date 2008-11-11
//! last 2008-11-11
//!
//!\brief 标准消息框
//-----------------------------------------------------------------------------
#pragma once

enum MsgBoxFlag
{
	MBF_Null = 0,	// 未确认
	MBF_OK = 1,		// 确定
	MBF_Cancel = 2,	// 取消
};


enum MsgBoxInputType
{
	MBIT_Number		=	0,	// 数值
	MBIT_Password	=	1,	// 密码
	MBIT_Name		=	2,	// 名称
	MBIT_Text		=	3,	// 文字（多文本）
};

enum MsgBoxMoneyType
{
	MBMT_Null		=	0,
	MBMT_Silver		=	1,	// 银两
	MBMT_Yuanbao	=	2,	// 元宝
};

// 互斥3选对话框选择结果
enum MsgOptionBoxFlag
{
	EMOBF_Option1 = 0,	
	EMOBF_Option2 = 1,
	EMOBF_Option3 = 2,
	EMOBF_Null
};

struct tagMsgBoxEvent : public tagGameEvent
{
	tagMsgBoxEvent(LPCTSTR szEventName, GameFrame* pSenderFrame):tagGameEvent(szEventName, pSenderFrame){}
	MsgBoxFlag eResult;
};

struct tagMsgInputBoxEvent : public tagGameEvent
{
	tagMsgInputBoxEvent(LPCTSTR szEventName, GameFrame* pSenderFrame):tagGameEvent(szEventName, pSenderFrame){}
	MsgBoxFlag		eResult;
	MsgBoxInputType eInputType;
	INT				nInputNum;
	tstring			strInputName;
};

struct tagMsgMoneyBoxEvent : public tagGameEvent
{
	tagMsgMoneyBoxEvent(LPCTSTR szEventName, GameFrame* pSenderFrame):tagGameEvent(szEventName, pSenderFrame){}
	MsgBoxFlag		eResult;
	MsgBoxMoneyType	eMonetType;
	INT				nYuanbao;
	INT64			n64Silver;
};

struct tagMsgOptionBoxEvent :public tagGameEvent
{
	tagMsgOptionBoxEvent(LPCTSTR szEventName, GameFrame *pSendFrame) : 
	tagGameEvent(szEventName, pSendFrame) {}
	MsgOptionBoxFlag eResult;	
};

/* 
	使用方法

	CreateObj("Box1", "MsgBox");
	TObjRef<MsgBox>("Box1")->Init(_T("Test"), _T("Hello World"), _T("event1"), MBF_OK, TRUE);

	MsgBoxFlag eResult = TObjRef<MsgBox>("Box1")->GetResult();
	KillObj("Box1");
*/



//-----------------------------------------------------------------------------
// MsgBox
//-----------------------------------------------------------------------------
class MsgBox
{
public:
	virtual BOOL Init(LPCTSTR szCaption, LPCTSTR szText, LPCTSTR szEvent=NULL, 
		MsgBoxFlag eFlag=(MsgBoxFlag)(MBF_OK|MBF_Cancel), BOOL bModal=FALSE);

	MsgBoxFlag GetResult() { return m_eResult; } // 返回MBF_Null代表用户未确认

	MsgBox();
	virtual ~MsgBox();

private:
	TSFPTrunk<MsgBox>		m_Trunk;
	TObjRef<GUISystem>		m_pGUI;
	TObjRef<Util>			m_pUtil;
	TObjRef<GameFrameMgr>	m_pGameFrameMgr;
	
	GUIWnd*					m_pWnd;
	GUIPatch*				m_pWndBack;
	GUIStaticEx*			m_pWndText;
	GUIButton*				m_pWndOK;
	GUIButton*				m_pWndCancel;

	tstring					m_strCaption;
	tstring					m_strText;
	tstring					m_strEvent;

	BOOL					m_bModal;
	MsgBoxFlag				m_eFlag;
	MsgBoxFlag				m_eResult;

	DWORD					m_dwID;

	BOOL EventHandler(tagGUIEvent* pEvent);

	virtual BOOL LoadGUI();
};


/* 
使用方法

CreateObj("Box1", "MsgInputBox");
TObjRef<MsgInputBox>("Box1")->Init(_T("Test"), _T("Hello World"), _T("event1"), GT_INVALID, MBIT_Number, TRUE);

MsgBoxFlag eResult = TObjRef<MsgInputBox>("Box1")->GetResult();
KillObj("Box1");
*/

//--------------------------------------------------------------------------------
// MsgInputBox(输入框)
//--------------------------------------------------------------------------------
class MsgInputBox
{
public:
	virtual BOOL Init(LPCTSTR szCaption, LPCTSTR szText, LPCTSTR szEvent=NULL,
		INT nMaxValue = GT_INVALID, MsgBoxInputType eInput=MBIT_Number, LPCTSTR szDefault=NULL, BOOL bModal=FALSE);

	VOID SetContent(LPCTSTR szContent, BOOL bSelectAll);

	MsgBoxFlag GetResult() { return m_eResult; } // 返回MBF_Null代表用户未确认

	MsgInputBox();
	virtual ~MsgInputBox();

private:
	TSFPTrunk<MsgInputBox>	m_Trunk;
	TObjRef<GUISystem>		m_pGUI;
	TObjRef<Util>			m_pUtil;
	TObjRef<GameFrameMgr>	m_pGameFrameMgr;

	GUIWnd*					m_pWnd;
	GUIPatch*				m_pWndBack;
	GUIStaticEx*			m_pWndText;
	GUIButton*				m_pWndOK;
	GUIButton*				m_pWndCancel;
	GUIEditBox*				m_pWndInput;
	GUIEditBoxEx*			m_pWndInputEx;

	tstring					m_strCaption;
	tstring					m_strText;
	tstring					m_strEvent;

	BOOL					m_bModal;
	MsgBoxFlag				m_eResult;
	MsgBoxInputType			m_eInput;
	
	BOOL EventHandler(tagGUIEvent* pEvent);

	virtual BOOL LoadGUI();
};


//--------------------------------------------------------------------------
// MsgMoneyInputBox(金钱输入框)
//--------------------------------------------------------------------------
class MsgMoneyInputBox
{
public:
	virtual BOOL Init(LPCTSTR szCaption, LPCTSTR szText, LPCTSTR szEvent=NULL,
		INT64 n64MaxGold = 0, INT nMaxYuanBao = 0, INT64 n64Default = 0, INT nDefault = 0,
		MsgBoxMoneyType eInput=(MsgBoxMoneyType)(MBMT_Silver|MBMT_Yuanbao),
		BOOL bModal=FALSE);

	MsgBoxFlag GetResult() { return m_eResult; } // 返回MBF_Null代表用户未确认

	MsgMoneyInputBox();
	virtual ~MsgMoneyInputBox();

private:
	TSFPTrunk<MsgMoneyInputBox>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pGameFrameMgr;

	GUIWnd*						m_pWnd;
	GUIPatch*					m_pWndBack;
	GUIStaticEx*				m_pWndText;
	GUIButton*					m_pWndOK;
	GUIButton*					m_pWndCancel;
	GUIEditBox*					m_pWndYuanBao;
	GUIEditBox*					m_pWndGold;
	GUIEditBox*					m_pWndSilver;

	tstring						m_strCaption;
	tstring						m_strText;
	tstring						m_strEvent;

	BOOL						m_bModal;
	MsgBoxFlag					m_eResult;
	MsgBoxMoneyType				m_eInput;
	INT64						m_n64MaxGold;

	BOOL EventHandler(tagGUIEvent* pEvent);

	virtual BOOL LoadGUI();
};


//--------------------------------------------------------------------------
// MsgPasswordCheckBox(密码确认框)
//--------------------------------------------------------------------------
class MsgPasswordCheckBox
{
	
};




/*!
\brief 互斥3选对话框类
*/
class MsgOptionBox
{
public:
	MsgOptionBox();

	virtual ~MsgOptionBox();


	/*!
	\brief 初始化
	*/
	virtual BOOL Init(LPCTSTR szCaption, LPCTSTR szDescription, 
					  LPCTSTR szOptionText1, LPCTSTR szOptionText2, LPCTSTR szOptionText3, 
					  bool bOption1, bool bOption2, bool bOption3,
					  LPCTSTR szEvent = NULL, BOOL bModal = FALSE);

	/*!
	\brief 取得选择结果
	*/
	MsgOptionBoxFlag GetResult() const { return m_eResult; }

private:
	TSFPTrunk<MsgOptionBox>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pGameFrameMgr;

	GUIWnd*						m_pWnd;
	GUIPatch*					m_pWndBack;
	GUIButton*					m_pBtnCancel;	// 取消按钮
	GUIButton*					m_pBtnSure;		// 确定按钮
	GUIStaticEx*				m_pSeDescption;	// 内容描述
	GUIStaticEx*				m_pSeOption1;	// 选项1描述
	GUIStaticEx*				m_pSeOption2;	// 选项2描述
	GUIStaticEx*				m_pSeOption3;	// 选项3描述
	GUIFrameMutex*				m_pFrameMutex;	// 互斥框架
	GUIPushButton*				m_pBtnOption1;	// 选项1按钮	
	GUIPushButton*				m_pBtnOption2;	// 选项2按钮
	GUIPushButton*				m_pBtnOption3;	// 选项3按钮

	tstring						m_strEvent;		// 事件名
	BOOL						m_bModal;		// 是否模态
	MsgOptionBoxFlag			m_eResult;		// 选择结果
	MsgOptionBoxFlag			m_eTemp;		// 临时结果

	/*!
	\brief 加载GUI
	*/
	virtual BOOL LoadGUI();

	/*!
	\brief 事件处理函数
	*/
	BOOL EventHandler(tagGUIEvent *pEvent);
};


//--------------------------------------------------------------------------------
// MsgBoxEx(只有一个确定按钮)
//--------------------------------------------------------------------------------

/* 
使用方法

CreateObj("Box1", "MsgBoxEx");
TObjRef<MsgBoxEx>("Box1")->Init(_T("Test"), _T("Hello World"), _T("event1"), );

MsgBoxFlag eResult = TObjRef<MsgBoxEx>("Box1")->GetResult();
KillObj("Box1");
*/

class MsgBoxEx
{
public:
	virtual BOOL Init(LPCTSTR szCaption, LPCTSTR szText, LPCTSTR szEvent=NULL, 
		MsgBoxFlag eFlag=(MsgBoxFlag)(MBF_OK), BOOL bModal=TRUE, BOOL bTop=FALSE);

	MsgBoxFlag GetResult() { return m_eResult; } // 返回MBF_Null代表用户未确认

	MsgBoxEx();
	virtual ~MsgBoxEx();

private:
	TSFPTrunk<MsgBoxEx>		m_Trunk;
	TObjRef<GUISystem>		m_pGUI;
	TObjRef<Util>			m_pUtil;
	TObjRef<GameFrameMgr>	m_pGameFrameMgr;

	GUIWnd*					m_pWnd;
	GUIPatch*				m_pWndBack;
	GUIStaticEx*			m_pWndText;
	GUIButton*				m_pWndOK;

	tstring					m_strCaption;
	tstring					m_strText;
	tstring					m_strEvent;

	BOOL					m_bModal;
	MsgBoxFlag				m_eFlag;
	MsgBoxFlag				m_eResult;

	DWORD					m_dwID;

	BOOL EventHandler(tagGUIEvent* pEvent);

	virtual BOOL LoadGUI();
};