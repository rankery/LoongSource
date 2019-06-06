#pragma once

/** \class SysOptionMenuFrame
    \brief 系统选单界面
*/
class SysOptionMenuFrame : public GameFrame
{
public:
	SysOptionMenuFrame();
	~SysOptionMenuFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
private:
	BOOL GUIEventHandler(tagGUIEvent* pEvent);
	VOID SafeDestroyUI();
	VOID OnOpenGameSettings();
	VOID OnOpenInputSettings();
	VOID OnLogOut( const int nLogType );
private:
	TSFPTrunk<SysOptionMenuFrame>	m_Trunk;
	TObjRef<GUISystem>				m_pGUI;

	GUIWnd*							m_pWnd;						  // 系统选单窗口
	GUIWnd*							m_pPanel;					  // 底版
	GUIButton*						m_pBn_ResumeGame;             // 返回游戏
	GUIButton*						m_pBn_GameSettings;           // 游戏设置
	GUIButton*						m_pBn_InputSettings;	      // 自定义键位
	GUIButton*						m_pBn_ResumeRoleSelect;       // 返回角色选择
	GUIButton*						m_pBn_ResumeLogin;            // 返回帐号登陆
	GUIButton*						m_pBn_QuitGame;			      // 离开游戏
};

