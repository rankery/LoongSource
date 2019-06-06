#pragma once

class Role;
class Player;
class RoleHeadPicIcon;
class BuffUIList;
struct tagTeammateStateInitEvent;
struct tagTeamLeaderChangeEvent;
struct tagTeammateLevelChangeEvent;
struct tagTeammateNameUpdataEvent;
struct tagTeammateAttChangeEvent;
struct tagRoleBuffChangeEvent;
struct tagUpdataAvatarEvent;
struct tagUpdataAvatarDisplaySetEvent;

/** \class TeammateStateWnd
    \brief 一个队友的显示窗口，管理显示队友的控件，
	       外部可创建多个实例来同时显示多个队友的状态
*/
class TeammateStateWnd
{
public:
	enum { BUFFUI_NUM = 20 };

	TeammateStateWnd();
	~TeammateStateWnd();

	BOOL Create( XmlElement* pEle, const DWORD dwRoleID );
	BOOL SafeDestroy();
	VOID Update();
	VOID Render3D();
	
	GUIWnd* GetWnd() { return m_pWnd; }
	DWORD   GetRoleID() const { return m_dwRoleID; }
	VOID    InitState();
private:
	BOOL GUIEventHandler(tagGUIEvent* pEvent);
	VOID UpdataBuffs( Role* pRole );
	/** \游戏事件响应
	*/
	DWORD OnTeammateStateInitEvent(tagTeammateStateInitEvent* pEvent);
	DWORD OnTeamLeaderChangeEvent(tagTeamLeaderChangeEvent* pEvent);
	DWORD OnTeammateLevelChangeEvent(tagTeammateLevelChangeEvent* pEvent);
	DWORD OnTeammateNameUpdataEvent(tagTeammateNameUpdataEvent* pEvent);
	DWORD OnTeammateAttChangeEvent(tagTeammateAttChangeEvent* pEvent);
	DWORD OnRoleBuffChangeEvent(tagRoleBuffChangeEvent* pEvent);
	DWORD OnUpdataAvatarEvent(tagUpdataAvatarEvent* pEvent);
	DWORD OnUpdataAvatarDisplaySetEvent(tagUpdataAvatarDisplaySetEvent* pEvent);
private:
	TSFPTrunk<TeammateStateWnd>	    m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<GameFrameMgr>           m_pMgr;

	GUIWnd*                         m_pWnd;                    // 主窗口
	RoleHeadPicIcon*				m_pHead;				   // 头像图标
	GUIStatic*                      m_pSt_Name;                // 名称标签
	GUIStatic*                      m_pSt_Level;               // 等级标签
	GUIProgress*                    m_pPro_Hp;                 // HP进度条
	GUIProgress*                    m_pPro_Mp;                 // MP进度条
	BuffUIList*                     m_pBuffUIList;
	GUIStatic*                      m_pSt_Captain;             // 队长图标

	DWORD                           m_dwRoleID;
	DWORD							m_dwLastFindRoleTime;
	BOOL                            m_bInView;                 // 是否在视野中 
};

