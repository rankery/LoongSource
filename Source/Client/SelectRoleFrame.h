//-----------------------------------------------------------------------------
//!\file SelectRoleFrame.h
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief select role frame
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "..\WorldDefine\selectrole.h"

class NetSession;
struct tagMsgBoxEvent;
//-----------------------------------------------------------------------------
//!\brief select role frame
//!
//!
//-----------------------------------------------------------------------------
class SelectRoleFrame : public GameFrame
{
public:
	enum ESafeCodeState
	{
		ESCS_NULL			= 0,		//	没有安全码
		ESCS_HasSafeCode	= 1,		//	安全码已输入
		ESCS_Reseting		= 2,		//	安全码正在重置
		ESCS_Reseted		= 3,		//  安全码重置时候已到
	};
public:
	SelectRoleFrame();
	~SelectRoleFrame();

	enum ESRState
	{
		ESRS_Loading,					//加载资源
		ESRS_EnumRole,					//向zone服务器请求玩家的角色信息
		ESRS_Inputing,					//等待玩家输入
		ESRS_Deling,					//删除角色中
		ESRS_SentSelectRole,			//发送选择角色进入游戏世界
		ESRS_RecvSelectRole,			//接收角色进入游戏世界
		ESRS_SelectedRole,				//选择了角色
		ESRS_Disconnect,				//与服务器断开连接
		ESRS_Back,						//返回到登录
	};
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Render3D();
	virtual VOID Update();
	VOID LoadMap();
	
	VOID SetCurState(ESRState state) { m_eCurState = state; m_dwStateTime = timeGetTime();}
	
	// 设置摄像机
	void SetCamera();

private:
	DWORD EventHandler(tagGUIEvent* pEvent);

	BOOL EnterGame(DWORD dwLocalRoleID);
	VOID SendEnterGameMsg();
	BOOL ReturnGame();
	BOOL CreateRole();

	VOID SendDeleRoleMsg();

	BOOL DeleteRole();

	//重新设置PButton状态
	void ReSetPButtonState(int nSele);

	//--UI设置
	VOID UpdateUI();
	VOID UpdateSafeCodeUI();
	VOID SetDeleteRoleUI(BOOL bVal);

	// 接收服务器删除角色消息
	DWORD NetRecvDeleteRole(tagNS_DeleteRole* pMsg, DWORD pPrama);

	// 接收玩家的角色信息
	DWORD NetRecvEnumRole(tagNS_EnumRole* pMsg, DWORD pPrama);

	// 接收玩家的选择角色信息
	DWORD NetRecvSelectRole(tagNS_SelectRole* pMsg, DWORD pPrama);

	//--安全码
	void JudeSafeCodeState();
	VOID SendSafeCodeMsg();
	DWORD NetRecvNS_SafeCode(tagNS_SafeCode* pMsg, DWORD pParam);
	DWORD NetRecvNS_ResetSafeCode(tagNS_ResetSafeCode* pMsg, DWORD pParam);
	DWORD NetRecvNS_CancelSafeCodeReset(tagNS_CancelSafeCodeReset* pMsg, DWORD pParam);
	DWORD OnResetSafeCodeEvent(tagMsgBoxEvent* pEvent);
	DWORD OnCancelSafeCodeEvent(tagMsgBoxEvent* pEvent);
	DWORD OnSafeCodeIntroductionEvent(tagMsgBoxEvent* pEvent);
	DWORD OnAcceptDeleteRole( tagMsgBoxEvent* pEvent );
	void DisplayResetSafeCodeTime();

	//检查安全码是否符合标准
	int IsValidSafeCode(const TCHAR* szSafeCode);

	void CreateSafeCodeTime();

	//
	DWORD OnDisconnectServer(tagMsgBoxEvent* pGameEvent);

private:
	TSFPTrunk<SelectRoleFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<InputDX8>			m_pInput;
	
	GUIWnd*						m_pWndSelct;
	GUIWnd*						m_pWndMain;
	GUIWnd*						m_pWndDel;

	GUIStatic*					m_pStc_Sele_Name;
	GUIStatic*					m_pStc_Sele_Info1;
	GUIStatic*					m_pStc_Sele_Info2;
	GUIStatic*					m_pWndHintInfo;
	GUIButton*					m_pButtonCreate;
	GUIButton*					m_pButtonDelete;
	GUIButton*					m_pButtonEenter;
	GUIButton*					m_pButtonReturn;
	GUIButton*					m_pBtnSafeCode;
	GUIButton*					m_pBtnDelOk;
	GUIButton*					m_pBtnDelCancel;
	GUIEditBox*					m_pEditSafeCode;
	GUIStatic*					m_pStcDelHint;

	GUIWnd*						m_pWndSD;
	GUIStatic*					m_pStcSDHint1;
	GUIStatic*					m_pStcSDHint2;
	GUIEditBox*					m_pEdtSafeCode1;
	GUIEditBox*					m_pEdtSafeCode2;
	GUIButton*					m_pBtnSDOK;
	GUIButton*					m_pBtnSDCancel;
	GUIStatic*					m_pStcSDHint;

	int							m_nCurSelRole;
	int							m_nLastSelRole;
	DWORD						m_nSelectRoleID;
	ESRState					m_eCurState;	
	DWORD						m_dwStateTime;
	DWORD						m_dwMouseDown;
	DWORD						m_dwErrorTime;		//错误提示框开始的时间
	ESafeCodeState				m_eSafeCodeState;
};
