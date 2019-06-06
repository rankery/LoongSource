#pragma once

class TalentTreeFrame;
class SkillBookFrame;

/** class	TalentFrame
	brief	天资界面
*/
class TalentFrame :
	public GameFrame
{
public:
	TalentFrame(void);
	~TalentFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	GUIWnd*	GetSkillBook(){return m_pWndSkillBook;}
	GUIWnd*	GetTalentTree(){return m_pWndTalentTree;}

protected:
	VOID ShowWnd(BOOL bVal);

	//--处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);
	
	//--处理游戏事件
	//--开启天资界面
	DWORD OnOpenDower(tagGameEvent* pGameEvent);
	//--关闭天资界面
	DWORD OnCloseDower(tagGameEvent* pGameEvent);

private:

	TSFPTrunk<TalentFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;
	GUIWnd*						m_pWndSkillBook;
	GUIWnd*						m_pWndTalentTree;
	GUIButton*					m_pBtnExit;

	TalentTreeFrame*			m_pTalentTree;
	SkillBookFrame*				m_pSkillBook;


};
