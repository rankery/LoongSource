#pragma once

/** \私聊游戏事件
*/

class BuffUIList;
class RoleHeadPicIcon;
class Role;
class Player;
class NPC;
struct tagRolePickEvent;
struct tagRoleGetNameEvent;
struct tagUpdataAvatarEvent;
struct tagRoleBuffChangeEvent;
struct tagUpdataAvatarDisplaySetEvent;
struct tagApplyJoinTeamEvent;

/** \class TargetStateFrame
    \brief 当前目标界面
*/
class TargetStateFrame : public GameFrame
{
public:
	enum { BUFFUI_NUM = 20 };

	/** \目标菜单选项
	*/
	enum EMenuItem
	{
		EMI_PrivateChat,       // 私    聊
		EMI_Friend,            // 好    友
		EMI_Trade,             // 交    易
		EMI_Follow,            // 跟    随
		EMI_TeamInvite,        // 组队邀请
		EMI_FactionInvite,     // 帮派邀请
		EMI_PlayerInfo,        // 玩家信息
		EMI_AddToBlackList,    // 加黑名单
        EMI_PetTrade,          // 灵兽交易
		EMT_MountInvite,	   // 邀请驾御
		EMI_TeamApply,         // 申请入队
		EMI_PlayerInfo2,	   // 玩家名帖
		EMI_Num,
	};

	TargetStateFrame();
	~TargetStateFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID Render3D();
	virtual BOOL EscCancel();
private:
	BOOL GUIEventHandler(tagGUIEvent* pEvent);
	VOID SafeDestroyUI();

	VOID OnOpen( const DWORD dwRoleID );
	VOID OnClose();

	VOID UpdataRoleData( Role* pRole );           // 更新角色数据显示
	VOID UpdataPlayerData( Player* pPlayer );     // 更新Player特有的数据显示
	VOID UpdataNPCData( NPC* pNpc );              // 更新NPC特有的数据显示
	VOID UpdataMenuItem();
	VOID ForceResizeMenu( GUIListBox* pListBox, const INT nNumRow );
	VOID SetMenuFromStrTable( GUIListBox* pListBox, const TCHAR* szBaseKey, const INT nLbIndex, const INT nStrIndex );
	VOID SetMenuFromStrTable( GUIListBox* pListBox, const TCHAR* szBaseKey, const INT nNumRow, const INT nStartRow, const INT nEndRow, const BOOL bAutoResize );
	VOID RebuildBuffs( Role* pRole );
    VOID SetPlayerClassName(DWORD dwRoleID);
    DWORD OnChangePlayerClass(tagGameEvent* pEvent);
	/** \游戏事件响应函数
	*/
	DWORD OnRolePickEvent(tagRolePickEvent* pEvent);
	DWORD OnRoleGetNameEvent(tagRoleGetNameEvent* pEvent);
	DWORD OnUpdataAvatarEvent(tagUpdataAvatarEvent* pEvent);
	DWORD OnRoleBuffChangeEvent(tagRoleBuffChangeEvent* pEvent);
	DWORD OnUpdataAvatarDisplaySetEvent(tagUpdataAvatarDisplaySetEvent* pEvent);

private:
	TSFPTrunk<TargetStateFrame>	    m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	
	GUIWnd*                         m_pWnd;                    // 主窗口
	RoleHeadPicIcon*				m_pHead;				   // 头像图标
	GUIStatic*                      m_pSt_PVP;				   // PVP状态标签
	GUIStatic*                      m_pSt_Name;                // 名称标签
	GUIStatic*                      m_pSt_Level;               // 等级标签
	GUIProgress*                    m_pPro_Hp;                 // HP进度条
	GUIProgress*                    m_pPro_Mp;                 // MP进度条
	GUIStatic*                      m_pSt_AppendInfo;          // 附加信息标签
	GUIPushButton*                  m_pBn_Menu;                // 菜单按钮
	
	GUIWnd*							m_pWnd_Menu;               // 目标菜单窗口
	GUIListBox*                     m_pLb_Menu;                // 目标菜单列表框

	BuffUIList*                     m_pBuffUIList;             // BUFF控件列表
	DWORD                           m_dwCurShowRoleID;         // 当前显示角色的ID
	DWORD                           m_dwSelectEffectID;        // 选中特效ID
	INT								m_nCurMenuIndex[EMI_Num];  // 当前目标菜单选项对应ListBox行索引
	INT								m_nMenuPanelHeightExt;

	INT								m_nLastLevel;              // 记录上次显示的数据，不必每次都刷新界面
	INT								m_nLastMaxHp;
	INT								m_nLastHp;
	INT								m_nLastMaxMp;
	INT								m_nLastMp;
	bool							m_bLastNameGray;
};

