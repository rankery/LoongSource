#pragma once
#include ".\WorldFrame.h"
class ButtonEx;
class ShortcutButton;
class DragBtn;

struct tagUpdateLPItemEvent;
struct tagUpdateLPSkillEvent;
struct tagRoleGetNameEvent;
struct tagShortcutCacheEvent;
struct tagRolePickEvent;
struct tagNS_VirginOnline;
//-----------------------------------------------------------------------------
// !class ShortcutBarFrame.h
// !brief 快捷栏框架
//-----------------------------------------------------------------------------
class ShortcutBarFrame :
	public GameFrame
{
public:
	ShortcutBarFrame(void);
	~ShortcutBarFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID OnEvent(tagGameEvent* pEvent);
	virtual VOID Update();

	//控件事件处理函数
	DWORD EventHandler(tagGUIEvent* pEvent);

	//加载快捷按钮
	BOOL LinkShortcutBtn(TCHAR* szName, int nBar);

	//创建扩展快捷按钮
	void CreateShortcutBtnEx(char* szName);

	//经验条刷新
	void RefreshRoleExp();

	//获得指定的快捷按钮
	ShortcutButton* GetShortcutButtion(int group, int num);

	void ReflashCDTime();
	void SetBtnEnable();

	// 保存快捷拦数据	
	void SaveBtnData();

private:
    // 角色初次登录设置快捷按钮
    DWORD OnNS_VirginOnline(tagNS_VirginOnline* pMsg, DWORD);

	//刷新普通攻击图片
	DWORD OnWeaponSwap(tagGameEvent* pEvent);

	//显示/隐藏快捷按钮
	void ShowBar(int bar);
	void HideBar(int bar);

	DWORD OnKey(DWORD dwID, BOOL bDown);
	DWORD OnKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnKeyEx(DWORD dwID, BOOL bDown);
	DWORD OnKeyExChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnKey2Move(DWORD dwID, BOOL bDown);
	DWORD OnKey2MoveChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnKey2Move1(DWORD dwID, BOOL bDown);
	DWORD OnKey2MoveChange1(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnRolePickEvent(tagRolePickEvent* pEvent);
	DWORD OnInitLPAttEvent(tagGameEvent* pEvent);
	DWORD OnKeyUpDown(DWORD dwID, BOOL bDown);
	DWORD OnKey2MoveOpenOrClose(DWORD dwID, BOOL bDown);

	// 由DIK得到按键字符串名
	LPCTSTR GetKeyName(DIK key);

	//注册键盘
	void RegisterKeyBoard();

	//更新按钮的可用属性
	void UpdateBtnEnable();		

	// 与背包同步数据
	DWORD OnUpdateLPItemEvent(tagUpdateLPItemEvent* pEvent);
	DWORD OnUpdateLPSkillEvent(tagUpdateLPSkillEvent* pEvent);
	DWORD OnInitedSkillEvent(tagGameEvent* pEvent);

	// 
	DWORD OnShortcutCacheEvent(tagShortcutCacheEvent* pGameEvent);

	// 读取快捷拦数据	
	void LoadBtnData(DWORD dwLocalID);
	void ReflashShortcutBtn();

	DWORD OnRecvNetRoleGetName( tagRoleGetNameEvent* pMsg);

	void ShowMoveShortcutBar(BOOL bShow);
	void ShowMove1ShortcutBar(BOOL bShow);
private:

	TSFPTrunk<ShortcutBarFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameInputMap>		m_pKeyMap;
	TObjRef<NetSession>			m_pSession;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	

	static const int			BAR_COUNT = 10;					//快捷栏分组数
	static const int			BAR_BUTTON_COUNT = 8;			//每组快捷键个数
	static const int			BAR_TITLE_COUNT = 12;
	static const int			BAR_COUNT_TIPS = 4;
	static TCHAR*				m_szAccArray[BAR_TITLE_COUNT+BAR_COUNT_TIPS];
	static int					m_nAccArray[ BAR_TITLE_COUNT+BAR_COUNT_TIPS ];
	static TCHAR*				m_szMoveArray[BAR_BUTTON_COUNT];
	static byte					m_byMoveArray[BAR_BUTTON_COUNT];
	static TCHAR*				m_szMoveArray1[BAR_BUTTON_COUNT];
	static int					m_nMoveArray1[BAR_BUTTON_COUNT];
	static TCHAR*				m_szBarGroup[BAR_COUNT_TIPS];
	static byte					m_byBarGroup[BAR_COUNT_TIPS];
	static TCHAR*				m_szKeyUpDown[2];
	static int					m_nKeyUpDown[2];
	static TCHAR*				m_szKeyMoveOC[2];
	static int					m_nKeyMoveOC[2];
	int							m_nBarGroupCount;				//记录当前快捷栏组号
	FLOAT						m_fCurrentExp;					//当前经验
	DWORD						m_LastSuccSkill;

	GUIWnd*						m_pWnd;
	GUIStatic*					m_pPthBack;						//快捷栏背景
	GUIStatic*					m_pPthShortcutBack;				//快捷栏按钮背景
	GUIStatic*					m_pPthExpBack;					//经验条背景
	GUIProgress*				m_pProExp;						//经验条
	GUIStatic*					m_pStcChoose;					//快捷栏组提示F1-F4
	GUIButton*					m_pBtnMoveDn;					//移动快捷栏组切换向下切换按钮
	GUIButton*					m_pBtnMoveUp;					//移动快捷栏组切换向上切换按钮
	GUIButton*					m_pBtnDn;						//快捷栏组切换向下切换按钮
	GUIButton*					m_pBtnUp;						//快捷栏组切换向上切换按钮

	ShortcutButton*				m_pShortcutBtn[BAR_COUNT][BAR_BUTTON_COUNT];		//4组快捷按钮

	GUIWnd*						m_pWnd_Move1;
	GUIWnd*						m_pWnd_Move2;

	INT							m_nCurExp;						//当前经验
	INT							m_nCurLev;						//当前等级
	INT							m_nCurTotalExp;					//下一次需要的经验
	INT							m_nPctNExp;						//N%经验达成
	BOOL						m_bInitShortcut;				//是否读取过数据

	bool						m_bLPAttIsOK;

public:
	ShortcutButton*				m_pLastPress;					//上次按的快捷键

};
