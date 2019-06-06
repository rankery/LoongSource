#pragma once
#include "..\WorldDefine\specfunc_define.h"
class ComposeStatic;
struct tagItemPosChangeExEvent;
/*
	class InsuranceFrame
	brief 保底符,根据不同的特殊物品发送相应的网络消息
*/
class InsuranceFrame : public GameFrame
{
public:
	InsuranceFrame(void);
	~InsuranceFrame(void);
	virtual BOOL Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	void ShowFrame(BOOL bShow, EItemSpecFunc eType=EISF_Null, INT64 n64SrcID=GT_INVALID, INT64 n64DstID=GT_INVALID);

private:
	DWORD EventHandler(tagGUIEvent* pEvent);

	DWORD OnEventConsolidateAddItem(tagItemPosChangeExEvent* pGameEvent);

	void SendMsg();

private:
	TSFPTrunk<InsuranceFrame>				m_Trunk;
	TObjRef<Util>							m_pUtil;
	TObjRef<NetCmdMgr>						m_pCmdMgr;
	TObjRef<NetSession>						m_pSession;
	TObjRef<GUISystem>						m_pGUI;
	TObjRef<GameFrameMgr>					m_pFrameMgr;

	GUIWnd*									m_pWnd;
	GUIWnd*									m_pWndBackPic;
	GUIButton*								m_pBtnOk;
	GUIButton*								m_pBtnCancel;
	GUIStatic*								m_pStcHint;
	ComposeStatic*							m_pStcInsurance;

	INT64									m_n64SrcID;
	INT64									m_n64DstID;
	INT64									m_n64InsuranceID;
	EItemSpecFunc							m_eType;

};