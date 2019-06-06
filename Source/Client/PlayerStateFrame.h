#pragma once

const INT FREE_PK_LEVEL = 30; // 玩家等级达到30级时，PK模式改为自由模式 

/** \行凶状态倒计时开始事件
*/
struct tagPKStateStartCountDownEvent : public tagGameEvent
{
	DWORD dwTimeCount;           // 倒计时总时间（单位：毫秒）
	tagPKStateStartCountDownEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName,pSenderFrame)
	{}
};

class ActiveProgress;
class BuffUIList;
class RoleHeadPicIcon;
struct tagRoleGetNameEvent;
struct tagUpdataAvatarEvent;
struct tagRoleBuffChangeEvent;
struct tagUpdateLPAttEvent;
struct tagLPRoleStateChangeEvent;
struct tagUpdateLPLevel;
struct tagTeamLeaderChangeEvent;
struct tagUpdataAvatarDisplaySetEvent;

/** \class PlayerStateFrame
    \brief 左上角角色属性界面
*/
class PlayerStateFrame : public GameFrame
{
public:
	enum { BUFFUI_NUM = 20 };

	/** \本地玩家菜单选项
	*/
	enum EMenuItem
	{
		EMI_PKMode,             // PK模式
		EMI_RemoveTeamMate,     // 驱逐队员
		EMI_CaptainHandOut,     // 队长移交
		EMI_PickMode,           // 拾取模式
		EMI_ExitTeam,           // 退出队伍
		EMI_RefuseApply,		// 拒绝申请
		EMI_Num
	};

	/** \PK模式菜单选项
	*/
	enum EMenuPKModeItem
	{
		EMPKI_Protected,         // 保护模式
		EMPKI_Freedom,           // 自由PK
		EMPKI_Num
	};

	/** \拾取模式菜单选项
	*/
	enum EMenuPickModeItem
	{
		EMPickI_Freedom,         // 自由拾取
		EMPickI_InTurn,          // 轮流拾取
		EMPickI_Num
	};

	/** \拒绝申请菜单选项
	*/
	enum EMenuRefuseModeItem
	{
		EMRefuse_No,          // 接受申请
		EMRefuse_Yes,         // 拒绝申请
		EMRefuse_Num
	};

	PlayerStateFrame();
	~PlayerStateFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID Render3D();

private:
	BOOL GUIEventHandler(tagGUIEvent* pEvent);
	VOID SafeDestroyUI();
	BOOL CreateMenus();
	VOID UpdataMenuItem();
	VOID ForceResizeMenu( GUIListBox* pListBox, const INT nNumRow );
	VOID SetMenuFromStrTable( GUIListBox* pListBox, const TCHAR* szBaseKey, const INT nLbIndex, const INT nStrIndex );
	VOID SetMenuFromStrTable( GUIListBox* pListBox, const TCHAR* szBaseKey, const INT nNumRow, const INT nStartRow, const INT nEndRow, const BOOL bAutoResize );
	VOID UpdataSecondMenuDisplay();
	VOID ResetSecondMenuPos( const INT nCurMainMenuSel );
	VOID ReBuildTeammateList( GUIListBox* pListBox );
	VOID OnSecondMenuItemClick( const INT nSelItem );
    VOID RefreshClassText();
	BOOL CreatePKMenu();
	/** \游戏事件响应函数
	*/
	DWORD OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent);
	DWORD OnUpdataAvatarEvent(tagUpdataAvatarEvent* pEvent);
	DWORD OnPKStateStartCountDownEvent(tagPKStateStartCountDownEvent* pEvent);
	DWORD OnRoleBuffChangeEvent(tagRoleBuffChangeEvent* pEvent);
	DWORD OnUpdateLPLevel(tagUpdateLPLevel* pEvent);
	DWORD OnUpdateLPAttEvent(tagUpdateLPAttEvent* pEvent);
	DWORD OnLPRoleStateChangeEvent(tagLPRoleStateChangeEvent* pEvent);
	DWORD OnTeamLeaderChangeEvent(tagTeamLeaderChangeEvent* pEvent);
	DWORD OnUpdataAvatarDisplaySetEvent(tagUpdataAvatarDisplaySetEvent* pEvent);
    DWORD OnLocalPlayerSetClass(tagGameEvent* pEvent);
	/** \热键响应函数
	*/
	DWORD OnOpen_ViolenceOnlyHotKey(DWORD dwID, BOOL bDown);		// L_CTL + A
	DWORD OnOpen_ViolenceAllHotKey(DWORD dwID, BOOL bDown);			// L_ALT + A

private:
	TSFPTrunk<PlayerStateFrame>	    m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<NetSession>				m_pSession;
	TObjRef<GameInputMap>			m_pKeyMap;

	GUIWnd*                         m_pWnd;                    // 主窗口
	RoleHeadPicIcon*				m_pHead;				   // 头像图标
	GUIStatic*                      m_pSt_PVP;                 // PVP状态标签
	GUIStatic*                      m_pSt_Name;                // 名称标签
	GUIStatic*                      m_pSt_Level;               // 等级标签
	ActiveProgress*                 m_pPro_Hp;                 // HP进度条
	ActiveProgress*                 m_pPro_Mp;                 // MP进度条
	GUIProgress*                    m_pPro_Sta;                // 持久力进度条
	GUIProgress*					m_pPro_LoongYin;			   // 龙隐值
	GUIStatic*                      m_pSt_Vocation;            // 职业标签
	GUIPushButton*                  m_pBn_Menu;                // 菜单按钮
	    
	GUIStatic*                      m_pSt_Captain;             // 队长图标
	GUIWnd*							m_pWnd_Menu;               // 本地玩家主菜单窗口
	GUIWnd*							m_pWnd_SecondMenu;         // 二级菜单窗口
	GUIListBox*                     m_pLb_Menu;                // 本地玩家主菜单列表框
	GUIListBox*                     m_pLb_SecondMenu;          // 二级菜单菜单列表框
	GUIWnd*							m_pPKMenu;				   // PK菜单
	GUIListBox*						m_pPK_LbMenu;			   // PK菜单列表框

	GUIWnd*                         m_pWnd_Violence;           // 行凶窗口
	GUIPushButton*                  m_pBn_Violence;            // 行凶按钮
	GUIStatic*                      m_pSt_ViolenceTime;        // 行凶倒计时标签

	BuffUIList*                     m_pBuffUIList;                  // buff界面列表
	EMenuPKModeItem					m_ePKMode;                      // 当前的PK模式
	EMenuPickModeItem				m_ePickMode;                    // 当前的拾取模式
	EMenuRefuseModeItem				m_eRefuseApplyMode;				// 当前的拒绝申请模式
	DWORD							m_dwViolenceTimeCount;			// 行凶状态开始计时总时间
	DWORD                           m_dwViolenceStartCountDownTime; // 行凶状态开始计时时间
	INT                             m_nCurMenuIndex[EMI_Num];       // 当前玩家菜单选项对应ListBox行索引
	INT								m_nLastSelMainMenu;             // 一级菜单上一次选中的索引
	INT                             m_nMenuPanelHeightExt;          // 菜单底板高度与listBox的差(缩放菜单时使用)
};

