//-----------------------------------------------------------------------------
//!\file console_gui.h
//!\author Lyp
//!
//!\date 2004-06-17
//! last 2008-02-15
//!
//!\brief GUI控制台
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {
class ConsoleCommand;
//-----------------------------------------------------------------------------
//!\brief GUI控制台
//!
//!
//-----------------------------------------------------------------------------
class VENGINE_API Console
{
public:
	Console();
	~Console();

	BOOL Init(LPCSTR szVFS, INT nMaxLine=500, BOOL bVisable=TRUE, BOOL bShowCommand=TRUE);
	BOOL Destroy();

	// 设置标题
	VOID SetCaption(LPCTSTR szCaption);
	// 注册命令
	VOID Register(LPCTSTR szName, LPVOID fpFunction, LPCTSTR szDesc=NULL, INT nParamNum=1);
	// 显示文字
	VOID Print(LPCWSTR lpFormat, ...);
	// 监视变量
	VOID Watch(LPCTSTR szName, DWORD dwValue);

	// 控制台开关
	DWORD Switch(DWORD dwID, BOOL bDown);

	TSFPTrunk<Console>& GetTrunk() { return m_Trunk; }
	GUIWindow* GetWindow() { return (GUIWindow*)m_pWnd; }
	ConsoleCommand* GetCommand() { return m_pCommander; }

	GUIEditBoxEx* GetDisplay() { return m_pWndDisplay; }

private:
	TSFPTrunk<Console>			m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<InputMap>			m_pInputMap;
	ConsoleCommand*				m_pCommander;
	
	GUIWnd*						m_pWnd;
	GUIPatch*					m_pWndCaption;
	GUIEditBoxEx*				m_pWndDisplay;
	GUIEditBox*					m_pWndCommand;
	GUIListBox*					m_pWndWatch;
	GUIWnd*						m_pWndSubmit;

	INT							m_nMaxLine;
	BOOL						m_bShowCommand;

	struct tagWatchItem
	{
		DWORD	dwLastRefreshTime;
		DWORD	dwValue;
		INT		nListIndex;
	};

	TMap<DWORD, tagWatchItem*>	m_mapWatch;

	BOOL EventHandler(tagGUIEvent* pEvent);
	DWORD Output(LPCTSTR szText);
	DWORD Cls(DWORD);
	DWORD Excute(LPCTSTR szCommand, BOOL bShowCommand=TRUE);
};






} // namespace vEngine {
