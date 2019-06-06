#pragma once
#include "gameframe.h"
#include "QuestMgr.h"

struct tagNS_ExitGamePrompt;

class ExitPromptFrame : public GameFrame
{
public:
	ExitPromptFrame(void);
	~ExitPromptFrame(void);

	virtual BOOL Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam );
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

private:
	BOOL GUIEventHandler(tagGUIEvent* pEvent);
	VOID SafeDestroyUI();
	VOID OnLogOut( const int nLogType );

	//处理网络事件消息，得到需要提示那些内容
	DWORD OnNS_ExitGamePrompt( tagNS_ExitGamePrompt* pNetCmd, DWORD dwParams );

	VOID Navigate( tagGUIEvent* pEvent,int nWndNum );
private:
	TSFPTrunk<ExitPromptFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<NetSession>				m_pSession;

	vector<tagIDLink>				m_vecIdLinks[2];
	vector<tagPosLink>				m_vecPosLinks[2];

	GUIWnd*							m_pWnd;
	GUIStaticEx*					m_stcPrompts[2];		// 杀怪提示
	GUIButton*						m_pBtnContinue;			// 返回游戏
	GUIButton*						m_pBtnQuit;				// 退出游戏
	GUIButton*						m_pBtnClose;			// 关闭按钮

	DWORD							m_dwTime;				// 如果
};
