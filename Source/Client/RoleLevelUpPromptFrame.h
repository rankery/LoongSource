#pragma once
#include "gameframe.h"
#include "QuestMgr.h"

class RoleLevelUpPromptFrame : public GameFrame
{
public:
	RoleLevelUpPromptFrame( void );
	~RoleLevelUpPromptFrame( void );

	virtual BOOL Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam );
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

	VOID LevelUpTo( DWORD dwLevel );

private:
	BOOL EventHandler( tagGUIEvent* pEvent );
	VOID Navigate( tagGUIEvent* pEvent );
	
	VOID ShowWnd( BOOL bShow );

private:
	TSFPTrunk<RoleLevelUpPromptFrame>		m_Trunk;
	TObjRef<GUISystem>						m_pGUI;
	TObjRef<Util>							m_pUtil;
	TObjRef<GameFrameMgr>					m_pMgr;

	vector<tagIDLink>						m_vecIdLinks;
	vector<tagPosLink>						m_vecPosLinks;

	GUIWnd*									m_pWnd;					//主窗口
	GUIButton*								m_pOk;					//确定
	GUIStaticEx*							m_pStcContent;			//显示内容的面板

	tstring									m_strDesc;				//升级时需要提示的信息
};
