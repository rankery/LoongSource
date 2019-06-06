//-----------------------------------------------------------------------------
//!\file LoginFrame.h
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief login frame
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once
#include "..\Cool3D\Animation\AnimationCtrl.h"
#include "..\WorldDefine\selectrole.h"

//-----------------------------------------------------------------------------
//!\brief login frame
//!
//!
//-----------------------------------------------------------------------------
class NetSession;
class Client;
class LoginFrame : public GameFrame
{
public:
	LoginFrame();
	~LoginFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Render3D();
	virtual VOID Update();
	
	VOID SetPlayState(int nState) { m_nPlayState = nState; }

	VOID Connect();

	void SetEncrypt( const char* szEncry , const char* szUserName ); 
	void SetUserNameUI( const char* szUserName );

private:
	TSFPTrunk<LoginFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<XClient>			m_pStream;
	TObjRef<VarContainer>		m_pVarContainer;
	TObjRef<NetSession>			m_pSession;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<Client>				m_pClient;
	
	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndKey;
	GUIStatic*					m_pWndCaption;
	GUIStatic*					m_pWndPswHint;
	GUIEditBox*					m_pEditAccount;
	GUIEditBox*					m_pEditPassword;
	GUIButton*					m_pButtonLogin;
	GUIButton*					m_pButtonQuit;
	GUIStatic*					m_pStcHint;				//错误提示
	GUIPushButton*				m_pPBtn;
	GUIPushButton*				m_pPBtnSaveAccount;
	GUIPushButton*				m_pPBtn_Tab;			//大小写切换
	GUIStatic*					m_pStcSavePass;
	GUIStatic*					m_pStcUser;
	GUIEditBox*					m_pLastActive;

	GUIWnd*						m_pWndWorldList;		//服务器选择
	GUIStatic*					m_pStcWLHint;
	GUIButton*					m_pBtnOKWorld;
	GUIButton*					m_pBtnCancelWorld;
	GUIListBox*					m_pWorldList;

	GUIWnd*						m_pWndQueue;
	GUIStatic*					m_pStcTime;				//排队剩余时间
	GUIStatic*					m_pStcPosition;			//排队的位置
	GUIButton*					m_pBtnCancelQueue;		//取消排队

	GUIWnd*						m_pWndSecurity;			//密保
	GUIStatic*					m_pStcSecuHint;			//密保提示
	GUIEditBox*					m_pStcSecuNum;			//提示的数字

	GUIWnd*						m_pWndSeInputBack;		//密保输入背景
	GUIButton*					m_pBtnSecuInput[10];	//
	GUIStatic*					m_pStcInputHint;		//
	GUIButton*					m_pBtnSecuOK;			//
	GUIButton*					m_pBtnSecuCancel;		

	bool						m_bLoginType;			// 是否是命令行登陆方式
	

	enum EMBPhase
	{
		EMBP_NULL	= -1,					// 开始
		EMBP_First	= 0,					// 密保第一阶段
		EMBP_Second	= 2,					// 密保第二阶段
		EMBP_Third	= 4,					// 密保第三阶段
		EMBP_End	= 6,					// 结束
	};

	enum ELoginState
	{
		ELS_WaitInput,
		ELS_Connect,
		ELS_WaitProofResult,
		ELS_MiBao,
		ELS_Queue,
		ELS_ConnectZoneServer,		// 连接区域服务器
		ELS_SentJoinGame,			// 发送JoinGame消息
		ELS_JoinGame,				// 接收到JoinGame消息后
		ELS_Launcher,				// 版本不对,启动更新程序
	};

	enum
	{
		Loading	= 0,				//0：读盘状态，
		Preface,					//1：播放开场动画，
		Loop,						//2：播放中间动画，
		Over,						//3：播放结束动画，
		Enter,						//4：进入人物选择界面
	};

	enum EXLHint
	{
		EXLHint_User,				//用户名密码提示
		EXLHint_UserError,			//用户名密码错误提示
		EXLHint_SendMsg,			//发送消息提示
		EXLHint_ConnectFail,		//超时
		EXLHint_Account_No_Match,	
		EXLHint_Account_In_Use,
		EXLHint_Account_Freezed,
		EXLHint_Wrong_Build_Number,
		EXLHint_Disabled,
		EXLHint_NoFindLauncher,
		EXLHint_Wrong_Type,
		EXLHint_Server_Full,
		EXLHint_Server_Suspended,
		EXLHint_Server_Maintenance,
		EXLHint_Failed,
		EXLHint_Short_Time,
		EXLHint_No_Select,
		EXLHint_GameWorldName_Wrong,
		EXLHint_SystemError,
		EXLHint_ProofResult_Forbid_MemberCenter,
		EXLHint_ProofResult_Forbid_GMTool,
		EXLHint_ProofResult_Forbid_CellPhone,
		EXLHint_ProofResult_Forbid_MiBao,
	};

	ELoginState					m_eState;
	DWORD						m_dwStateTime;
	DWORD						m_dwZoneIP;			//选择的Zone服务器IP
	DWORD						m_dwZonePort;		//选择的Zone服务器port
	FLOAT						m_fPingTime;		//ping时间
	NullSceneGraph*				m_pSG;				//场景
	CameraEuler					m_Camera;			//摄像机
	EntityNode*					m_EntityNode;		//地物
	VertexAnimationCtrl*		m_pAniCtrl;			//关键帧动画
	float						m_pTime;			//每段动画的开始时间
	ResRenderable*				m_pResRenderable;	//资源
	int							m_nPlayState;		//动画播放的状态
	BOOL						m_bEnter;			//是否进入人物选择界面
	DWORD						m_dwAccountID;		//joingame用到
	DWORD						m_dwVerifyCode;		//joingame用到
	DWORD						m_dwSentJoinGame;
	BOOL						m_bSelectedWorld;
	BOOL						m_bGuard;			//是否防沉迷账号
	BOOL						m_bSave;

	INT							m_nPolicy;			//验证策略
	EMBPhase					m_eMB;				//输入密保阶段
	BOOL						m_bMiBao;			//是否进入密保界面
	tstring						m_strRand;			//记录随机字符串
	string						m_strMiBao;			//记录密保提示
	tstring						m_strInputMiBao;	//记录输入的密保
	BOOL						m_bTab;				//大小写

	char						m_szEncrypt[100];		//迅雷接口返回的加密
	char						m_szUserName[20];		//迅雷接口返回的帐号
	EXLHint						m_eHint;			//迅雷接口提示
	HINSTANCE					m_hDll;

	char						m_szPass[64];	// 台湾密码

	VOID SetState(ELoginState eState);

	DWORD EventHandler(tagGUIEvent* pEvent);
	DWORD EventHandlerForSecurity(tagGUIEvent* pEvent);
	
	VOID WaitProofResult();	
	VOID WaitQueue();
	VOID TryIsConnectZoneSer();

	void JudegPolicy();
	void SendLoginMsg();
	
	// 加载动画
	VOID LoadMap();		

	// 显示用户名\密码错误信息
	VOID ShowUserPwdInfo(DWORD nInfo);

	// 设置UI
	VOID SetUIEnable(BOOL bVale);

	// 随机交换键盘按钮位置
	VOID SetKeyButtonPos();	

	// 处理JoinGame消息
	DWORD NetRecvJoinGame(tagNS_JoinGame* pMsg, DWORD pPrama); 

	// 断开与zone服务器的连接
	VOID CutOffZone();

	// 断开与login服务器的连接
	VOID CutOffLogin();

	// 从注册表读取当前用户名和密码
	VOID LoadLastLoginInfo();
	// 记录当前用户名和密码到注册表
	VOID SaveLastLoginInfo();

	// 过滤用户名和密码中的回车换行等无意义字符
	VOID FilterString(tstring& str);

	// 随机置换密保按钮数字
	void RandSetKeyMB();

	// 判断密判输入阶段
	void MBPhaseHint();
	void ClearMBPhase(){ m_eMB = EMBP_NULL; } 
	void SetMBWndShow(BOOL bVal);

	LPCTSTR GetXunLeiHint(EXLHint eType);

	INT GetWindowMode(DWORD dwIndex);

	void ShellExecuteXunLei();

	void LetterTab();

	bool GetCmdLineUserAndPassword();

	void HideLoginFrame( bool bHide );

	void LoginGame();
};

