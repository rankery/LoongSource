#pragma once
#include "..\WorldDefine\selectrole.h"

enum EGUI_Mouse_Event
{
	EMouse_LeftDown = EGUIE_User + 110,		//鼠标左键一直按下旋转按钮(原事件中没有鼠标一直按下的事件)
};

class Player;
class NetSession;
/** \class CreateRoleFrame
\brief 创建角色框架
*/
class CreateRoleFrame :
	public GameFrame
{
public:
	enum ECRState
	{
		ECRState_Cartoon = 0,				//片头动画
		ECRState_Inputing,					//等待玩家输入姓名、选定所有数据
		ECRState_WaitRepose,				//等待服务器响应
		ECRState_PlayAction,				//玩家输入数据后播放一段动画
		ECRState_Close,						//关闭
		ECRState_End,						
	};
public:
	CreateRoleFrame(void);
	~CreateRoleFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID Render3D();

	EPreChapStatus GetChapterStatus() { return m_eStatus; }

	//开始创建角色
	void BeginCreateRole();

	//结束创建角色
	void EndCreateRole();

	//获得角色的序章身份
	ECRState GetCurState()const { return m_curState; }

	//接收服务器消息(参数待定)
	DWORD NetRecvCreateRole(tagNS_CreateRole* pMsg, DWORD pPrama);

private:

	//状态转换开关
	void SwitchToState(ECRState nextState);

	DWORD EventHandler(tagGUIEvent* pEvent);

	//当玩家输入了姓名后并且没有发过创建消息，确定按钮才可用
	BOOL CheckInputedName(); 

	//随机设置角色属性
	void RandAvatarAtt(tagAvatarAtt& att, bool bRandAll=false);

	//更新属性设置
	void UpdateAttUI(bool bSave);

	void Clear();

	//鼠标左键按下旋转按钮
	void CheckMouseLeftDown(float deltaTime);

	//角色旋转
	void RoleRotate(float deltaTime);

	//是否拾取了角色
	bool IsPickRole(int mouseX, int mouseY);

	void CanRolerAnimation(float time);

	void RefreshCamera(bool bFar);

	VOID FilterString(tstring& str);

private:

	TSFPTrunk<CreateRoleFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
	TObjRef<NetSession>				m_pSession;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<VarContainer>			m_pVarContainer;
	TObjRef<InputDX8>				m_pInput;

	GUIWnd*							m_pWnd;
	GUIWnd*							m_pWndCaption;
	GUIEditBox*						m_pName;			//姓名输入控件
	GUIStatic*						m_pStcErrHint;		//错误提示static
	GUIStatic*						m_pStcSex;			//
	GUIFrameMutex*					m_pSexMutex;		//性别互斥控件
	GUIPushButton*					m_pMalePBtn;		//男性pushbutton
	GUIPushButton*					m_pFmalePBtn;		//女性pushbutton
	GUIPushButton*					m_pStatusPBtn[EPCS_End];	//序章身份pushbutton

	GUIButton*						m_pLBtnHType;		//发型左按钮
	GUIButton*						m_pRBtnHType;		//发型右按钮
	GUIStatic*						m_pStcHType;		//发型号显示static

	GUIButton*						m_pLBtnHColor;		//发色左按钮
	GUIButton*						m_pRBtnHColor;		//发色右按钮
	GUIStatic*						m_pStcHColor;		//发色号显示static

	GUIButton*						m_pLBtnFace;		//面部左按钮
	GUIButton*						m_pRBtnFace;		//面部右按钮
	GUIStatic*						m_pStcFace;			//面部号显示static

	GUIButton*						m_pLBtnFDetail;		//面部细节左按钮
	GUIButton*						m_pRBtnFDetail;		//面部细节右按钮
	GUIStatic*						m_pStcFDetail;		//面部细节号显示static

	GUIButton*						m_pLBtnDress;		//服式左按钮
	GUIButton*						m_pRBtnDress;		//服式右按钮
	GUIStatic*						m_pStcFDress;		//服式号显示static

	GUIButton*						m_pBtnOK;			//确定
	GUIButton*						m_pBtnRandom;		//随机
	GUIButton*						m_pBtnCancel;		//返回

	GUIWnd*							m_pWnd_Control;		//
	GUIButton*						m_pLBtnRotate;		//左旋转按钮
	GUIButton*						m_pRBtnRotate;		//右旋转按钮
	GUIPushButton*					m_pPBtn_Push;
	GUIPushButton*					m_pPBtn_Pull;

	ECRState						m_curState;			//当前状态
	float							m_stateTime;		//当前状态持续的时间
	bool							m_bRotate;			//人物是否旋转
	bool							m_bLRotate;			//人物左旋转
	bool							m_bRRotate;			//人物右旋转
	EPreChapStatus					m_eStatus;			//序章身份
	bool							m_bSentCreate;		//是否已经发送过创建角色消息,防止重复发送
	tagAvatarAtt					m_avatarAtt;		//角色属性
	tagAvatarEquip					m_avatarEquip;      //角色装备（新手时装）
	float							m_fRoleYaw;			//角色旋转的航偏值(绕Y轴)
	Player*							m_pPlayer;			//角色	
	INT								m_nRoleNameLength;	//人物姓名的最大长度
	bool							m_bPickRole;		//是否拾取了角色
	BOOL							m_bAnimation;
	float							m_fRolerTimer;
	float							m_fAnimDelay;
};
