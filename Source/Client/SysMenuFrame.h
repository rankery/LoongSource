#pragma once

/** \class SysMenuFrame
    \brief 右下角系统菜单界面（包括扩展菜单）
*/
class SysMenuFrame : public GameFrame
{
public:
	SysMenuFrame();
	~SysMenuFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	enum EMenuExItem
	{
		EMI_Help,
		EMI_Sociality,
		EMI_Faction,
		EMI_Pet,
		EMI_ZoneMap,
		EMI_WorldMap,
		EMI_Encrypt,
		EMI_SysOptionMenu,
		EMI_Num
	};
private:
	/** \游戏事件响应函数
	*/
	DWORD OnClose_Role_UIEvent(tagGameEvent* );
	DWORD OnClose_Package_UIEvent(tagGameEvent* );
	DWORD OnClose_Dower_UIEvent(tagGameEvent* );
	DWORD OnOpen_Task_UIEvent(tagGameEvent* );
	DWORD OnClose_Task_UIEvent(tagGameEvent* );
	DWORD OnClose_Mall_UIEvent(tagGameEvent* );
	DWORD OnOpen_Role_UIEvent(tagGameEvent*);
	DWORD OnOpen_Dower_UIEvent(tagGameEvent*);
	DWORD OnOpen_Guild_UIEvent(tagGameEvent*);
	DWORD OnClose_Guild_UIEvent(tagGameEvent*);
	DWORD OnClose_Pet_UIEvent(tagGameEvent*);
	DWORD OnClose_Sociality_UIEvent(tagGameEvent*);
	/** \热键响应函数
	*/
	DWORD OnOpenClose_RoleHotKey( DWORD dwID, BOOL bDown );
    DWORD OnOpenClose_RoleInfoKey( DWORD dwID, BOOL bDown );
    DWORD OnOpenClose_RoleEquipKey( DWORD dwID, BOOL bDown );
    DWORD OnOpenClose_RoleTitleKey( DWORD dwID, BOOL bDown );
	DWORD OnOpenClose_PackageHotKey( DWORD dwID, BOOL bDown );
	DWORD OnOpenClose_DowerHotKey( DWORD dwID, BOOL bDown );
	DWORD OnOpenClose_TaskHotKey( DWORD dwID, BOOL bDown );
	DWORD OnOpenClose_ZoneMapHotKey( DWORD dwID, BOOL bDown );
	DWORD OnOpenClose_WorldMapHotKey( DWORD dwID, BOOL bDown );
	DWORD OnOpenClose_SocialityHotKey( DWORD dwID, BOOL bDown );
	DWORD OnOpenClose_HelpHotKey(DWORD dwID, BOOL bDown);
    DWORD OnOpenClose_PetHotKey(DWORD dwID, BOOL bDown);
	DWORD OnOpenClose_Guild(DWORD dwID, BOOL bDown);
	DWORD OnOpenClose_RoleHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_PackageHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_DowerHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_TaskHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_GuildChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_PetHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_SocialityHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_HelpHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_ZoneMapHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnOpenClose_WorldMapHotKeyChange(DWORD dwID, DWORD key1, DWORD key2);
private:
	TSFPTrunk<SysMenuFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<GameInputMap>		m_pKeyMap;
	TObjRef<Util>				m_pUtil;
	TObjRef<GUIRender>			m_pRender;

	GUIWnd*                     m_pWnd;                   //系统菜单窗口
	GUIWnd*						m_pPanel;                 //底版
	GUIPushButton*				m_pBn_Role;               //角色按钮
	GUIPushButton*				m_pBn_Package;            //行囊按钮
	GUIPushButton*				m_pBn_Dower;              //天资按钮
	GUIPushButton*				m_pBn_Task;               //任务按钮
	GUIPushButton*				m_pBn_Mall;               //商城按钮
	GUIPushButton*				m_pBn_Guild;			  //帮派按钮
    GUIPushButton*              m_pBn_Pet;                //灵兽按钮
	GUIPushButton*              m_pBn_Sociality;          //人际按钮
	GUIButton*		            m_pBn_Expand;             //菜单按钮
	
	// TIPS描述，不包含热键
	tstring						m_strTip_Role;               //角色
	tstring						m_strTip_Package;            //行囊
	tstring						m_strTip_Dower;              //天资
	tstring						m_strTip_Task;               //任务
	tstring						m_strTip_Mall;               //商城
	tstring						m_strTip_Guild;				 //帮派
	tstring						m_strTip_Pet;                //灵兽
	tstring						m_strTip_Sociality;          //人际

	BOOL GUIEventHandlerMenu(tagGUIEvent* pEvent);        //系统菜单事件响应回调函数

	GUIWnd*                     m_pWndMenuEx;             //系统扩展菜单窗口
	GUIListBox*                 m_pListMenuEx;            //系统扩展菜单条目
	tstring                     m_strMenuExItemText[EMI_Num];

	BOOL GUIEventHandlerMenuEx(tagGUIEvent* pEvent);      //系统扩展菜单事件响应回调函数
	VOID SafeDestroyUI();
	VOID UpdataMenuSize();
};

