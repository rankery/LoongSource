#pragma once
#include "gameframe.h"
#include "ActivityMgr.h"
#include "QuestMgr.h"

/**节日活动上线提示
*/
class FestivalActivityPromptFrame : public GameFrame
{

public:
	FestivalActivityPromptFrame( void );
	~FestivalActivityPromptFrame( void );

	virtual BOOL Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam );
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

	BOOL CanDestroy();

private:
	BOOL EventHandler( tagGUIEvent* pEvent );
	VOID Navigate( tagGUIEvent* pEvent );
	VOID ShowWnd( BOOL bShow );
	VOID Prompt();

	/**需要上线提示的节日活动 的过滤器
	*/
	bool NeedPrompt4FestivalActivity( const tagActivityProtoData &activity );

private:
	TSFPTrunk<FestivalActivityPromptFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameFrameMgr>			m_pMgr;

	std::list<tagActivityProtoData>		m_needPromptActivities;	//需要提示的忌日活动
	tagActivityProtoData			m_curActivityProtoData;	//当前正处理的节日活动信息
	vector<tagIDLink>				m_vecIdLinks;			//NPC导航的链接
	vector<tagPosLink>				m_vecPosLinks;			//位置导航的链接

	GUIWnd*							m_pWnd;					//主窗口
	GUIButton*						m_pOk;					//确定
	GUIStaticEx*					m_pStcContent;			//显示内容的面板
	GUIScrollBar*					m_pScrollBar;			//滚动条

	BOOL							m_bCanDestroy;			//是否可销毁此模块

};
