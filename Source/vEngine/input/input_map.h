//-----------------------------------------------------------------------------
//!\file input_map.h
//!\author Lyp
//!
//!\date 2008-02-27
//! last 2008-02-27
//!
//!\brief 键盘命令注册
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {
typedef DWORD (WINAPI* INPUTMAPEVENTHANDLE)(DWORD dwID, BOOL bDown);
typedef DWORD (WINAPI* INPUTMAPCHANGEEVENTHANDLE)(DWORD dwID, DWORD key1, DWORD key2);
//-----------------------------------------------------------------------------
//!\brief GUI键盘命令注册
//! 使用形如 INPUTMAPEVENTHANDLE 的函数来注册按键回调事件
//! 使用形如 INPUTMAPCHANGEEVENTHANDLE 的函数来注册热键变化回调事件
//-----------------------------------------------------------------------------
class VENGINE_API InputMap
{
public:
	InputMap();
	~InputMap();

	
	virtual BOOL LoadGUI(LPCSTR szVFS, LPCTSTR szWindowName, tagPoint& ptPos);
	virtual VOID UnloadGUI();
	virtual VOID Update();

	// 注册命令
	virtual BOOL Register(DWORD dwID, LPCTSTR szDesc, INPUTMAPEVENTHANDLE fpFunction, 
		DWORD key, BOOL bShow=TRUE, BOOL bEnableInEdit=FALSE, INT nUserData=0, 
		INPUTMAPCHANGEEVENTHANDLE fpChangeFunction=0);
	virtual BOOL Unregister(LPCTSTR szDesc);

	// 临时设置按键是否有效
	virtual BOOL SetEnable(LPCTSTR szDesc, BOOL bEnable);


	// 由DIK得到按键字符串名
	LPCTSTR GetKeyName(DIK key);

protected:
	TSFPTrunk<InputMap>			m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<InputDX8>			m_pInput;
	TObjRef<Util>				m_pUtil;
	
	GUIWindow*					m_pWnd;
	GUIStatic*					m_pWndCaption;
	GUIListBox*					m_pWndList;
	GUIButton*					m_pWndOK;

	struct tagKeyMapInfo
	{
		DWORD		dwID;
		DWORD		dwKey;
		DWORD		dwOriginKey;
		DWORD		dwDefaultKey;
		tstring		strDesc;		// 描述
		BOOL		bEnable;		// 是否有效
		BOOL		bEnableInEdit;	// 当焦点是Edit时，是否仍有效
		BOOL		bShow;
		INT			nUserData;

		INPUTMAPEVENTHANDLE	pfn;
		INPUTMAPCHANGEEVENTHANDLE pfnChange;
	};


	TMap<DWORD, tagKeyMapInfo*>	m_mapKeyFunction;	// DescCRC -> info	注意是描述的ID
	TList<DWORD>				m_listKeyDesc;		// 用来记录注册顺序
	DWORD						m_dwCurrentSet;		// 当前正在设置的键
	DWORD						m_dwTempKey;		// 输入暂存

	BOOL EventHandler(tagGUIEvent* pEvent);
	
	VOID UpdateSetup();	// 设置状态时的更新
	virtual VOID ShowKeys();	// 重新显示所有按键信息
	
};






} // namespace vEngine {
